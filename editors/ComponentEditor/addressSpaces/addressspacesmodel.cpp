//-----------------------------------------------------------------------------
// File: addressspacesmodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 11.06.2012
//
// Description:
// The model class to manage the objects for address spaces editor.
//-----------------------------------------------------------------------------

#include "addressspacesmodel.h"

#include "AddressSpaceColumns.h"

#include <editors/ComponentEditor/common/ExpressionFormatter.h>

#include <editors/ComponentEditor/addressSpaces/AddressSpaceExpressionsGatherer.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/ReferenceCalculator.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/AddressSpace.h>

#include <IPXACTmodels/Component/validators/AddressSpaceValidator.h>

#include <QColor>
#include <QRegularExpression>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>

//-----------------------------------------------------------------------------
// Function: AddressSpacesModel::AddressSpacesModel()
//-----------------------------------------------------------------------------
AddressSpacesModel::AddressSpacesModel(QSharedPointer<Component> component,
                                       QSharedPointer<ParameterFinder> parameterFinder,
                                       QSharedPointer<ExpressionFormatter> expressionFormatter,
                                       QSharedPointer<AddressSpaceValidator> addressSpaceValidator,
                                       QObject *parent):
ReferencingTableModel(parameterFinder, parent),
ParameterizableTable(parameterFinder),
component_(component),
addressSpaces_(component->getAddressSpaces()),
expressionFormatter_(expressionFormatter),
addressSpaceValidator_(addressSpaceValidator)
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
	return addressSpaces_->size();
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
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
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
            return tr("Master interface\nbinding(s)");
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
	if (!index.isValid() || index.row() < 0 || index.row() >= addressSpaces_->size())
    {
		return QVariant();
	}

	if (role == Qt::DisplayRole) 
    {
        if (isValidExpressionColumn(index))
        {
            return expressionFormatter_->formatReferringExpression(expressionOrValueForIndex(index).toString());
        }
        else if (index.column() == AddressSpaceColumns::DESCRIPTION)
        {
            return expressionOrValueForIndex(index).toString().replace(QRegularExpression("\n.*$", 
                QRegularExpression::DotMatchesEverythingOption), "...");
        }
        return expressionOrValueForIndex(index);
	}
    else if (role == Qt::EditRole)
    {
        if (index.column() == AddressSpaceColumns::DESCRIPTION)
        {
            return expressionOrValueForIndex(index);
        }

        return expressionOrValueForIndex(index);  
    }
	else if (Qt::UserRole == role)
    {
		return component_->getMasterInterfaces(addressSpaces_->at(index.row())->name());
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
	if (!index.isValid() || index.row() < 0 || index.row() >= addressSpaces_->size())
    {
		return false;
	}

	if (Qt::EditRole == role)
    {
        if (index.column() == AddressSpaceColumns::NAME)
        {
            addressSpaces_->at(index.row())->setName(value.toString());
        }
        else if (index.column() == AddressSpaceColumns::AUB)
        {
            addressSpaces_->at(index.row())->setAddressUnitBits(value.toString());
            emit aubChangedOnRow(index.row());
        }
        else if (index.column() == AddressSpaceColumns::WIDTH)
        {
            addressSpaces_->at(index.row())->setWidth(value.toString());
        }
        else if (index.column() == AddressSpaceColumns::RANGE)
        {
            addressSpaces_->at(index.row())->setRange(value.toString());
        }
        else if (index.column() == AddressSpaceColumns::DESCRIPTION)
        {
            addressSpaces_->at(index.row())->setDescription(value.toString());
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
	int row = addressSpaces_->size();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	addressSpaces_->insert(row, QSharedPointer<AddressSpace>(new AddressSpace()));
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
	if (!index.isValid() || index.row() < 0 || index.row() >= addressSpaces_->size())
    {
		return;
	}

    decreaseReferencesWithRemovedAddressSpace(addressSpaces_->at(index.row()));

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	addressSpaces_->removeAt(index.row());
	endRemoveRows();

	// inform navigation tree that file set has been removed
	emit addrSpaceRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
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
        return addressSpaces_->at(index.row())->name();
    }
    else if (index.column() == AddressSpaceColumns::AUB)
    {
        return addressSpaces_->at(index.row())->getAddressUnitBits();
    }
    else if (index.column() == AddressSpaceColumns::WIDTH)
    {
        return addressSpaces_->at(index.row())->getWidth();
    }
    else if (index.column() == AddressSpaceColumns::RANGE)
    {
        return addressSpaces_->at(index.row())->getRange();
    }
    else if (index.column() == AddressSpaceColumns::INTERFACE_BINDING)
    {
        QStringList interfaceNames = component_->getMasterInterfaces(addressSpaces_->at(index.row())->name());

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
        return addressSpaces_->at(index.row())->description();
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
    QSharedPointer<AddressSpace> currentSpace = addressSpaces_->at(index.row());

    if (index.column() == AddressSpaceColumns::NAME)
    {
        return addressSpaceValidator_->hasValidName(currentSpace->name());
    }
    else if (index.column() == AddressSpaceColumns::AUB)
    {
        return addressSpaceValidator_->hasValidAddressUnitBits(currentSpace);
    }
    else if (index.column() == AddressSpaceColumns::RANGE)
    {
        return addressSpaceValidator_->hasValidRange(currentSpace);
    }
    else if (index.column() == AddressSpaceColumns::WIDTH)
    {
        return addressSpaceValidator_->hasValidWidth(currentSpace);
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
    int referencesInRange = addressSpaces_->at(row)->getRange().count(valueID);
    int referencesInWidth = addressSpaces_->at(row)->getWidth().count(valueID);

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

//-----------------------------------------------------------------------------
// Function: AddressSpacesModel::onCopyRows()
//-----------------------------------------------------------------------------
void AddressSpacesModel::onCopyRows(QModelIndexList indexList)
{
    QList<QSharedPointer<AddressSpace> > copiedSpaces;
    foreach (QModelIndex index, indexList)
    {
        QSharedPointer<AddressSpace> space = addressSpaces_->at(index.row());
        copiedSpaces.append(space);
    }

    QVariant addressSpaceVariant;
    addressSpaceVariant.setValue(copiedSpaces);

    QMimeData* newMimeData = new QMimeData();
    newMimeData->setData("text/xml/ipxact:addressSpace", QByteArray());
    newMimeData->setImageData(addressSpaceVariant);

    QApplication::clipboard()->setMimeData(newMimeData);
}

//-----------------------------------------------------------------------------
// Function: AddressSpacesModel::onPasteRows()
//-----------------------------------------------------------------------------
void AddressSpacesModel::onPasteRows()
{
    const QMimeData* pasteData = QApplication::clipboard()->mimeData();

    if (pasteData->hasImage())
    {
        QVariant pasteVariant = pasteData->imageData();
        if (pasteVariant.canConvert<QList<QSharedPointer<AddressSpace> > >())
        {
            AddressSpaceExpressionGatherer gatherer;
            ReferenceCalculator referenceCalculator(getParameterFinder());

            QList<QSharedPointer<AddressSpace> > newAddressSpaces =
                pasteVariant.value<QList<QSharedPointer<AddressSpace> > >();

            int rowBegin = addressSpaces_->size();
            int rowEnd = rowBegin + newAddressSpaces.size() - 1;

            beginInsertRows(QModelIndex(), rowBegin, rowEnd);

            foreach(QSharedPointer<AddressSpace> copySpace, newAddressSpaces)
            {
                QSharedPointer<AddressSpace> newSpace (new AddressSpace(*copySpace.data()));
                newSpace->setName(getUniqueName(newSpace->name(), getCurrentItemNames()));
                
                addressSpaces_->append(newSpace);
                
                increaseReferencesInPastedAddressSpace(newSpace, gatherer, referenceCalculator);
                
                emit addrSpaceAdded(rowBegin);
            }

            endInsertRows();

            emit contentChanged();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSpacesModel::getCurrentItemNames()
//-----------------------------------------------------------------------------
QStringList AddressSpacesModel::getCurrentItemNames()
{
    QStringList names;
    foreach (QSharedPointer<AddressSpace> space, *addressSpaces_)
    {
        names.append(space->name());
    }

    return names;
}

//-----------------------------------------------------------------------------
// Function: AddressSpacesModel::increaseReferencesInPastedAddressBlock()
//-----------------------------------------------------------------------------
void AddressSpacesModel::increaseReferencesInPastedAddressSpace(QSharedPointer<AddressSpace> pastedSpace,
    AddressSpaceExpressionGatherer& gatherer, ReferenceCalculator& referenceCalculator)
{
    QStringList spaceExpressions = gatherer.getExpressions(pastedSpace);

    QMap<QString, int> referencedParameters = referenceCalculator.getReferencedParameters(spaceExpressions);

    QMapIterator<QString, int> refParameterIterator (referencedParameters);
    while (refParameterIterator.hasNext())
    {
        refParameterIterator.next();
        for (int i = 0; i < refParameterIterator.value(); ++i)
        {
            emit increaseReferences(refParameterIterator.key());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSpacesModel::mimeTypes()
//-----------------------------------------------------------------------------
QStringList AddressSpacesModel::mimeTypes() const
{
    QStringList types(QAbstractItemModel::mimeTypes());

    types << "text/xml/ipxact:addressSpace";

    return types;
}
