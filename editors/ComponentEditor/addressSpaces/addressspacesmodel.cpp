/* 
 *  	Created on: 11.6.2012
 *      Author: Antti Kamppi
 * 		filename: addressspacesmodel.cpp
 *		Project: Kactus 2
 */

#include "addressspacesmodel.h"

#include "AddressSpaceColumns.h"

#include <editors/ComponentEditor/common/ExpressionFormatter.h>

#include <editors/ComponentEditor/addressSpaces/AddressSpaceExpressionsGatherer.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/ReferenceCalculator.h>

#include <QColor>

//-----------------------------------------------------------------------------
// Function: AddressSpacesModel::AddressSpacesModel()
//-----------------------------------------------------------------------------
AddressSpacesModel::AddressSpacesModel(QSharedPointer<Component> component, 
    QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter,
    QObject *parent):
ReferencingTableModel(parameterFinder, parent),
    ParameterizableTable(parameterFinder),
    component_(component),
    addrSpaces_(component->getAddressSpaces()),
    expressionFormatter_(expressionFormatter)
{

}

//-----------------------------------------------------------------------------
// Function: AddressSpacesModel::~AddressSpacesModel()
//-----------------------------------------------------------------------------
AddressSpacesModel::~AddressSpacesModel()
{
}

//-----------------------------------------------------------------------------
// Function: AddressSpacesModel::rowCount()
//-----------------------------------------------------------------------------
int AddressSpacesModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}
	return addrSpaces_.size();
}

//-----------------------------------------------------------------------------
// Function: AddressSpacesModel::columnCount()
//-----------------------------------------------------------------------------
int AddressSpacesModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

	return AddressSpaceColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: AddressSpacesModel::rowCount()
//-----------------------------------------------------------------------------
Qt::ItemFlags AddressSpacesModel::flags(QModelIndex const& index ) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
	}
	// interface references are made in bus interface editor
	if (index.column() == AddressSpaceColumns::INTERFACE_BINDING)
    {
		return Qt::ItemIsSelectable;
	}

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: AddressSpacesModel::headerData()
//-----------------------------------------------------------------------------
QVariant AddressSpacesModel::headerData( int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Horizontal)
    {
		return QVariant();
	}
	if (Qt::DisplayRole == role) 
    {
        if (section == AddressSpaceColumns::NAME)
        {
            return tr("Name");
        }
        else if (section == AddressSpaceColumns::AUB)
        {
            return tr("Addressable\nunit bits (AUB)");
        }
        else if (section == AddressSpaceColumns::RANGE)
        {
            return tr("Range\n[AUB]") + getExpressionSymbol();
        }
        else if (section == AddressSpaceColumns::WIDTH)
        {
            return tr("Width\n [bits]") + getExpressionSymbol();
        }
        else if (section == AddressSpaceColumns::INTERFACE_BINDING)
        {
            return tr("Master interface\nbinding");
        }
        else if (section == AddressSpaceColumns::DESCRIPTION)
        {
            return tr("Description");
        }
        else
        {
            return QVariant();
        }
	}
	else
    {
		return QVariant();
	}
}

//-----------------------------------------------------------------------------
// Function: AddressSpacesModel::data()
//-----------------------------------------------------------------------------
QVariant AddressSpacesModel::data(QModelIndex const& index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= addrSpaces_.size())
    {
		return QVariant();
	}

	if (role == Qt::DisplayRole) 
    {
        if (isValidExpressionColumn(index))
        {
            return expressionFormatter_->formatReferringExpression(expressionOrValueForIndex(index).toString());
        }

        return expressionOrValueForIndex(index);
	}
    else if (role == Qt::EditRole)
    {
        return expressionOrValueForIndex(index);  
    }
	else if (Qt::UserRole == role)
    {
		return component_->getMasterInterfaces(addrSpaces_.at(index.row())->getName());
	}
	else if (Qt::ForegroundRole == role)
    {
        if (index.column() == AddressSpaceColumns::INTERFACE_BINDING)
        {
            return QColor("gray");
        }
        
        return blackForValidOrRedForInvalidIndex(index);
    }
    else if (Qt::BackgroundRole == role)
    {
        if (index.column() == AddressSpaceColumns::NAME || index.column() ==  AddressSpaceColumns::AUB ||
            index.column() ==  AddressSpaceColumns::WIDTH || index.column() ==  AddressSpaceColumns::RANGE)
        {
            return QColor("LemonChiffon");
        }
        else
        {
            return QColor("white");
        }
    }
    else if (role == Qt::ToolTipRole)
    {
        if (isValidExpressionColumn(index))
        {
            return formattedValueFor(expressionOrValueForIndex(index).toString());
        }

        return expressionOrValueForIndex(index);        
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSpacesModel::setData()
//-----------------------------------------------------------------------------
bool AddressSpacesModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
	if (!index.isValid() || index.row() < 0 || index.row() >= addrSpaces_.size())
    {
		return false;
	}

	if (Qt::EditRole == role)
    {
        if (index.column() == AddressSpaceColumns::NAME)
        {
            addrSpaces_[index.row()]->setName(value.toString());
        }
        else if (index.column() == AddressSpaceColumns::AUB)
        {
            addrSpaces_[index.row()]->setAddressUnitBits(value.toUInt());
        }
        else if (index.column() == AddressSpaceColumns::WIDTH)
        {
            addrSpaces_[index.row()]->setWidth(parseExpressionToDecimal(value.toString()).toUInt());
            addrSpaces_[index.row()]->setWidthExpression(value.toString());
        }
        else if (index.column() == AddressSpaceColumns::RANGE)
        {
            addrSpaces_[index.row()]->setRange(value.toString());
        }
        else if (index.column() == AddressSpaceColumns::DESCRIPTION)
        {
            addrSpaces_[index.row()]->setDescription(value.toString());
        }
        else
        {
            return false;
        }

		emit dataChanged(index, index);
		emit contentChanged();
		return true;
	}
	else
    {
		return false;
	}
}

//-----------------------------------------------------------------------------
// Function: AddressSpacesModel::onAddItem()
//-----------------------------------------------------------------------------
void AddressSpacesModel::onAddItem(QModelIndex const& index )
{
	int row = addrSpaces_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	addrSpaces_.insert(row, QSharedPointer<AddressSpace>(new AddressSpace()));
	endInsertRows();

	// inform navigation tree that file set is added
	emit addrSpaceAdded(row);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AddressSpacesModel::onRemoveItem()
//-----------------------------------------------------------------------------
void AddressSpacesModel::onRemoveItem(QModelIndex const& index )
{
	// don't remove anything if index is invalid
	if (!index.isValid() || index.row() < 0 || index.row() >= addrSpaces_.size())
    {
		return;
	}

    decreaseReferencesWithRemovedAddressSpace(addrSpaces_.at(index.row()));

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	addrSpaces_.removeAt(index.row());
	endRemoveRows();

	// inform navigation tree that file set has been removed
	emit addrSpaceRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AddressSpacesModel::isValid()
//-----------------------------------------------------------------------------
bool AddressSpacesModel::isValid() const
{
	// if at least one address space is invalid
	foreach (QSharedPointer<AddressSpace> addrSpace, addrSpaces_) 
    {
		if (!addrSpace->isValid(component_->getChoices(), component_->getRemapStateNames()))
        {
			return false;
		}
	}
	// all address spaces were valid
	return true;
}

//-----------------------------------------------------------------------------
// Function: AddressSpacesModel::isValidExpressionColumn()
//-----------------------------------------------------------------------------
bool AddressSpacesModel::isValidExpressionColumn(QModelIndex const& index) const
{
    return index.column() == AddressSpaceColumns::WIDTH || index.column() == AddressSpaceColumns::RANGE;
}

//-----------------------------------------------------------------------------
// Function: AddressSpacesModel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant AddressSpacesModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    if (index.column() == AddressSpaceColumns::NAME)
    {
        return addrSpaces_.at(index.row())->getName();
    }
    else if (index.column() == AddressSpaceColumns::AUB)
    {
        return addrSpaces_.at(index.row())->getAddressUnitBits();
    }
    else if (index.column() == AddressSpaceColumns::WIDTH)
    {
        return addrSpaces_.at(index.row())->getWidthExpression();
    }
    else if (index.column() == AddressSpaceColumns::RANGE)
    {
        return addrSpaces_.at(index.row())->getRange();
    }
    else if (index.column() == AddressSpaceColumns::INTERFACE_BINDING)
    {
        QStringList interfaceNames = component_->getMasterInterfaces(addrSpaces_.at(index.row())->getName());

        // if no interface refers to the memory map
        if (interfaceNames.isEmpty())
        {
            return tr("No binding");
        }
        // if there are then show them separated by space
        else
        {
            return interfaceNames.join(", ");
        }
    }
    else if (index.column() == AddressSpaceColumns::DESCRIPTION)
    {
        return addrSpaces_.at(index.row())->getDescription();
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSpacesModel::validateIndex()
//-----------------------------------------------------------------------------
bool AddressSpacesModel::validateIndex(QModelIndex const& index) const
{
    if (isValidExpressionColumn(index))
    {
        return isValuePlainOrExpression(expressionOrValueForIndex(index).toString());
    }
    else
    {
        return true;
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSpacesModel::getAllReferencesToIdInItemOnRow()
//-----------------------------------------------------------------------------
int AddressSpacesModel::getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const
{
    int referencesInRange = addrSpaces_.at(row)->getRange().count(valueID);
    int referencesInWidth = addrSpaces_.at(row)->getWidthExpression().count(valueID);

    return referencesInRange + referencesInWidth;
}

//-----------------------------------------------------------------------------
// Function: addressspacesmodel::decreaseReferencesWithRemovedAddressSpace()
//-----------------------------------------------------------------------------
void AddressSpacesModel::decreaseReferencesWithRemovedAddressSpace(
    QSharedPointer<AddressSpace> removedAddressSpace)
{
    AddressSpaceExpressionGatherer addressSpaceGatherer;

    QStringList expressionList = addressSpaceGatherer.getExpressions(removedAddressSpace);

    ReferenceCalculator addressSpaceReferenceCalculator(getParameterFinder());

    QMap<QString, int> referencedParameters = addressSpaceReferenceCalculator.getReferencedParameters(
        expressionList);

    foreach (QString referencedId, referencedParameters.keys())
    {
        for (int i = 0 ; i < referencedParameters.value(referencedId); ++i)
        {
            emit decreaseReferences(referencedId);
        }
    }
}
