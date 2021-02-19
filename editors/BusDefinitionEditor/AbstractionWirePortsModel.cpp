//-----------------------------------------------------------------------------
// File: AbstractionWirePortsModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 21.6.2011
//
// Description:
// Data model for the wires within abstraction definition.
//-----------------------------------------------------------------------------

#include "AbstractionWirePortsModel.h"
#include "LogicalPortColumns.h"

#include <IPXACTmodels/BusDefinition/BusDefinition.h>
#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/common/PresenceTypes.h>
#include <IPXACTmodels/utilities/BusDefinitionUtils.h>

#include <common/KactusColors.h>

#include <editors/BusDefinitionEditor/interfaces/PortAbstractionInterface.h>

#include <QStringList>
#include <QIcon>

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::AbstractionWirePortsModel()
//-----------------------------------------------------------------------------
AbstractionWirePortsModel::AbstractionWirePortsModel(LibraryInterface* libraryAccess,
    PortAbstractionInterface* portInterface, QObject *parent):
QAbstractTableModel(parent),
busDefinition_(),
portInterface_(portInterface),
libraryAccess_(libraryAccess)
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::rowCount()
//-----------------------------------------------------------------------------
int AbstractionWirePortsModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
    }

    return portInterface_->itemCount();
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::columnCount()
//-----------------------------------------------------------------------------
int AbstractionWirePortsModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
    }

	return LogicalPortColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags AbstractionWirePortsModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    // Do not allow setting system groups for other than system mode.
    if (index.column() == LogicalPortColumns::SYSTEM_GROUP &&
        portInterface_->getMode(index.row()) != General::SYSTEM &&
        portInterface_->getSystemGroup(index.row()).empty())
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
    
    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::headerData()
//-----------------------------------------------------------------------------
QVariant AbstractionWirePortsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        return QVariant();
    }

    if (section == LogicalPortColumns::NAME)
    {		   
        return tr("Name");
    }
    else if (section == LogicalPortColumns::QUALIFIER)
    {
        return tr("Qualifier");
    }
    else if (section == LogicalPortColumns::WIDTH)
    {
        return tr("Width");
    }
    else if (section == LogicalPortColumns::DIRECTION)
    {
        return tr("Direction");
    }
    else if (section == LogicalPortColumns::MODE)
    {
        return tr("Mode");
    }
    else if (section == LogicalPortColumns::SYSTEM_GROUP)
    {
        return tr("System group");
    }
    else if (section == LogicalPortColumns::PRESENCE)
    {
        return tr("Presence");
    }
    else if (section == LogicalPortColumns::DEFAULT_VALUE)
    {
        return tr("Default value");
    }
    else if (section == LogicalPortColumns::DRIVER)
    {
        return tr("Driver");
    }
    else if (section == LogicalPortColumns::DESCRIPTION)
    {
        return tr("Description");
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::data()
//-----------------------------------------------------------------------------
QVariant AbstractionWirePortsModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= portInterface_->itemCount())
    {
		return QVariant();
    }
            
	if (role == Qt::DisplayRole) 
    {
        if (index.column() == LogicalPortColumns::NAME) 
        {
            QString logicalName = QString::fromStdString(portInterface_->getIndexedItemName(index.row()));
            if (logicalName.isEmpty())
            {
                return "unnamed";
            }
            else
            {
                return logicalName;
            }
        }
        else if (index.column() == LogicalPortColumns::QUALIFIER)
        {
            return QString::fromStdString(portInterface_->getQualifierString(index.row()));
        }
        else if (index.column() == LogicalPortColumns::WIDTH)
        {
            return QString::fromStdString(portInterface_->getWidth(index.row()));
        }
        else if (index.column() == LogicalPortColumns::DIRECTION)
        {
            return QString::fromStdString(portInterface_->getDirectionString(index.row()));
        }
        else if (index.column() == LogicalPortColumns::MODE)
        {
            return QString::fromStdString(portInterface_->getModeString(index.row()));
        }
        else if (index.column() == LogicalPortColumns::SYSTEM_GROUP)
        {
            return QString::fromStdString(portInterface_->getSystemGroup(index.row()));
        }
        else if (index.column() == LogicalPortColumns::PRESENCE)
        {
            return QString::fromStdString(portInterface_->getPresenceString(index.row()));
        }
        else if (index.column() == LogicalPortColumns::DEFAULT_VALUE)
        {
            return QString::fromStdString(portInterface_->getDefaultValue(index.row()));
        }
        else if (index.column() == LogicalPortColumns::DRIVER)
        {
            return QString::fromStdString(portInterface_->getDriverString(index.row()));
        }
        else if (index.column() == LogicalPortColumns::DESCRIPTION)
        {
            std::string portName = portInterface_->getIndexedItemName(index.row());
            return QString::fromStdString(portInterface_->getDescription(portName));
        }
        else
        {
            return QVariant();
        }
    }
    else if (role == Qt::ForegroundRole)
    {
        std::string portName = portInterface_->getIndexedItemName(index.row());
        General::InterfaceMode portMode = portInterface_->getMode(index.row());
        std::string portModeString = portInterface_->getModeString(index.row());

        if ((index.column() == LogicalPortColumns::NAME && QString::fromStdString(portName).isEmpty()) ||
            (index.column() == LogicalPortColumns::MODE &&
            (portMode == General::INTERFACE_MODE_COUNT ||
                portInterface_->portHasMultiplesOfMasterOrSlave(portName, portModeString))))
        {
            return  KactusColors::ERROR;
        }
        else if (index.column() == LogicalPortColumns::SYSTEM_GROUP)
        {
            QString systemGroup = QString::fromStdString(portInterface_->getSystemGroup(index.row()));
            if (!busDefinition_ || (portMode == General::SYSTEM &&
                !BusDefinitionUtils::getSystemGroups(busDefinition_, libraryAccess_).contains(systemGroup)))
            {
                return  KactusColors::ERROR;
            }
        }
        else
        {
            return QVariant();
        }
    }
    else if (role == Qt::DecorationRole && index.column() == LogicalPortColumns::DIRECTION)
    {
        std::string iconPath = portInterface_->getIconPathForSignal(index.row());
        if (!iconPath.empty())
        {
            return QIcon(QString::fromStdString(iconPath));
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::setData()
//-----------------------------------------------------------------------------
bool AbstractionWirePortsModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
    QString oldData = data(index, Qt::DisplayRole).toString();

    if (!index.isValid() || index.row() < 0 || index.row() >= portInterface_->itemCount() ||
        role != Qt::EditRole || oldData.compare(value.toString()) == 0)
    {
        return false;
    }

    std::string portName = portInterface_->getIndexedItemName(index.row());
    if (index.column() == LogicalPortColumns::NAME)
    {
        QString newName = value.toString();

        portInterface_->setName(portName, newName.toStdString());
    }
    else if (index.column() == LogicalPortColumns::QUALIFIER)
    {
        portInterface_->setQualifier(index.row(), value.toString().toStdString());
    }
    else if (index.column() == LogicalPortColumns::WIDTH)
    {
        portInterface_->setWidth(index.row(), value.toString().toStdString());
    }
    else if (index.column() == LogicalPortColumns::DEFAULT_VALUE)
    {
        portInterface_->setDefaultValue(index.row(), value.toString().toStdString());
    }
    else if (index.column() == LogicalPortColumns::MODE)
    {
        portInterface_->setMode(index.row(), value.toString().toStdString());
    }
    else if (index.column() == LogicalPortColumns::SYSTEM_GROUP)
    {
        portInterface_->setSystemGroup(index.row(), value.toString().toStdString());
    }
    else if (index.column() == LogicalPortColumns::DIRECTION)
    {
        portInterface_->setDirection(index.row(), value.toString().toStdString());
    }
    else if (index.column() == LogicalPortColumns::PRESENCE)
    {
        portInterface_->setPresence(index.row(), value.toString().toStdString());
    }
    else if (index.column() == LogicalPortColumns::DRIVER)
    {
        portInterface_->setDriverType(index.row(), value.toString().toStdString());
    }
    else if (index.column() == LogicalPortColumns::DESCRIPTION)
    {
        portInterface_->setDescription(portName, value.toString().toStdString());
    }
    else
    {
        return false;
    }

    //! Indexing doesn't work correctly (select other than first index).
    QModelIndexList indexList;
    indexList.append(index);

    if (index.column() == LogicalPortColumns::NAME || index.column() == LogicalPortColumns::DEFAULT_VALUE ||
        index.column() == LogicalPortColumns::DRIVER || index.column() == LogicalPortColumns::QUALIFIER ||
        index.column() == LogicalPortColumns::DESCRIPTION)
    {
        QString portNameQ = QString::fromStdString(portName);
        for (int i = 0; i < portInterface_->itemCount(); ++i)
        {
            QString comparisonName = QString::fromStdString(portInterface_->getIndexedItemName(i));
            if (index.row() != i && portInterface_->portIsWire(portNameQ.toStdString()) &&
                portInterface_->portIsWire(comparisonName.toStdString()) &&
                portNameQ.compare(comparisonName) == 0)
            {
                QModelIndex signalIndex = index.sibling(i, index.column());
                indexList.append(signalIndex);
            }
        }
    }

    sendDataChangeForAllChangedItems(indexList);
    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::sendDataChangeForAllChangedItems()
//-----------------------------------------------------------------------------
void AbstractionWirePortsModel::sendDataChangeForAllChangedItems(QModelIndexList changedIndexes)
{
    QModelIndex firstIndex = changedIndexes.first();
    QModelIndex lastIndex = changedIndexes.last();

    foreach (QModelIndex currentIndex, changedIndexes)
    {
        if (currentIndex.row() < firstIndex.row())
        {
            firstIndex = currentIndex;
        }
        else if (currentIndex.row() > lastIndex.row())
        {
            lastIndex = currentIndex;
        }
    }

    emit dataChanged(firstIndex, lastIndex);
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::resetPortModel()
//-----------------------------------------------------------------------------
void AbstractionWirePortsModel::resetPortModel()
{
    beginResetModel();
    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::setBusDef()
//-----------------------------------------------------------------------------
void AbstractionWirePortsModel::setBusDef(QSharedPointer<BusDefinition> busDefinition)
{
    busDefinition_ = busDefinition;
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::addSignal()
//-----------------------------------------------------------------------------
void AbstractionWirePortsModel::addSignal()
{
    beginInsertRows(QModelIndex(), portInterface_->itemCount(), portInterface_->itemCount());

    portInterface_->addWirePort();

    endInsertRows();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::addMaster()
//-----------------------------------------------------------------------------
void AbstractionWirePortsModel::addMaster(QModelIndexList const& indexes)
{
    createNewSignal(General::MASTER, indexes);
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::addSlave()
//-----------------------------------------------------------------------------
void AbstractionWirePortsModel::addSlave(QModelIndexList const& indexes)
{
    createNewSignal(General::SLAVE, indexes);
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::createNewSignal()
//-----------------------------------------------------------------------------
void AbstractionWirePortsModel::createNewSignal(General::InterfaceMode newSignalMode,
    QModelIndexList const& selection)
{
    QVector<int> targetSignals = getSelectedSignalRows(selection);
    QVector<QString> visitedPorts;

    beginResetModel();

    for (auto signalIndex : targetSignals)
    {
        std::string portName = portInterface_->getIndexedItemName(signalIndex);
        QString portNameQ = QString::fromStdString(portName);
        if (!visitedPorts.contains(portNameQ))
        {
            visitedPorts.append(portNameQ);
            portInterface_->addModeSpecificWireSignal(portName, newSignalMode);
        }
    }

    endResetModel();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::getSelectedSignalRows()
//-----------------------------------------------------------------------------
QVector<int> AbstractionWirePortsModel::getSelectedSignalRows(QModelIndexList const& selection)
{
    QVector<int> targetSignalRows;

    // Find all ports that match given indexes.
    for(auto index : selection)
    {
        int indexRow = index.row();
        if (0 <= indexRow && indexRow <= portInterface_->itemCount())
        {
            if (!targetSignalRows.contains(indexRow))
            {
                targetSignalRows.append(indexRow);
            }
        }
    }

    return targetSignalRows;
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::addSystem()
//-----------------------------------------------------------------------------
void AbstractionWirePortsModel::addSystem(QModelIndexList const& indexes)
{
    createNewSignal(General::SYSTEM, indexes);
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::addAllSystems()
//-----------------------------------------------------------------------------
void AbstractionWirePortsModel::addAllSystems(QModelIndexList const& indexes)
{
    QVector<int> targetSignals = getSelectedSignalRows(indexes);

    beginResetModel();

    for (auto signalIndex : targetSignals)
    {
        std::string portName = portInterface_->getIndexedItemName(signalIndex);
        for (auto group : getMissingSystemGroupsForSignal(signalIndex))
        {
            portInterface_->addWireSystemSignal(portName, group.toStdString());
        }
    }

    endResetModel();
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::getMissingSystemGroupsForSignal()
//-----------------------------------------------------------------------------
QStringList AbstractionWirePortsModel::getMissingSystemGroupsForSignal(int const& signalIndex) const
{
    QStringList missingSystemGroups;
    if (busDefinition_)
    {
        missingSystemGroups = BusDefinitionUtils::getSystemGroups(busDefinition_, libraryAccess_);
    }

    for (auto currentSystem :
        portInterface_->getSystemGroupsForPort(portInterface_->getIndexedItemName(signalIndex)))
    {
        missingSystemGroups.removeAll(QString::fromStdString(currentSystem));
    }

    return missingSystemGroups;
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::onRemoveItem()
//-----------------------------------------------------------------------------
void AbstractionWirePortsModel::onRemoveItem(QModelIndex const& index)
{
    int indexRow = index.row();
    if (0 <= index.row() && index.row() <= portInterface_->itemCount())
    {
        QString removedName = QString::fromStdString(portInterface_->getIndexedItemName(indexRow));
        General::InterfaceMode removedMode = portInterface_->getMode(indexRow);

        beginRemoveRows(QModelIndex(), index.row(), index.row());

        portInterface_->removeSignal(indexRow);

        endRemoveRows();

        emit contentChanged();
        emit portRemoved(removedName, removedMode);      
    }
}
