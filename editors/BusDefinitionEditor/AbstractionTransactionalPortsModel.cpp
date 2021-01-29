//-----------------------------------------------------------------------------
// File: AbstractionTransactionalPortsModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 05.06.2018
//
// Description:
// Data model for the transactional ports within abstraction definition.
//-----------------------------------------------------------------------------

#include "AbstractionTransactionalPortsModel.h"

#include <common/KactusColors.h>

#include <editors/BusDefinitionEditor/AbstractionTransactionalPortColumns.h>
#include <editors/BusDefinitionEditor/interfaces/PortAbstractionInterface.h>

#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/common/PresenceTypes.h>
#include <IPXACTmodels/common/Protocol.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <IPXACTmodels/utilities/BusDefinitionUtils.h>

#include <QStringList>

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::AbstractionTransactionalPortsModel()
//-----------------------------------------------------------------------------
AbstractionTransactionalPortsModel::AbstractionTransactionalPortsModel(LibraryInterface* libraryAccess,
    PortAbstractionInterface* portInterface, QObject *parent):
QAbstractTableModel(parent),
busDefinition_(),
portInterface_(portInterface),
libraryAccess_(libraryAccess)
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::rowCount()
//-----------------------------------------------------------------------------
int AbstractionTransactionalPortsModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return portInterface_->itemCount();
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::columnCount()
//-----------------------------------------------------------------------------
int AbstractionTransactionalPortsModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
    }

	return AbstractionTransactionalPortColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags AbstractionTransactionalPortsModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    // Do not allow setting system groups for other than system mode.
    if (index.column() == AbstractionTransactionalPortColumns::SYSTEM_GROUP &&
        portInterface_->getMode(index.row()) != General::SYSTEM &&
        portInterface_->getSystemGroup(index.row()).empty())
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
    else
    {
        return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::headerData()
//-----------------------------------------------------------------------------
QVariant AbstractionTransactionalPortsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        return QVariant();
    }
    if (section == AbstractionTransactionalPortColumns::NAME)
    {
        return tr("Name");
    }
    else if (section == AbstractionTransactionalPortColumns::MODE)
    {
        return tr("Mode");
    }
    else if (section == AbstractionTransactionalPortColumns::PRESENCE)
    {
        return tr("Presence");
    }
    else if (section == AbstractionTransactionalPortColumns::INITIATIVE)
    {
        return tr("Initiative");
    }
    else if (section == AbstractionTransactionalPortColumns::KIND)
    {
        return tr("Kind");
    }
    else if (section == AbstractionTransactionalPortColumns::BUSWIDTH)
    {
        return tr("Bus width");
    }
    else if (section == AbstractionTransactionalPortColumns::SYSTEM_GROUP)
    {
        return tr("System\ngroup");
    }
    else if (section == AbstractionTransactionalPortColumns::QUALIFIER)
    {
        return tr("Qualifier");
    }
    else if (section == AbstractionTransactionalPortColumns::PROTOCOLTYPE)
    {
        return tr("Protocol\ntype");
    }
    else if (section == AbstractionTransactionalPortColumns::PAYLOADNAME)
    {
        return tr("Payload\nname");
    }
    else if (section == AbstractionTransactionalPortColumns::PAYLOADTYPE)
    {
        return tr("Payload\ntype");
    }
    else if (section == AbstractionTransactionalPortColumns::PAYLOADEXTENSION)
    {
        return tr("Payload\nextension");
    }
    else if (section == AbstractionTransactionalPortColumns::DESCRIPTION)
    {
        return tr("Description");
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::data()
//-----------------------------------------------------------------------------
QVariant AbstractionTransactionalPortsModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= portInterface_->itemCount())
    {
		return QVariant();
    }
            
    std::string portName = portInterface_->getIndexedItemName(index.row());

	if (role == Qt::DisplayRole) 
    {
        if (index.column() == AbstractionTransactionalPortColumns::NAME) 
        {
            if (portName.empty())
            {
                return "unnamed";
            }
            else
            {
                return QString::fromStdString(portName);
            }
        }
        else if (index.column() == AbstractionTransactionalPortColumns::MODE)
        {
            return QString::fromStdString(portInterface_->getModeString(index.row()));
        }
        else if (index.column() == AbstractionTransactionalPortColumns::PRESENCE)
        {
            return QString::fromStdString(portInterface_->getPresenceString(index.row()));
        }
        else if (index.column() == AbstractionTransactionalPortColumns::INITIATIVE)
        {
            return QString::fromStdString(portInterface_->getInitiative(index.row()));
        }
        else if (index.column() == AbstractionTransactionalPortColumns::KIND)
        {
            return QString::fromStdString(portInterface_->getKind(index.row()));
        }
        else if (index.column() == AbstractionTransactionalPortColumns::BUSWIDTH)
        {
            return QString::fromStdString(portInterface_->getBusWidthValue(index.row()));
        }
        else if (index.column() == AbstractionTransactionalPortColumns::SYSTEM_GROUP)
        {
            return QString::fromStdString(portInterface_->getSystemGroup(index.row()));
        }
        else if (index.column() == AbstractionTransactionalPortColumns::QUALIFIER)
        {
            return QString::fromStdString(portInterface_->getQualifierString(index.row()));
        }
        else if ((index.column() == AbstractionTransactionalPortColumns::PROTOCOLTYPE ||
            index.column() == AbstractionTransactionalPortColumns::PAYLOADNAME ||
            index.column() == AbstractionTransactionalPortColumns::PAYLOADTYPE ||
            index.column() == AbstractionTransactionalPortColumns::PAYLOADEXTENSION) &&
            portInterface_->hasProtocol(index.row()))
        {
            if (index.column() == AbstractionTransactionalPortColumns::PROTOCOLTYPE)
            {
                return QString::fromStdString(portInterface_->getProtocolType(index.row()));
            }
            else if (index.column() == AbstractionTransactionalPortColumns::PAYLOADNAME)
            {
                return QString::fromStdString(portInterface_->getPayloadName(index.row()));
            }
            else if (index.column() == AbstractionTransactionalPortColumns::PAYLOADTYPE)
            {
                return QString::fromStdString(portInterface_->getPayloadType(index.row()));
            }
            else if (index.column() == AbstractionTransactionalPortColumns::PAYLOADEXTENSION)
            {
                return QString::fromStdString(portInterface_->getPayloadExtension(index.row()));
            }
        }

        else if (index.column() == AbstractionTransactionalPortColumns::DESCRIPTION)
        {
            return QString::fromStdString(portInterface_->getDescription(portName));
        }
    }
    else if (role == Qt::ForegroundRole)
    {
        std::string portModeString = portInterface_->getModeString(index.row());

        if ((index.column() == AbstractionTransactionalPortColumns::NAME && portName.empty()) ||
            (index.column() == AbstractionTransactionalPortColumns::MODE &&
                (portInterface_->getMode(index.row()) == General::INTERFACE_MODE_COUNT ||
                portInterface_->portHasMultiplesOfMasterOrSlave(portName, portModeString))))
        {
            return KactusColors::ERROR;
        }
        else if (index.column() == AbstractionTransactionalPortColumns::SYSTEM_GROUP)
        {
            QString system = QString::fromStdString(portInterface_->getSystemGroup(index.row()));
            if (!busDefinition_ || (portInterface_->getMode(index.row()) == General::SYSTEM &&
                !BusDefinitionUtils::getSystemGroups(busDefinition_, libraryAccess_).contains(system)))
            {
                return KactusColors::ERROR;
            }
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::setData()
//-----------------------------------------------------------------------------
bool AbstractionTransactionalPortsModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
    QString oldData = data(index, Qt::DisplayRole).toString();

    if (!index.isValid() || index.row() < 0 || index.row() >= portInterface_->itemCount() ||
        role != Qt::EditRole || oldData.compare(value.toString()) == 0)
    {
        return false;
    }

    std::string portName = portInterface_->getIndexedItemName(index.row());
    if (index.column() == AbstractionTransactionalPortColumns::NAME)
    {
        portInterface_->setName(portName, value.toString().toStdString());

    }
    else if (index.column() == AbstractionTransactionalPortColumns::MODE)
    {
        portInterface_->setMode(index.row(), value.toString().toStdString());
    }
    else if (index.column() == AbstractionTransactionalPortColumns::PRESENCE)
    {
        portInterface_->setPresence(index.row(), value.toString().toStdString());
    }
    else if (index.column() == AbstractionTransactionalPortColumns::INITIATIVE)
    {
        portInterface_->setInitiative(index.row(), value.toString().toStdString());
    }
    else if (index.column() == AbstractionTransactionalPortColumns::KIND)
    {
        portInterface_->setKind(index.row(), value.toString().toStdString());
    }
    else if (index.column() == AbstractionTransactionalPortColumns::BUSWIDTH)
    {
        portInterface_->setBusWidth(index.row(), value.toString().toStdString());
    }
    else if (index.column() == AbstractionTransactionalPortColumns::SYSTEM_GROUP)
    {
        portInterface_->setSystemGroup(index.row(), value.toString().toStdString());
    }
    else if (index.column() == AbstractionTransactionalPortColumns::QUALIFIER)
    {
        portInterface_->setQualifier(index.row(), value.toString().toStdString());
    }
    else if (index.column() == AbstractionTransactionalPortColumns::PROTOCOLTYPE)
    {
        portInterface_->setProtocolType(index.row(), value.toString().toStdString());
    }
    else if (index.column() == AbstractionTransactionalPortColumns::PAYLOADNAME)
    {
        portInterface_->setPayloadName(index.row(), value.toString().toStdString());
    }
    else if (index.column() == AbstractionTransactionalPortColumns::PAYLOADTYPE)
    {
        portInterface_->setPayloadType(index.row(), value.toString().toStdString());
    }
    else if (index.column() == AbstractionTransactionalPortColumns::PAYLOADEXTENSION)
    {
        portInterface_->setPayloadExtension(index.row(), value.toString().toStdString());
    }
    else if (index.column() == AbstractionTransactionalPortColumns::DESCRIPTION)
    {
        portInterface_->setDescription(portName, value.toString().toStdString());
    }
    else
    {
        return false;
    }

    QModelIndexList indexList;
    indexList.append(index);

    if (index.column() == AbstractionTransactionalPortColumns::QUALIFIER ||
        index.column() == AbstractionTransactionalPortColumns::DESCRIPTION)
    {
        for (int i = 0; i < portInterface_->itemCount(); ++i)
        {
            bool isNotSameRow = i != index.row();
            bool isSamePort = portName == portInterface_->getIndexedItemName(i);
            if (isNotSameRow && isSamePort)
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
// Function: AbstractionTransactionalPortsModel::sendDataChangeForAllChangedItems()
//-----------------------------------------------------------------------------
void AbstractionTransactionalPortsModel::sendDataChangeForAllChangedItems(QModelIndexList changedIndexes)
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
// Function: AbstractionTransactionalPortsModel::resetPortModel()
//-----------------------------------------------------------------------------
void AbstractionTransactionalPortsModel::resetPortModel()
{
    beginResetModel();
    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::setBusDef()
//-----------------------------------------------------------------------------
void AbstractionTransactionalPortsModel::setBusDef(QSharedPointer<BusDefinition> busDefinition)
{
    busDefinition_ = busDefinition;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::addSignal()
//-----------------------------------------------------------------------------
void AbstractionTransactionalPortsModel::addSignal()
{
    beginInsertRows(QModelIndex(), portInterface_->itemCount(), portInterface_->itemCount());

    portInterface_->addTransactionalPort();

    endInsertRows();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::addMaster()
//-----------------------------------------------------------------------------
void AbstractionTransactionalPortsModel::addMaster(QModelIndexList const& indexes)
{
    createNewSignal(General::MASTER, indexes);
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::addSlave()
//-----------------------------------------------------------------------------
void AbstractionTransactionalPortsModel::addSlave(QModelIndexList const& indexes)
{
    createNewSignal(General::SLAVE, indexes);
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::createNewSignal()
//-----------------------------------------------------------------------------
void AbstractionTransactionalPortsModel::createNewSignal(General::InterfaceMode newSignalMode,
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
            portInterface_->addModeSpecificTransactionalSignal(portName, newSignalMode);
        }
    }

    endResetModel();
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::getSelectedSignalRows()
//-----------------------------------------------------------------------------
QVector<int> AbstractionTransactionalPortsModel::getSelectedSignalRows(QModelIndexList const& selection)
{
    QVector<int> targetSignalRows;

    for (auto index : selection)
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
// Function: AbstractionTransactionalPortsModel::addSystem()
//-----------------------------------------------------------------------------
void AbstractionTransactionalPortsModel::addSystem(QModelIndexList const& indexes)
{
    createNewSignal(General::SYSTEM, indexes);
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::addAllSystems()
//-----------------------------------------------------------------------------
void AbstractionTransactionalPortsModel::addAllSystems(QModelIndexList const& indexes)
{
    QVector<int> targetSignals = getSelectedSignalRows(indexes);

    beginResetModel();

    for (auto signalIndex : targetSignals)
    {
        std::string portName = portInterface_->getIndexedItemName(signalIndex);
        for (auto group : getMissingSystemGroupsForSignal(signalIndex))
        {
            portInterface_->addTransactionalSystemSignal(portName, group.toStdString());
        }
    }

    endResetModel();
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::getMissingSystemGroupsForSignal()
//-----------------------------------------------------------------------------
QStringList AbstractionTransactionalPortsModel::getMissingSystemGroupsForSignal(int const& signalIndex) const
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
// Function: AbstractionTransactionalPortsModel::onRemoveItem()
//-----------------------------------------------------------------------------
void AbstractionTransactionalPortsModel::onRemoveItem(QModelIndex const& index)
{
    if (0 <= index.row() && index.row() <= portInterface_->itemCount())
    {
        QString removedPortName = QString::fromStdString(portInterface_->getIndexedItemName(index.row()));
        General::InterfaceMode signalMode = portInterface_->getMode(index.row());

        beginRemoveRows(QModelIndex(), index.row(), index.row());
        portInterface_->removeSignal(index.row());
        endRemoveRows();

        emit contentChanged();
        emit portRemoved(removedPortName, signalMode);
    }
}
