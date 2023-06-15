//-----------------------------------------------------------------------------
// File: AbstractionPortsModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 15.11.2019
//
// Description:
// Data model for the signals within abstraction definition.
//-----------------------------------------------------------------------------

#include "AbstractionPortsModel.h"
#include "LogicalPortColumns.h"

#include <IPXACTmodels/AbstractionDefinition/TransactionalAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/TransactionalPort.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WirePort.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/common/PresenceTypes.h>
#include <IPXACTmodels/common/Protocol.h>

#include <IPXACTmodels/utilities/BusDefinitionUtils.h>

#include <common/KactusColors.h>

#include <KactusAPI/include/PortAbstractionInterface.h>

#include <QStringList>
#include <QIcon>

namespace {

    const QList<LogicalPortColumns::Columns> extendLockedColumns({
        LogicalPortColumns::NAME,
        LogicalPortColumns::MODE,
        LogicalPortColumns::QUALIFIER,
        LogicalPortColumns::DIRECTION,
        LogicalPortColumns::INITIATIVE,
        LogicalPortColumns::KIND,
        LogicalPortColumns::BUSWIDTH,
        LogicalPortColumns::PROTOCOLTYPE,
        LogicalPortColumns::PAYLOADNAME,
        LogicalPortColumns::PAYLOADTYPE,
        LogicalPortColumns::PAYLOADEXTENSION,
        LogicalPortColumns::MATCH
        });
};

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::AbstractionPortsModel()
//-----------------------------------------------------------------------------
AbstractionPortsModel::AbstractionPortsModel(LibraryInterface* libraryAccess,
    PortAbstractionInterface* portInterface, 
    PortAbstractionInterface* extendInterface,
    QObject *parent):
QAbstractTableModel(parent),
busDefinition_(),
libraryAccess_(libraryAccess),
portInterface_(portInterface),
extendInterface_(extendInterface)
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::rowCount()
//-----------------------------------------------------------------------------
int AbstractionPortsModel::rowCount(QModelIndex const& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return portInterface_->itemCount();
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::columnCount()
//-----------------------------------------------------------------------------
int AbstractionPortsModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
    }

	return LogicalPortColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags AbstractionPortsModel::flags(const QModelIndex& index) const
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

    // Do not allow setting data defined in extended abstraction definition.
    if (isLocked(index))
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
    
    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::headerData()
//-----------------------------------------------------------------------------
QVariant AbstractionPortsModel::headerData(int section, Qt::Orientation orientation, int role) const
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
    else if (section == LogicalPortColumns::MATCH)
    {
        return tr("Port match");
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
    else if (section == LogicalPortColumns::INITIATIVE)
    {
        return tr("Initiative");
    }
    else if (section == LogicalPortColumns::KIND)
    {
        return tr("Kind");
    }
    else if (section == LogicalPortColumns::BUSWIDTH)
    {
        return tr("Bus width");
    }
    else if (section == LogicalPortColumns::PROTOCOLTYPE)
    {
        return tr("Protocol\ntype");
    }
    else if (section == LogicalPortColumns::PAYLOADNAME)
    {
        return tr("Payload\nname");
    }
    else if (section == LogicalPortColumns::PAYLOADTYPE)
    {
        return tr("Payload\ntype");
    }
    else if (section == LogicalPortColumns::PAYLOADEXTENSION)
    {
        return tr("Payload\nextension");
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::data()
//-----------------------------------------------------------------------------
QVariant AbstractionPortsModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= portInterface_->itemCount())
    {
		return QVariant();
    }
            
	if (role == Qt::DisplayRole || role == Qt::ToolTipRole)
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
            QStringList qualifierList;
            for (auto qualifier : portInterface_->getQualifierStringList(index.row()))
            {
                qualifierList.append(QString::fromStdString(qualifier));
            }

            return qualifierList.join(" ");
        }
        else if (index.column() == LogicalPortColumns::MATCH)
        {
            return General::bool2Str(portInterface_->getMatch(index.row()));
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
        else if (index.column() == LogicalPortColumns::KIND)
        {
            return QString::fromStdString(portInterface_->getKind(index.row()));
        }
        else if (index.column() == LogicalPortColumns::INITIATIVE)
        {
            return QString::fromStdString(portInterface_->getInitiative(index.row()));
        }
        else if (index.column() == LogicalPortColumns::BUSWIDTH)
        {
            return QString::fromStdString(portInterface_->getBusWidthValue(index.row()));
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
        else if ((index.column() == LogicalPortColumns::PROTOCOLTYPE ||
            index.column() == LogicalPortColumns::PAYLOADNAME ||
            index.column() == LogicalPortColumns::PAYLOADTYPE ||
            index.column() == LogicalPortColumns::PAYLOADEXTENSION) &&
            portInterface_->hasProtocol(index.row()))
        {
            if (index.column() == LogicalPortColumns::PROTOCOLTYPE)
            {
                return QString::fromStdString(portInterface_->getProtocolType(index.row()));
            }
            else if (index.column() == LogicalPortColumns::PAYLOADNAME)
            {
                return QString::fromStdString(portInterface_->getPayloadName(index.row()));
            }
            else if (index.column() == LogicalPortColumns::PAYLOADTYPE)
            {
                return QString::fromStdString(portInterface_->getPayloadType(index.row()));
            }
            else if (index.column() == LogicalPortColumns::PAYLOADEXTENSION)
            {
                return QString::fromStdString(portInterface_->getPayloadExtension(index.row()));
            }
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
        else if (isLocked(index))
        {
            return  KactusColors::DISABLED_TEXT;
        }
    }
    else if (role == Qt::DecorationRole && 
             (index.column() == LogicalPortColumns::DIRECTION || index.column() == LogicalPortColumns::INITIATIVE))
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
// Function: AbstractionPortsModel::setData()
//-----------------------------------------------------------------------------
bool AbstractionPortsModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
    QString newData = value.toString();
    QString oldData = data(index, Qt::DisplayRole).toString();

    if (!index.isValid() || index.row() < 0 || index.row() >= portInterface_->itemCount() || !(flags(index) & Qt::ItemIsEditable) || role != Qt::EditRole)
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
        QStringList listOfQualifiers = value.toStringList();

        std::vector<std::string> qualifierList;
        for (auto qualifier : value.toStringList())
        {
            qualifierList.push_back(qualifier.toStdString());
        }

        portInterface_->setQualifierStringList(index.row(), qualifierList);
    }
    else if (index.column() == LogicalPortColumns::MATCH)
    {
        portInterface_->setMatch(index.row(), value.toBool());
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
    else if (index.column() == LogicalPortColumns::INITIATIVE)
    {
        portInterface_->setInitiative(index.row(), value.toString().toStdString());
    }
    else if (index.column() == LogicalPortColumns::KIND)
    {
        portInterface_->setKind(index.row(), value.toString().toStdString());
    }
    else if (index.column() == LogicalPortColumns::BUSWIDTH)
    {
        portInterface_->setBusWidth(index.row(), value.toString().toStdString());
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
    else if (index.column() == LogicalPortColumns::PROTOCOLTYPE)
    {
        portInterface_->setProtocolType(index.row(), value.toString().toStdString());
    }
    else if (index.column() == LogicalPortColumns::PAYLOADNAME)
    {
        portInterface_->setPayloadName(index.row(), value.toString().toStdString());
    }
    else if (index.column() == LogicalPortColumns::PAYLOADTYPE)
    {
        portInterface_->setPayloadType(index.row(), value.toString().toStdString());
    }
    else if (index.column() == LogicalPortColumns::PAYLOADEXTENSION)
    {
        portInterface_->setPayloadExtension(index.row(), value.toString().toStdString());
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
        for (int i = 0; i < portInterface_->itemCount(); ++i)
        {
            std::string comparisonName = portInterface_->getIndexedItemName(i);
            if (index.row() != i && portInterface_->portIsWire(portName) &&
                portInterface_->portIsWire(comparisonName) &&
                portName == comparisonName)
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
// Function: AbstractionPortsModel::sendDataChangeForAllChangedItems()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::sendDataChangeForAllChangedItems(QModelIndexList changedIndexes)
{
    QModelIndex firstIndex = changedIndexes.first();
    QModelIndex lastIndex = changedIndexes.last();

    for (QModelIndex const& currentIndex : changedIndexes)
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
// Function: AbstractionPortsModel::resetPortModel()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::resetPortModel()
{
    beginResetModel();
    endResetModel();
}


//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::setExtendedPorts()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::setExtendedPorts()
{
    beginResetModel();
    for (auto const& port : extendInterface_->getItemNames())
    {
        if (extendInterface_->portIsWire(port))
        {
            extendWireMode(port, General::MASTER);
            extendWireMode(port, General::SLAVE);

            for (auto const& systemGroup : extendInterface_->getSystemGroupsForPort(port))
            {
                extendWireMode(port, General::SYSTEM, systemGroup);
            }
        }
        else if (extendInterface_->portIsTransactional(port))
        {
            extendTransactionalMode(port, General::MASTER);
            extendTransactionalMode(port, General::SLAVE);

            for (auto const& systemGroup : extendInterface_->getSystemGroupsForPort(port))
            {
                extendTransactionalMode(port, General::SYSTEM, systemGroup);
            }
        }
    }

    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::removeExtendedPorts()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::removeExtendedPorts()
{
    for (int i = portInterface_->itemCount() - 1; i >= 0; --i)
    {
        if (isExtendedRow(i))
        {
            portInterface_->removeSignal(i);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::onResetExtendPorts()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::onResetExtendPorts()
{
    setExtendedPorts();
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::setBusDef()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::setBusDef(QSharedPointer<BusDefinition> busDefinition)
{
    busDefinition_ = busDefinition;
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::addWireSignal()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::addWireSignal()
{
    int itemCount = portInterface_->itemCount();
    beginInsertRows(QModelIndex(), itemCount, itemCount);
    portInterface_->addWirePort();
    endInsertRows();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::addTransactionalSignal()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::addTransactionalSignal()
{
    int itemCount = portInterface_->itemCount();
    beginInsertRows(QModelIndex(), itemCount, itemCount);
    portInterface_->addTransactionalPort();
    endInsertRows();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::addMaster()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::addMaster(QModelIndexList const& indexes)
{
    createNewSignal(General::MASTER, indexes);
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::addSlave()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::addSlave(QModelIndexList const& indexes)
{
    createNewSignal(General::SLAVE, indexes);
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::createNewSignal()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::createNewSignal(General::InterfaceMode newSignalMode,
    QModelIndexList const& selection)
{
    QVector<int> targetSignals = getSelectedSignalRows(selection);
    QVector<std::string> visitedPorts;

    beginResetModel();

    for (auto signalIndex : targetSignals)
    {
        std::string portName = portInterface_->getIndexedItemName(signalIndex);        
        if (!visitedPorts.contains(portName))
        {
            visitedPorts.append(portName);
            if (portInterface_->portIsWire(portName))
            {
                portInterface_->addModeSpecificWireSignal(portName, newSignalMode);
            }
            else 
            {
                portInterface_->addModeSpecificTransactionalSignal(portName, newSignalMode);
            }
        }
    }

    endResetModel();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::getSelectedSignalRows()
//-----------------------------------------------------------------------------
QVector<int> AbstractionPortsModel::getSelectedSignalRows(QModelIndexList const& selection)
{
    QVector<int> targetSignalRows;

    // Find all ports that match given indexes.
    for(auto const& index : selection)
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
// Function: AbstractionPortsModel::addSystem()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::addSystem(QModelIndexList const& indexes)
{
    createNewSignal(General::SYSTEM, indexes);
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::addAllSystems()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::addAllSystems(QModelIndexList const& indexes)
{
    QVector<int> targetSignals = getSelectedSignalRows(indexes);

    beginResetModel();

    for (auto const& signalIndex : targetSignals)
    {
        std::string portName = portInterface_->getIndexedItemName(signalIndex);
        for (auto group : getMissingSystemGroupsForSignal(signalIndex))
        {
            if (portInterface_->portIsWire(portName))
            {
                portInterface_->addWireSystemSignal(portName, group.toStdString());
            }
            else
            {
                portInterface_->addTransactionalSystemSignal(portName, group.toStdString());
            }
        }
    }

    endResetModel();
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::getMissingSystemGroupsForSignal()
//-----------------------------------------------------------------------------
QStringList AbstractionPortsModel::getMissingSystemGroupsForSignal(int const& signalIndex) const
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
void AbstractionPortsModel::onRemoveItem(QModelIndex const& index)
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

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::isLocked()
//-----------------------------------------------------------------------------
bool AbstractionPortsModel::isLocked(QModelIndex const& index) const
{
    return extendLockedColumns.contains(LogicalPortColumns::Columns(index.column())) &&
        isExtendedRow(index.row());
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::isExtended()
//-----------------------------------------------------------------------------
bool AbstractionPortsModel::isExtendedRow(int row) const
{
    return extendInterface_->portHasMode(portInterface_->getIndexedItemName(row),
        portInterface_->getModeString(row), portInterface_->getSystemGroup(row));
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::extendWireMode()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::extendWireMode(std::string const& port, General::InterfaceMode mode,
    std::string const& systemGroup)
{
    int extendIndex = extendInterface_->getItemIndex(port, mode, systemGroup);

    if (extendIndex != -1)
    {
        std::string modeString = General::interfaceMode2Str(mode).toStdString();

        if (portInterface_->portHasMode(port, modeString, systemGroup) == false)
        {
            if (mode == General::SYSTEM)
            {
                portInterface_->addWireSystemSignal(port, systemGroup);
            }
            else
            {
                portInterface_->addModeSpecificWireSignal(port, mode);
            }

            portInterface_->setDescription(port, extendInterface_->getDescription(port));
        }

        int index = portInterface_->getItemIndex(port, mode, systemGroup);

        portInterface_->setDirection(index, extendInterface_->getDirectionString(extendIndex));
        portInterface_->setMatch(index, extendInterface_->getMatch(extendIndex));
        portInterface_->setQualifierStringList(index, extendInterface_->getQualifierStringList(extendIndex));
        portInterface_->setQualifierAttributes(index, extendInterface_->getQualifierAttributes(extendIndex));
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::extendTransactionalMode()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::extendTransactionalMode(std::string const& port, General::InterfaceMode mode,
    std::string const& systemGroup)
{
    int extendIndex = extendInterface_->getItemIndex(port, mode, systemGroup);

    if (extendIndex != -1)
    {
        std::string modeString = General::interfaceMode2Str(mode).toStdString();

        if (portInterface_->portHasMode(port, modeString, systemGroup) == false)
        {
            if (mode == General::SYSTEM)
            {
                portInterface_->addTransactionalSystemSignal(port, systemGroup);
            }
            else
            {
                portInterface_->addModeSpecificTransactionalSignal(port, mode);
            }

            portInterface_->setDescription(port, extendInterface_->getDescription(port));
        }

        int index = portInterface_->getItemIndex(port, mode, systemGroup);

        portInterface_->setQualifierStringList(index, extendInterface_->getQualifierStringList(extendIndex));
        portInterface_->setQualifierAttributes(index, extendInterface_->getQualifierAttributes(extendIndex));
        portInterface_->setInitiative(index, extendInterface_->getInitiative(extendIndex));
        portInterface_->setKind(index, extendInterface_->getKind(extendIndex));
        portInterface_->setBusWidth(index, extendInterface_->getBusWidthValue(extendIndex));
        portInterface_->setMatch(index, extendInterface_->getMatch(extendIndex));

        portInterface_->setProtocolType(index, extendInterface_->getProtocolType(extendIndex));
        portInterface_->setPayloadName(index, extendInterface_->getPayloadName(extendIndex));
        portInterface_->setPayloadType(index, extendInterface_->getPayloadType(extendIndex));
        portInterface_->setPayloadExtension(index, extendInterface_->getPayloadExtension(extendIndex));
    }
}