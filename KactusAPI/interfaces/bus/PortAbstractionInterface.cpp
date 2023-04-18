//-----------------------------------------------------------------------------
// File: PortAbstractionInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 10.12.2020
//
// Description:
// Interface for editing abstraction definition port abstractions.
//-----------------------------------------------------------------------------

#include "PortAbstractionInterface.h"

#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/TransactionalAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WirePort.h>
#include <IPXACTmodels/AbstractionDefinition/TransactionalPort.h>
#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>

#include <IPXACTmodels/common/Protocol.h>

namespace
{
    std::string const DEFAULT_NAME("port");
};

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::PortAbstractionInterface()
//-----------------------------------------------------------------------------
PortAbstractionInterface::PortAbstractionInterface() :
MasterPortInterface(),
ports_(0),
signals_()
{

}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::setAbsDef()
//-----------------------------------------------------------------------------
void PortAbstractionInterface::setAbsDef(QSharedPointer<AbstractionDefinition const> absDef)
{
    signals_.clear();
    ports_.clear();
    if (absDef.isNull())
    {
        return;
    }

    ports_ = absDef->getLogicalPorts();

    for (auto port : *ports_)
    {
        if (port->hasWire())
        {
            bool hasValidPort = false;
            QSharedPointer<WireAbstraction> wire = port->getWire();
            if (wire->hasMasterPort())
            {
                createWireSignal(port, wire->getMasterPort(), General::MASTER);
                hasValidPort = true;
            }
            if (port->getWire()->hasSlavePort())
            {
                createWireSignal(port, wire->getSlavePort(), General::SLAVE);
                hasValidPort = true;
            }
            for (auto systemPort : *wire->getSystemPorts())
            {
                createWireSignal(port, systemPort, General::SYSTEM);
                hasValidPort = true;
            }

            if (!hasValidPort)
            {
                createWireSignal(port, QSharedPointer<WirePort>(new WirePort()), General::INTERFACE_MODE_COUNT);
            }
        }
        if (port->hasTransactional())
        {
            bool hasValidPort = false;
            QSharedPointer<TransactionalAbstraction> transactional = port->getTransactional();
            if (transactional->hasMasterPort())
            {
                createTransactionalSignal(port, transactional->getMasterPort(), General::MASTER);
                hasValidPort = true;
            }
            if (transactional->hasSlavePort())
            {
                createTransactionalSignal(port, transactional->getSlavePort(), General::SLAVE);
                hasValidPort = true;
            }
            for (auto systemPort : *transactional->getSystemPorts())
            {
                createTransactionalSignal(port, systemPort, General::SYSTEM);
                hasValidPort = true;
            }

            if (!hasValidPort)
            {
                createTransactionalSignal(port, QSharedPointer<TransactionalPort>(new TransactionalPort()),
                    General::INTERFACE_MODE_COUNT);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::createWireSignal()
//-----------------------------------------------------------------------------
void PortAbstractionInterface::createWireSignal(QSharedPointer<PortAbstraction> portAbs,
    QSharedPointer<WirePort> wireModeSpesific, General::InterfaceMode mode)
{
    QSharedPointer<PortAbstractionInterface::SignalRow> newRow(
        new PortAbstractionInterface::SignalRow(true, false));
    newRow->mode_ = mode;
    newRow->abstraction_ = portAbs;
    newRow->wire_ = wireModeSpesific;

    signals_.append(newRow);
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::createTransactionalSignal()
//-----------------------------------------------------------------------------
void PortAbstractionInterface::createTransactionalSignal(QSharedPointer<PortAbstraction> portAbs,
    QSharedPointer<TransactionalPort> transactionalModeSpesific, General::InterfaceMode mode)
{
    QSharedPointer<PortAbstractionInterface::SignalRow> newRow(
        new PortAbstractionInterface::SignalRow(false, true));
    newRow->mode_ = mode;
    newRow->abstraction_ = portAbs;
    newRow->transactional_ = transactionalModeSpesific;

    signals_.append(newRow);
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getPort()
//-----------------------------------------------------------------------------
QSharedPointer<PortAbstraction> PortAbstractionInterface::getPort(std::string const& portName) const
{
    QString portNameQ(QString::fromStdString(portName));
    for (auto port : *ports_)
    {
        if (port->name() == portNameQ)
        {
            return port;
        }
    }

    return QSharedPointer<PortAbstraction>();
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getSignal()
//-----------------------------------------------------------------------------
QSharedPointer<PortAbstractionInterface::SignalRow> PortAbstractionInterface::getSignal(int const& signalIndex)
const
{
    if (signalIndex < 0 || signalIndex >= signals_.size())
    {
        return QSharedPointer<PortAbstractionInterface::SignalRow>();
    }

    return signals_.at(signalIndex);
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getItemIndex()
//-----------------------------------------------------------------------------
int PortAbstractionInterface::getItemIndex(std::string const& itemName) const
{
    QString itemNameQ(QString::fromStdString(itemName));
    for (int i = 0; i < signals_.size(); ++i)
    {
        if (signals_.at(i)->abstraction_->name() == itemNameQ)
        {
            return i;
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getItemIndex()
//-----------------------------------------------------------------------------
int PortAbstractionInterface::getItemIndex(std::string const& itemName, General::InterfaceMode mode,
    std::string const& systemGroup) const
{
    QString itemNameQ(QString::fromStdString(itemName));
    QString systemGroupQ(QString::fromStdString(systemGroup));
    for (int i = 0; i < signals_.size(); ++i)
    {
        if (signals_.at(i)->abstraction_->name() == itemNameQ && signals_.at(i)->mode_ == mode)
        {
            if (mode == General::SYSTEM)
            {
                QSharedPointer<WirePort> wirePort = signals_.at(i)->wire_;
                if (wirePort && wirePort->getSystemGroup() == systemGroupQ)
                {
                    return i;
                }
                QSharedPointer<TransactionalPort> transactional = signals_.at(i)->transactional_;
                if (transactional && transactional->getSystemGroup() == systemGroupQ)
                {
                    return i;
                }
            }
            else
            {
                return i;

            }
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getIndexedItemName()
//-----------------------------------------------------------------------------
std::string PortAbstractionInterface::getIndexedItemName(int const& itemIndex) const
{
    std::string portName("");
    if (itemIndex >= 0 && itemIndex < signals_.size())
    {
        portName = signals_.at(itemIndex)->abstraction_->getLogicalName().toStdString();
    }

    return portName;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::itemCount()
//-----------------------------------------------------------------------------
int PortAbstractionInterface::itemCount() const
{
    return signals_.count();
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::signalCount()
//-----------------------------------------------------------------------------
int PortAbstractionInterface::signalCount(std::string const& portName) const
{
    int portSignalCount = 0;
    QString portNameQ(QString::fromStdString(portName));
    for (auto currentSignal : signals_)
    {
        if (currentSignal->abstraction_->name() == portNameQ)
        {
            portSignalCount++;
        }
    }

    return portSignalCount;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::portHasMultiplesOfMasterOrSlave()
//-----------------------------------------------------------------------------
bool PortAbstractionInterface::portHasMultiplesOfMasterOrSlave(std::string const& portName,
    std::string const& mode)
{
    General::InterfaceMode actualMode =
        General::str2Interfacemode(QString::fromStdString(mode), General::INTERFACE_MODE_COUNT);
    if (actualMode == General::MASTER || actualMode == General::SLAVE)
    {
        QString portNameQ(QString::fromStdString(portName));
        int masterModes = 0;
        int slaveModes = 0;
        for (auto currentSignal : signals_)
        {
            if (currentSignal->abstraction_->name() == portNameQ)
            {
                if (currentSignal->mode_ == General::MASTER)
                {
                    masterModes++;
                }
                else if (currentSignal->mode_ == General::SLAVE)
                {
                    slaveModes++;
                }
            }
        }

        if (actualMode == General::MASTER && masterModes > 1)
        {
            return true;
        }
        else if (actualMode == General::SLAVE && slaveModes > 1)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getItemNames()
//-----------------------------------------------------------------------------
std::vector<std::string> PortAbstractionInterface::getItemNames() const
{
    std::vector<std::string> portNames;
    if (ports_)
    {
        for (auto port : *ports_)
        {
            std::string currentPortName(port->name().toStdString());
            if (std::find(portNames.cbegin(), portNames.cend(), currentPortName) == portNames.cend())
            {
                portNames.push_back(currentPortName);
            }
        }
    }

    return portNames;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getItemNamesWithModeAndGroup()
//-----------------------------------------------------------------------------
std::vector<std::string> PortAbstractionInterface::getItemNamesWithModeAndGroup(std::string const& busModeString,
    std::string systemGroup) const
{
    std::vector<std::string> portNames;
    if (ports_)
    {
        for (auto port : *ports_)
        {
            std::string currentPortName(port->name().toStdString());
            if (std::find(portNames.cbegin(), portNames.cend(), currentPortName) == portNames.cend() &&
                portHasMode(currentPortName, busModeString, systemGroup))
            {
                portNames.push_back(currentPortName);
            }
        }
    }

    return portNames;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::setName()
//-----------------------------------------------------------------------------
bool PortAbstractionInterface::setName(std::string const& currentName, std::string const& newName)
{
    QSharedPointer<PortAbstraction> editedPort = getPort(currentName);
    if (editedPort && nameHasChanged(newName, currentName))
    {
        QString uniqueNewName(getUniqueName(newName, DEFAULT_NAME));

        editedPort->setLogicalName(uniqueNewName);
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getDescription()
//-----------------------------------------------------------------------------
std::string PortAbstractionInterface::getDescription(std::string const& itemName) const
{
    QSharedPointer<PortAbstraction> editedPort = getPort(itemName);
    if (editedPort)
    {
        return editedPort->description().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::setDescription()
//-----------------------------------------------------------------------------
bool PortAbstractionInterface::setDescription(std::string const& itemName, std::string const& newDescription)
{
    QSharedPointer<PortAbstraction> editedPort = getPort(itemName);
    if (!editedPort)
    {
        return false;
    }

    editedPort->setDescription(QString::fromStdString(newDescription));
    return true;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::validateItems()
//-----------------------------------------------------------------------------
bool PortAbstractionInterface::validateItems() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::itemHasValidName()
//-----------------------------------------------------------------------------
bool PortAbstractionInterface::itemHasValidName(std::string const& itemName) const
{
    return !itemName.empty();
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getMode()
//-----------------------------------------------------------------------------
General::InterfaceMode PortAbstractionInterface::getMode(int const& portIndex) const
{
    QSharedPointer<SignalRow> selectedSignal = getSignal(portIndex);
    if (!selectedSignal)
    {
        return General::INTERFACE_MODE_COUNT;
    }

    return selectedSignal->mode_;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getModeString()
//-----------------------------------------------------------------------------
std::string PortAbstractionInterface::getModeString(int const& portIndex) const
{
    QSharedPointer<SignalRow> selectedSignal = getSignal(portIndex);
    if (!selectedSignal)
    {
        return std::string("");
    }

    return General::interfaceMode2Str(selectedSignal->mode_).toStdString();
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::setMode()
//-----------------------------------------------------------------------------
bool PortAbstractionInterface::setMode(int const& portIndex, std::string const& newMode)
{
    QSharedPointer<SignalRow> selectedSignal = getSignal(portIndex);
    if (!selectedSignal)
    {
        return false;
    }

    selectedSignal->mode_ =
        General::str2Interfacemode(QString::fromStdString(newMode), General::INTERFACE_MODE_COUNT);
    return true;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getSystemGroup()
//-----------------------------------------------------------------------------
std::string PortAbstractionInterface::getSystemGroup(int const& portIndex) const
{
    std::string signalSystem("");
    QSharedPointer<SignalRow> selectedSignal = getSignal(portIndex);
    if (selectedSignal)
    {
        if (selectedSignal->wire_)
        {
            signalSystem = selectedSignal->wire_->getSystemGroup().toStdString();
        }
        else if (selectedSignal->transactional_)
        {
            signalSystem = selectedSignal->transactional_->getSystemGroup().toStdString();
        }
    }

    return signalSystem;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getSystemGroupsForPort()
//-----------------------------------------------------------------------------
std::vector<std::string> PortAbstractionInterface::getSystemGroupsForPort(std::string const& portName) const
{
    std::vector<std::string> systemGroups;
    QString portNameQ(QString::fromStdString(portName));

    for (auto currentSignal : signals_)
    {
        if (currentSignal->abstraction_->name() == portNameQ)
        {
            QSharedPointer<WirePort> wire = currentSignal->wire_;
            QSharedPointer<TransactionalPort> transactional = currentSignal->transactional_;
            if (wire && !wire->getSystemGroup().isEmpty())
            {
                std::string newSystemGroup(wire->getSystemGroup().toStdString());
                if (std::find(systemGroups.cbegin(), systemGroups.cend(), newSystemGroup) == systemGroups.cend())
                {
                    systemGroups.push_back(newSystemGroup);
                }
            }
            if (transactional && !transactional->getSystemGroup().isEmpty())
            {
                std::string newSystemGroup(transactional->getSystemGroup().toStdString());
                if (std::find(systemGroups.cbegin(), systemGroups.cend(), newSystemGroup) == systemGroups.cend())
                {
                    systemGroups.push_back(newSystemGroup);
                }
            }
        }
    }

    return systemGroups;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::setSystemGroup()
//-----------------------------------------------------------------------------
bool PortAbstractionInterface::setSystemGroup(int const& portIndex, std::string const& newSystem)
{
    QSharedPointer<SignalRow> selectedSignal = getSignal(portIndex);
    if (selectedSignal)
    {
        if (selectedSignal->wire_)
        {
            selectedSignal->wire_->setSystemGroup(QString::fromStdString(newSystem));
        }
        else if (selectedSignal->transactional_)
        {
            selectedSignal->transactional_->setSystemGroup(QString::fromStdString(newSystem));
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getQualifierString()
//-----------------------------------------------------------------------------
std::string PortAbstractionInterface::getQualifierString(int const& portIndex) const
{
    QSharedPointer<SignalRow> selectedSignal = getSignal(portIndex);
    if (selectedSignal)
    {
        if (selectedSignal->wire_)
        {
            return qualifierToString(selectedSignal->abstraction_->getWire()->getQualifier()).toStdString();
        }
        if (selectedSignal->transactional_)
        {
            return qualifierToString(
                selectedSignal->abstraction_->getTransactional()->getQualifier()).toStdString();
        }
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getQualifierList()
//-----------------------------------------------------------------------------
std::vector<std::string> PortAbstractionInterface::getQualifierStringList(int const& portIndex) const
{
    std::vector<std::string> qualifierList;

    QSharedPointer<SignalRow> selectedSignal = getSignal(portIndex);
    if (selectedSignal)
    {
        Qualifier portQualifier;
        if (selectedSignal->wire_)
        {
            portQualifier = selectedSignal->abstraction_->getWire()->getQualifier();
        }
        if (selectedSignal->transactional_)
        {
            portQualifier = selectedSignal->abstraction_->getTransactional()->getQualifier();
        }

        if (portQualifier.isAddress())
        {
            qualifierList.push_back("address");
        }
        if (portQualifier.isData())
        {
            qualifierList.push_back("data");
        }
        if (portQualifier.isClock())
        {
            qualifierList.push_back("clock");
        }
        if (portQualifier.isReset())
        {
            qualifierList.push_back("reset");
        }
    }

    return qualifierList;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::qualifierToString()
//-----------------------------------------------------------------------------
QString PortAbstractionInterface::qualifierToString(Qualifier const& qualifier) const
{
    if (qualifier.isData() && qualifier.isAddress())
    {
        return QStringLiteral("data/address");
    }
    else if (qualifier.isData())
    {
        return QStringLiteral("data");
    }
    else if (qualifier.isAddress())
    {
        return QStringLiteral("address");
    }
    else if (qualifier.isClock())
    {
        return QStringLiteral("clock");
    }
    else if (qualifier.isReset())
    {
        return QStringLiteral("reset");
    }
    else
    {
        return QString();
    }
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::setQualifier()
//-----------------------------------------------------------------------------
bool PortAbstractionInterface::setQualifier(int const& portIndex, std::string const& newQualifier)
{
    QSharedPointer<SignalRow> selectedSignal = getSignal(portIndex);
    if (selectedSignal)
    {
        if (selectedSignal->wire_)
        {
            selectedSignal->abstraction_->getWire()->setQualifier(stringToQualifier(newQualifier));
        }
        else if (selectedSignal->transactional_)
        {
            selectedSignal->abstraction_->getTransactional()->setQualifier(stringToQualifier(newQualifier));
        }

        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::setQualifierList()
//-----------------------------------------------------------------------------
bool PortAbstractionInterface::setQualifierList(int const& portIndex, std::vector<std::string> const& newQualifierList)
{
    QSharedPointer<SignalRow> selectedSignal = getSignal(portIndex);
    if (selectedSignal)
    {
        Qualifier::Type newQualifierType = Qualifier::Any;

        if (!newQualifierList.empty())
        {
            for (auto newQualifier : newQualifierList)
            {
                Qualifier::Type comparisonQualifier = stringToQualifier(newQualifier);
                if ((newQualifierType == Qualifier::Data && comparisonQualifier == Qualifier::Address) ||
                    (newQualifierType == Qualifier::Address && comparisonQualifier == Qualifier::Data))
                {
                    newQualifierType = Qualifier::Data_Address;
                }
                else
                {
                    if (newQualifierType != Qualifier::Any && newQualifierType != comparisonQualifier)
                    {
                        newQualifierType = Qualifier::Any;
                        break;
                    }

                    newQualifierType = comparisonQualifier;
                }
            }
        }

        if (selectedSignal->wire_)
        {
            selectedSignal->abstraction_->getWire()->setQualifier(newQualifierType);
        }
        if (selectedSignal->transactional_)
        {
            selectedSignal->abstraction_->getTransactional()->setQualifier(newQualifierType);
        }

        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::stringToQualifier()
//-----------------------------------------------------------------------------
Qualifier::Type PortAbstractionInterface::stringToQualifier(std::string const& str) const
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
// Function: PortAbstractionInterface::getDirectionString()
//-----------------------------------------------------------------------------
std::string PortAbstractionInterface::getDirectionString(int const& portIndex) const
{
    QSharedPointer<SignalRow> selectedSignal = getSignal(portIndex);
    if (selectedSignal && selectedSignal->wire_)
    {
        return DirectionTypes::direction2Str(selectedSignal->wire_->getDirection()).toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getDirection()
//-----------------------------------------------------------------------------
DirectionTypes::Direction PortAbstractionInterface::getDirection(int const& portIndex) const
{
    QSharedPointer<SignalRow> selectedSignal = getSignal(portIndex);
    if (selectedSignal && selectedSignal->wire_)
    {
        return selectedSignal->wire_->getDirection();
    }

    return DirectionTypes::DIRECTION_INVALID;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getDirection()
//-----------------------------------------------------------------------------
DirectionTypes::Direction PortAbstractionInterface::getDirection(std::string const& portName,
    General::InterfaceMode mode, std::string const& systemGroup) const
{
    DirectionTypes::Direction portDirection(DirectionTypes::DIRECTION_INVALID);

    QSharedPointer<PortAbstraction> port = getPort(portName);
    if (port && port->getWire() && mode != General::INTERFACE_MODE_COUNT)
    {
        portDirection =
            port->getWire()->getDirection(mode, QString::fromStdString(systemGroup));
    }

    return portDirection;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::setDirection()
//-----------------------------------------------------------------------------
bool PortAbstractionInterface::setDirection(int const& portIndex, std::string const& newDirection)
{
    QSharedPointer<SignalRow> selectedSignal = getSignal(portIndex);
    if (!selectedSignal || !selectedSignal->wire_)
    {
        return false;
    }

    QString directionQ = QString::fromStdString(newDirection);
    selectedSignal->wire_->setDirection(
        DirectionTypes::str2Direction(directionQ, DirectionTypes::DIRECTION_INVALID));

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getWidth()
//-----------------------------------------------------------------------------
std::string PortAbstractionInterface::getWidth(int const& portIndex) const
{
    QSharedPointer<SignalRow> selectedSignal = getSignal(portIndex);
    if (selectedSignal && selectedSignal->wire_)
    {
        return selectedSignal->wire_->getWidth().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getWidth()
//-----------------------------------------------------------------------------
std::string PortAbstractionInterface::getWidth(std::string const& portName, std::string const& interfaceMode,
    std::string const& systemGroup) const
{
    QString signalWidth("");

    QSharedPointer<PortAbstraction> port = getPort(portName);
    General::InterfaceMode busMode = General::str2Interfacemode(QString::fromStdString(interfaceMode),
        General::INTERFACE_MODE_COUNT);
    if (port && port->getWire() && busMode != General::INTERFACE_MODE_COUNT)
    {
        signalWidth = port->getWire()->getWidth(busMode, QString::fromStdString(systemGroup));
    }

    return signalWidth.toStdString();
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getWidth()
//-----------------------------------------------------------------------------
std::string PortAbstractionInterface::getWidth(std::string const& portName, General::InterfaceMode interfaceMode,
    std::string const& systemGroup) const
{
    QString signalWidth("");

    QSharedPointer<PortAbstraction> port = getPort(portName);
    if (port && port->getWire() && interfaceMode != General::INTERFACE_MODE_COUNT)
    {
        signalWidth = port->getWire()->getWidth(interfaceMode, QString::fromStdString(systemGroup));
    }

    return signalWidth.toStdString();
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::setWidth()
//-----------------------------------------------------------------------------
bool PortAbstractionInterface::setWidth(int const& portIndex, std::string const& newWidth)
{
    QSharedPointer<SignalRow> selectedSignal = getSignal(portIndex);
    if (!selectedSignal || !selectedSignal->wire_)
    {
        return false;
    }

    selectedSignal->wire_->setWidth(QString::fromStdString(newWidth));
    return true;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getPresence()
//-----------------------------------------------------------------------------
PresenceTypes::Presence PortAbstractionInterface::getPresence(int const& portIndex) const
{
    QSharedPointer<SignalRow> selectedSignal = getSignal(portIndex);
    if (selectedSignal && (selectedSignal->wire_ || selectedSignal->transactional_))
    {
        if (selectedSignal->wire_)
        {
            return selectedSignal->wire_->getPresence();
        }
        if (selectedSignal->transactional_)
        {
            return selectedSignal->transactional_->getPresence();
        }
    }

    return PresenceTypes::UNKNOWN;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getPresenceString()
//-----------------------------------------------------------------------------
PresenceTypes::Presence PortAbstractionInterface::getPresence(std::string const& portName,
    std::string const& interfaceMode, std::string const& systemGroup) const
{
    QSharedPointer<PortAbstraction> port = getPort(portName);
    General::InterfaceMode busMode = General::str2Interfacemode(QString::fromStdString(interfaceMode),
        General::INTERFACE_MODE_COUNT);
    if (port && busMode != General::INTERFACE_MODE_COUNT)
    {
        return port->getPresence(busMode, QString::fromStdString(systemGroup));
    }

    return PresenceTypes::UNKNOWN;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getPresenceString()
//-----------------------------------------------------------------------------
std::string PortAbstractionInterface::getPresenceString(int const& portIndex) const
{
    QSharedPointer<SignalRow> selectedSignal = getSignal(portIndex);
    if (selectedSignal && (selectedSignal->wire_ || selectedSignal->transactional_))
    {
        if (selectedSignal->wire_)
        {
            return PresenceTypes::presence2Str(selectedSignal->wire_->getPresence()).toStdString();
        }
        if (selectedSignal->transactional_)
        {
            return PresenceTypes::presence2Str(selectedSignal->transactional_->getPresence()).toStdString();
        }
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getPresenceString()
//-----------------------------------------------------------------------------
std::string PortAbstractionInterface::getPresenceString(std::string const& portName,
    std::string const& interfaceMode, std::string const& systemGroup) const
{
    std::string presence("");

    QSharedPointer<PortAbstraction> port = getPort(portName);
    General::InterfaceMode busMode = General::str2Interfacemode(QString::fromStdString(interfaceMode),
        General::INTERFACE_MODE_COUNT);
    if (port && busMode != General::INTERFACE_MODE_COUNT)
    {
        presence = PresenceTypes::presence2Str(
            port->getPresence(busMode, QString::fromStdString(systemGroup))).toStdString();
    }

    return presence;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::setPresence()
//-----------------------------------------------------------------------------
bool PortAbstractionInterface::setPresence(int const& portIndex, std::string const& newPresence)
{
    QSharedPointer<SignalRow> selectedSignal = getSignal(portIndex);
    if (selectedSignal && (selectedSignal->wire_ || selectedSignal->transactional_))
    {
        PresenceTypes::Presence formattedPresence =
            PresenceTypes::str2Presence(QString::fromStdString(newPresence), PresenceTypes::UNKNOWN);
        if (selectedSignal->wire_)
        {
            selectedSignal->wire_->setPresence(formattedPresence);
        }
        if (selectedSignal->transactional_)
        {
            selectedSignal->transactional_->setPresence(formattedPresence);
        }

        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getDefaultValue()
//-----------------------------------------------------------------------------
std::string PortAbstractionInterface::getDefaultValue(int const& portIndex) const
{
    QSharedPointer<SignalRow> selectedSignal = getSignal(portIndex);
    if (selectedSignal && selectedSignal->abstraction_ && selectedSignal->abstraction_->hasWire())
    {
        return selectedSignal->abstraction_->getWire()->getDefaultValue().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::setDefaultValue()
//-----------------------------------------------------------------------------
bool PortAbstractionInterface::setDefaultValue(int const& portIndex, std::string const& newDefaultValue)
{
    QSharedPointer<SignalRow> selectedSignal = getSignal(portIndex);
    if (!selectedSignal || !selectedSignal->abstraction_ || !selectedSignal->abstraction_->getWire())
    {
        return false;
    }

    selectedSignal->abstraction_->getWire()->setDefaultValue(QString::fromStdString(newDefaultValue));
    return true;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getDriverString()
//-----------------------------------------------------------------------------
std::string PortAbstractionInterface::getDriverString(int const& portIndex) const
{
    QSharedPointer<SignalRow> selectedSignal = getSignal(portIndex);
    if (selectedSignal && selectedSignal->abstraction_ && selectedSignal->abstraction_->hasWire())
    {
        return General::driverType2Str(selectedSignal->abstraction_->getWire()->getDriverType()).toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::setDriverType()
//-----------------------------------------------------------------------------
bool PortAbstractionInterface::setDriverType(int const& portIndex, std::string const& newDriver)
{
    QSharedPointer<SignalRow> selectedSignal = getSignal(portIndex);
    if (!selectedSignal || !selectedSignal->abstraction_ || !selectedSignal->abstraction_->getWire())
    {
        return false;
    }

    selectedSignal->abstraction_->getWire()->setDriverType(
        General::str2DriverType(QString::fromStdString(newDriver), General::NO_DRIVER));
    return true;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::()
//-----------------------------------------------------------------------------
std::string PortAbstractionInterface::getBusWidthValue(int const& portIndex) const
{
    QSharedPointer<SignalRow> selectedSignal = getSignal(portIndex);
    if (selectedSignal && selectedSignal->transactional_)
    {
        return selectedSignal->transactional_->getBusWidth().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getBusWidthValue()
//-----------------------------------------------------------------------------
std::string PortAbstractionInterface::getBusWidthValue(std::string const& portName,
    std::string const& interfaceMode, std::string const& systemGroup) const
{
    QString signalWidth("");

    QSharedPointer<PortAbstraction> port = getPort(portName);
    General::InterfaceMode busMode = General::str2Interfacemode(QString::fromStdString(interfaceMode),
        General::INTERFACE_MODE_COUNT);
    if (port && port->getTransactional() && busMode != General::INTERFACE_MODE_COUNT)
    {
        signalWidth = port->getTransactional()->getWidth(busMode, QString::fromStdString(systemGroup));
    }

    return signalWidth.toStdString();
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::setBusWidth()
//-----------------------------------------------------------------------------
bool PortAbstractionInterface::setBusWidth(int const& portIndex, std::string const& newBusWidth)
{
    QSharedPointer<SignalRow> selectedSignal = getSignal(portIndex);
    if (!selectedSignal || !selectedSignal->transactional_)
    {
        return false;
    }

    QString busWidthQ(QString::fromStdString(newBusWidth));
    selectedSignal->transactional_->setBusWidth(busWidthQ);

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getInitiative()
//-----------------------------------------------------------------------------
std::string PortAbstractionInterface::getInitiative(int const& portIndex) const
{
    QSharedPointer<PortAbstractionInterface::SignalRow> selectedSignal = getSignal(portIndex);
    if (selectedSignal && selectedSignal->transactional_)
    {
        QString portInitiative = selectedSignal->transactional_->getInitiative();
        if (portInitiative.compare(QStringLiteral("both"), Qt::CaseInsensitive) == 0)
        {
            return QStringLiteral("requires/provides").toStdString();
        }
        else
        {
            return portInitiative.toStdString();
        }
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getInitiative()
//-----------------------------------------------------------------------------
std::string PortAbstractionInterface::getInitiative(std::string const& portName, std::string const& interfaceMode,
    std::string const& systemGroup) const
{
    std::string portInitiative("");

    QSharedPointer<PortAbstraction> port = getPort(portName);
    General::InterfaceMode busMode = General::str2Interfacemode(QString::fromStdString(interfaceMode),
        General::INTERFACE_MODE_COUNT);
    if (port && port->getTransactional() && busMode != General::INTERFACE_MODE_COUNT)
    {
        portInitiative =
            port->getTransactional()->getInitiative(busMode, QString::fromStdString(systemGroup)).toStdString();
    }

    return portInitiative;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::setInitiative()
//-----------------------------------------------------------------------------
bool PortAbstractionInterface::setInitiative(int const& portIndex, std::string const& newInitiative)
{
    QSharedPointer<SignalRow> selectedSignal = getSignal(portIndex);
    if (!selectedSignal || !selectedSignal->transactional_)
    {
        return false;
    }

    QString initiativeQ(QString::fromStdString(newInitiative));
    if (initiativeQ.compare(QStringLiteral("requires/provides"), Qt::CaseInsensitive) == 0)
    {
        initiativeQ = QLatin1String("both");
    }
    else if (initiativeQ.compare(QStringLiteral("requires"), Qt::CaseInsensitive) != 0 && initiativeQ.compare(QStringLiteral("provides"), Qt::CaseInsensitive) != 0)
    {
        initiativeQ = QLatin1String("");
    }

    selectedSignal->transactional_->setInitiative(initiativeQ);
    return true;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getKind()
//-----------------------------------------------------------------------------
std::string PortAbstractionInterface::getKind(int const& portIndex) const
{
    QSharedPointer<SignalRow> selectedSignal = getSignal(portIndex);
    if (selectedSignal && selectedSignal->transactional_)
    {
        return selectedSignal->transactional_->getKind().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::setKind()
//-----------------------------------------------------------------------------
bool PortAbstractionInterface::setKind(int const& portIndex, std::string const& newKind)
{
    QSharedPointer<SignalRow> selectedSignal = getSignal(portIndex);
    if (!selectedSignal || !selectedSignal->transactional_)
    {
        return false;
    }

    QString kindQ(QString::fromStdString(newKind));
    selectedSignal->transactional_->setKind(kindQ);

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::hasProtocol()
//-----------------------------------------------------------------------------
bool PortAbstractionInterface::hasProtocol(int const& portIndex) const
{
    QSharedPointer<SignalRow> selectedSignal = getSignal(portIndex);
    if (selectedSignal && selectedSignal->transactional_ && selectedSignal->transactional_->getProtocol())
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getProtocolType()
//-----------------------------------------------------------------------------
std::string PortAbstractionInterface::getProtocolType(int const& portIndex) const
{
    QSharedPointer<SignalRow> selectedSignal = getSignal(portIndex);
    if (selectedSignal && selectedSignal->transactional_ && selectedSignal->transactional_->getProtocol())
    {
        return getProtocolTypeText(selectedSignal->transactional_->getProtocol()).toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::getProtocolTypeText()
//-----------------------------------------------------------------------------
QString PortAbstractionInterface::getProtocolTypeText(QSharedPointer<Protocol> portProtocol) const
{
    QString protocolType(portProtocol->getProtocolType());
    if (protocolType.compare(QLatin1String("tlm"), Qt::CaseSensitive) != 0)
    {
        protocolType = portProtocol->getCustomProtocolType();
    }

    return protocolType;
} 

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::setProtocolType()
//-----------------------------------------------------------------------------
bool PortAbstractionInterface::setProtocolType(int const& portIndex, std::string const& newProtocolType)
{
    QSharedPointer<Protocol> selectedProtocol = getOrCreateProtocol(portIndex);
    if (!selectedProtocol)
    {
        return false;
    }

    selectedProtocol->setProtocolType(QString::fromStdString(newProtocolType));
    removeEmptyProtocol(portIndex, selectedProtocol);
    return true;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getProtocol()
//-----------------------------------------------------------------------------
QSharedPointer<Protocol> PortAbstractionInterface::getOrCreateProtocol(int const& signalIndex) const
{
    QSharedPointer<SignalRow> selectedSignal = getSignal(signalIndex);
    if (selectedSignal && selectedSignal->transactional_)
    {
        QSharedPointer<Protocol> portProtocol = selectedSignal->transactional_->getProtocol();
        if (!portProtocol)
        {
            portProtocol = QSharedPointer<Protocol>(new Protocol());
            selectedSignal->transactional_->setProtocol(portProtocol);
        }

        return portProtocol;
    }

    return QSharedPointer<Protocol>();
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::removeEmptyProtocol()
//-----------------------------------------------------------------------------
void PortAbstractionInterface::removeEmptyProtocol(int const& signalIndex, QSharedPointer<Protocol> signalProtocol)
{
    if (signalProtocol && portProcotolTypeIsEmpty(signalProtocol) &&
        signalProtocol->getPayloadName().isEmpty() && signalProtocol->getPayloadType().isEmpty() &&
        signalProtocol->getPayloadExtension().isEmpty())
    {
        QSharedPointer<PortAbstractionInterface::SignalRow> containingSignal = getSignal(signalIndex);
        if (containingSignal)
        {
            containingSignal->transactional_->setProtocol(QSharedPointer<Protocol>());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::portProcotolTypeIsEmpty()
//-----------------------------------------------------------------------------
bool PortAbstractionInterface::portProcotolTypeIsEmpty(QSharedPointer<Protocol> portProtocol) const
{
    return portProtocol->getProtocolType().isEmpty() ||
        (portProtocol->getProtocolType().compare(QLatin1String("custom"), Qt::CaseInsensitive) == 0 &&
            portProtocol->getCustomProtocolType().isEmpty());
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getPayloadName()
//-----------------------------------------------------------------------------
std::string PortAbstractionInterface::getPayloadName(int const& portIndex)
{
    QSharedPointer<SignalRow> selectedSignal = getSignal(portIndex);
    if (selectedSignal && selectedSignal->transactional_ && selectedSignal->transactional_->getProtocol())
    {
        QSharedPointer<Protocol> protocol = selectedSignal->transactional_->getProtocol();
        return protocol->getPayloadName().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::setPayloadName()
//-----------------------------------------------------------------------------
bool PortAbstractionInterface::setPayloadName(int const& portIndex, std::string const& newPayloadName)
{
    QSharedPointer<Protocol> selectedProtocol = getOrCreateProtocol(portIndex);
    if (!selectedProtocol)
    {
        return false;
    }

    selectedProtocol->setPayloadName(QString::fromStdString(newPayloadName));
    if (newPayloadName.empty())
    {
        removeEmptyProtocol(portIndex, selectedProtocol);
    }
    return true;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getPayloadType()
//-----------------------------------------------------------------------------
std::string PortAbstractionInterface::getPayloadType(int const& portIndex)
{
    QSharedPointer<SignalRow> selectedSignal = getSignal(portIndex);
    if (selectedSignal && selectedSignal->transactional_ && selectedSignal->transactional_->getProtocol())
    {
        QSharedPointer<Protocol> protocol = selectedSignal->transactional_->getProtocol();
        return protocol->getPayloadType().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::setPayloadType()
//-----------------------------------------------------------------------------
bool PortAbstractionInterface::setPayloadType(int const& portIndex, std::string const& newPayloadType)
{
    QSharedPointer<Protocol> selectedProtocol = getOrCreateProtocol(portIndex);
    if (!selectedProtocol)
    {
        return false;
    }

    QString payloadType = QString::fromStdString(newPayloadType);
    if (payloadType.compare(QStringLiteral("none"), Qt::CaseInsensitive) == 0)
    {
        payloadType = QStringLiteral("");
    }
    selectedProtocol->setPayloadType(payloadType);
    if (payloadType.isEmpty())
    {
        removeEmptyProtocol(portIndex, selectedProtocol);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getPayloadExtension()
//-----------------------------------------------------------------------------
std::string PortAbstractionInterface::getPayloadExtension(int const& portIndex)
{
    QSharedPointer<SignalRow> selectedSignal = getSignal(portIndex);
    if (selectedSignal && selectedSignal->transactional_ && selectedSignal->transactional_->getProtocol())
    {
        QSharedPointer<Protocol> protocol = selectedSignal->transactional_->getProtocol();
        return protocol->getPayloadExtension().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::setPayloadExtension()
//-----------------------------------------------------------------------------
bool PortAbstractionInterface::setPayloadExtension(int const& portIndex, std::string const& newPayloadExtension)
{
    QSharedPointer<Protocol> selectedProtocol = getOrCreateProtocol(portIndex);
    if (!selectedProtocol)
    {
        return false;
    }

    selectedProtocol->setPayloadExtension(QString::fromStdString(newPayloadExtension), false);
    if (newPayloadExtension.empty())
    {
        removeEmptyProtocol(portIndex, selectedProtocol);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::addWirePort()
//-----------------------------------------------------------------------------
void PortAbstractionInterface::addWirePort(std::string const& newPortName)
{
    QString uniqueNewName(getUniqueName(newPortName, DEFAULT_NAME));

    QSharedPointer<PortAbstractionInterface::SignalRow> newPort(
        new PortAbstractionInterface::SignalRow(true, false));
    newPort->abstraction_ = QSharedPointer<PortAbstraction>(new PortAbstraction());
    newPort->abstraction_->setLogicalName(uniqueNewName);
    newPort->abstraction_->setWire(QSharedPointer<WireAbstraction>(new WireAbstraction()));
    newPort->wire_ = QSharedPointer<WirePort>(new WirePort());    
    
    signals_.append(newPort);
    ports_->append(newPort->abstraction_);
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::addTransactionalPort()
//-----------------------------------------------------------------------------
void PortAbstractionInterface::addTransactionalPort(std::string const& newPortName)
{
    QString uniqueNewName = getUniqueName(newPortName, DEFAULT_NAME);

    QSharedPointer<PortAbstractionInterface::SignalRow> newPort(
        new PortAbstractionInterface::SignalRow(false, true));
    newPort->abstraction_ = QSharedPointer<PortAbstraction>(new PortAbstraction());
    newPort->abstraction_->setLogicalName(uniqueNewName);
    newPort->abstraction_->setTransactional(
        QSharedPointer<TransactionalAbstraction>(new TransactionalAbstraction()));
    newPort->transactional_ = QSharedPointer<TransactionalPort>(new TransactionalPort());

    signals_.append(newPort);
    ports_->append(newPort->abstraction_);
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::addModeSpecificWireSignal()
//-----------------------------------------------------------------------------
void PortAbstractionInterface::addModeSpecificWireSignal(std::string const& portName,
    General::InterfaceMode const& newMode)
{
    if (newMode == General::SYSTEM || !modeExistsForPort(newMode, QString::fromStdString(portName)))
    {
        QSharedPointer<PortAbstraction> selectedPort = getPort(portName);
        QSharedPointer<PortAbstractionInterface::SignalRow> newSignal;
        if (selectedPort.isNull())
        {
            addWirePort(portName);
            selectedPort = ports_->last();
            newSignal = signals_.last();
        }
        else
        {
            newSignal = constructCopySignal(selectedPort, true, false);
            signals_.append(newSignal);
        }

        newSignal->mode_ = newMode;
        newSignal->wire_->setSystemGroup("");

        if (newMode != General::SYSTEM)
        {
            General::InterfaceMode opposingSignal = General::MASTER;
            if (newMode == General::MASTER)
            {
                opposingSignal = General::SLAVE;
            }

            DirectionTypes::Direction mirroredDirection =
                getMirroredDirectionForSignal(selectedPort->getLogicalName(), opposingSignal);
            if (mirroredDirection != DirectionTypes::DIRECTION_INVALID)
            {
                newSignal->wire_->setDirection(mirroredDirection);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::addWireSystemSignal()
//-----------------------------------------------------------------------------
void PortAbstractionInterface::addWireSystemSignal(std::string const& portName, std::string const& systemGroup)
{
    QSharedPointer<PortAbstraction> selectedPort = getPort(portName);
    QSharedPointer<PortAbstractionInterface::SignalRow> newSignal;
    
    if (selectedPort.isNull())
    {
        addWirePort(portName);
        selectedPort = ports_->last();
        newSignal = signals_.last();
    }
    else
    {
        newSignal = constructCopySignal(selectedPort, true, false);        
        signals_.append(newSignal);
    }

    newSignal->mode_ = General::SYSTEM;
    newSignal->wire_->setSystemGroup(QString::fromStdString(systemGroup));
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::addModeSpecificTransactionalSignal()
//-----------------------------------------------------------------------------
void PortAbstractionInterface::addModeSpecificTransactionalSignal(std::string const& portName,
    General::InterfaceMode const& newMode)
{
    if (newMode == General::SYSTEM || !modeExistsForPort(newMode, QString::fromStdString(portName)))
    {
        QSharedPointer<PortAbstraction> selectedPort = getPort(portName);
        QSharedPointer<PortAbstractionInterface::SignalRow> newSignal;
        if (selectedPort.isNull())
        {
            addTransactionalPort(portName);
            selectedPort = ports_->last();
            newSignal = signals_.last();
        }
        else
        {
            newSignal = constructCopySignal(selectedPort, false, true);
            signals_.append(newSignal);
        }

        newSignal->mode_ = newMode;
        newSignal->transactional_->setSystemGroup("");
    }
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::addTransactionalSystemSignal()
//-----------------------------------------------------------------------------
void PortAbstractionInterface::addTransactionalSystemSignal(std::string const& portName,
    std::string const& systemGroup)
{
    QSharedPointer<PortAbstraction> selectedPort = getPort(portName);
    QSharedPointer<PortAbstractionInterface::SignalRow> newSignal = constructCopySignal(selectedPort, false, true);
    newSignal->mode_ = General::SYSTEM;
    newSignal->transactional_->setSystemGroup(QString::fromStdString(systemGroup));

    signals_.append(newSignal);
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::modeExistsForPort()
//-----------------------------------------------------------------------------
bool PortAbstractionInterface::modeExistsForPort(General::InterfaceMode const& mode, QString const& portName,
    QString const& systemGroup /* = "" */) const
{
    General::InterfaceMode selectedMode = mode;
    if (selectedMode == General::MIRROREDMASTER)
    {
        selectedMode = General::MASTER;
    }
    else if (selectedMode == General::MIRROREDSLAVE)
    {
        selectedMode = General::SLAVE;
    }
    else if (selectedMode == General::MIRROREDSYSTEM)
    {
        selectedMode = General::SYSTEM;
    }

    for (auto signal : signals_)
    {
        if (signal->abstraction_->getLogicalName().compare(portName) == 0 && signal->mode_ == selectedMode)
        {
            if (selectedMode == General::SYSTEM)
            {
                if (systemGroup.isEmpty() ||
                    (signal->wire_ && signal->wire_->getSystemGroup() == systemGroup) ||
                    (signal->transactional_ && signal->transactional_->getSystemGroup() == systemGroup))
                {
                    return true;
                }
            }
            else
            {
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::constructCopySignal()
//-----------------------------------------------------------------------------
QSharedPointer<PortAbstractionInterface::SignalRow> PortAbstractionInterface::constructCopySignal(
    QSharedPointer<PortAbstraction> port, bool isWire, bool isTransactional) const
{
    QSharedPointer<PortAbstractionInterface::SignalRow> newSignal(
        new PortAbstractionInterface::SignalRow(isWire, isTransactional));
    newSignal->abstraction_ = port;

    return newSignal;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getMirroredDirectionForSignal()
//-----------------------------------------------------------------------------
DirectionTypes::Direction PortAbstractionInterface::getMirroredDirectionForSignal(QString const& portName,
    General::InterfaceMode const& opposingMode) const
{
    for (auto signal : signals_)
    {
        if (signal->mode_ == opposingMode && signal->abstraction_->getLogicalName().compare(portName) == 0)
        {
            return DirectionTypes::convert2Mirrored(signal->wire_->getDirection());
        }
    }

    return DirectionTypes::DIRECTION_INVALID;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::removeSignal()
//-----------------------------------------------------------------------------
bool PortAbstractionInterface::removeSignal(int const& signalIndex)
{
    QSharedPointer<SignalRow> removedSignal = getSignal(signalIndex);
    if (!removedSignal)
    {
        return false;
    }

    signals_.removeAll(removedSignal);

    bool removeAbstraction = true;
    for (auto currentSignal : signals_)
    {
        if (currentSignal->abstraction_ == removedSignal->abstraction_)
        {
            removeAbstraction = false;
        }
    }

    if (removeAbstraction)
    {
        ports_->removeOne(removedSignal->abstraction_);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::portIsWire()
//-----------------------------------------------------------------------------
bool PortAbstractionInterface::portIsWire(std::string const& portName) const
{
    QSharedPointer<PortAbstraction> selectedPort = getPort(portName);
    if (selectedPort && selectedPort->getWire())
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::portIsTransactional()
//-----------------------------------------------------------------------------
bool PortAbstractionInterface::portIsTransactional(std::string const& portName) const
{
    QSharedPointer<PortAbstraction> selectedPort = getPort(portName);
    if (selectedPort && selectedPort->getTransactional())
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getIconPathForSignal()
//-----------------------------------------------------------------------------
std::string PortAbstractionInterface::getIconPathForSignal(int const& signalIndex) const
{
    std::string path("");
    QSharedPointer<SignalRow> selectedSignal = getSignal(signalIndex);
    if (!selectedSignal)
    {
        path = getIconPathForMissingPort();
    }
    else
    {
        if (selectedSignal->wire_)
        {
            DirectionTypes::Direction direction = selectedSignal->wire_->getDirection();
            if (direction != DirectionTypes::DIRECTION_INVALID)
            {
                path = getIconPathForDirection(direction);
            }
        }
        else if (selectedSignal->transactional_)
        {
            QString initiative(selectedSignal->transactional_->getInitiative());
            if (!initiative.isEmpty())
            {
                path = getIconPathForInitiative(initiative);
            }
        }
    }

    return path;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::getIconPathForSignal()
//-----------------------------------------------------------------------------
std::string PortAbstractionInterface::getIconPathForSignal(std::string const& portName,
    std::string const& interfaceMode, std::string const& systemGroup) const
{
    std::string path("");

    QSharedPointer<PortAbstraction> port = getPort(portName);
    if (!port)
    {
        path = getIconPathForMissingPort();
    }
    else
    {
        General::InterfaceMode busMode = General::str2Interfacemode(QString::fromStdString(interfaceMode),
            General::INTERFACE_MODE_COUNT);
        if (port && busMode != General::INTERFACE_MODE_COUNT)
        {
            if (port->getWire())
            {
                DirectionTypes::Direction portDirection =
                    port->getWire()->getDirection(busMode, QString::fromStdString(systemGroup));
                path = getIconPathForDirection(portDirection);
            }
            else if (port->getTransactional())
            {
                QString portInitiative =
                    port->getTransactional()->getInitiative(busMode, QString::fromStdString(systemGroup));
                path = getIconPathForInitiative(portInitiative);
            }
        }
    }

    return path;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::SignalRow::SignalRow()
//-----------------------------------------------------------------------------
PortAbstractionInterface::SignalRow::SignalRow(bool isWirePort, bool isTransactionalPort):
abstraction_(QSharedPointer<PortAbstraction>(new PortAbstraction())),
mode_(General::INTERFACE_MODE_COUNT),
transactional_(),
wire_()
{
    if (isWirePort)
    {
        wire_ = QSharedPointer<WirePort>(new WirePort());
        transactional_ = 0;
    }
    else if (isTransactionalPort)
    {
        transactional_ = QSharedPointer<TransactionalPort>(new TransactionalPort());
        wire_ = 0;
    }
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::SignalRow::operator==()
//-----------------------------------------------------------------------------
bool PortAbstractionInterface::SignalRow::operator==(SignalRow const& other) const
{
    bool isEquals = false;
    if (wire_)
    {
        isEquals = abstraction_->getLogicalName() == other.abstraction_->getLogicalName() &&
            mode_ == other.mode_ && (mode_ != General::SYSTEM
                || wire_->getSystemGroup() == other.wire_->getSystemGroup());
    }
    else if (transactional_)
    {
        isEquals = abstraction_->getLogicalName() == other.abstraction_->getLogicalName() &&
            mode_ == other.mode_ && (mode_ != General::SYSTEM ||
                transactional_->getSystemGroup() == other.transactional_->getSystemGroup());
    }

    return isEquals;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::SignalRow::operator!=()
//-----------------------------------------------------------------------------
bool PortAbstractionInterface::SignalRow::operator!=(SignalRow const& other) const
{
    bool isNotEquals = false;
    if (wire_)
    {
        isNotEquals = abstraction_->getLogicalName() != other.abstraction_->getLogicalName() ||
            mode_ != other.mode_ || wire_->getSystemGroup() != other.wire_->getSystemGroup();
    }
    else if (transactional_)
    {
        isNotEquals = abstraction_->getLogicalName() != other.abstraction_->getLogicalName() ||
            mode_ != other.mode_ || transactional_->getSystemGroup() != other.transactional_->getSystemGroup();
    }

    return isNotEquals;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::SignalRow::operator<()
//-----------------------------------------------------------------------------
bool PortAbstractionInterface::SignalRow::operator<(const SignalRow& other) const
{
    // Order by name, mode and system group.
    if (abstraction_->getLogicalName() == other.abstraction_->getLogicalName())
    {
        if (mode_ == other.mode_)
        {
            if (wire_)
            {
                return wire_->getSystemGroup() < other.wire_->getSystemGroup();
            }
            else if (transactional_)
            {
                return transactional_->getSystemGroup() < other.transactional_->getSystemGroup();
            }
        }

        return mode_ < other.mode_;
    }
    else
    {
        return abstraction_->getLogicalName() < other.abstraction_->getLogicalName();
    }
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::save()
//-----------------------------------------------------------------------------
void PortAbstractionInterface::save()
{
    QVector<QSharedPointer<PortAbstraction> > savedPorts;
    for (int i = 0; i < itemCount(); i++)
    {
        QSharedPointer<PortAbstractionInterface::SignalRow> currentSignal = signals_.at(i);
        QSharedPointer<PortAbstraction> currentPort = currentSignal->abstraction_;

        if (!savedPorts.contains(currentPort))
        {
            QSharedPointer<WireAbstraction> wire = currentPort->getWire();
            QSharedPointer<TransactionalAbstraction> transactional = currentPort->getTransactional();
            if (wire)
            {
                currentPort->getWire()->setMasterPort(QSharedPointer<WirePort>());
                currentPort->getWire()->setSlavePort(QSharedPointer<WirePort>());
                currentPort->getWire()->getSystemPorts()->clear();

                // Save the port for the first mode.
                savePort(currentPort, i);

                // Save different modes for the port abstraction.
                for (int j = i + 1; j < itemCount(); j++)
                {
                    if (signals_.at(j)->abstraction_ == currentPort)
                    {
                        savePort(currentPort, j);
                    }
                }
            }
            if (transactional)
            {
                transactional->setMasterPort(QSharedPointer<TransactionalPort>());
                transactional->setSlavePort(QSharedPointer<TransactionalPort>());
                transactional->getSystemPorts()->clear();

                // Save the port for the first mode.
                savePort(currentPort, i);

                // Save different modes for the port abstraction.
                for (int j = i + 1; j < itemCount(); j++)
                {
                    if (signals_.at(j)->abstraction_ == currentPort)
                    {
                        savePort(currentPort, j);
                    }
                }
            }

            savedPorts.append(currentPort);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::savePort()
//-----------------------------------------------------------------------------
void PortAbstractionInterface::savePort(QSharedPointer<PortAbstraction> portAbs, int i)
{
    QSharedPointer<PortAbstractionInterface::SignalRow> sourceSignal = signals_.at(i);

    if (sourceSignal->wire_)
    {
        // Set the wirePort placement according to the mode in the table.
        if (sourceSignal->mode_ == General::MASTER)
        {
            portAbs->getWire()->setMasterPort(sourceSignal->wire_);
        }
        else if (sourceSignal->mode_ == General::SLAVE)
        {
            portAbs->getWire()->setSlavePort(sourceSignal->wire_);
        }
        else if (sourceSignal->mode_ == General::SYSTEM)
        {
            portAbs->getWire()->addSystemPort(sourceSignal->wire_);
        }
    }
    if (sourceSignal->transactional_)
    {
        if (sourceSignal->mode_ == General::MASTER)
        {
            portAbs->getTransactional()->setMasterPort(sourceSignal->transactional_);
        }
        else if (sourceSignal->mode_ == General::SLAVE)
        {
            portAbs->getTransactional()->setSlavePort(sourceSignal->transactional_);
        }
        else if (sourceSignal->mode_ == General::SYSTEM)
        {
            portAbs->getTransactional()->addSystemPort(sourceSignal->transactional_);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionInterface::portHasMode()
//-----------------------------------------------------------------------------
bool PortAbstractionInterface::portHasMode(std::string const& portName, std::string const& interfaceMode,
    std::string const& systemGroup) const
{
    General::InterfaceMode busMode =
        General::str2Interfacemode(QString::fromStdString(interfaceMode), General::INTERFACE_MODE_COUNT);

    return modeExistsForPort(busMode, QString::fromStdString(portName), QString::fromStdString(systemGroup));
}
