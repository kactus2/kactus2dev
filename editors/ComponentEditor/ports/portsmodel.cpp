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

#include <KactusAPI/include/PortAbstractionInterface.h>
#include <KactusAPI/include/PortsInterface.h>

#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/common/TransactionalTypes.h>

#include <QString>
#include <QStringList>
#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: PortsModel::PortsModel()
//-----------------------------------------------------------------------------
PortsModel::PortsModel(QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<PortsInterface> portInterface, QSharedPointer<PortAbstractionInterface> signalInterface,
    QObject *parent):
ReferencingTableModel(parameterFinder, parent),
ParameterizableTable(parameterFinder),
portsInterface_(portInterface),
signalInterface_(signalInterface)
{
    Q_ASSERT(portInterface);
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

    return portsInterface_->itemCount();
}

//-----------------------------------------------------------------------------
// Function: PortsModel::data()
//-----------------------------------------------------------------------------
QVariant PortsModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= portsInterface_->itemCount())
    {
		return QVariant();
	}

    if (role == Qt::DisplayRole) 
    {
        if (isValidExpressionColumn(index))
        {
            return formattedExpressionForIndex(index);
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
        if (isValidExpressionColumn(index))
        {
            return expressionForIndex(index);
        }
        else
        {
            return valueForIndex(index);
        }
    }
    else if (role == Qt::ToolTipRole)
    {
        return valueForIndex(index);
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
        if (indexedItemIsDisabled(index))
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
            std::string portName = portsInterface_->getIndexedItemName(index.row());
            if (portsInterface_->isAdHoc(portName))
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
        return int(Qt::AlignRight | Qt::AlignVCenter);
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
            return QString::fromStdString(portsInterface_->getIndexedItemName(section));
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
    if (!index.isValid() || index.row() < 0 || index.row() >= portsInterface_->itemCount())
    {
		return false;
    }

    std::string portName = portsInterface_->getIndexedItemName(index.row());
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
            portsInterface_->setName(portName, value.toString().toStdString());
            emit headerDataChanged(Qt::Vertical, index.row(), index.row());
            emit portExtensionDataChanged(index);
        }
        else if (index.column() == typeColumn())
        {
            portsInterface_->setTypeName(portName, value.toString().toStdString());
        }
        else if (index.column() == arrayLeftColumn())
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(
                    QString::fromStdString(portsInterface_->getArrayLeftExpression(portName)));
            }

            portsInterface_->setArrayLeft(portName, value.toString().toStdString());

            if (value.isValid() && portsInterface_->getArrayRightExpression(portName).empty() &&
                !portsInterface_->getArrayLeftExpression(portName).empty())
            {
                portsInterface_->setArrayRight(portName, QString::number(0).toStdString());
            }
        }
        else if (index.column() == arrayRightColum())
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(
                    QString::fromStdString(portsInterface_->getArrayRightExpression(portName)));
            }

            portsInterface_->setArrayRight(portName, value.toString().toStdString());

            if (value.isValid() && portsInterface_->getArrayLeftExpression(portName).empty() &&
                !portsInterface_->getArrayRightExpression(portName).empty())
            {
                portsInterface_->setArrayLeft(portName, std::to_string(0));
            }
        }
        else if (index.column() == tagColumn())
        {
            portsInterface_->setTags(portName, value.toString().toStdString());

            emit portExtensionDataChanged(index);
        }
        else if (index.column() == adHocColumn())
        {
            portsInterface_->setAdHoc(portName, value.toBool());
        }
        else if (index.column() == descriptionColumn())
        {
            portsInterface_->setDescription(portName, value.toString().toStdString());
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
        portsInterface_->setAdHoc(portName, value == Qt::Checked);
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
    return portsInterface_->validateItems();
}

//-----------------------------------------------------------------------------
// Function: PortsModel::onRemoveRow()
//-----------------------------------------------------------------------------
void PortsModel::onRemoveRow(int row)
{
	// if row is invalid
    if (row < 0 || row >= portsInterface_->itemCount() || rowIsLocked(row))
    {
		return;
    }

    std::string portName(portsInterface_->getIndexedItemName(row));

    beginRemoveRows(QModelIndex(), row, row);
    portsInterface_->removePort(portName);

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
    if (!index.isValid() || index.row() < 0 || index.row() >= portsInterface_->itemCount() ||
        rowIsLocked(index.row()))
    {
		return;
	}

    int portRow = index.row();
    std::string portName(portsInterface_->getIndexedItemName(portRow));

    beginRemoveRows(QModelIndex(), portRow, portRow);

    removeReferencesInItemOnRow(portRow);
    portsInterface_->removePort(portName);

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
    int lastRow = portsInterface_->itemCount();

 	beginInsertRows(QModelIndex(), lastRow, lastRow);

    addNewPort();

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
    int row = portsInterface_->itemCount();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
    addNewPort();
    endInsertRows();

    emit invalidateOtherFilter();
    emit portCountChanged();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: PortsModel::index()
//-----------------------------------------------------------------------------
QModelIndex PortsModel::index(QString const& portName) const
{
	// find the correct row
    int row = portsInterface_->getItemIndex(portName.toStdString());

	// if the named port is not found
	if (row < 0)
    {
		return QModelIndex();
	}

	// the base class creates the index for the row
	return QAbstractTableModel::index(row, 0, QModelIndex());
}

//-----------------------------------------------------------------------------
// Function: portsmodel::resetModelAndLockCurrentPorts()
//-----------------------------------------------------------------------------
void PortsModel::resetModelAndLockCurrentPorts()
{
    beginResetModel();

    lockedIndexes_.clear();

    endResetModel();

    for (auto const& portName : getInterface()->getItemNames())
    {
        DirectionTypes::Direction portDirection = DirectionTypes::str2Direction(QString::fromStdString(
            portsInterface_->getDirection(portName)), DirectionTypes::DIRECTION_INVALID);
        if (portDirection != DirectionTypes::DIRECTION_PHANTOM)
        {
            lockPort(QString::fromStdString(portName));
        }
    }

    emit contentChanged();
    emit portCountChanged();
}

//-----------------------------------------------------------------------------
// Function: lockPort()
//-----------------------------------------------------------------------------
void PortsModel::lockPort(QString const& portName)
{
    QModelIndex portIndex = index(portName);
    QModelIndexList lockedPortIndexes = getLockedPortIndexes(portIndex);

    for (auto const& index : lockedPortIndexes)
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
void PortsModel::unlockPort(QString const& portName)
{
    QModelIndex portIndex = index(portName);
    QModelIndexList lockedPortIndexes = getLockedPortIndexes(portIndex);
    for (auto const& index : lockedPortIndexes)
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
bool PortsModel::rowIsLocked(int row) const
{
    QModelIndex nameIndex = QAbstractTableModel::index(row, nameColumn(), QModelIndex());
    return nameIndex.isValid() && isLocked(nameIndex);
}

//-----------------------------------------------------------------------------
// Function: portsmodel::formattedExpressionForIndex()
//-----------------------------------------------------------------------------
QVariant PortsModel::formattedExpressionForIndex(QModelIndex const& index) const
{
    std::string portName(portsInterface_->getIndexedItemName(index.row()));

    if (index.column() == arrayLeftColumn())
    {
        return QString::fromStdString(portsInterface_->getArrayLeftFormattedExpression(portName));
    }
    else if (index.column() == arrayRightColum())
    {
        return QString::fromStdString(portsInterface_->getArrayRightFormattedExpression(portName));
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: portsmodel::expressionForIndex()
//-----------------------------------------------------------------------------
QVariant PortsModel::expressionForIndex(QModelIndex const& index) const
{
    std::string portName(portsInterface_->getIndexedItemName(index.row()));

    if (index.column() == arrayLeftColumn())
    {
        return QString::fromStdString(portsInterface_->getArrayLeftExpression(portName));
    }
    else if (index.column() == arrayRightColum())
    {
        return QString::fromStdString(portsInterface_->getArrayRightExpression(portName));
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: portsmodel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant PortsModel::valueForIndex(QModelIndex const& index) const
{
    std::string portName = portsInterface_->getIndexedItemName(index.row());

    if (index.column() == nameColumn())
    {
        return QString::fromStdString(portName);
    }
    else if (index.column() == typeColumn())
    {
        return QString::fromStdString(portsInterface_->getTypeName(portName));
    }
    else if (index.column() == arrayLeftColumn())
    {
        return QString::fromStdString(portsInterface_->getArrayLeftValue(portName));
    }
    else if (index.column() == arrayRightColum())
    {
        return QString::fromStdString(portsInterface_->getArrayRightValue(portName));
    }
    else if (index.column() == tagColumn())
    {
        return QString::fromStdString(portsInterface_->getTags(portName));
    }
    else if (index.column() == adHocColumn())
    {
        return portsInterface_->isAdHoc(portName);
    }
    else if (index.column() == descriptionColumn())
    {
        return QString::fromStdString(portsInterface_->getDescription(portName));
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: portsmodel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant PortsModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    if (index.column() == arrayLeftColumn() || index.column() == arrayRightColum())
    {
        return expressionForIndex(index);
    }
    else
    {
        return valueForIndex(index);
    }
}

//-----------------------------------------------------------------------------
// Function: portsmodel::validateIndex()
//-----------------------------------------------------------------------------
bool PortsModel::validateIndex(QModelIndex const& index) const
{
    std::string portName(portsInterface_->getIndexedItemName(index.row()));

    if (index.column() == nameColumn())
    {
        return portsInterface_->itemHasValidName(portName);
    }
    else if (index.column() == arrayLeftColumn())
    {
        return portsInterface_->portLeftArrayValueIsValid(portName);
    }
    else if (index.column() == arrayRightColum())
    {
        return portsInterface_->portRightArrayValueIsValid(portName);
    }
    else if (index.column() == typeColumn())
    {
        return portsInterface_->portHasValidTypes(portName);
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
    return portsInterface_->getAllReferencesToIdInItem(
        portsInterface_->getIndexedItemName(row), valueID.toStdString());
}

//-----------------------------------------------------------------------------
// Function: portsmodel::getInterface()
//-----------------------------------------------------------------------------
QSharedPointer<PortsInterface> PortsModel::getInterface() const
{
    return portsInterface_;
}

//-----------------------------------------------------------------------------
// Function: portsmodel::getIconPath()
//-----------------------------------------------------------------------------
QString PortsModel::getIconPath(int const& portIndex) const
{
    std::string portName(getInterface()->getIndexedItemName(portIndex));
    std::string iconPath(getInterface()->getIconPathForPort(portName));
    return QString::fromStdString(iconPath);
}

//-----------------------------------------------------------------------------
// Function: portsmodel::onCreatePortsFromAbstraction()
//-----------------------------------------------------------------------------
void PortsModel::onCreatePortsFromAbstraction(QSharedPointer<const AbstractionDefinition> abstraction,
    General::InterfaceMode const& busMode, General::InterfaceMode const& busMonitorMode,
    QString const& systemGroup)
{
    beginResetModel();

    signalInterface_->setAbsDef(abstraction);
    General::InterfaceMode busInterfaceMode = busMode;
    if (busInterfaceMode == General::MONITOR)
    {
        busInterfaceMode = busMonitorMode;
    }

    for (int signalIndex = 0; signalIndex < signalInterface_->itemCount(); ++signalIndex)
    {
        if (modesMatch(busInterfaceMode, systemGroup, signalInterface_->getMode(signalIndex),
            QString::fromStdString(signalInterface_->getSystemGroup(signalIndex))))
        {
            std::string portName = signalInterface_->getIndexedItemName(signalIndex);

            if (signalInterface_->portIsWire(portName))
            {
                portsInterface_->addWirePort(portName);

                int portIndex = portsInterface_->itemCount() - 1;
                portName = portsInterface_->getIndexedItemName(portIndex);

                portsInterface_->setDirection(portName, DirectionTypes::direction2Str(getDirectionFromSignal(
                    busInterfaceMode, signalInterface_->getDirection(signalIndex))).toStdString());

                quint64 leftBound = 0;
                bool signalIntegerOk = false;
                quint64 signalWidth =
                    QString::fromStdString(signalInterface_->getWidth(signalIndex)).toULongLong(&signalIntegerOk);
                if (signalIntegerOk == true && signalWidth > 0)
                {
                    leftBound = signalWidth - 1;
                }

                portsInterface_->setLeftBound(portName, QString::number(leftBound).toStdString());
                portsInterface_->setRightBound(portName, "0");

                portsInterface_->setDefaultValue(portName, signalInterface_->getDefaultValue(signalIndex));
            }
            else if (signalInterface_->portIsTransactional(portName))
            {
                portsInterface_->addTransactionalPort(portName);

                int portIndex = portsInterface_->itemCount() - 1;
                portName = portsInterface_->getIndexedItemName(portIndex);

                portsInterface_->setInitiative(portName, getInitiativeFromSignal(busInterfaceMode,
                    QString::fromStdString(signalInterface_->getInitiative(signalIndex))).toStdString());
                portsInterface_->setKind(portName, signalInterface_->getKind(signalIndex));
                portsInterface_->setBusWidth(portName, signalInterface_->getBusWidthValue(signalIndex));
            }

            portsInterface_->setDescription(portName, signalInterface_->getDescription(portName));
        }
    }

    endResetModel();

    emit invalidateOtherFilter();
    emit portCountChanged();
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: portsmodel::modesMatch()
//-----------------------------------------------------------------------------
bool PortsModel::modesMatch(General::InterfaceMode const& busMode, QString const& busSystemGroup,
    General::InterfaceMode const& signalMode, QString const& signalSystemGroup) const
{
    return ((busMode == General::MASTER || busMode == General::MIRRORED_MASTER) && signalMode == General::MASTER) ||
        ((busMode == General::SLAVE || busMode == General::MIRRORED_SLAVE) && signalMode == General::SLAVE) ||
        ((busMode == General::SYSTEM || busMode == General::MIRRORED_SYSTEM) && signalMode == General::SYSTEM &&
            busSystemGroup == signalSystemGroup);
}

//-----------------------------------------------------------------------------
// Function: portsmodel::getSignalDirection()
//-----------------------------------------------------------------------------
DirectionTypes::Direction PortsModel::getDirectionFromSignal(General::InterfaceMode const& busMode,
    DirectionTypes::Direction const& signalDirection) const
{
    if (busMode == General::MIRRORED_MASTER || busMode == General::MIRRORED_SLAVE ||
        busMode == General::MIRRORED_SYSTEM)
    {
        return DirectionTypes::convert2Mirrored(signalDirection);
    }
    else
    {
        return signalDirection;
    }
}

//-----------------------------------------------------------------------------
// Function: portsmodel::getInitiativeFromSignal()
//-----------------------------------------------------------------------------
QString PortsModel::getInitiativeFromSignal(General::InterfaceMode const& busMode, QString const& signalInitiative)
const
{
    if (busMode == General::MIRRORED_MASTER || busMode == General::MIRRORED_SLAVE ||
        busMode == General::MIRRORED_SYSTEM)
    {
        return TransactionalTypes::initiativeToString(TransactionalTypes::convertToMirrored(signalInitiative));
    }
    else
    {
        return signalInitiative;
    }
}
