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

#include <editors/BusDefinitionEditor/AbstractionExtendHandler.h>
#include <editors/BusDefinitionEditor/interfaces/PortAbstractionInterface.h>

#include <QStringList>
#include <QIcon>

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::AbstractionPortsModel()
//-----------------------------------------------------------------------------
AbstractionPortsModel::AbstractionPortsModel(LibraryInterface* libraryAccess,
    PortAbstractionInterface* portInterface, QObject *parent):
QAbstractTableModel(parent),
busDefinition_(),
portInterface_(portInterface),
libraryAccess_(libraryAccess)
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
        else
        {
            return QVariant();
        }
    }
   /* TODO else if (role == portTypeRoles::isExtendLockedRole)
    {
        return port.isExtendDataLocked();
    }
    else if (role == portTypeRoles::isPortLockedRole)
    {
        return port.isPortDataLocked();
    }
	else if (role == Qt::DisplayRole || role == Qt::ToolTipRole)
    {
        return valueForIndexedItem(index, port, portWireAbs, portTransactionalAbs);
    }*/
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
// Function: AbstractionPortsModel::getInitiativeForData()
//-----------------------------------------------------------------------------
QString AbstractionPortsModel::getInitiativeForData(QSharedPointer<TransactionalPort> port) const
{
    QString portInitiative = port->getInitiative();
    if (portInitiative.compare(QStringLiteral("both"), Qt::CaseInsensitive) == 0)
    {
        return QStringLiteral("requires/provides");
    }
    else
    {
        return portInitiative;
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::getProtocolTypeText()
//-----------------------------------------------------------------------------
QString AbstractionPortsModel::getProtocolTypeText(QSharedPointer<Protocol> portProtocol) const
{
    QString protocolType = portProtocol->getProtocolType();
    if (protocolType.compare("tlm", Qt::CaseSensitive) != 0)
    {
        protocolType = portProtocol->getCustomProtocolType();
    }

    return protocolType;
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::setData()
//-----------------------------------------------------------------------------
bool AbstractionPortsModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
    QString oldData = data(index, Qt::DisplayRole).toString();

    if (!index.isValid() || index.row() < 0 || index.row() >= portInterface_->itemCount() ||
        !(flags(index) & Qt::ItemIsEditable) || role != Qt::EditRole || oldData.compare(value.toString()) == 0)
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
void AbstractionPortsModel::setExtendedPorts(QList<QSharedPointer<PortAbstraction>> const& extendPorts)
{
    if (extendPorts.isEmpty())
    {
        return;
    }

    QVector<AbstractionDefinitionSignalRow> extendRows = getCurrentExtendRows(extendPorts);

    QList<QSharedPointer<PortAbstraction> > editedExtendPorts;
    for (auto extendPort : extendPorts)
    {
        editedExtendPorts.append(QSharedPointer<PortAbstraction>(new PortAbstraction(*extendPort.data())));
    }

    if (!editedExtendPorts.isEmpty())
    {

        extendRows = AbstractionExtendHandler::getEditedExtendSignals(extendRows, editedExtendPorts);

        //absDef_->getLogicalPorts()->append(editedExtendPorts);

        int itemCount = table_.count();
        beginInsertRows(QModelIndex(), itemCount, itemCount +
            AbstractionExtendHandler::getPortAbstractionRowCount(editedExtendPorts) - 1 + extendRows.count());

        for (auto logicalPort : editedExtendPorts)
        {
           // TODO createRowsForPortAbstraction(logicalPort, true, true);
        }

        for (auto additionalRow : extendRows)
        {
/*            AbstractionExtendHandler::addSignalRowToPortAbstraction(additionalRow, absDef_);
            additionalRow.lockPortData(true);
            table_.append(additionalRow);*/
        }

        endInsertRows();
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::getCurrentExtendPorts()
//-----------------------------------------------------------------------------
QVector<AbstractionDefinitionSignalRow> AbstractionPortsModel::getCurrentExtendRows(
    QList<QSharedPointer<PortAbstraction>> const& extendPorts)
{
    QVector<AbstractionDefinitionSignalRow> currentRows;

    for (int i = table_.count() - 1; i >= 0; i--)
    {
        AbstractionDefinitionSignalRow currentSignal = table_.at(i);
        //if (AbstractionExtendHandler::removeExtendSignalFromAbstraction(currentSignal, extendPorts, absDef_))
        {
            if (!currentRows.contains(currentSignal))
            {
                currentRows.append(currentSignal);
            }

            beginRemoveRows(QModelIndex(), i, i);
            table_.removeAt(i);
            endRemoveRows();
        }
    }

    return currentRows;
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::removeExtendedPorts()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::removeExtendedPorts(
    QSharedPointer<QList<QSharedPointer<PortAbstraction>>> const& extendPorts)
{
    if (extendPorts->isEmpty())
    {
        return;
    }

    // TODO AbstractionExtendHandler::removeExtendedPortsFromAbstraction(absDef_, *extendPorts.data());
 
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::onResetExtendPorts()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::onResetExtendPorts()
{
    QList<QSharedPointer<PortAbstraction> > extendPorts;
    //= AbstractionExtendHandler::getAllExtendPorts(absDef_, libraryAccess_);

    //AbstractionExtendHandler::removeExtendedPortsFromAbstraction(absDef_, extendPorts);


    int itemCount = table_.count();
    beginInsertRows(QModelIndex(), itemCount,
        itemCount + AbstractionExtendHandler::getPortAbstractionRowCount(extendPorts) - 1);

  //  absDef_->getLogicalPorts()->append(extendPorts);
    for (auto logicalPort : extendPorts)
    {
        // TODO createRowsForPortAbstraction(logicalPort, true, true);
    }

    endInsertRows();

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
            portInterface_->addWireSystemSignal(portName, group.toStdString());
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
// Function: AbstractionPortsModel::toString()
//-----------------------------------------------------------------------------
QString AbstractionPortsModel::toString(Qualifier qualifier) const
{
    if (qualifier.isData() && qualifier.isAddress())
    {
        return "data/address";        
    }
    else if (qualifier.isData())
    {
        return "data";
    }
    else if (qualifier.isAddress())
    {
        return "address";
    }
    else if (qualifier.isClock())
    {
        return "clock";
    }
    else if (qualifier.isReset())
    {
        return "reset";
    }
    else 
    {
        return QString();
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::toQualifier()
//-----------------------------------------------------------------------------
Qualifier::Type AbstractionPortsModel::toQualifier(QString const& str) const
{
    if (str == "address")
    {
        return Qualifier::Address;
    }
    else if (str == "data")
    {
        return Qualifier::Data;
    }
    else if (str == "data/address")
    {
        return Qualifier::Data_Address;
    }
    else if (str == "clock")
    {
        return Qualifier::Clock;
    }
    else if (str == "reset")
    {
        return Qualifier::Reset;
    }
    else
    {
        return Qualifier::Any;
    }
}
