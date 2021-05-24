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

#include <QStringList>

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::AbstractionWirePortsModel()
//-----------------------------------------------------------------------------
AbstractionPortsModel::AbstractionPortsModel(LibraryInterface* libraryAccess, QObject *parent):
QAbstractTableModel(parent),
absDef_(),
busDefinition_(),
table_(),
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

	return table_.size();
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
    
    const AbstractionDefinitionSignalRow& signal = table_.at(index.row());

    if ((index.column() == LogicalPortColumns::SYSTEM_GROUP && signal.getMode() != General::SYSTEM && (
            (signal.getWire() && signal.getWire()->getSystemGroup().isEmpty()) ||
            (signal.getTransactional() && signal.getTransactional()->getSystemGroup().isEmpty()))) ||
        (signal.getMode() == General::INTERFACE_MODE_COUNT && (index.column() == LogicalPortColumns::PRESENCE ||
            index.column() == LogicalPortColumns::DIRECTION || index.column() == LogicalPortColumns::WIDTH ||
            index.column() == LogicalPortColumns::DEFAULT_VALUE ||
            index.column() == LogicalPortColumns::INITIATIVE || index.column() == LogicalPortColumns::KIND ||
            index.column() == LogicalPortColumns::BUSWIDTH || index.column() == LogicalPortColumns::PROTOCOLTYPE ||
            index.column() == LogicalPortColumns::PAYLOADNAME ||
            index.column() == LogicalPortColumns::PAYLOADTYPE ||
            index.column() == LogicalPortColumns::PAYLOADEXTENSION)) ||
        (signal.isExtendDataLocked() && (index.column() == LogicalPortColumns::MODE ||
            index.column() == LogicalPortColumns::DIRECTION || index.column() == LogicalPortColumns::INITIATIVE ||
            index.column() == LogicalPortColumns::KIND || index.column() == LogicalPortColumns::BUSWIDTH ||
            index.column() == LogicalPortColumns::PROTOCOLTYPE ||
            index.column() == LogicalPortColumns::PAYLOADNAME ||
            index.column() == LogicalPortColumns::PAYLOADTYPE ||
            index.column() == LogicalPortColumns::PAYLOADEXTENSION)) ||
        (signal.isPortDataLocked() && (index.column() == LogicalPortColumns::NAME ||
            index.column() == LogicalPortColumns::QUALIFIER || index.column() == LogicalPortColumns::DESCRIPTION)))
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
    else
    {
        return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
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
	if (!index.isValid() || index.row() < 0 || index.row() >= table_.size())
    {
		return QVariant();
    }
            
    AbstractionDefinitionSignalRow const& port = table_.at(index.row());
    QSharedPointer<WireAbstraction> portWireAbs = port.getPortAbstraction()->getWire();
    QSharedPointer<TransactionalAbstraction> portTransactionalAbs = port.getPortAbstraction()->getTransactional();

    if (role == portTypeRoles::isWireRole)
    {
        if (portWireAbs)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (role == portTypeRoles::isTransactionalRole)
    {
        if (portTransactionalAbs)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (role == portTypeRoles::isExtendLockedRole)
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
    }
    else if (role == Qt::ForegroundRole)
    {
        if ((index.column() == LogicalPortColumns::NAME &&
                port.getPortAbstraction()->getLogicalName().isEmpty()) ||
            (index.column() == LogicalPortColumns::MODE && 
                (port.getMode() == General::INTERFACE_MODE_COUNT || table_.count(port) > 1)))
        {
            return  KactusColors::ERROR;
        }
        else if (index.column() == LogicalPortColumns::SYSTEM_GROUP)
        {
            if (!busDefinition_ || (port.getMode() == General::SYSTEM &&
                (port.getWire() && !BusDefinitionUtils::getSystemGroups(busDefinition_, libraryAccess_).
                    contains(port.getWire()->getSystemGroup()))) ||
                (port.getTransactional() && !BusDefinitionUtils::getSystemGroups(busDefinition_, libraryAccess_).
                    contains(port.getTransactional()->getSystemGroup())))
            {
                return  KactusColors::ERROR;
            }
        }
        else
        {
            return QVariant();
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::valueForIndexedItem()
//-----------------------------------------------------------------------------
QVariant AbstractionPortsModel::valueForIndexedItem(QModelIndex const& index,
    AbstractionDefinitionSignalRow const& port, QSharedPointer<WireAbstraction> portWireAbs,
    QSharedPointer<TransactionalAbstraction> portTransactionalAbs) const
{
    if (index.column() == LogicalPortColumns::NAME)
    {
        if (port.getPortAbstraction()->getLogicalName().isEmpty())
        {
            return "unnamed";
        }
        else
        {
            return port.getPortAbstraction()->getLogicalName();
        }
    }
    else if (index.column() == LogicalPortColumns::QUALIFIER)
    {
        if (portWireAbs)
        {
            return toString(portWireAbs->getQualifier());
        }
        else if (portTransactionalAbs)
        {
            return toString(portTransactionalAbs->getQualifier());
        }
    }
    else if (index.column() == LogicalPortColumns::MODE)
    {
        return General::interfaceMode2Str(port.getMode());
    }
    else if (index.column() == LogicalPortColumns::SYSTEM_GROUP)
    {
        if (port.getWire())
        {
            return port.getWire()->getSystemGroup();
        }
        else if (port.getTransactional())
        {
            return port.getTransactional()->getSystemGroup();
        }
    }
    else if (index.column() == LogicalPortColumns::PRESENCE)
    {
        if (port.getWire())
        {
            return presence2Str(port.getWire()->getPresence());
        }
        else if (port.getTransactional())
        {
            return presence2Str(port.getTransactional()->getPresence());
        }
    }
    else if (index.column() == LogicalPortColumns::DESCRIPTION)
    {
        return port.getPortAbstraction()->description();
    }
    else if (portWireAbs)
    {
        if (index.column() == LogicalPortColumns::WIDTH)
        {
            return port.getWire()->getWidth();
        }
        else if (index.column() == LogicalPortColumns::DIRECTION)
        {
            return DirectionTypes::direction2Str(port.getWire()->getDirection());
        }
        else if (index.column() == LogicalPortColumns::DEFAULT_VALUE)
        {
            return port.getPortAbstraction()->getWire()->getDefaultValue();
        }
        else if (index.column() == LogicalPortColumns::DRIVER)
        {
            return General::driverType2Str(port.getPortAbstraction()->getWire()->getDriverType());
        }
    }
    else if (portTransactionalAbs)
    {
        if (index.column() == LogicalPortColumns::INITIATIVE)
        {
            return getInitiativeForData(port.getTransactional());
        }
        else if (index.column() == LogicalPortColumns::KIND)
        {
            return port.getTransactional()->getKind();
        }
        else if (index.column() == LogicalPortColumns::BUSWIDTH)
        {
            return port.getTransactional()->getBusWidth();
        }
        else if ((index.column() == LogicalPortColumns::PROTOCOLTYPE ||
            index.column() == LogicalPortColumns::PAYLOADNAME ||
            index.column() == LogicalPortColumns::PAYLOADTYPE ||
            index.column() == LogicalPortColumns::PAYLOADEXTENSION) && port.getTransactional()->getProtocol())
        {
            QSharedPointer<Protocol> portProtocol = port.getTransactional()->getProtocol();
            if (index.column() == LogicalPortColumns::PROTOCOLTYPE)
            {
                return getProtocolTypeText(portProtocol);
            }
            else if (index.column() == LogicalPortColumns::PAYLOADNAME)
            {
                return portProtocol->getPayloadName();
            }
            else if (index.column() == LogicalPortColumns::PAYLOADTYPE)
            {
                return portProtocol->getPayloadType();
            }
            else if (index.column() == LogicalPortColumns::PAYLOADEXTENSION)
            {
                return portProtocol->getPayloadExtension();
            }
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
    if (!value.isValid() && !(flags(index) & Qt::ItemIsEditable))
    {
        return false;
    }

    QString oldData = data(index, Qt::DisplayRole).toString();

    if (!index.isValid() || index.row() < 0 || index.row() >= table_.size() || role != Qt::EditRole ||
        oldData.compare(value.toString()) == 0)
    {
        return false;
    }

    AbstractionDefinitionSignalRow& port = table_[index.row()];
    if (index.column() == LogicalPortColumns::NAME)
    {
        changePortName(value, port);
    }
    else if (index.column() == LogicalPortColumns::QUALIFIER)
    {
        changeQualifier(value, port);
    }
    else if (index.column() == LogicalPortColumns::MODE)
    {
        port.setMode(General::str2Interfacemode(value.toString(), General::INTERFACE_MODE_COUNT));
    }
    else if (index.column() == LogicalPortColumns::SYSTEM_GROUP)
    {
        changeSystemGroup(value, port);
    }
    else if (index.column() == LogicalPortColumns::PRESENCE)
    {
        changePresence(value, port);
    }
    else if (index.column() == LogicalPortColumns::DESCRIPTION)
    {
        port.getPortAbstraction()->setDescription(value.toString());
    }
    else if (port.getWire())
    {
        if (changeWireData(index, value, port) == false)
        {
            return false;
        }
    }
    else if (port.getTransactional())
    {
        if (changeTransactionalData(index, value, port) == false)
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    QModelIndexList indexList;
    indexList.append(index);

    if (index.column() == LogicalPortColumns::NAME || index.column() == LogicalPortColumns::DEFAULT_VALUE ||
        index.column() == LogicalPortColumns::DRIVER || index.column() == LogicalPortColumns::QUALIFIER ||
        index.column() == LogicalPortColumns::DESCRIPTION)
    {
        for (int i = 0; i < table_.size(); ++i)
        {
            AbstractionDefinitionSignalRow const& signal = table_.at(i);

            if (signal.getPortAbstraction()->getLogicalName().compare(
                    port.getPortAbstraction()->getLogicalName()) == 0 && signal != port)
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
// Function: AbstractionPortsModel::changePortName()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::changePortName(QVariant const& value, AbstractionDefinitionSignalRow& port)
{
    QString newName = value.toString();
    bool portIsTransactional = false;
    bool portIsWire = false;
    if (port.getPortAbstraction())
    {
        if (port.getPortAbstraction()->hasTransactional())
        {
            portIsTransactional = true;
        }
        else
        {
            portIsWire = true;
        }
    }

    QPair<QString, QSharedPointer<PortAbstraction> > nameAbstractionPair =
        getNameAbstractionPairFromName(newName, portIsWire, portIsTransactional);
    newName = nameAbstractionPair.first;
    QSharedPointer<PortAbstraction> selectedPort = nameAbstractionPair.second;
    if (!selectedPort)
    {
        selectedPort = QSharedPointer<PortAbstraction>(new PortAbstraction(*port.getPortAbstraction().data()));
        selectedPort->setLogicalName(newName);

        if (portIsWire)
        {
            if (selectedPort->hasTransactional())
            {
                selectedPort->setTransactional(QSharedPointer<TransactionalAbstraction>());
            }
        }
        else if (portIsTransactional)
        {
            if (selectedPort->hasWire())
            {
                selectedPort->setWire(QSharedPointer<WireAbstraction>());
            }
        }

        absDef_->getLogicalPorts()->append(selectedPort);
    }

    if (!portHasOtherSignals(port))
    {
        absDef_->getLogicalPorts()->removeOne(port.getPortAbstraction());
    }

    port.setPortAbstraction(selectedPort);
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::getNameAbstractionPairFromName()
//-----------------------------------------------------------------------------
QPair<QString, QSharedPointer<PortAbstraction> > AbstractionPortsModel::getNameAbstractionPairFromName(
    QString const& portName, bool portIsWire, bool portIsTransactional) const
{
    QString format = "$portName$_$portNumber$";

    int runningNumber = 0;
    QString newName = portName;

    QSharedPointer<PortAbstraction> selectedPort = absDef_->getPort(newName);
    while (selectedPort &&
        ((portIsWire && selectedPort->hasTransactional()) || (portIsTransactional && selectedPort->hasWire())))
    {
        newName = format;
        newName.replace("$portName$", portName);
        newName.replace("$portNumber$", QString::number(runningNumber));

        runningNumber++;
        selectedPort = absDef_->getPort(newName);
    }

    QPair<QString, QSharedPointer<PortAbstraction> > namePortAbstractionPair;
    namePortAbstractionPair.first = newName;
    namePortAbstractionPair.second = selectedPort;

    return namePortAbstractionPair;
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::changeQualifier()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::changeQualifier(QVariant const& value, AbstractionDefinitionSignalRow& port)
{
    Qualifier::Type newQualifier = toQualifier(value.toString());

    if (port.getPortAbstraction()->getWire())
    {
        port.getPortAbstraction()->getWire()->setQualifier(newQualifier);
    }
    else if (port.getPortAbstraction()->getTransactional())
    {
        port.getPortAbstraction()->getTransactional()->setQualifier(newQualifier);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::changeSystemGroup()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::changeSystemGroup(QVariant const& value, AbstractionDefinitionSignalRow& port)
{
    if (port.getWire())
    {
        port.getWire()->setSystemGroup(value.toString());
    }
    else if (port.getTransactional())
    {
        port.getTransactional()->setSystemGroup(value.toString());
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::changePresence()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::changePresence(QVariant const& value, AbstractionDefinitionSignalRow& port)
{
    PresenceTypes::Presence newPresence = PresenceTypes::str2Presence(value.toString(), PresenceTypes::UNKNOWN);

    if (port.getWire())
    {
        port.getWire()->setPresence(newPresence);
    }
    else if (port.getTransactional())
    {
        port.getTransactional()->setPresence(newPresence);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::changeWireData()
//-----------------------------------------------------------------------------
bool AbstractionPortsModel::changeWireData(QModelIndex const& index, QVariant const& value,
    AbstractionDefinitionSignalRow& port)
{
    if (index.column() == LogicalPortColumns::WIDTH)
    {
        port.getWire()->setWidth(value.toString());
    }
    else if (index.column() == LogicalPortColumns::DEFAULT_VALUE)
    {
        port.getPortAbstraction()->getWire()->setDefaultValue(value.toString());
    }
    else if (index.column() == LogicalPortColumns::DIRECTION)
    {
        port.getWire()->setDirection(DirectionTypes::str2Direction(value.toString(),
            DirectionTypes::DIRECTION_INVALID));
    }
    else if (index.column() == LogicalPortColumns::DRIVER)
    {
        port.getPortAbstraction()->getWire()->setDriverType(
            General::str2DriverType(value.toString(), General::NO_DRIVER));
    }
    else
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::changeTransactionalData()
//-----------------------------------------------------------------------------
bool AbstractionPortsModel::changeTransactionalData(QModelIndex const& index, QVariant const& value,
    AbstractionDefinitionSignalRow& port)
{
    if (index.column() == LogicalPortColumns::INITIATIVE)
    {
        port.getTransactional()->setInitiative(getInitiativeForSetData(value.toString()));
    }
    else if (index.column() == LogicalPortColumns::KIND)
    {
        port.getTransactional()->setKind(value.toString());
    }
    else if (index.column() == LogicalPortColumns::BUSWIDTH)
    {
        port.getTransactional()->setBusWidth(value.toString());
    }
    else if (index.column() == LogicalPortColumns::PROTOCOLTYPE ||
        index.column() == LogicalPortColumns::PAYLOADNAME ||
        index.column() == LogicalPortColumns::PAYLOADTYPE ||
        index.column() == LogicalPortColumns::PAYLOADEXTENSION)
    {
        QSharedPointer<Protocol> portProtocol = port.getTransactional()->getProtocol();
        if (!portProtocol)
        {
            if (value.toString().isEmpty())
            {
                return false;
            }
            else
            {
                portProtocol = QSharedPointer<Protocol>(new Protocol());
                port.getTransactional()->setProtocol(portProtocol);
            }
        }

        if (index.column() == LogicalPortColumns::PROTOCOLTYPE)
        {
            portProtocol->setProtocolType(value.toString());
        }
        else if (index.column() == LogicalPortColumns::PAYLOADNAME)
        {
            portProtocol->setPayloadName(value.toString());
        }
        else if (index.column() == LogicalPortColumns::PAYLOADTYPE)
        {
            QString payloadType = value.toString();
            if (payloadType.compare("none", Qt::CaseInsensitive) == 0)
            {
                payloadType = "";
            }
            portProtocol->setPayloadType(payloadType);
        }
        else if (index.column() == LogicalPortColumns::PAYLOADEXTENSION)
        {
            portProtocol->setPayloadExtension(value.toString(), false);
        }

        if (portProtocol && portProcotolTypeIsEmpty(portProtocol) &&
            portProtocol->getPayloadName().isEmpty() && portProtocol->getPayloadType().isEmpty() &&
            portProtocol->getPayloadExtension().isEmpty())
        {
            port.getTransactional()->setProtocol(QSharedPointer<Protocol>());
        }
    }
    else
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::getInitiative()
//-----------------------------------------------------------------------------
QString AbstractionPortsModel::getInitiativeForSetData(QString const& newInitiativeValue) const
{
    if (newInitiativeValue.compare(QStringLiteral("requires"), Qt::CaseInsensitive) == 0 ||
        newInitiativeValue.compare(QStringLiteral("provides"), Qt::CaseInsensitive) == 0)
    {
        return newInitiativeValue;
    }
    else if (newInitiativeValue.compare(QStringLiteral("requires/provides"), Qt::CaseInsensitive) == 0)
    {
        return QStringLiteral("both");
    }
    else
    {
        return QString();
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::portProcotolTypeIsEmpty()
//-----------------------------------------------------------------------------
bool AbstractionPortsModel::portProcotolTypeIsEmpty(QSharedPointer<Protocol> portProtocol) const
{
    return portProtocol->getProtocolType().isEmpty() ||
        (portProtocol->getProtocolType().compare("custom", Qt::CaseInsensitive) == 0 &&
            portProtocol->getCustomProtocolType().isEmpty());
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::portHasOtherSignals()
//-----------------------------------------------------------------------------
bool AbstractionPortsModel::portHasOtherSignals(AbstractionDefinitionSignalRow const& portSignal) const
{
    General::InterfaceMode portMode = portSignal.getMode();
    QSharedPointer<WireAbstraction> portWire = portSignal.getPortAbstraction()->getWire();
    QSharedPointer<TransactionalAbstraction> portTransactional =
        portSignal.getPortAbstraction()->getTransactional();

    if (portWire)
    {
        if (portMode == General::MASTER)
        {
            return portWire->hasSlavePort() || portWire->getSystemPorts()->count() > 0;
        }
        else if (portMode == General::SLAVE)
        {
            return portWire->hasMasterPort() || portWire->getSystemPorts()->count() > 0;
        }
        else
        {
            return portWire->hasMasterPort() || portWire->hasSlavePort() || portWire->getSystemPorts()->count() > 1;
        }
    }
    else if (portTransactional)
    {
        if (portMode == General::MASTER)
        {
            return portTransactional->hasSlavePort() || portTransactional->getSystemPorts()->count() > 0;
        }
        else if (portMode == General::SLAVE)
        {
            return portTransactional->hasMasterPort() || portTransactional->getSystemPorts()->count() > 0;
        }
        else
        {
            return portTransactional->hasMasterPort() || portTransactional->hasSlavePort() ||
                portTransactional->getSystemPorts()->count() > 1;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::sendDataChangeForAllChangedItems()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::sendDataChangeForAllChangedItems(QModelIndexList changedIndexes)
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
// Function: AbstractionPortsModel::setAbsDef()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::setAbsDef(QSharedPointer<AbstractionDefinition> absDef)
{
    Q_ASSERT_X(absDef, "BusPortsModel::setAbsDef", "Null Abstraction Definition given as parameter");

    absDef_ = absDef;

    beginResetModel();
    table_.clear();

    for (auto portAbs : *absDef_->getLogicalPorts())
    {
        createRowsForPortAbstraction(portAbs, false, false);
    }

    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::createRowsForPortAbstraction()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::createRowsForPortAbstraction(QSharedPointer<PortAbstraction> portAbs,
    bool lockExtendData, bool lockPortData)
{
    bool hasValidPort = false;
    if (portAbs->hasWire())
    {
        if (portAbs->getWire()->hasMasterPort())
        {
            createWireRow(
                portAbs, portAbs->getWire()->getMasterPort(), General::MASTER, lockExtendData, lockPortData);
            hasValidPort = true;
        }
        if (portAbs->getWire()->hasSlavePort())
        {
            createWireRow(
                portAbs, portAbs->getWire()->getSlavePort(), General::SLAVE, lockExtendData, lockPortData);
            hasValidPort = true;
        }
        foreach(QSharedPointer<WirePort> system, *portAbs->getWire()->getSystemPorts())
        {
            createWireRow(portAbs, system, General::SYSTEM, lockExtendData, lockPortData);
            hasValidPort = true;
        }
    }
    if (portAbs->hasTransactional())
    {
        if (portAbs->getTransactional()->hasMasterPort())
        {
            createTransactionalRow(portAbs, portAbs->getTransactional()->getMasterPort(), General::MASTER,
                lockExtendData, lockPortData);
            hasValidPort = true;
        }
        if (portAbs->getTransactional()->hasSlavePort())
        {
            createTransactionalRow(portAbs, portAbs->getTransactional()->getSlavePort(), General::SLAVE,
                lockExtendData, lockPortData);
            hasValidPort = true;
        }
        foreach(QSharedPointer<TransactionalPort> system, *portAbs->getTransactional()->getSystemPorts())
        {
            createTransactionalRow(portAbs, system, General::SYSTEM, lockExtendData, lockPortData);
            hasValidPort = true;
        }
    }

    if (!hasValidPort)
    {
        if (portAbs->getWire())
        {
            createWireRow(portAbs, QSharedPointer<WirePort>(new WirePort()), General::INTERFACE_MODE_COUNT,
                lockExtendData, lockPortData);
        }
        else if (portAbs->getTransactional())
        {
            createTransactionalRow(portAbs, QSharedPointer<TransactionalPort>(new TransactionalPort()),
                General::INTERFACE_MODE_COUNT, lockExtendData, lockPortData);
        }
    }

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

        absDef_->getLogicalPorts()->append(editedExtendPorts);

        int itemCount = table_.count();
        beginInsertRows(QModelIndex(), itemCount, itemCount +
            AbstractionExtendHandler::getPortAbstractionRowCount(editedExtendPorts) - 1 + extendRows.count());

        for (auto logicalPort : editedExtendPorts)
        {
            createRowsForPortAbstraction(logicalPort, true, true);
        }

        for (auto additionalRow : extendRows)
        {
            AbstractionExtendHandler::addSignalRowToPortAbstraction(additionalRow, absDef_);
            additionalRow.lockPortData(true);
            table_.append(additionalRow);
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
        if (AbstractionExtendHandler::removeExtendSignalFromAbstraction(currentSignal, extendPorts, absDef_))
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

    AbstractionExtendHandler::removeExtendedPortsFromAbstraction(absDef_, *extendPorts.data());
    setAbsDef(absDef_);
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::onResetExtendPorts()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::onResetExtendPorts()
{
    QList<QSharedPointer<PortAbstraction> > extendPorts =
        AbstractionExtendHandler::getAllExtendPorts(absDef_, libraryAccess_);

    AbstractionExtendHandler::removeExtendedPortsFromAbstraction(absDef_, extendPorts);
    setAbsDef(absDef_);

    int itemCount = table_.count();
    beginInsertRows(QModelIndex(), itemCount,
        itemCount + AbstractionExtendHandler::getPortAbstractionRowCount(extendPorts) - 1);

    absDef_->getLogicalPorts()->append(extendPorts);
    for (auto logicalPort : extendPorts)
    {
        createRowsForPortAbstraction(logicalPort, true, true);
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
// Function: AbstractionPortsModel::save()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::save()
{
    QVector<QSharedPointer<PortAbstraction> > savedPorts;

    for (int i = 0; i < table_.size(); i++) 
    {
        AbstractionDefinitionSignalRow portOnRow = table_.at(i);
        QSharedPointer<PortAbstraction> portAbs = portOnRow.getPortAbstraction();

        if (!savedPorts.contains(portAbs))
        {
            if (portAbs->getWire())
            {
                portAbs->getWire()->setMasterPort(QSharedPointer<WirePort>());
                portAbs->getWire()->setSlavePort(QSharedPointer<WirePort>());
                portAbs->getWire()->getSystemPorts()->clear();
            }
            else if (portAbs->getTransactional())
            {
                portAbs->getTransactional()->setMasterPort(QSharedPointer<TransactionalPort>());
                portAbs->getTransactional()->setSlavePort(QSharedPointer<TransactionalPort>());
                portAbs->getTransactional()->getSystemPorts()->clear();
            }

            // Save the port for the first mode.
            savePort(portAbs, i);

            // Save different modes for the port abstraction.
            for (int j = i + 1; j < table_.size(); j++)
            {
                if (table_.at(j).getPortAbstraction() == portAbs)
                {                    
                    savePort(portAbs, j);
                }
            }

            savedPorts.append(portAbs);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::addWireSignal()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::addWireSignal()
{
    AbstractionDefinitionSignalRow port;
    port.setPortAbstraction(QSharedPointer<PortAbstraction>(new PortAbstraction()));
    port.getPortAbstraction()->setWire(QSharedPointer<WireAbstraction>(new WireAbstraction()));
    port.setWire(QSharedPointer<WirePort>(new WirePort()));

    absDef_->getLogicalPorts()->append(port.getPortAbstraction());

    beginInsertRows(QModelIndex(), table_.count(), table_.count());
    table_.append(port);
    endInsertRows();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::addTransactionalSignal()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::addTransactionalSignal()
{
    AbstractionDefinitionSignalRow port;
    port.setPortAbstraction(QSharedPointer<PortAbstraction>(new PortAbstraction()));
    port.getPortAbstraction()->setTransactional(
        QSharedPointer<TransactionalAbstraction>(new TransactionalAbstraction()));
    port.setTransactional(QSharedPointer<TransactionalPort>(new TransactionalPort()));

    absDef_->getLogicalPorts()->append(port.getPortAbstraction());

    beginInsertRows(QModelIndex(), table_.count(), table_.count());
    table_.append(port);
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
void AbstractionPortsModel::createNewSignal(General::InterfaceMode newSignalMode, QModelIndexList const& selection)
{
    QVector<AbstractionDefinitionSignalRow> targetPorts = getIndexedPorts(selection);

    beginResetModel();

    foreach(AbstractionDefinitionSignalRow signal, targetPorts)
    {
        if (!modeExistsForPort(newSignalMode, signal.getPortAbstraction()->getLogicalName()))
        {
            AbstractionDefinitionSignalRow newSignal = constructCopySignal(signal);
            newSignal.setMode(newSignalMode);

            if (newSignal.getWire())
            {
                newSignal.getWire()->setSystemGroup("");

                General::InterfaceMode opposingSignal = General::MASTER;
                if (newSignalMode == General::MASTER)
                {
                    opposingSignal = General::SLAVE;
                }

                DirectionTypes::Direction mirroredDirection =
                    getMirroredDirectionForSignal(signal.getPortAbstraction()->getLogicalName(), opposingSignal);
                if (mirroredDirection != DirectionTypes::DIRECTION_INVALID)
                {
                    newSignal.getWire()->setDirection(mirroredDirection);
                }
            }
            else if (newSignal.getTransactional())
            {
                newSignal.getTransactional()->setSystemGroup("");
            }

            table_.append(newSignal);
        }
    }

    endResetModel();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::constructCopySignal()
//-----------------------------------------------------------------------------
AbstractionDefinitionSignalRow AbstractionPortsModel::constructCopySignal(
    AbstractionDefinitionSignalRow const& signal) const
{
    AbstractionDefinitionSignalRow newSignal;
    newSignal.setPortAbstraction(signal.getPortAbstraction());

    newSignal.lockPortData(signal.isPortDataLocked());

    if (signal.getWire())
    {
        newSignal.setWire(QSharedPointer<WirePort>(new WirePort()));
    }
    else if (signal.getTransactional())
    {
        newSignal.setTransactional(QSharedPointer<TransactionalPort>(new TransactionalPort()));
    }


    return newSignal;
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::addSystem()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::addSystem(QModelIndexList const& indexes)
{
    QVector<AbstractionDefinitionSignalRow> targetPorts = getIndexedPorts(indexes);
    
    beginResetModel();

    foreach(AbstractionDefinitionSignalRow signal, targetPorts)
    {
        AbstractionDefinitionSignalRow newSystemSignal = constructCopySignal(signal);
        newSystemSignal.setMode(General::SYSTEM);

        if (newSystemSignal.getWire())
        {
            newSystemSignal.getWire()->setSystemGroup("");
        }
        else if (newSystemSignal.getTransactional())
        {
            newSystemSignal.getTransactional()->setSystemGroup("");
        }

        table_.append(newSystemSignal);
    }

    endResetModel();
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::addAllSystems()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::addAllSystems(QModelIndexList const& indexes)
{
    QVector<AbstractionDefinitionSignalRow> targetPorts = getIndexedPorts(indexes);

    beginResetModel();

    foreach(AbstractionDefinitionSignalRow signal, targetPorts)
    {
        QStringList systemGroups = getMissingSystemGroupsForSignal(signal);
        foreach(QString group, systemGroups)
        {
            AbstractionDefinitionSignalRow newSystemSignal = constructCopySignal(signal);
            newSystemSignal.setMode(General::SYSTEM);
            if (newSystemSignal.getWire())
            {
                newSystemSignal.getWire()->setSystemGroup(group);
            }
            else if (newSystemSignal.getTransactional())
            {
                newSystemSignal.getTransactional()->setSystemGroup(group);
            }

            table_.append(newSystemSignal);
        }
    }

    endResetModel();
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::getMissingSystemGroupsForSignal()
//-----------------------------------------------------------------------------
QStringList AbstractionPortsModel::getMissingSystemGroupsForSignal(AbstractionDefinitionSignalRow const& signal)
    const
{
    QStringList missingSystemGroups;
    if (busDefinition_)
    {
        missingSystemGroups = busDefinition_->getSystemGroupNames();
    }

    foreach(AbstractionDefinitionSignalRow const& comparisonSignal, table_)
    {
        if (comparisonSignal.getPortAbstraction()->getLogicalName() ==
                signal.getPortAbstraction()->getLogicalName() && comparisonSignal.getMode() == General::SYSTEM)
        {
            if (comparisonSignal.getWire())
            {
                missingSystemGroups.removeAll(comparisonSignal.getWire()->getSystemGroup());
            }
            else if (comparisonSignal.getTransactional())
            {
                missingSystemGroups.removeAll(comparisonSignal.getTransactional()->getSystemGroup());
            }
        }
    }

    return missingSystemGroups;
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::getIndexedPorts()
//-----------------------------------------------------------------------------
QVector<AbstractionDefinitionSignalRow> AbstractionPortsModel::getIndexedPorts(QModelIndexList const& selection)
{
    QVector<AbstractionDefinitionSignalRow> targetPorts;

    foreach(QModelIndex index, selection)
    {
        if (0 <= index.row() && index.row() <= table_.size())
        {
            AbstractionDefinitionSignalRow temp = table_.at(index.row());
            if (!portHasBeenSelected(temp, targetPorts))
            {
                targetPorts.append(temp);
            }
        }
    }

    return targetPorts;
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::portHasBeenSelected()
//-----------------------------------------------------------------------------
bool AbstractionPortsModel::portHasBeenSelected(AbstractionDefinitionSignalRow const& portSignal,
    QVector<AbstractionDefinitionSignalRow> const& selectedPorts) const
{
    foreach(AbstractionDefinitionSignalRow const& signal, selectedPorts)
    {
        if (signal.getPortAbstraction()->getLogicalName() == portSignal.getPortAbstraction()->getLogicalName())
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::modeExistsForPort()
//-----------------------------------------------------------------------------
bool AbstractionPortsModel::modeExistsForPort(General::InterfaceMode const& mode, QString const& portName) const
{
    foreach(AbstractionDefinitionSignalRow signal, table_)
    {
        if (signal.getPortAbstraction()->getLogicalName().compare(portName) == 0 && signal.getMode() == mode)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::getMirroredDirectionForSignal()
//-----------------------------------------------------------------------------
DirectionTypes::Direction AbstractionPortsModel::getMirroredDirectionForSignal(QString const& portName,
    General::InterfaceMode const& opposingMode) const
{
    foreach(AbstractionDefinitionSignalRow signal, table_)
    {
        if (signal.getMode() == opposingMode &&
            signal.getPortAbstraction()->getLogicalName().compare(portName) == 0)
        {
            return DirectionTypes::convert2Mirrored(signal.getWire()->getDirection());
        }
    }

    return DirectionTypes::DIRECTION_INVALID;
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::onRemoveItem()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::onRemoveItem(QModelIndex const& index)
{
    if (0 <= index.row() && index.row() <= table_.size()) 
    {
        AbstractionDefinitionSignalRow portToRemove = table_.at(index.row());

        QString removedName = portToRemove.getPortAbstraction()->getLogicalName();
        General::InterfaceMode removedMode = portToRemove.getMode();

        beginRemoveRows(QModelIndex(), index.row(), index.row());
        table_.removeAt(table_.indexOf(portToRemove));
        
        bool removeAbstraction = true;
        foreach (AbstractionDefinitionSignalRow const& row, table_)
        {
            if (row.getPortAbstraction() == portToRemove.getPortAbstraction())
            {
                removeAbstraction = false;
                break;
            }
        }

        if (removeAbstraction)
        {
            absDef_->getLogicalPorts()->removeOne(portToRemove.getPortAbstraction());
        }

        endRemoveRows();

        emit contentChanged();
        emit portRemoved(removedName, removedMode);      
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::savePort()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::savePort(QSharedPointer<PortAbstraction> portAbs, int i)
{
    AbstractionDefinitionSignalRow sourcePort = table_.at(i);

    if (portAbs->getWire())
    {
        if (sourcePort.getMode() == General::MASTER)
        {
            portAbs->getWire()->setMasterPort(sourcePort.getWire());
        }
        else if (sourcePort.getMode() == General::SLAVE)
        {
            portAbs->getWire()->setSlavePort(sourcePort.getWire());
        }
        else if (sourcePort.getMode() == General::SYSTEM)
        {
            portAbs->getWire()->addSystemPort(sourcePort.getWire());
        }
    }
    else if (portAbs->getTransactional())
    {
        if (sourcePort.getMode() == General::MASTER)
        {
            portAbs->getTransactional()->setMasterPort(sourcePort.getTransactional());
        }
        else if (sourcePort.getMode() == General::SLAVE)
        {
            portAbs->getTransactional()->setSlavePort(sourcePort.getTransactional());
        }
        else if (sourcePort.getMode() == General::SYSTEM)
        {
            portAbs->getTransactional()->addSystemPort(sourcePort.getTransactional());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::createRow()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::createWireRow(QSharedPointer<PortAbstraction> portAbs,
    QSharedPointer<WirePort> modeSpesificWire, General::InterfaceMode mode, bool lockExtendData, bool lockPortData)
{
    Q_ASSERT_X(portAbs, "BusPortsModel::createRow", "Null Port Abstraction pointer given as parameter");

    AbstractionDefinitionSignalRow port;
    port.setMode(mode);
    port.setPortAbstraction(portAbs);
    port.setWire(modeSpesificWire);
    port.lockExtendData(lockExtendData);
    port.lockPortData(lockPortData);

    table_.append(port);
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::createTransactionalRow()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::createTransactionalRow(QSharedPointer<PortAbstraction> portAbs,
    QSharedPointer<TransactionalPort> modeSpecificTransactional, General::InterfaceMode mode, bool lockExtendData,
    bool lockPortData)
{
    Q_ASSERT_X(portAbs, "BusPortsModel::createRow", "Null Port Abstraction pointer given as parameter");

    AbstractionDefinitionSignalRow port;
    port.setMode(mode);
    port.setPortAbstraction(portAbs);
    port.setTransactional(modeSpecificTransactional);
    port.lockExtendData(lockExtendData);
    port.lockPortData(lockPortData);

    table_.append(port);
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
