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
    
    const AbstractionPortsModel::SignalRow& signal = table_.at(index.row());

    if ((index.column() == LogicalPortColumns::SYSTEM_GROUP &&
            signal.mode_ != General::SYSTEM && ((signal.wire_ && signal.wire_->getSystemGroup().isEmpty()) ||
            (signal.transactional_ && signal.transactional_->getSystemGroup().isEmpty()))) ||
        (signal.mode_ == General::INTERFACE_MODE_COUNT && (index.column() == LogicalPortColumns::PRESENCE ||
            index.column() == LogicalPortColumns::DIRECTION || index.column() == LogicalPortColumns::WIDTH ||
            index.column() == LogicalPortColumns::DEFAULT_VALUE ||
            index.column() == LogicalPortColumns::INITIATIVE || index.column() == LogicalPortColumns::KIND ||
            index.column() == LogicalPortColumns::BUSWIDTH || index.column() == LogicalPortColumns::PROTOCOLTYPE ||
            index.column() == LogicalPortColumns::PAYLOADNAME ||
            index.column() == LogicalPortColumns::PAYLOADTYPE ||
            index.column() == LogicalPortColumns::PAYLOADEXTENSION)))
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
            
    AbstractionPortsModel::SignalRow const& port = table_.at(index.row());
    QSharedPointer<WireAbstraction> portWireAbs = port.abstraction_->getWire();
    QSharedPointer<TransactionalAbstraction> portTransactionalAbs = port.abstraction_->getTransactional();

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
	else if (role == Qt::DisplayRole || role == Qt::ToolTipRole)
    {
        return valueForIndexedItem(index, port, portWireAbs, portTransactionalAbs);
    }
    else if (role == Qt::ForegroundRole)
    {
        if ((index.column() == LogicalPortColumns::NAME && port.abstraction_->getLogicalName().isEmpty()) ||
            (index.column() == LogicalPortColumns::MODE && 
                (port.mode_ == General::INTERFACE_MODE_COUNT || table_.count(port) > 1)))
        {
            return  KactusColors::ERROR;
        }
        else if (index.column() == LogicalPortColumns::SYSTEM_GROUP)
        {
            if (!busDefinition_ || (port.mode_ == General::SYSTEM &&
                (port.wire_ && !BusDefinitionUtils::getSystemGroups(busDefinition_, libraryAccess_).
                    contains(port.wire_->getSystemGroup()))) ||
                (port.transactional_ && !BusDefinitionUtils::getSystemGroups(busDefinition_, libraryAccess_).
                    contains(port.transactional_->getSystemGroup())))
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
    AbstractionPortsModel::SignalRow const& port, QSharedPointer<WireAbstraction> portWireAbs,
    QSharedPointer<TransactionalAbstraction> portTransactionalAbs) const
{
    if (index.column() == LogicalPortColumns::NAME)
    {
        if (port.abstraction_->getLogicalName().isEmpty())
        {
            return "unnamed";
        }
        else
        {
            return port.abstraction_->getLogicalName();
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
        return General::interfaceMode2Str(port.mode_);
    }
    else if (index.column() == LogicalPortColumns::SYSTEM_GROUP)
    {
        if (port.wire_)
        {
            return port.wire_->getSystemGroup();
        }
        else if (port.transactional_)
        {
            return port.transactional_->getSystemGroup();
        }
    }
    else if (index.column() == LogicalPortColumns::PRESENCE)
    {
        if (port.wire_)
        {
            return presence2Str(port.wire_->getPresence());
        }
        else if (port.transactional_)
        {
            return presence2Str(port.transactional_->getPresence());
        }
    }
    else if (index.column() == LogicalPortColumns::DESCRIPTION)
    {
        return port.abstraction_->description();
    }
    else if (portWireAbs)
    {
        if (index.column() == LogicalPortColumns::WIDTH)
        {
            return port.wire_->getWidth();
        }
        else if (index.column() == LogicalPortColumns::DIRECTION)
        {
            return DirectionTypes::direction2Str(port.wire_->getDirection());
        }
        else if (index.column() == LogicalPortColumns::DEFAULT_VALUE)
        {
            return port.abstraction_->getWire()->getDefaultValue();
        }
        else if (index.column() == LogicalPortColumns::DRIVER)
        {
            return General::driverType2Str(port.abstraction_->getWire()->getDriverType());
        }
    }
    else if (portTransactionalAbs)
    {
        if (index.column() == LogicalPortColumns::INITIATIVE)
        {
            return getInitiativeForData(port.transactional_);
        }
        else if (index.column() == LogicalPortColumns::KIND)
        {
            return port.transactional_->getKind();
        }
        else if (index.column() == LogicalPortColumns::BUSWIDTH)
        {
            return port.transactional_->getBusWidth();
        }
        else if ((index.column() == LogicalPortColumns::PROTOCOLTYPE ||
            index.column() == LogicalPortColumns::PAYLOADNAME ||
            index.column() == LogicalPortColumns::PAYLOADTYPE ||
            index.column() == LogicalPortColumns::PAYLOADEXTENSION) && port.transactional_->getProtocol())
        {
            QSharedPointer<Protocol> portProtocol = port.transactional_->getProtocol();
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
    QString oldData = data(index, Qt::DisplayRole).toString();

    if (!index.isValid() || index.row() < 0 || index.row() >= table_.size() || role != Qt::EditRole ||
        oldData.compare(value.toString()) == 0)
    {
        return false;
    }

    AbstractionPortsModel::SignalRow& port = table_[index.row()];
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
        port.mode_ = General::str2Interfacemode(value.toString(), General::INTERFACE_MODE_COUNT);
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
        port.abstraction_->setDescription(value.toString());
    }
    else if (port.wire_)
    {
        if (changeWireData(index, value, port) == false)
        {
            return false;
        }
    }
    else if (port.transactional_)
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
            AbstractionPortsModel::SignalRow signal = table_.at(i);

            if (signal.abstraction_->getLogicalName().compare(port.abstraction_->getLogicalName()) == 0 &&
                signal != port)
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
void AbstractionPortsModel::changePortName(QVariant const& value, SignalRow& port)
{
    QString newName = value.toString();
    bool portIsTransactional = false;
    bool portIsWire = false;
    if (port.abstraction_)
    {
        if (port.abstraction_->hasTransactional())
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
        selectedPort = QSharedPointer<PortAbstraction>(new PortAbstraction(*port.abstraction_.data()));
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
        absDef_->getLogicalPorts()->removeOne(port.abstraction_);
    }

    port.abstraction_ = selectedPort;
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
void AbstractionPortsModel::changeQualifier(QVariant const& value, SignalRow& port)
{
    Qualifier::Type newQualifier = toQualifier(value.toString());

    if (port.abstraction_->getWire())
    {
        port.abstraction_->getWire()->setQualifier(newQualifier);
    }
    else if (port.abstraction_->getTransactional())
    {
        port.abstraction_->getTransactional()->setQualifier(newQualifier);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::changeSystemGroup()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::changeSystemGroup(QVariant const& value, SignalRow& port)
{
    if (port.wire_)
    {
        port.wire_->setSystemGroup(value.toString());
    }
    else if (port.transactional_)
    {
        port.transactional_->setSystemGroup(value.toString());
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::changePresence()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::changePresence(QVariant const& value, SignalRow& port)
{
    PresenceTypes::Presence newPresence = PresenceTypes::str2Presence(value.toString(), PresenceTypes::UNKNOWN);

    if (port.wire_)
    {
        port.wire_->setPresence(newPresence);
    }
    else if (port.transactional_)
    {
        port.transactional_->setPresence(newPresence);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::changeWireData()
//-----------------------------------------------------------------------------
bool AbstractionPortsModel::changeWireData(QModelIndex const& index, QVariant const& value, SignalRow& port)
{
    if (index.column() == LogicalPortColumns::WIDTH)
    {
        port.wire_->setWidth(value.toString());
    }
    else if (index.column() == LogicalPortColumns::DEFAULT_VALUE)
    {
        port.abstraction_->getWire()->setDefaultValue(value.toString());
    }
    else if (index.column() == LogicalPortColumns::DIRECTION)
    {
        port.wire_->setDirection(DirectionTypes::str2Direction(value.toString(),
            DirectionTypes::DIRECTION_INVALID));
    }
    else if (index.column() == LogicalPortColumns::DRIVER)
    {
        port.abstraction_->getWire()->setDriverType(General::str2DriverType(value.toString(), General::NO_DRIVER));
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
    SignalRow& port)
{
    if (index.column() == LogicalPortColumns::INITIATIVE)
    {
        port.transactional_->setInitiative(getInitiativeForSetData(value.toString()));
    }
    else if (index.column() == LogicalPortColumns::KIND)
    {
        port.transactional_->setKind(value.toString());
    }
    else if (index.column() == LogicalPortColumns::BUSWIDTH)
    {
        port.transactional_->setBusWidth(value.toString());
    }
    else if (index.column() == LogicalPortColumns::PROTOCOLTYPE ||
        index.column() == LogicalPortColumns::PAYLOADNAME ||
        index.column() == LogicalPortColumns::PAYLOADTYPE ||
        index.column() == LogicalPortColumns::PAYLOADEXTENSION)
    {
        QSharedPointer<Protocol> portProtocol = port.transactional_->getProtocol();
        if (!portProtocol)
        {
            if (value.toString().isEmpty())
            {
                return false;
            }
            else
            {
                portProtocol = QSharedPointer<Protocol>(new Protocol());
                port.transactional_->setProtocol(portProtocol);
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
            port.transactional_->setProtocol(QSharedPointer<Protocol>());
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
bool AbstractionPortsModel::portHasOtherSignals(AbstractionPortsModel::SignalRow const& portSignal) const
{
    General::InterfaceMode portMode = portSignal.mode_;
    QSharedPointer<WireAbstraction> portWire = portSignal.abstraction_->getWire();
    QSharedPointer<TransactionalAbstraction> portTransactional = portSignal.abstraction_->getTransactional();

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
        bool hasValidPort = false;
        if (portAbs->hasWire())
        {
            if (portAbs->getWire()->hasMasterPort())
            {
                createWireRow(portAbs, portAbs->getWire()->getMasterPort(), General::MASTER);
                hasValidPort = true;
            }
            if (portAbs->getWire()->hasSlavePort())
            {
                createWireRow(portAbs, portAbs->getWire()->getSlavePort(), General::SLAVE);
                hasValidPort = true;
            }
            foreach(QSharedPointer<WirePort> system, *portAbs->getWire()->getSystemPorts())
            {
                createWireRow(portAbs, system, General::SYSTEM);
                hasValidPort = true;
            }
        }
        if (portAbs->hasTransactional())
        {
            if (portAbs->getTransactional()->hasMasterPort())
            {
                createTransactionalRow(portAbs, portAbs->getTransactional()->getMasterPort(), General::MASTER);
                hasValidPort = true;
            }
            if (portAbs->getTransactional()->hasSlavePort())
            {
                createTransactionalRow(portAbs, portAbs->getTransactional()->getSlavePort(), General::SLAVE);
                hasValidPort = true;
            }
            foreach(QSharedPointer<TransactionalPort> system, *portAbs->getTransactional()->getSystemPorts())
            {
                createTransactionalRow(portAbs, system, General::SYSTEM);
                hasValidPort = true;
            }
        }

        if (!hasValidPort)
        {
            if (portAbs->getWire())
            {
                createWireRow(portAbs, QSharedPointer<WirePort>(new WirePort()), General::INTERFACE_MODE_COUNT);
            }
            else if (portAbs->getTransactional())
            {
                createTransactionalRow(portAbs, QSharedPointer<TransactionalPort>(new TransactionalPort()),
                    General::INTERFACE_MODE_COUNT);
            }
        }
    }

    endResetModel();
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
        AbstractionPortsModel::SignalRow portOnRow = table_.at(i);

        QSharedPointer<PortAbstraction> portAbs = portOnRow.abstraction_;

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
                if (table_.at(j).abstraction_ == portAbs)
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
    AbstractionPortsModel::SignalRow port;
    port.abstraction_ = QSharedPointer<PortAbstraction>(new PortAbstraction());
    port.abstraction_->setWire(QSharedPointer<WireAbstraction>(new WireAbstraction()));
    port.wire_ = QSharedPointer<WirePort>(new WirePort());

    absDef_->getLogicalPorts()->append(port.abstraction_);

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
    AbstractionPortsModel::SignalRow port;
    port.abstraction_ = QSharedPointer<PortAbstraction>(new PortAbstraction());
    port.abstraction_->setTransactional(QSharedPointer<TransactionalAbstraction>(new TransactionalAbstraction()));
    port.transactional_ = QSharedPointer<TransactionalPort>(new TransactionalPort());

    absDef_->getLogicalPorts()->append(port.abstraction_);

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
    QVector<AbstractionPortsModel::SignalRow> targetPorts = getIndexedPorts(selection);

    beginResetModel();

    foreach(AbstractionPortsModel::SignalRow signal, targetPorts)
    {
        if (!modeExistsForPort(newSignalMode, signal.abstraction_->getLogicalName()))
        {
            AbstractionPortsModel::SignalRow newSignal = constructCopySignal(signal);
            newSignal.mode_ = newSignalMode;

            if (newSignal.wire_)
            {
                newSignal.wire_->setSystemGroup("");

                General::InterfaceMode opposingSignal = General::MASTER;
                if (newSignalMode == General::MASTER)
                {
                    opposingSignal = General::SLAVE;
                }

                DirectionTypes::Direction mirroredDirection =
                    getMirroredDirectionForSignal(signal.abstraction_->getLogicalName(), opposingSignal);
                if (mirroredDirection != DirectionTypes::DIRECTION_INVALID)
                {
                    newSignal.wire_->setDirection(mirroredDirection);
                }
            }
            else if (newSignal.transactional_)
            {
                newSignal.transactional_->setSystemGroup("");
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
AbstractionPortsModel::SignalRow AbstractionPortsModel::constructCopySignal(
    AbstractionPortsModel::SignalRow signal) const
{
    AbstractionPortsModel::SignalRow newSignal;
    newSignal.abstraction_ = signal.abstraction_;

    if (signal.wire_)
    {
        newSignal.wire_ = QSharedPointer<WirePort>(new WirePort());
    }
    else if (signal.transactional_)
    {
        newSignal.transactional_ = QSharedPointer<TransactionalPort>(new TransactionalPort());
    }


    return newSignal;
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::addSystem()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::addSystem(QModelIndexList const& indexes)
{
    QVector<AbstractionPortsModel::SignalRow> targetPorts = getIndexedPorts(indexes);
    
    beginResetModel();

    foreach(AbstractionPortsModel::SignalRow signal, targetPorts)
    {
        AbstractionPortsModel::SignalRow newSystemSignal = constructCopySignal(signal);
        newSystemSignal.mode_ = General::SYSTEM;

        if (newSystemSignal.wire_)
        {
            newSystemSignal.wire_->setSystemGroup("");
        }
        else if (newSystemSignal.transactional_)
        {
            newSystemSignal.transactional_->setSystemGroup("");
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
    QVector<AbstractionPortsModel::SignalRow> targetPorts = getIndexedPorts(indexes);

    beginResetModel();

    foreach(AbstractionPortsModel::SignalRow signal, targetPorts)
    {
        QStringList systemGroups = getMissingSystemGroupsForSignal(signal);
        foreach(QString group, systemGroups)
        {
            AbstractionPortsModel::SignalRow newSystemSignal = constructCopySignal(signal);
            newSystemSignal.mode_ = General::SYSTEM;
            if (newSystemSignal.wire_)
            {
                newSystemSignal.wire_->setSystemGroup(group);
            }
            else if (newSystemSignal.transactional_)
            {
                newSystemSignal.transactional_->setSystemGroup(group);
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
QStringList AbstractionPortsModel::getMissingSystemGroupsForSignal(AbstractionPortsModel::SignalRow const& signal)
    const
{
    QStringList missingSystemGroups;
    if (busDefinition_)
    {
        missingSystemGroups = busDefinition_->getSystemGroupNames();
    }

    foreach(AbstractionPortsModel::SignalRow const& comparisonSignal, table_)
    {
        if (comparisonSignal.abstraction_->getLogicalName() == signal.abstraction_->getLogicalName() &&
            comparisonSignal.mode_ == General::SYSTEM)
        {
            if (comparisonSignal.wire_)
            {
                missingSystemGroups.removeAll(comparisonSignal.wire_->getSystemGroup());
            }
            else if (comparisonSignal.transactional_)
            {
                missingSystemGroups.removeAll(comparisonSignal.transactional_->getSystemGroup());
            }
        }
    }

    return missingSystemGroups;
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::getIndexedPorts()
//-----------------------------------------------------------------------------
QVector<AbstractionPortsModel::SignalRow> AbstractionPortsModel::getIndexedPorts(QModelIndexList const& selection)
{
    QVector<AbstractionPortsModel::SignalRow> targetPorts;

    foreach(QModelIndex index, selection)
    {
        if (0 <= index.row() && index.row() <= table_.size())
        {
            AbstractionPortsModel::SignalRow temp = table_.at(index.row());
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
bool AbstractionPortsModel::portHasBeenSelected(AbstractionPortsModel::SignalRow const& portSignal,
    QVector<AbstractionPortsModel::SignalRow> const& selectedPorts) const
{
    foreach(AbstractionPortsModel::SignalRow const& signal, selectedPorts)
    {
        if (signal.abstraction_->getLogicalName() == portSignal.abstraction_->getLogicalName())
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
    foreach(AbstractionPortsModel::SignalRow signal, table_)
    {
        if (signal.abstraction_->getLogicalName().compare(portName) == 0 && signal.mode_ == mode)
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
    foreach(AbstractionPortsModel::SignalRow signal, table_)
    {
        if (signal.mode_ == opposingMode && signal.abstraction_->getLogicalName().compare(portName) == 0)
        {
            return DirectionTypes::convert2Mirrored(signal.wire_->getDirection());
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
        AbstractionPortsModel::SignalRow portToRemove = table_.at(index.row());

        QString removedName = portToRemove.abstraction_->getLogicalName();
        General::InterfaceMode removedMode = portToRemove.mode_;

        beginRemoveRows(QModelIndex(), index.row(), index.row());
        table_.removeAt(table_.indexOf(portToRemove));
        
        bool removeAbstraction = true;
        foreach (AbstractionPortsModel::SignalRow const& row, table_)
        {
            if (row.abstraction_ == portToRemove.abstraction_)
            {
                removeAbstraction = false;
            }
        }

        if (removeAbstraction)
        {
            absDef_->getLogicalPorts()->removeOne(portToRemove.abstraction_);
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
    AbstractionPortsModel::SignalRow sourcePort = table_.at(i);

    if (portAbs->getWire())
    {
        if (sourcePort.mode_ == General::MASTER)
        {
            portAbs->getWire()->setMasterPort(sourcePort.wire_);
        }
        else if (sourcePort.mode_ == General::SLAVE)
        {
            portAbs->getWire()->setSlavePort(sourcePort.wire_);
        }
        else if (sourcePort.mode_ == General::SYSTEM)
        {
            portAbs->getWire()->addSystemPort(sourcePort.wire_);
        }
    }
    else if (portAbs->getTransactional())
    {
        if (sourcePort.mode_ == General::MASTER)
        {
            portAbs->getTransactional()->setMasterPort(sourcePort.transactional_);
        }
        else if (sourcePort.mode_ == General::SLAVE)
        {
            portAbs->getTransactional()->setSlavePort(sourcePort.transactional_);
        }
        else if (sourcePort.mode_ == General::SYSTEM)
        {
            portAbs->getTransactional()->addSystemPort(sourcePort.transactional_);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::createRow()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::createWireRow(QSharedPointer<PortAbstraction> portAbs,
    QSharedPointer<WirePort> modeSpesificWire, General::InterfaceMode mode)
{
    Q_ASSERT_X(portAbs, "BusPortsModel::createRow", "Null Port Abstraction pointer given as parameter");

    AbstractionPortsModel::SignalRow port;
    port.mode_ = mode;
    port.abstraction_ = portAbs;
    port.wire_ = modeSpesificWire;
    
    table_.append(port);
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::createTransactionalRow()
//-----------------------------------------------------------------------------
void AbstractionPortsModel::createTransactionalRow(QSharedPointer<PortAbstraction> portAbs,
    QSharedPointer<TransactionalPort> modeSpecificTransactional, General::InterfaceMode mode)
{
    Q_ASSERT_X(portAbs, "BusPortsModel::createRow", "Null Port Abstraction pointer given as parameter");

    AbstractionPortsModel::SignalRow port;
    port.mode_ = mode;
    port.abstraction_ = portAbs;
    port.transactional_ = modeSpecificTransactional;

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

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::SignalRow::SignalRow()
//-----------------------------------------------------------------------------
AbstractionPortsModel::SignalRow::SignalRow():
abstraction_(QSharedPointer<PortAbstraction>(new PortAbstraction())),
mode_(General::INTERFACE_MODE_COUNT),
wire_(),
transactional_()
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::SignalRow::operator==()
//-----------------------------------------------------------------------------
bool AbstractionPortsModel::SignalRow::operator==(SignalRow const& other) const
{
    return  abstraction_->getLogicalName() == other.abstraction_->getLogicalName() &&
        mode_ == other.mode_ && comparedSignalIsSameType(other) &&
        (mode_ != General::SYSTEM || getSystemGroup(*this) == getSystemGroup(other));
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::getSystemGroup()
//-----------------------------------------------------------------------------
QString AbstractionPortsModel::SignalRow::getSystemGroup(AbstractionPortsModel::SignalRow const& signal) const
{
    if (signal.mode_ == General::SYSTEM)
    {
        if (signal.wire_)
        {
            return signal.wire_->getSystemGroup();
        }
        else if (signal.transactional_)
        {
            return signal.transactional_->getSystemGroup();
        }
    }

    return QString("");
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::comparedSignalIsSameType()
//-----------------------------------------------------------------------------
bool AbstractionPortsModel::SignalRow::comparedSignalIsSameType(
    AbstractionPortsModel::SignalRow const& comparisonSignal) const
{
    if ((wire_ && comparisonSignal.wire_) || (transactional_ && comparisonSignal.transactional_))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::SignalRow::operator!=()
//-----------------------------------------------------------------------------
bool AbstractionPortsModel::SignalRow::operator!=(SignalRow const& other) const
{
    return  abstraction_->getLogicalName() != other.abstraction_->getLogicalName() ||
        mode_ != other.mode_ ||
        !comparedSignalIsSameType(other) ||
        getSystemGroup(*this) != getSystemGroup(other);
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsModel::SignalRow::operator<()
//-----------------------------------------------------------------------------
bool AbstractionPortsModel::SignalRow::operator<( const SignalRow& other ) const
{
	// Order by name, mode and system group.
	if (abstraction_->getLogicalName() == other.abstraction_->getLogicalName()) 
    {
        if (mode_ == other.mode_)
        {
            return getSystemGroup(*this) < getSystemGroup(other);
        }

		return mode_ < other.mode_;
	}
	else
    {
		return abstraction_->getLogicalName() < other.abstraction_->getLogicalName();
    }
}
