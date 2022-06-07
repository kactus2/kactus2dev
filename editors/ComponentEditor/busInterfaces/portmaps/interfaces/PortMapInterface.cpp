//-----------------------------------------------------------------------------
// File: PortMapInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 22.10.2020
//
// Description:
// Interface for editing port maps.
//-----------------------------------------------------------------------------

#include "PortMapInterface.h"

#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/TransactionalAbstraction.h>
#include <IPXACTmodels/common/PresenceTypes.h>
#include <IPXACTmodels/common/ConfigurableVLNVReference.h>
#include <IPXACTmodels/Component/validators/PortMapValidator.h>
#include <IPXACTmodels/Component/AbstractionType.h>
#include <IPXACTmodels/Component/Port.h>

#include <editors/ComponentEditor/ports/interfaces/PortsInterface.h>
#include <editors/BusDefinitionEditor/interfaces/PortAbstractionInterface.h>

#include <QVector>
#include <QString>

namespace
{
    std::string const PORTMAP_TYPE = "portMap";

    QString const MULTIPLE_SELECTED = "[multiple]";
    QString const UNKNOWN_PORT = "Unknown";
};

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::ComponentInstantiationInterface()
//-----------------------------------------------------------------------------
PortMapInterface::PortMapInterface(QSharedPointer<PortMapValidator> validator,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ExpressionFormatter> expressionFormatter,
    PortsInterface* physicalPortInterface, PortAbstractionInterface* logicalPortInterface):
ParameterizableInterface(expressionParser, expressionFormatter),
CommonInterface(),
portMaps_(),
validator_(validator),
abstractionDef_(),
interfaceMode_(),
systemGroup_(""),
logicalPortInterface_(logicalPortInterface),
physicalPortInterface_(physicalPortInterface)
{

}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::setupLogicalPorts()
//-----------------------------------------------------------------------------
void PortMapInterface::setupAbstractionDefinition(QSharedPointer<AbstractionDefinition const> absDef)
{
    abstractionDef_ = absDef;

    logicalPortInterface_->setAbsDef(absDef);

    validator_->abstractionDefinitionChanged(absDef, interfaceMode_);
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::setupPhysicalPorts()
//-----------------------------------------------------------------------------
void PortMapInterface::setupPhysicalPorts(QSharedPointer<Component> component)
{
    physicalPortInterface_->setPorts(component);
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::setupPortMaps()
//-----------------------------------------------------------------------------
void PortMapInterface::setupPortMaps(QSharedPointer<AbstractionType> abstractionType)
{
    portMaps_ = abstractionType->getPortMaps();
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::setupBusMode()
//-----------------------------------------------------------------------------
void PortMapInterface::setupBusMode(General::InterfaceMode busMode)
{
    interfaceMode_ = busMode;

    if (abstractionDef_)
    {
        validator_->abstractionDefinitionChanged(abstractionDef_, interfaceMode_);
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::setupSystemGroup()
//-----------------------------------------------------------------------------
void PortMapInterface::setupSystemGroup(QString const& systemGroup)
{
    systemGroup_ = systemGroup.toStdString();
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getIndexedItemName()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getIndexedItemName(int const& itemIndex) const
{
    return getLogicalPortName(itemIndex);
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::itemCount()
//-----------------------------------------------------------------------------
int PortMapInterface::itemCount() const
{
    int mapCount = 0;
    if (portMaps_)
    {
        mapCount = portMaps_->size();
    }

    return mapCount;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getItemNames()
//-----------------------------------------------------------------------------
std::vector<std::string> PortMapInterface::getItemNames() const
{
    std::vector<std::string> names;

    for (int i = 0; i < itemCount(); ++i)
    {
        std::string currentPortName = getLogicalPortName(i);
        if (std::find(names.cbegin(), names.cend(), currentPortName) != names.cend())
        {
            names.push_back(currentPortName);
        }
    }

    return names;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getLogicalPortName()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getLogicalPortName(int const& portMapIndex) const
{
    std::string logicalPortName("");

    QSharedPointer<PortMap> map = getPortMap(portMapIndex);
    if (map && map->getLogicalPort())
    {
        logicalPortName = map->getLogicalPort()->name_.toStdString();
    }

    return logicalPortName;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::setLogicalPort()
//-----------------------------------------------------------------------------
bool PortMapInterface::setLogicalPort(int const& portMapIndex, std::string const& newName)
{
    std::string busModeString(General::interfaceMode2Str(interfaceMode_).toStdString());

    QSharedPointer<PortMap> map = getPortMap(portMapIndex);
    if (map && logicalPortInterface_->portHasMode(newName, busModeString, systemGroup_))
    {
        QSharedPointer<PortMap::LogicalPort> logicalPort = map->getLogicalPort();
        if (!logicalPort && !newName.empty())
        {
            logicalPort = QSharedPointer<PortMap::LogicalPort>(new PortMap::LogicalPort());
            map->setLogicalPort(logicalPort);
        }

        logicalPort->name_ = QString::fromStdString(newName);

        if (newName.empty())
        {
            removeEmptyLogicalPort(map);
        }

        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::removeEmptyLogicalPort()
//-----------------------------------------------------------------------------
bool PortMapInterface::removeEmptyLogicalPort(QSharedPointer<PortMap> editedPortMap)
{
    QSharedPointer<PortMap::LogicalPort> mappedLogical = editedPortMap->getLogicalPort();
    if (mappedLogical->name_.isEmpty() && !mappedLogical->range_)
    {
        editedPortMap->setLogicalPort(QSharedPointer<PortMap::LogicalPort>());
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::hasPhysicalPort()
//-----------------------------------------------------------------------------
bool PortMapInterface::hasPhysicalPort(int const& portMapIndex) const
{
    if (portMapIndex >= 0 && portMapIndex < itemCount() && portMaps_->at(portMapIndex)->getPhysicalPort())
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getPhysicalPortName()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getPhysicalPortName(int const& portMapIndex) const
{
    QString physicalName("");

    QSharedPointer<PortMap> portMap = getPortMap(portMapIndex);
    if (portMap)
    {
        QSharedPointer<PortMap::PhysicalPort> physicalPort = portMap->getPhysicalPort();
        if (physicalPort)
        {
            physicalName = physicalPort->name_;
        }
    }

    return physicalName.toStdString();
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::setPhysicalPort()
//-----------------------------------------------------------------------------
bool PortMapInterface::setPhysicalPort(int const& portMapIndex, std::string const& newPhysicalName)
{
    QSharedPointer<PortMap> editedPortMap = getPortMap(portMapIndex);
    if (!editedPortMap)
    {
        return false;
    }

    QSharedPointer<PortMap::PhysicalPort> physicalPort = editedPortMap->getPhysicalPort();
    if (!physicalPort)
    {
        physicalPort = QSharedPointer<PortMap::PhysicalPort>(
            new PortMap::PhysicalPort(QString::fromStdString(newPhysicalName)));

        editedPortMap->setPhysicalPort(physicalPort);
    }
    else
    {
        physicalPort->name_ = QString::fromStdString(newPhysicalName);
    }

    if (newPhysicalName.empty())
    {
        removeEmptyPhysicalPort(editedPortMap);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::removeEmptyPhysicalPort()
//-----------------------------------------------------------------------------
bool PortMapInterface::removeEmptyPhysicalPort(QSharedPointer<PortMap> editedPortMap)
{
    QSharedPointer<PortMap::PhysicalPort> physicalPort = editedPortMap->getPhysicalPort();
    if (physicalPort)
    {
        if (removeEmptyPhysicalPartSelect(physicalPort) && physicalPort->name_.isEmpty())
        {
            editedPortMap->getPhysicalPort().clear();
            editedPortMap->setPhysicalPort(QSharedPointer<PortMap::PhysicalPort>());
        }
        else
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::removeEmptyPhysicalPartSelect()
//-----------------------------------------------------------------------------
bool PortMapInterface::removeEmptyPhysicalPartSelect(QSharedPointer<PortMap::PhysicalPort> physicalPort)
{
    if (physicalPort->partSelect_)
    {
        QSharedPointer<PartSelect> physicalPart = physicalPort->partSelect_;
        if (physicalPart->getIndices()->isEmpty() &&
            physicalPart->getLeftRange().isEmpty() && physicalPart->getRightRange().isEmpty())
        {
            physicalPort->partSelect_.clear();
            physicalPort->partSelect_ = QSharedPointer<PartSelect>();
        }
        else
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getLogicalPresence()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getLogicalPresence(int const& portMapIndex)
{
    std::string logicalPortName = getLogicalPortName(portMapIndex);
    return getLogicalPresenceWithLogicalPort(logicalPortName);
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getLogicalPresenceWithLogicalPort()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getLogicalPresenceWithLogicalPort(std::string const& logicalName)
{
    std::string busMode = General::interfaceMode2Str(interfaceMode_).toStdString();

    PresenceTypes::Presence requirement =
        logicalPortInterface_->getPresence(logicalName, busMode, systemGroup_);
    if (requirement == PresenceTypes::UNKNOWN)
    {
        requirement = PresenceTypes::OPTIONAL;
    }

    return PresenceTypes::presence2Str(requirement).toStdString();
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getInvertString()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getInvertString(int const& portMapIndex) const
{
    BooleanValue invertValue;
    QSharedPointer<PortMap> map = getPortMap(portMapIndex);
    if (map)
    {
        invertValue = map->getInvert();
    }

    return invertValue.toString().toStdString();
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getInvertValue()
//-----------------------------------------------------------------------------
BooleanValue PortMapInterface::getInvertValue(int const& portMapIndex) const
{
    QSharedPointer<PortMap> map = getPortMap(portMapIndex);
    if (map)
    {
        return map->getInvert();
    }

    return BooleanValue();
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getInvertBool()
//-----------------------------------------------------------------------------
bool PortMapInterface::getInvertBool(int const& portMapIndex) const
{
    QSharedPointer<PortMap> map = getPortMap(portMapIndex);
    if (map)
    {
        return map->getInvert().toBool();
    }
    
    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::setInvertValue()
//-----------------------------------------------------------------------------
bool PortMapInterface::setInvertValue(int const& portMapIndex, bool newInvertValue) const
{
    QSharedPointer<PortMap> map = getPortMap(portMapIndex);
    if (!map)
    {
        return false;
    }

    map->setInvert(newInvertValue);
    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getIsInformativeString()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getIsInformativeString(int const& portMapIndex) const
{
    BooleanValue isInfo;
    QSharedPointer<PortMap> map = getPortMap(portMapIndex);
    if (map)
    {
        isInfo = map->getIsInformative();
    }

    return isInfo.toString().toStdString();
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getIsInformativeValue()
//-----------------------------------------------------------------------------
bool PortMapInterface::getIsInformativeValue(int const& portMapIndex) const
{
    QSharedPointer<PortMap> map = getPortMap(portMapIndex);
    if (map)
    {
        return map->getIsInformative().toBool();
    }
    
    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::setIsInformativeValue()
//-----------------------------------------------------------------------------
bool PortMapInterface::setIsInformativeValue(int const& portMapIndex, bool newIsInformativeValue) const
{
    QSharedPointer<PortMap> editedPortMap = getPortMap(portMapIndex);
    if (!editedPortMap)
    {
        return false;
    }

    editedPortMap->setIsInformative(newIsInformativeValue);
    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getLogicalTieOffValue()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getLogicalTieOffValue(int const& portMapIndex, int const& baseNumber) const
{
    QString logicalTieOff("");
    QSharedPointer<PortMap> selectedPortMap = getPortMap(portMapIndex);
    if (selectedPortMap)
    {
        logicalTieOff = selectedPortMap->getLogicalTieOff();
    }

    return parseExpressionToBaseNumber(logicalTieOff, baseNumber).toStdString();
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getLogicalTieOffFormattedExpression()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getLogicalTieOffFormattedExpression(int const& portMapIndex) const
{
    QString logicalTieOff("");
    QSharedPointer<PortMap> selectedPortMap = getPortMap(portMapIndex);
    if (selectedPortMap)
    {
        logicalTieOff = selectedPortMap->getLogicalTieOff();
    }

    return formattedValueFor(logicalTieOff).toStdString();
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getLogicalTieOffExpression()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getLogicalTieOffExpression(int const& portMapIndex) const
{
    QSharedPointer<PortMap> selectedPortMap = getPortMap(portMapIndex);
    if (selectedPortMap)
    {
        return selectedPortMap->getLogicalTieOff().toStdString();
    }

    return ("");
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::setLogicalTieOff()
//-----------------------------------------------------------------------------
bool PortMapInterface::setLogicalTieOff(int const& portMapIndex, std::string const& newTieOff)
{
    QSharedPointer<PortMap> portMap = getPortMap(portMapIndex);
    if (!portMap)
    {
        return false;
    }

    portMap->setLogicalTieOff(QString::fromStdString(newTieOff));
    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getLogicalLeftBoundValue()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getLogicalLeftBoundValue(int const& portMapIndex, int const& baseNumber) const
{
    QSharedPointer<PortMap> selectedPortMap = getPortMap(portMapIndex);
    if (selectedPortMap && selectedPortMap->getLogicalPort() && selectedPortMap->getLogicalPort()->range_)
    {
        return parseExpressionToBaseNumber(selectedPortMap->getLogicalPort()->range_->getLeft(), baseNumber).
            toStdString();
    }

    return ("");
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getLogicalLeftBoundFormattedExpression()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getLogicalLeftBoundFormattedExpression(int const& portMapIndex) const
{
    QSharedPointer<PortMap> selectedPortMap = getPortMap(portMapIndex);
    if (selectedPortMap && selectedPortMap->getLogicalPort() && selectedPortMap->getLogicalPort()->range_)
    {
        return formattedValueFor(selectedPortMap->getLogicalPort()->range_->getLeft()).toStdString();
    }

    return ("");
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getLogicalLeftBoundExpression()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getLogicalLeftBoundExpression(int const& portMapIndex) const
{
    QSharedPointer<PortMap> selectedPortMap = getPortMap(portMapIndex);
    if (selectedPortMap && selectedPortMap->getLogicalPort() && selectedPortMap->getLogicalPort()->range_)
    {
        return selectedPortMap->getLogicalPort()->range_->getLeft().toStdString();
    }

    return ("");
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::setLogicalLeftBound()
//-----------------------------------------------------------------------------
bool PortMapInterface::setLogicalLeftBound(int const& portMapIndex, std::string const& newLogicalLeft)
{
    QSharedPointer<PortMap> selectedPortMap = getPortMap(portMapIndex);
    if (!selectedPortMap || !selectedPortMap->getLogicalPort())
    {
        return false;
    }

    QSharedPointer<PortMap::LogicalPort> logicalPort = selectedPortMap->getLogicalPort();
    if (!logicalPort->range_ && !newLogicalLeft.empty())
    {
        QSharedPointer<Range> newRange(new Range(QString::fromStdString(newLogicalLeft), ""));
        logicalPort->range_ = newRange;
    }
    else if (logicalPort->range_)
    {
        logicalPort->range_->setLeft(QString::fromStdString(newLogicalLeft));
    }

    if (newLogicalLeft.empty())
    {
        if (removeEmptyRange(logicalPort))
        {
            removeEmptyLogicalPort(selectedPortMap);
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::removeEmptyRange()
//-----------------------------------------------------------------------------
bool PortMapInterface::removeEmptyRange(QSharedPointer<PortMap::LogicalPort> editedPortLogical)
{
    if (editedPortLogical->range_ && editedPortLogical->range_->getLeft().isEmpty() &&
        editedPortLogical->range_->getRight().isEmpty())
    {
        editedPortLogical->range_.clear();
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getLogicalRightBoundValue()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getLogicalRightBoundValue(int const& portMapIndex, int const& baseNumber) const
{
    QSharedPointer<PortMap> selectedPortMap = getPortMap(portMapIndex);
    if (selectedPortMap && selectedPortMap->getLogicalPort() && selectedPortMap->getLogicalPort()->range_)
    {
        return parseExpressionToBaseNumber(
            selectedPortMap->getLogicalPort()->range_->getRight(), baseNumber).toStdString();
    }

    return ("");
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getLogicalRightBoundFormattedExpression()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getLogicalRightBoundFormattedExpression(int const& portMapIndex) const
{
    QSharedPointer<PortMap> selectedPortMap = getPortMap(portMapIndex);
    if (selectedPortMap && selectedPortMap->getLogicalPort() && selectedPortMap->getLogicalPort()->range_)
    {
        QString logicalRightBound(selectedPortMap->getLogicalPort()->range_->getRight());
        return formattedValueFor(logicalRightBound).toStdString();
    }

    return ("");
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getLogicalRightBoundExpression()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getLogicalRightBoundExpression(int const& portMapIndex) const
{
    QSharedPointer<PortMap> selectedPortMap = getPortMap(portMapIndex);
    if (selectedPortMap && selectedPortMap->getLogicalPort() && selectedPortMap->getLogicalPort()->range_)
    {
        QString logicalRightBound(selectedPortMap->getLogicalPort()->range_->getRight());
        return logicalRightBound.toStdString();
    }

    return ("");
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::setLogicalRightBound()
//-----------------------------------------------------------------------------
bool PortMapInterface::setLogicalRightBound(int const& portMapIndex, std::string const& newLogicalRight)
{
    QSharedPointer<PortMap> selectedPortMap = getPortMap(portMapIndex);
    if (!selectedPortMap || !selectedPortMap->getLogicalPort())
    {
        return false;
    }

    QSharedPointer<PortMap::LogicalPort> logicalPort = selectedPortMap->getLogicalPort();
    if (!logicalPort->range_ && !newLogicalRight.empty())
    {
        QSharedPointer<Range> newRange(new Range("", QString::fromStdString(newLogicalRight)));
        logicalPort->range_ = newRange;
    }
    else if (logicalPort->range_)
    {
        logicalPort->range_->setRight(QString::fromStdString(newLogicalRight));
    }

    if (newLogicalRight.empty())
    {
        if (removeEmptyRange(logicalPort))
        {
            removeEmptyLogicalPort(selectedPortMap);
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getPhysicalLeftBoundValue()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getPhysicalLeftBoundValue(int const& portMapIndex, int const& baseNumber) const
{
    QSharedPointer<PortMap> selectedPortMap = getPortMap(portMapIndex);
    if (selectedPortMap && selectedPortMap->getPhysicalPort() && selectedPortMap->getPhysicalPort()->partSelect_)
    {
        QString physicalLeftBound(selectedPortMap->getPhysicalPort()->partSelect_->getLeftRange());
        return parseExpressionToBaseNumber(physicalLeftBound, baseNumber).toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getPhysicalLeftBoundFormattedExpression()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getPhysicalLeftBoundFormattedExpression(int const& portMapIndex) const
{
    QSharedPointer<PortMap> selectedPortMap = getPortMap(portMapIndex);
    if (selectedPortMap && selectedPortMap->getPhysicalPort() && selectedPortMap->getPhysicalPort()->partSelect_)
    {
        QString physicalLeftBound(selectedPortMap->getPhysicalPort()->partSelect_->getLeftRange());
        return formattedValueFor(physicalLeftBound).toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getPhysicalLefttBoundExpression()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getPhysicalLeftBoundExpression(int const& portMapIndex) const
{
    QSharedPointer<PortMap> selectedPortMap = getPortMap(portMapIndex);
    if (selectedPortMap && selectedPortMap->getPhysicalPort() && selectedPortMap->getPhysicalPort()->partSelect_)
    {
        QString physicalLeftBound(selectedPortMap->getPhysicalPort()->partSelect_->getLeftRange());
        return physicalLeftBound.toStdString();
    }

    return ("");
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::setPhysicalLeftBound()
//-----------------------------------------------------------------------------
bool PortMapInterface::setPhysicalLeftBound(int const& portMapIndex, std::string const& newPhysicalLeft)
{
    QSharedPointer<PartSelect> physicalPart = getPhysicalPartSelect(portMapIndex);
    if (physicalPart)
    {
        physicalPart->setLeftRange(QString::fromStdString(newPhysicalLeft));
    }

    if (newPhysicalLeft.empty())
    {
        removeEmptyPhysicalPort(getPortMap(portMapIndex));
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getPhysicalPartSelect()
//-----------------------------------------------------------------------------
QSharedPointer<PartSelect> PortMapInterface::getPhysicalPartSelect(int const& portMapIndex)
{
    QSharedPointer<PortMap> portMap = getPortMap(portMapIndex);
    if (!portMap)
    {
        return QSharedPointer<PartSelect>();
    }

    QSharedPointer<PortMap::PhysicalPort> physicalPort = portMap->getPhysicalPort();
    if (!physicalPort)
    {
        physicalPort = QSharedPointer<PortMap::PhysicalPort>(new PortMap::PhysicalPort());
        portMap->setPhysicalPort(physicalPort);
    }

    if (!physicalPort->partSelect_)
    {
        QSharedPointer<PartSelect> newPartSelect(new PartSelect());
        physicalPort->partSelect_ = newPartSelect;
    }

    return physicalPort->partSelect_;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getPhysicalRightBoundValue()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getPhysicalRightBoundValue(int const& portMapIndex, int const& baseNumber) const
{
    QSharedPointer<PortMap> selectedPortMap = getPortMap(portMapIndex);
    if (selectedPortMap && selectedPortMap->getPhysicalPort() && selectedPortMap->getPhysicalPort()->partSelect_)
    {
        QString physicalRightBound(selectedPortMap->getPhysicalPort()->partSelect_->getRightRange());
        return parseExpressionToBaseNumber(physicalRightBound, baseNumber).toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getPhysicalRightBoundFormattedExpression()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getPhysicalRightBoundFormattedExpression(int const& portMapIndex) const
{
    QSharedPointer<PortMap> selectedPortMap = getPortMap(portMapIndex);
    if (selectedPortMap && selectedPortMap->getPhysicalPort() && selectedPortMap->getPhysicalPort()->partSelect_)
    {
        QString physicalRightBound(selectedPortMap->getPhysicalPort()->partSelect_->getRightRange());
        return formattedValueFor(physicalRightBound).toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getPhysicalRightBoundExpression()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getPhysicalRightBoundExpression(int const& portMapIndex) const
{
    QSharedPointer<PortMap> selectedPortMap = getPortMap(portMapIndex);
    if (selectedPortMap && selectedPortMap->getPhysicalPort() && selectedPortMap->getPhysicalPort()->partSelect_)
    {
        QString physicalRightBound(selectedPortMap->getPhysicalPort()->partSelect_->getRightRange());
        return physicalRightBound.toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::setPhysicalRightBound()
//-----------------------------------------------------------------------------
bool PortMapInterface::setPhysicalRightBound(int const& portMapIndex, std::string const& newPhysicalRight)
{
    QSharedPointer<PartSelect> physicalPart = getPhysicalPartSelect(portMapIndex);
    if (physicalPart)
    {
        physicalPart->setRightRange(QString::fromStdString(newPhysicalRight));
    }

    if (newPhysicalRight.empty())
    {
        removeEmptyPhysicalPort(getPortMap(portMapIndex));
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getLogicalPortIconPath()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getLogicalPortIconPath(int const& portMapIndex) const
{
    std::string mappingPath(":icons/common/graphics/cross.png");
    std::string busMode(General::interfaceMode2Str(interfaceMode_).toStdString());

    QSharedPointer<PortMap> map = getPortMap(portMapIndex);
    if (map)
    {
        std::string logicalPortName(getLogicalPortName(portMapIndex));
        if (logicalPortInterface_->portHasMode(logicalPortName, busMode, systemGroup_))
        {
            mappingPath = logicalPortInterface_->getIconPathForSignal(logicalPortName, busMode, systemGroup_);
        }
    }

    return mappingPath;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getPhysicalPortIconPath()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getPhysicalPortIconPath(int const& portMapIndex) const
{
    std::string iconPath("");

    QSharedPointer<PortMap> portMap = getPortMap(portMapIndex);
    if (portMap && portMap->getPhysicalPort())
    {
        std::string portName(getPhysicalPortName(portMapIndex));
        iconPath = physicalPortInterface_->getIconPathForPort(portName);
    }

    return iconPath;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::logicalPortExists()
//-----------------------------------------------------------------------------
bool PortMapInterface::logicalPortExists(int const& portMapIndex) const
{
    std::string interfaceMode = General::interfaceMode2Str(interfaceMode_).toStdString();
    std::string logicalPortName = getLogicalPortName(portMapIndex);
    return logicalPortInterface_->portHasMode(logicalPortName, interfaceMode, systemGroup_);
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getAllReferencesToIdInItem()
//-----------------------------------------------------------------------------
int PortMapInterface::getAllReferencesToIdInItem(const std::string& itemName, std::string const&  valueID) const
{
    int totalReferencesToParameter = 0;

    QList<QSharedPointer<PortMap> > logicalMaps;
    for (int i = 0; i < itemCount(); ++i)
    {
        if (getLogicalPortName(i) == itemName)
        {
            logicalMaps.append(portMaps_->at(i));
        }
    }

    for (auto portMap : logicalMaps)
    {
        QString idString(QString::fromStdString(valueID));

        int referencesInLogicalPort = 0;
        QSharedPointer<PortMap::LogicalPort> logicalPort = portMap->getLogicalPort();
        if (logicalPort && logicalPort->range_)
        {
            referencesInLogicalPort += logicalPort->range_->getLeft().count(idString);
            referencesInLogicalPort += logicalPort->range_->getRight().count(idString);
        }

        int referencesInPhysicalPort = 0;
        QSharedPointer<PortMap::PhysicalPort> physicalPort = portMap->getPhysicalPort();
        if (physicalPort && physicalPort->partSelect_)
        {
            QSharedPointer<PartSelect> physicalPart = physicalPort->partSelect_;

            referencesInPhysicalPort += physicalPart->getLeftRange().count(idString);
            referencesInPhysicalPort += physicalPart->getRightRange().count(idString);

            if (physicalPart->getIndices())
            {
                for (auto index : *physicalPart->getIndices())
                {
                    referencesInPhysicalPort += index.count(idString);
                }
            }
        }

        int referencesInTieOff = portMap->getLogicalTieOff().count(idString);

        totalReferencesToParameter = referencesInLogicalPort + referencesInPhysicalPort + referencesInTieOff;
    }

    return totalReferencesToParameter;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getExpressionsInSelectedPortMap()
//-----------------------------------------------------------------------------
std::vector<std::string> PortMapInterface::getExpressionsInSelectedPortMap(int const& portMapIndex) const
{
    std::vector<std::string> expressionList;

    QSharedPointer<PortMap> portMap = getPortMap(portMapIndex);
    if (portMap)
    {
        QSharedPointer<PortMap::LogicalPort> logicalPort = portMap->getLogicalPort();
        if (logicalPort && logicalPort->range_)
        {
            expressionList.push_back(logicalPort->range_->getLeft().toStdString());
            expressionList.push_back(logicalPort->range_->getRight().toStdString());
        }

        QSharedPointer<PortMap::PhysicalPort> physicalPort = portMap->getPhysicalPort();
        if (physicalPort && physicalPort->partSelect_)
        {
            QSharedPointer<PartSelect> physicalPart = physicalPort->partSelect_;

            expressionList.push_back(physicalPart->getLeftRange().toStdString());
            expressionList.push_back(physicalPart->getRightRange().toStdString());

            if (physicalPart->getIndices())
            {
                for (auto index : *physicalPart->getIndices())
                {
                    expressionList.push_back(index.toStdString());
                }
            }
        }

        expressionList.push_back(portMap->getLogicalTieOff().toStdString());
    }

    return expressionList;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::validateItems()
//-----------------------------------------------------------------------------
bool PortMapInterface::validateItems() const
{
    for (auto portMap : *portMaps_)
    {
        if (!validator_->validate(portMap))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::logicalPortIsValid()
//-----------------------------------------------------------------------------
bool PortMapInterface::logicalPortIsValid(int const& portMapIndex) const
{
    QSharedPointer<PortMap> validatedPortMap = getPortMap(portMapIndex);
    if (validatedPortMap)
    {
        return validator_->hasValidLogicalPort(validatedPortMap->getLogicalPort());
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::physicalPortIsValid()
//-----------------------------------------------------------------------------
bool PortMapInterface::physicalPortIsValid(int const& portMapIndex) const
{
    QSharedPointer<PortMap> validatedPortMap = getPortMap(portMapIndex);
    if (validatedPortMap)
    {
        return validator_->hasValidPhysicalPort(validatedPortMap->getPhysicalPort());
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::physicalPortIsValid()
//-----------------------------------------------------------------------------
bool PortMapInterface::physicalMappingIsValid(int const& portMapIndex) const
{
    QSharedPointer<PortMap> validatedPortMap = getPortMap(portMapIndex);
    if (validatedPortMap)
    {
        return validator_->hasValidPhysicalMapping(validatedPortMap);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::connectedPortsHaveValidPortTypes()
//-----------------------------------------------------------------------------
bool PortMapInterface::connectedPortsHaveValidPortTypes(int const& portMapIndex) const
{
    QSharedPointer<PortMap> validatedPortMap = getPortMap(portMapIndex);
    if (validatedPortMap)
    {
        std::string physicalPortName(getPhysicalPortName(portMapIndex));
        std::string logicalPortName(getLogicalPortName(portMapIndex));

        QSharedPointer<Port> physicalPortPointer = physicalPortInterface_->getPort(physicalPortName);
        QSharedPointer<PortAbstraction> logicalPortPointer = logicalPortInterface_->getPort(logicalPortName);

        return validator_->connectedPortsHaveValidPortTypes(logicalPortPointer, physicalPortPointer);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::connectedPortsHaveValidDirections()
//-----------------------------------------------------------------------------
bool PortMapInterface::connectedPortsHaveValidDirections(int const& portMapIndex) const
{
    QSharedPointer<PortMap> validatedPortMap = getPortMap(portMapIndex);
    if (validatedPortMap)
    {
        std::string physicalPortName(getPhysicalPortName(portMapIndex));
        std::string logicalPortName(getLogicalPortName(portMapIndex));

        QSharedPointer<Port> physicalPortPointer = physicalPortInterface_->getPort(physicalPortName);
        QSharedPointer<PortAbstraction> logicalPortPointer = logicalPortInterface_->getPort(logicalPortName);

        return validator_->connectedPortsHaveValidDirections(logicalPortPointer, physicalPortPointer);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::connectedPortsHaveValidInitiatives()
//-----------------------------------------------------------------------------
bool PortMapInterface::connectedPortsHaveValidInitiatives(int const& portMapIndex) const
{
    QSharedPointer<PortMap> validatedPortMap = getPortMap(portMapIndex);
    if (validatedPortMap)
    {
        std::string physicalPortName(getPhysicalPortName(portMapIndex));
        std::string logicalPortName(getLogicalPortName(portMapIndex));

        QSharedPointer<Port> physicalPortPointer = physicalPortInterface_->getPort(physicalPortName);
        QSharedPointer<PortAbstraction> logicalPortPointer = logicalPortInterface_->getPort(logicalPortName);

        return validator_->connectedPortsHaveValidInitiatives(logicalPortPointer, physicalPortPointer);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::connectedPortsHaveSameRange()
//-----------------------------------------------------------------------------
bool PortMapInterface::connectedPortsHaveSameRange(int const& portMapIndex) const
{
    QSharedPointer<PortMap> validatedPortMap = getPortMap(portMapIndex);
    if (validatedPortMap)
    {
        return validator_->connectedPortsHaveSameRange(
            validatedPortMap->getLogicalPort()->range_, validatedPortMap->getPhysicalPort()->partSelect_);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::tieOffIsValid()
//-----------------------------------------------------------------------------
bool PortMapInterface::tieOffIsValid(int const& portMapIndex) const
{
    QSharedPointer<PortMap> validatedPortMap = getPortMap(portMapIndex);
    if (validatedPortMap)
    {
        return validator_->hasValidTieOff(validatedPortMap->getLogicalTieOff());
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::logicalPortHasValidRange()
//-----------------------------------------------------------------------------
bool PortMapInterface::logicalPortHasValidRange(int const& portMapIndex) const
{
    QSharedPointer<PortMap> validatedPortMap = getPortMap(portMapIndex);
    if (validatedPortMap && validatedPortMap->getLogicalPort())
    {
        std::string logicalPortName(getLogicalPortName(portMapIndex));

        QSharedPointer<PortAbstraction> logicalPortPointer = logicalPortInterface_->getPort(logicalPortName);
        validator_->logicalPortHasValidRange(validatedPortMap->getLogicalPort()->range_, logicalPortPointer);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getComponentInstantiation()
//-----------------------------------------------------------------------------
QSharedPointer<PortMap> PortMapInterface::getPortMap(int const& portMapIndex) const
{
    if (portMaps_ && portMapIndex >= 0 && portMapIndex < portMaps_->size())
    {
        return portMaps_->at(portMapIndex);
    }

    return QSharedPointer<PortMap>();
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::mapPorts()
//-----------------------------------------------------------------------------
bool PortMapInterface::connectPorts(std::string const& logicalPortName, std::string const& physicalPortName)
{
    std::string interfaceMode(General::interfaceMode2Str(interfaceMode_).toStdString());
    std::string systemGroup(systemGroup_);

    if (logicalPortName.empty() || physicalPortName.empty() ||
        !logicalPortInterface_->portHasMode(logicalPortName, interfaceMode, systemGroup) ||
        !physicalPortInterface_->portExists(physicalPortName))
    {
        return false;
    }

    QSharedPointer<PortMap::PhysicalPort> newMappedPhysical(
        new PortMap::PhysicalPort(QString::fromStdString(physicalPortName)));
    QSharedPointer<PortMap::LogicalPort> newMappedLogical(
        new PortMap::LogicalPort(QString::fromStdString(logicalPortName)));
    
    QString logicalWireWidth(QString::fromStdString(
        logicalPortInterface_->getWidth(logicalPortName, interfaceMode, systemGroup)));
    if (!logicalWireWidth.isEmpty())
    {
        int logicalSize = parseExpressionToDecimal(logicalWireWidth).toInt();
        int logicalLeft = logicalSize - 1;
        int logicalRight = 0;
        
        int physicalLeft = std::stoi(physicalPortInterface_->getLeftBoundValue(physicalPortName));
        int physicalRight = std::stoi(physicalPortInterface_->getRightBoundValue(physicalPortName));
        int physicalSize = abs(physicalLeft - physicalRight) + 1;
        
        if (logicalSize != physicalSize)
        {
            if (physicalSize < logicalSize)
            {
                logicalLeft = logicalRight + physicalSize - 1;
            }
            else
            {
                if (physicalLeft < physicalRight)
                {
                    physicalRight = physicalLeft + logicalSize - 1;
                }
                else
                {
                    physicalLeft = physicalRight + logicalSize - 1;
                }
            }
        }
        
        newMappedLogical->range_ = QSharedPointer<Range>(
            new Range(QString::number(logicalLeft), QString::number(logicalRight)));
        
        newMappedPhysical->partSelect_ = QSharedPointer<PartSelect>(
            new PartSelect(QString::number(physicalLeft), QString::number(physicalRight)));
    }
    
    QSharedPointer<PortMap> newPortMap(new PortMap());
    newPortMap->setLogicalPort(newMappedLogical);
    newPortMap->setPhysicalPort(newMappedPhysical);


    portMaps_->append(newPortMap);

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::addPortMap()
//-----------------------------------------------------------------------------
void PortMapInterface::addPortMap(int const& row)
{
    QSharedPointer<PortMap> newPortMap(new PortMap());
    portMaps_->insert(row, newPortMap);
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::removePortMap()
//-----------------------------------------------------------------------------
bool PortMapInterface::removePortMap(int const& portMapIndex)
{
    QSharedPointer<PortMap> removedPortMap = getPortMap(portMapIndex);
    if (removedPortMap)
    {
        portMaps_->removeAll(removedPortMap);
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::removeAllPortMapsFromLogicalPort()
//-----------------------------------------------------------------------------
std::vector<std::string> PortMapInterface::removeAllPortMapsFromLogicalPort(std::string const& logicalPortName)
{
    std::vector<std::string> removedPhysicals;
    for (int i = itemCount() - 1; i >= 0; --i)
    {
        if (logicalPortName == getLogicalPortName(i))
        {
            std::string connectedPhysical(getPhysicalPortName(i));
            if (std::find(removedPhysicals.cbegin(), removedPhysicals.cend(), connectedPhysical) == removedPhysicals.cend())
            {
                removedPhysicals.push_back(connectedPhysical);
            }

            removePortMap(i);
        }
    }

    return removedPhysicals;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getPortMapPointer()
//-----------------------------------------------------------------------------
PortMap* PortMapInterface::getPortMapPointer(int const& portMapIndex) const
{
    PortMap* portMapPointer = nullptr;
    if (portMapIndex >= 0 && portMapIndex < itemCount())
    {
        portMapPointer = portMaps_->at(portMapIndex).data();
    }

    return portMapPointer;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getPhysicalPortInterface()
//-----------------------------------------------------------------------------
PortsInterface* PortMapInterface::getPhysicalPortInterface() const
{
    return physicalPortInterface_;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getLogicalPortInterface()
//-----------------------------------------------------------------------------
PortAbstractionInterface* PortMapInterface::getLogicalPortInterface() const
{
    return logicalPortInterface_;
}
