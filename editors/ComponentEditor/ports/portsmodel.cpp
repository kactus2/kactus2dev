//-----------------------------------------------------------------------------
// File: portsmodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 31.3.2011
//
// Description:
// Table model that can be used to display ports to be edited.
//-----------------------------------------------------------------------------

#include "portsmodel.h"

#include <common/KactusColors.h>

#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/Component/Model.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/validators/PortValidator.h>

#include <QString>
#include <QStringList>
#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: PortsModel::PortsModel()
//-----------------------------------------------------------------------------
PortsModel::PortsModel(QSharedPointer<Model> model, QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<PortValidator> portValidator, QSortFilterProxyModel* filter, QObject *parent):
ReferencingTableModel(parameterFinder, parent),
ParameterizableTable(parameterFinder),
model_(model),
lockedIndexes_(),
parameterFinder_(parameterFinder),
expressionFormatter_(expressionFormatter),
portValidator_(portValidator),
filter_(filter)
{
    Q_ASSERT(model_);
    setExpressionParser(expressionParser);
}

//-----------------------------------------------------------------------------
// Function: PortsModel::rowCount()
//-----------------------------------------------------------------------------
int PortsModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
    }

 	return model_->getPorts()->count();
}   

//-----------------------------------------------------------------------------
// Function: PortsModel::data()
//-----------------------------------------------------------------------------
QVariant PortsModel::data(QModelIndex const& index, int role) const
{
 	if (!index.isValid() || index.row() < 0 || index.row() >= model_->getPorts()->count())
    {
		return QVariant();
	}

    QSharedPointer<Port> port = portOnRow(index.row());

    if (role == Qt::DisplayRole) 
    {
        if (isValidExpressionColumn(index))
        {
            return expressionFormatter_->formatReferringExpression(valueForIndex(index).toString());
        }
        else if (index.column() == descriptionColumn())
        {
            return valueForIndex(index).toString().replace(QRegularExpression("\n.*$", 
                QRegularExpression::DotMatchesEverythingOption), "...");
        }
        else
        {
            return valueForIndex(index);
        }
    }
    else if (role == Qt::EditRole)
    {
        if (index.column() == descriptionColumn())
        {
            return valueForIndex(index);
        }
        return expressionOrValueForIndex(index);
    }
    else if (role == Qt::ToolTipRole)
    {
        if (isValidExpressionColumn(index))
        {
            return formattedValueFor(valueForIndex(index).toString());
        }
        else
        {
            return valueForIndex(index);
        }
    }
    else if (role == Qt::ForegroundRole)
    {
        if (isLocked(index))
        {
            return KactusColors::DISABLED_TEXT;
        }
        else
        {
            return blackForValidOrRedForInvalidIndex(index);
        }
    }
	else if (role == Qt::BackgroundRole)
    {
        if (indexedItemIsDisabled(index, port))
        {
            return KactusColors::DISABLED_FIELD;
        }
        else if (indexedItemIsMandatory(index))
        {
            return KactusColors::MANDATORY_FIELD;
        }
        else
        {
            return KactusColors::REGULAR_FIELD;
        }
    }
    else if (Qt::CheckStateRole == role)
    {
        if (index.column() == adHocColumn())
        {
            if (portOnRow(index.row())->isAdHocVisible())
            {
                return Qt::Checked;
            }
            else
            {
                return Qt::Unchecked;
            }
        }
        else
        {
            return QVariant();
        }
    }
    else if (role == Qt::TextAlignmentRole && index.column() == rowNumberColumn())
    {
        return Qt::AlignRight + Qt::AlignVCenter;
    }
    else if (role == portRoles::getPortRole)
    {
        return QVariant::fromValue(port);
    }

	// if unsupported role
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: PortsModel::headerData()
//-----------------------------------------------------------------------------
QVariant PortsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && section >= 0)
    {
        if (orientation == Qt::Vertical)
        {
            return portOnRow(section)->name();
        }

        else if (orientation == Qt::Horizontal)
        {
            if (section == rowNumberColumn())
            {
                return tr("#");
            }
            else if (section == nameColumn())
            {
                return tr("Name");
            }
            else if (section == typeColumn())
            {
                return tr("Type");
            }
            else if (section == arrayLeftColumn())
            {
                QString arrayLeftHeader = tr("Array\nleft") + getExpressionSymbol();
                return arrayLeftHeader;
            }
            else if (section == arrayRightColum())
            {
                QString arrayRightHeader = tr("Array\nright") + getExpressionSymbol();
                return arrayRightHeader;
            }
            else if (section == tagColumn())
            {
                return tr("Port\ntags");
            }
            else if (section == adHocColumn())
            {
                return tr("Ad-hoc");
            }
            else if (section == descriptionColumn())
            {
                return tr("Description");
            }
            else
            {
                return QVariant();
            }
        } 
    }

    // if unsupported role
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: PortsModel::setData()
//-----------------------------------------------------------------------------
bool PortsModel::setData(QModelIndex const& index, QVariant const& value, int role)
{	
 	if (!index.isValid() || index.row() < 0 || index.row() >= model_->getPorts()->count())
    {
		return false;
    }

    QSharedPointer<Port> port = portOnRow(index.row());

	if (role == Qt::EditRole)
    {
        if (isLocked(index))
        {
            return false;
        }

        if (index.column() == rowNumberColumn())
        {
            return false;
        }
        else if (index.column() == nameColumn())
        {
            port->setName(value.toString());
            emit headerDataChanged(Qt::Vertical, index.row(), index.row());
            emit portExtensionDataChanged(index);
        }
        else if (index.column() == typeColumn())
        {
            QString typeName = value.toString();
            if (typeName.isEmpty())
            {
                port->getTypes()->clear();
            }
            else
            {
                port->setTypeName(typeName);

                // update the type definition for the new type name.
                port->setTypeDefinition(typeName, QString());
            }

            emit dataChanged(index, index);
            return true;
        }
        else if (index.column() == arrayLeftColumn())
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(port->getArrayLeft());
            }

            port->setArrayLeft(value.toString());

            if (value.isValid() && port->getArrayRight().isEmpty() && !port->getArrayLeft().isEmpty())
            {
                port->setArrayRight(QString::number(0));
            }
        }
        else if (index.column() == arrayRightColum())
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(port->getArrayRight());
            }

            port->setArrayRight(value.toString());

            if (value.isValid() && port->getArrayLeft().isEmpty() && !port->getArrayRight().isEmpty())
            {
                port->setArrayLeft(QString::number(0));
            }
        }
        else if (index.column() == tagColumn())
        {
            QString tagGroup = value.toString();
            port->setPortTags(tagGroup);

            emit portExtensionDataChanged(index);
        }
        else if (index.column() == adHocColumn())
        {
            port->setAdHocVisible(value.toBool());
        }
        else if (index.column() == descriptionColumn())
        {
            port->setDescription(value.toString());
        }
        else
        {
            return false;
        }

        emit dataChanged(index, index);
        return true;
    }
    else if (role == Qt::CheckStateRole)
    {
        port->setAdHocVisible(value == Qt::Checked);
        emit dataChanged(index, index);
        return true;
    }
    // unsupported role
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: PortsModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags PortsModel::flags(QModelIndex const& index) const
{
    if (!index.isValid() || index.column() == rowNumberColumn())
    {
		return Qt::NoItemFlags;
    }

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if (isLocked(index) || indexedItemIsLocked(index))
    {
        return flags;
    }
    else if (index.column() == adHocColumn())
    {
        flags |= Qt::ItemIsUserCheckable;
    }
    else
    {
        flags |= Qt::ItemIsEditable;
    }

    return flags;
}

//-----------------------------------------------------------------------------
// Function: PortsModel::isValid()
//-----------------------------------------------------------------------------
bool PortsModel::isValid() const
{	
	// check all ports in the table
	foreach (QSharedPointer<Port> port, *model_->getPorts())
    {
        if (!portValidator_->validate(port))
        {
			return false;
        }
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: PortsModel::onRemoveRow()
//-----------------------------------------------------------------------------
void PortsModel::onRemoveRow(int row)
{
	// if row is invalid
	if (row < 0 || row >=model_->getPorts()->count() || rowIsLocked(row))
    {
		return;
    }

    beginRemoveRows(QModelIndex(), row, row);
    model_->getPorts()->removeAt(row);
	endRemoveRows();

    emit invalidateOtherFilter();
    emit portCountChanged();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: PortsModel::onRemoveItem()
//-----------------------------------------------------------------------------
void PortsModel::onRemoveItem(QModelIndex const& index)
{
	// don't remove anything if index is invalid
	if (!index.isValid() || index.row() < 0 || index.row() >= model_->getPorts()->count() ||
        rowIsLocked(index.row()))
    {
		return;
	}

	beginRemoveRows(QModelIndex(), index.row(), index.row());

    removeReferencesInItemOnRow(index.row());
    model_->getPorts()->removeAt(index.row());

	endRemoveRows();

    emit invalidateOtherFilter();
    emit portCountChanged();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: PortsModel::onAddRow()
//-----------------------------------------------------------------------------
void PortsModel::onAddRow()
{
    int lastRow = model_->getPorts()->count();

 	beginInsertRows(QModelIndex(), lastRow, lastRow);

	QSharedPointer<Port> port(new Port());
    finalizePort(port);
	model_->getPorts()->append(port);

	endInsertRows();

    emit invalidateOtherFilter();
    emit portCountChanged();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: PortsModel::onAddItem()
//-----------------------------------------------------------------------------
void PortsModel::onAddItem(QModelIndex const& index)
{
	int row = model_->getPorts()->count();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

    QSharedPointer<Port> newPort(new Port());
    finalizePort(newPort);

	beginInsertRows(QModelIndex(), row, row);
    model_->getPorts()->insert(row, newPort);
    endInsertRows();

    emit invalidateOtherFilter();
    emit portCountChanged();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: PortsModel::addPort()
//-----------------------------------------------------------------------------
void PortsModel::addPort(QSharedPointer<Port> port)
{
    int lastRow = model_->getPorts()->count(); 

	beginInsertRows(QModelIndex(), lastRow, lastRow);

	model_->getPorts()->append(port);
    lockPort(port);

	endInsertRows();

    emit invalidateOtherFilter();
    emit portCountChanged();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: PortsModel::index()
//-----------------------------------------------------------------------------
QModelIndex PortsModel::index(QSharedPointer<Port> port) const
{
	// find the correct row
	int row = model_->getPorts()->indexOf(port);

	// if the named port is not found
	if (row < 0)
    {
		return QModelIndex();
	}

	// the base class creates the index for the row
	return QAbstractTableModel::index(row, 0, QModelIndex());
}

//-----------------------------------------------------------------------------
// Function: PortsModel::setModelAndLockCurrentPorts()
//-----------------------------------------------------------------------------
void PortsModel::setModelAndLockCurrentPorts(QSharedPointer<Model> model)
{
    beginResetModel();

    lockedIndexes_.clear();

    model_ = model;
    
    endResetModel();

    foreach(QSharedPointer<Port> port, *model_->getPorts())
    {
        if (port->getDirection() != DirectionTypes::DIRECTION_PHANTOM)
        {
            lockPort(port);
        }
    }

    emit contentChanged();
    emit portCountChanged();
}

//-----------------------------------------------------------------------------
// Function: PortsModel::portOnRow()
//-----------------------------------------------------------------------------
QSharedPointer<Port> PortsModel::portOnRow(int row) const
{
    return model_->getPorts()->at(row);
}

//-----------------------------------------------------------------------------
// Function: lockPort()
//-----------------------------------------------------------------------------
void PortsModel::lockPort(QSharedPointer<Port> port)
{
    QModelIndex portIndex = index(port);
    QModelIndexList lockedPortIndexes = getLockedPortIndexes(portIndex);

    for (auto index : lockedPortIndexes)
    {
        if (index.isValid())
        {
            lockIndex(index);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: unlockPort()
//-----------------------------------------------------------------------------
void PortsModel::unlockPort(QSharedPointer<Port> port)
{
    QModelIndex portIndex = index(port);
    QModelIndexList lockedPortIndexes = getLockedPortIndexes(portIndex);
    for (auto index : lockedPortIndexes)
    {
        if (index.isValid())
        {
            unlockIndex(index);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: lockIndex()
//-----------------------------------------------------------------------------
void PortsModel::lockIndex(QModelIndex const& index)
{
    if(!isLocked(index))
    {
        lockedIndexes_.append(QPersistentModelIndex(index));
    }
}

//-----------------------------------------------------------------------------
// Function: unlockIndex()
//-----------------------------------------------------------------------------
void PortsModel::unlockIndex(QModelIndex const& index)
{
    lockedIndexes_.removeAll(QPersistentModelIndex(index));
}

//-----------------------------------------------------------------------------
// Function: isLocked()
//-----------------------------------------------------------------------------
bool PortsModel::isLocked(QModelIndex const& index) const
{
    return lockedIndexes_.contains(QPersistentModelIndex(index));
}

//-----------------------------------------------------------------------------
// Function: PortsModel::rowIsLocked()
//-----------------------------------------------------------------------------
bool PortsModel::rowIsLocked(int row)
{
    QModelIndex nameIndex = QAbstractTableModel::index(row, nameColumn(), QModelIndex());
    return nameIndex.isValid() && isLocked(nameIndex);
}

//-----------------------------------------------------------------------------
// Function: portsmodel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant PortsModel::valueForIndex(QModelIndex const& index) const
{
    QSharedPointer<Port> port = portOnRow(index.row());

    if (index.column() == rowNumberColumn())
    {
        QModelIndex filteredIndex = filter_->mapFromSource(index);
        return filteredIndex.row() + 1;
    }
    else if (index.column() == nameColumn())
    {
        return port->name();
    }
    else if (index.column() == typeColumn())
    {
        return getTypeName(port);
    }
    else if (index.column() == arrayLeftColumn())
    {
        return port->getArrayLeft();
    }
    else if (index.column() == arrayRightColum())
    {
        return port->getArrayRight();
    }
    else if (index.column() == tagColumn())
    {
        return port->getPortTags();
    }
    else if (index.column() == adHocColumn())
    {
        return port->isAdHocVisible();
    }
    else if (index.column() == descriptionColumn())
    {
        return port->description();
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: portsmodel::getTypeName()
//-----------------------------------------------------------------------------
QString PortsModel::getTypeName(QSharedPointer<Port> port) const
{
    QSharedPointer<QList<QSharedPointer<WireTypeDef> > > definitionList;
    if (port->getWire())
    {
        definitionList = port->getWire()->getWireTypeDefs();
    }
    else if (port->getTransactional())
    {
        definitionList = port->getTransactional()->getTransTypeDef();
    }

    if (definitionList)
    {
        if (definitionList->count() > 1)
        {
            QString combinedType = "";
            foreach (QSharedPointer<WireTypeDef> typeDefinition, *definitionList)
            {
                combinedType.append(typeDefinition->getTypeName());
                if (typeDefinition != definitionList->last())
                {
                    combinedType.append(", ");
                }
            }

            return combinedType;
        }
        else if (!definitionList->isEmpty())
        {
            return definitionList->first()->getTypeName();
        }
    }
    return QString();
}

//-----------------------------------------------------------------------------
// Function: portsmodel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant PortsModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    QSharedPointer<Port> port = portOnRow(index.row());

    if (index.column() == arrayLeftColumn())
    {
        return port->getArrayLeft();
    }
    else if (index.column() == arrayRightColum())
    {
        return port->getArrayRight();
    }
    else
    {
        return data(index, Qt::DisplayRole);
    }
}

//-----------------------------------------------------------------------------
// Function: portsmodel::validateIndex()
//-----------------------------------------------------------------------------
bool PortsModel::validateIndex(QModelIndex const& index) const
{
    QSharedPointer<Port> port = portOnRow(index.row());

    if (index.column() == nameColumn())
    {
        return portValidator_->hasValidName(port->name());
    }
    else if (index.column() == arrayLeftColumn())
    {
        return portValidator_->arrayValueIsValid(port->getArrayLeft());
    }
    else if (index.column() == arrayRightColum())
    {
        return portValidator_->arrayValueIsValid(port->getArrayRight());
    }
    else if (index.column() == typeColumn())
    {
        return portValidator_->hasValidTypes(port);
    }
    else
    {
        return true;
    }
}

//-----------------------------------------------------------------------------
// Function: portsmodel::getAllReferencesToIdInItemOnRow()
//-----------------------------------------------------------------------------
int PortsModel::getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const
{
    QSharedPointer<Port> port = model_->getPorts()->at(row);

    int referencesInArrayLeft = port->getArrayLeft().count(valueID);
    int referencesInArrayRight = port->getArrayRight().count(valueID);

    int  totalReferences = referencesInArrayLeft + referencesInArrayRight;
    return totalReferences;
}

//-----------------------------------------------------------------------------
// Function: portsmodel::getPortAtIndex()
//-----------------------------------------------------------------------------
QSharedPointer<Port> PortsModel::getPortAtIndex(QModelIndex const& index) const
{
    return portOnRow(index.row());
}

//-----------------------------------------------------------------------------
// Function: portsmodel::getValidator()
//-----------------------------------------------------------------------------
QSharedPointer<PortValidator> PortsModel::getValidator() const
{
    return portValidator_;
}
