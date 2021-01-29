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

#include <QVector>
#include <QString>

namespace
{
    std::string const PORTMAP_TYPE = "portMap";

    QString const MULTIPLE_SELECTED = "[multiple]";
};

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::ComponentInstantiationInterface()
//-----------------------------------------------------------------------------
PortMapInterface::PortMapInterface(QSharedPointer<PortMapValidator> validator,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ExpressionFormatter> expressionFormatter,
    PortsInterface* portInterface):
ParameterizableInterface(expressionParser, expressionFormatter),
CommonInterface(),
validator_(validator),
portMappings_(),
abstraction_(),
interfaceMode_(General::MASTER),
systemGroup_(),
abstractionDef_(),
portInterface_(portInterface)
{

}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::setPortMaps()
//-----------------------------------------------------------------------------
void PortMapInterface::setPortMaps(QSharedPointer<AbstractionDefinition const> absDef,
    QSharedPointer<AbstractionType> abstraction, General::InterfaceMode interfaceMode, std::string systemGroup,
    QSharedPointer<Component> component)
{
    abstraction_ = abstraction;
    abstractionDef_ = absDef;
    portInterface_->setPorts(component);

    interfaceMode_ = interfaceMode;
    systemGroup_ = QString::fromStdString(systemGroup);

    portMappings_.clear();

    QSharedPointer<PortAbstraction> unconnectedPort(new PortAbstraction());
    unconnectedPort->setLogicalName("Unknown");

    QSharedPointer<PortMapping> unconnectedMapping(new PortMapping());
    unconnectedMapping->logicalPort_ = unconnectedPort;

    if (absDef)
    {
        foreach(QSharedPointer<PortAbstraction> logicalPort, *absDef->getLogicalPorts())
        {
            if (logicalPort->hasMode(interfaceMode, QString::fromStdString(systemGroup)))
            {
                QSharedPointer<PortMapping> newMapping(new PortMapping());
                newMapping->logicalPort_ = logicalPort;

                portMappings_.append(newMapping);
            }
        }
    }

    if (abstraction && abstraction->getPortMaps())
    {
        foreach(QSharedPointer<PortMap> currentMap, *abstraction->getPortMaps())
        {
            bool logicalPortFound = false;

            for (int mappingIndex = 0; mappingIndex < portMappings_.size(); ++mappingIndex)
            {
                if (portMappings_.at(mappingIndex)->logicalPort_->name() == currentMap->getLogicalPort()->name_)
                {
                    portMappings_[mappingIndex]->portMaps_.append(currentMap);
                    logicalPortFound = true;
                    break;;
                }
            }

            if (!logicalPortFound)
            {
                if (currentMap->getLogicalPort() && !currentMap->getLogicalPort()->name_.isEmpty())
                {
                    QSharedPointer<PortAbstraction> newAbstractionPort(new PortAbstraction());
                    newAbstractionPort->setLogicalName(currentMap->getLogicalPort()->name_);

                    QSharedPointer<PortMapping> newMapping(new PortMapping());
                    newMapping->logicalPort_ = newAbstractionPort;
                    newMapping->portMaps_.append(currentMap);

                    portMappings_.append(newMapping);
                }
                else
                {
                    unconnectedMapping->portMaps_.append(currentMap);
                }
            }
        }
    }

    portMappings_.append(unconnectedMapping);
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getIndexedItemName()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getIndexedItemName(int const& itemIndex) const
{
    std::string logicalPortName = "";

    if (itemIndex >= 0 && itemIndex < portMappings_.size())
    {
        QSharedPointer<PortMapping> mapping = portMappings_.at(itemIndex);
        if (mapping)
        {
            logicalPortName = mapping->logicalPort_->name().toStdString();
        }
    }

    return logicalPortName;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::itemCount()
//-----------------------------------------------------------------------------
int PortMapInterface::itemCount() const
{
    int mapCount = 0;
    for (auto mapping : portMappings_)
    {
        mapCount += mapping->portMaps_.count();
    }

    return mapCount;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::portMapCount()
//-----------------------------------------------------------------------------
int PortMapInterface::portMapCount(std::string const& logicalPortName) const
{
    int portMapsWithLogicalPort = 0;
    QSharedPointer<PortMapInterface::PortMapping> selectedMapping = getPortMapping(logicalPortName);
    if (selectedMapping)
    {
        return selectedMapping->portMaps_.count();
    }
    return portMapsWithLogicalPort;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::logicalPortCount()
//-----------------------------------------------------------------------------
int PortMapInterface::logicalPortCount() const
{
    return portMappings_.size();
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::mappingIndex()
//-----------------------------------------------------------------------------
int PortMapInterface::mappingIndex(std::string const& logicalPortName) const
{
    QString portName = QString::fromStdString(logicalPortName);
    for (int i = 0; i < portMappings_.size(); ++i)
    {
        QSharedPointer<PortMapInterface::PortMapping> mapping = portMappings_.at(i);
        if (mapping->logicalPort_->name() == portName)
        {
            return i;
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getItemNames()
//-----------------------------------------------------------------------------
std::vector<std::string> PortMapInterface::getItemNames() const
{
    QStringList qNames;
    for (auto mapping : portMappings_)
    {
        QString logicalName = mapping->logicalPort_->name();
        if (!qNames.contains(logicalName))
        {
            qNames.append(logicalName);
        }
    }

    std::vector<std::string> names;
    for (auto logicalName : qNames)
    {
        names.push_back(logicalName.toStdString());
    }

    return names;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getLogicalPortName()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getLogicalPortName(std::string const& logicalPortName, int const& portMapIndex) const
{
    QString logicalPortfinishedName = QString::fromStdString(logicalPortName);

    if (portMapIndex >= 0)
    {
        QSharedPointer<PortMap> portMap = getPortMap(logicalPortName, portMapIndex);
        if ((portMap && !portMap->getLogicalPort()) ||
            (portMap->getLogicalPort() && logicalPortfinishedName.isEmpty()))
        {
            logicalPortfinishedName = "Undefined";
        }

        QString numberText = " (" + QString::number(portMapIndex) + ")";
        logicalPortfinishedName.append(numberText);
    }

    return logicalPortfinishedName.toStdString();
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::hasLogicalPort()
//-----------------------------------------------------------------------------
bool PortMapInterface::hasLogicalPort(std::string const& logicalPortName) const
{
    QSharedPointer<PortMapInterface::PortMapping> mapping = getPortMapping(logicalPortName);
    if (mapping && mapping->logicalPort_)
    {
        return abstractionDef_->hasPort(mapping->logicalPort_->name(), interfaceMode_);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::setLogicalPort()
//-----------------------------------------------------------------------------
bool PortMapInterface::setLogicalPort(std::string const& currentName, int const& portMapIndex,
    std::string const& newName)
{
    QString newLogicalName = QString::fromStdString(newName);

    QSharedPointer<PortMap> editedPortMap = getPortMap(currentName, portMapIndex);
    if (!editedPortMap)
    {
        return false;
    }

    QSharedPointer<PortMap::LogicalPort> logicalPort = editedPortMap->getLogicalPort();
    if (!logicalPort)
    {
        logicalPort = QSharedPointer<PortMap::LogicalPort>(new PortMap::LogicalPort());
        editedPortMap->setLogicalPort(logicalPort);
    }

    if (logicalPort->name_.compare(newLogicalName, Qt::CaseInsensitive) != 0)
    {
        logicalPort->name_ = newLogicalName;

        QSharedPointer<PortMapInterface::PortMapping> oldMapping = getPortMapping(currentName);
        QSharedPointer<PortMapInterface::PortMapping> newMapping = getPortMapping(newName);
        QSharedPointer<PortMap> editedPortMap = getPortMap(currentName, portMapIndex);

        if (!editedPortMap || !oldMapping || !newMapping || oldMapping == newMapping)
        {
            return false;
        }

        oldMapping->portMaps_.removeOne(editedPortMap);
        newMapping->portMaps_.append(editedPortMap);

        if (logicalPort && logicalPort->name_.isEmpty() && (!logicalPort->range_ || (logicalPort->range_ &&
            logicalPort->range_->getLeft().isEmpty() && logicalPort->range_->getRight().isEmpty())))
        {
            logicalPort->range_.clear();
            editedPortMap->setLogicalPort(QSharedPointer<PortMap::LogicalPort>());
        }

        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::hasPhysicalPort()
//-----------------------------------------------------------------------------
bool PortMapInterface::hasPhysicalPort(std::string const& logicalPortName, int const& portMapIndex) const
{
    QSharedPointer<PortMap> portMap = getPortMap(logicalPortName, portMapIndex);
    if (portMap && portMap->getPhysicalPort())
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getPhysicalPortName()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getPhysicalPortName(std::string const& logicalPortName, int const& portMapIndex)
    const
{
    QString physicalName = "";

    QSharedPointer<PortMap> portMap = getPortMap(logicalPortName, portMapIndex);
    if (portMap)
    {
        QSharedPointer<PortMap::PhysicalPort> physicalPort = portMap->getPhysicalPort();
        if (physicalPort)
        {
            physicalName = physicalPort->name_;
        }
    }
    else
    {
        QSharedPointer<PortMapInterface::PortMapping> portMapping = getPortMapping(logicalPortName);
        if (portMapping)
        {
            QStringList physicalNames;
            for (auto portMap : portMapping->portMaps_)
            {
                if (portMap->getPhysicalPort() && !physicalNames.contains(portMap->getPhysicalPort()->name_))
                {
                    physicalNames.append(portMap->getPhysicalPort()->name_);
                }
            }
            if (physicalNames.size() == 1)
            {
                physicalName = physicalNames.first();
            }
            else if (physicalNames.size() > 1)
            {
                physicalName = MULTIPLE_SELECTED;
            }
        }
    }

    return physicalName.toStdString();
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::setPhysicalPort()
//-----------------------------------------------------------------------------
bool PortMapInterface::setPhysicalPort(std::string const& logicalName, int const& portMapIndex,
    std::string const& newPhysicalName)
{
    QSharedPointer<PortMap> editedPortMap = getPortMap(logicalName, portMapIndex);
    if (!editedPortMap)
    {
        editedPortMap = createNewPortMap(QString::fromStdString(logicalName));
        if (!editedPortMap)
        {
            return false;
        }
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
// Function: PortMapInterface::createNewPortMap()
//-----------------------------------------------------------------------------
QSharedPointer<PortMap> PortMapInterface::createNewPortMap(QString const& logicalPortName)
{
    QSharedPointer<PortMap> newPortMap;
    if (abstractionDef_->hasPort(logicalPortName, interfaceMode_))
    {
        newPortMap = QSharedPointer<PortMap>(new PortMap());

        QSharedPointer<PortMap::LogicalPort> newLogicalPort(new PortMap::LogicalPort(logicalPortName));
        newPortMap->setLogicalPort(newLogicalPort);

        abstraction_->getPortMaps()->append(newPortMap);

        QSharedPointer<PortMapping> mapping = getPortMapping(logicalPortName.toStdString());
        mapping->portMaps_.append(newPortMap);
    }

    return newPortMap;
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
std::string PortMapInterface::getLogicalPresence(std::string const& logicalPortName)
{
    PresenceTypes::Presence requirement = PresenceTypes::UNKNOWN;

    QSharedPointer<PortMapInterface::PortMapping> portMapping = getPortMapping(logicalPortName);
    if (portMapping)
    {
        QSharedPointer<PortAbstraction> abstractPort = portMapping->logicalPort_;
        if (abstractPort)
        {
            requirement = abstractPort->getPresence(interfaceMode_, systemGroup_);
            if (requirement == PresenceTypes::UNKNOWN)
            {
                requirement = PresenceTypes::OPTIONAL;
            }
        }
    }

    return PresenceTypes::presence2Str(requirement).toStdString();
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::invertExists()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getInvertString(std::string const& logicalPortName, int const& portMapIndex) const
{
    BooleanValue invertValue;
    QSharedPointer<PortMap> editedPortMap = getPortMap(logicalPortName, portMapIndex);
    if (editedPortMap)
    {
        invertValue = editedPortMap->getInvert();
    }
    else if (portMapIndex < 0)
    {
        QSharedPointer<PortMapping> mapping = getPortMapping(logicalPortName);
        if (mapping && mapping->portMaps_.count() > 0)
        {
            invertValue = mapping->portMaps_.first()->getInvert();
            for (auto map : mapping->portMaps_)
            {
                if (invertValue.toBool() != map->getInvert().toBool())
                {
                    return MULTIPLE_SELECTED.toStdString();
                }
            }
        }
    }

    return invertValue.toString().toStdString();
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getInvertValue()
//-----------------------------------------------------------------------------
bool PortMapInterface::getInvertValue(std::string const& logicalPortName, int const& portMapIndex) const
{
    QSharedPointer<PortMap> editedPortMap = getPortMap(logicalPortName, portMapIndex);
    if (editedPortMap)
    {
        return editedPortMap->getInvert().toBool();
    }
    
    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::setInvertValue()
//-----------------------------------------------------------------------------
bool PortMapInterface::setInvertValue(std::string const& logicalPortName, int const& portMapIndex,
    bool newInvertValue) const
{
    QSharedPointer<PortMap> editedPortMap = getPortMap(logicalPortName, portMapIndex);
    if (!editedPortMap)
    {
        return false;
    }

    editedPortMap->setInvert(newInvertValue);
    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getIsInformativeString()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getIsInformativeString(std::string const& logicalPortName, int const& portMapIndex)
const
{
    BooleanValue isInfo;
    QSharedPointer<PortMap> editedPortMap = getPortMap(logicalPortName, portMapIndex);
    if (editedPortMap)
    {
        isInfo = editedPortMap->getIsInformative();
    }
    else if (portMapIndex < 0)
    {
        QSharedPointer<PortMapping> mapping = getPortMapping(logicalPortName);
        if (mapping && mapping->portMaps_.count() > 0)
        {
            isInfo = mapping->portMaps_.first()->getIsInformative();
            for (auto map : mapping->portMaps_)
            {
                if (isInfo.toBool() != map->getIsInformative().toBool())
                {
                    return MULTIPLE_SELECTED.toStdString();
                }
            }
        }
    }

    return isInfo.toString().toStdString();
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getIsInformativeValue()
//-----------------------------------------------------------------------------
bool PortMapInterface::getIsInformativeValue(std::string const& logicalPortName, int const& portMapIndex) const
{
    QSharedPointer<PortMap> editedPortMap = getPortMap(logicalPortName, portMapIndex);
    if (editedPortMap)
    {
        return editedPortMap->getIsInformative().toBool();
    }
    
    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::setIsInformativeValue()
//-----------------------------------------------------------------------------
bool PortMapInterface::setIsInformativeValue(std::string const& logicalPortName, int const& portMapIndex,
    bool newIsInformativeValue) const
{
    QSharedPointer<PortMap> editedPortMap = getPortMap(logicalPortName, portMapIndex);
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
std::string PortMapInterface::getLogicalTieOffValue(std::string const& logicalPortName, int const& portMapIndex,
    int const& baseNumber) const
{
    QString logicalTieOff = "";
    QSharedPointer<PortMap> selectedPortMap = getPortMap(logicalPortName, portMapIndex);
    if (selectedPortMap)
    {
        logicalTieOff = selectedPortMap->getLogicalTieOff();
    }
    else if (portMapIndex < 0)
    {
        logicalTieOff = getTieOffForLogicalRow(logicalPortName);
        if (logicalTieOff == MULTIPLE_SELECTED)
        {
            return logicalTieOff.toStdString();
        }
    }

    return parseExpressionToBaseNumber(logicalTieOff, baseNumber).toStdString();
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getTieOffForLogicalRow()
//-----------------------------------------------------------------------------
QString PortMapInterface::getTieOffForLogicalRow(std::string const& logicalPortName) const
{
    QString aggregatedTieoff = "";
    QSharedPointer<PortMapInterface::PortMapping> mapping = getPortMapping(logicalPortName);
    if (mapping && !mapping->portMaps_.isEmpty())
    {
        aggregatedTieoff = mapping->portMaps_.first()->getLogicalTieOff();

        for (int i = 1; i < mapping->portMaps_.size(); ++i)
        {
            QSharedPointer<PortMap> map = mapping->portMaps_.at(i);
            if (map->getLogicalTieOff().compare(aggregatedTieoff) != 0)
            {
                return MULTIPLE_SELECTED;
            }
        }
    }

    return aggregatedTieoff;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getLogicalTieOffFormattedExpression()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getLogicalTieOffFormattedExpression(std::string const& logicalPortName,
    int const& portMapIndex) const
{
    QString logicalTieOff = "";
    QSharedPointer<PortMap> selectedPortMap = getPortMap(logicalPortName, portMapIndex);
    if (selectedPortMap)
    {
        logicalTieOff = selectedPortMap->getLogicalTieOff();
    }
    else if (portMapIndex < 0)
    {
        logicalTieOff = getTieOffForLogicalRow(logicalPortName);
        if (logicalTieOff == MULTIPLE_SELECTED)
        {
            return logicalTieOff.toStdString();
        }
    }

    return formattedValueFor(logicalTieOff).toStdString();
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getLogicalTieOffExpression()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getLogicalTieOffExpression(std::string const& logicalPortName,
    int const& portMapIndex) const
{
    QString logicalTieOff = "";
    QSharedPointer<PortMap> selectedPortMap = getPortMap(logicalPortName, portMapIndex);
    if (selectedPortMap)
    {
        logicalTieOff = selectedPortMap->getLogicalTieOff();
    }
    else if (portMapIndex < 0)
    {
        logicalTieOff = getTieOffForLogicalRow(logicalPortName);
    }

    return logicalTieOff.toStdString();
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::setLogicalTieOff()
//-----------------------------------------------------------------------------
bool PortMapInterface::setLogicalTieOff(std::string const& logicalPortName, int const& portMapIndex,
    std::string const& newTieOff)
{
    QSharedPointer<PortMap> portMap = getPortMap(logicalPortName, portMapIndex);
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
std::string PortMapInterface::getLogicalLeftBoundValue(std::string const& logicalPortName, int const& portMapIndex,
    int const& baseNumber) const
{
    QString logicalLeftBound = "";

    QSharedPointer<PortMap> selectedPortMap = getPortMap(logicalPortName, portMapIndex);
    if (selectedPortMap && selectedPortMap->getLogicalPort() && selectedPortMap->getLogicalPort()->range_)
    {
        logicalLeftBound =
            parseExpressionToBaseNumber(selectedPortMap->getLogicalPort()->range_->getLeft(), baseNumber);
    }
    else if (portMapIndex < 0)
    {
        logicalLeftBound = getLogicalLeftForLogicalPort(logicalPortName);
    }

    return logicalLeftBound.toStdString();
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getLogicalLeftForLogicalPort()
//-----------------------------------------------------------------------------
QString PortMapInterface::getLogicalLeftForLogicalPort(std::string const& logicalPortName) const
{
    QSharedPointer<PortMapInterface::PortMapping> mapping = getPortMapping(logicalPortName);
    if (mapping && mapping->logicalPort_)
    {
        QSharedPointer<PortAbstraction> logicalPort = mapping->logicalPort_;
        QString logicalWidth("");
        if (logicalPort->hasWire())
        {
            QSharedPointer<WireAbstraction> abstractWire = logicalPort->getWire();
            logicalWidth = abstractWire->getWidth(interfaceMode_, systemGroup_);
        }
        else if (logicalPort->hasTransactional())
        {
            QSharedPointer<TransactionalAbstraction> abstractTransactional = logicalPort->getTransactional();
            logicalWidth = abstractTransactional->getWidth(interfaceMode_, systemGroup_);
        }

        if (!logicalWidth.isEmpty())
        {
            QString logicalLeft = QString::number(parseExpressionToDecimal(logicalWidth).toInt() - 1);
            return logicalLeft;
        }
    }

    return QString("");
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getLogicalLeftBoundFormattedExpression()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getLogicalLeftBoundFormattedExpression(std::string const& logicalPortName,
    int const& portMapIndex) const
{
    QString logicalLeftBound = "";
    QSharedPointer<PortMap> selectedPortMap = getPortMap(logicalPortName, portMapIndex);
    if (selectedPortMap && selectedPortMap->getLogicalPort() && selectedPortMap->getLogicalPort()->range_)
    {
        logicalLeftBound = formattedValueFor(selectedPortMap->getLogicalPort()->range_->getLeft());
    }
    else if (portMapIndex < 0)
    {
        logicalLeftBound = getLogicalLeftForLogicalPort(logicalPortName);
    }

    return logicalLeftBound.toStdString();
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getLogicalLeftBoundExpression()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getLogicalLeftBoundExpression(std::string const& logicalPortName,
    int const& portMapIndex) const
{
    QString logicalLeftBound = "";
    QSharedPointer<PortMap> selectedPortMap = getPortMap(logicalPortName, portMapIndex);
    if (selectedPortMap && selectedPortMap->getLogicalPort() && selectedPortMap->getLogicalPort()->range_)
    {
        logicalLeftBound = selectedPortMap->getLogicalPort()->range_->getLeft();
    }
    else if (portMapIndex < 0)
    {
        logicalLeftBound = getLogicalLeftForLogicalPort(logicalPortName);
    }

    return logicalLeftBound.toStdString();
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::setLogicalLeftBound()
//-----------------------------------------------------------------------------
bool PortMapInterface::setLogicalLeftBound(std::string const& logicalPortName, int const& portMapIndex,
    std::string const& newLogicalLeft)
{
    QSharedPointer<PortMap> selectedPortMap = getPortMap(logicalPortName, portMapIndex);
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

    removeEmptyRange(logicalPort);

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::removeEmptyRange()
//-----------------------------------------------------------------------------
void PortMapInterface::removeEmptyRange(QSharedPointer<PortMap::LogicalPort> editedPortLogical)
{
    if (editedPortLogical->range_ && editedPortLogical->range_->getLeft().isEmpty() &&
        editedPortLogical->range_->getRight().isEmpty())
    {
        editedPortLogical->range_.clear();
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getLogicalRightBoundValue()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getLogicalRightBoundValue(std::string const& logicalPortName,
    int const& portMapIndex, int const& baseNumber) const
{
    QString logicalRightBound = "";
    QSharedPointer<PortMap> selectedPortMap = getPortMap(logicalPortName, portMapIndex);
    if (selectedPortMap && selectedPortMap->getLogicalPort() && selectedPortMap->getLogicalPort()->range_)
    {
        return parseExpressionToBaseNumber(
            selectedPortMap->getLogicalPort()->range_->getRight(), baseNumber).toStdString();
    }
    else if (portMapIndex < 0)
    {
        QString logicalLeftBound = getLogicalLeftForLogicalPort(logicalPortName);
        if (logicalLeftBound.isEmpty())
        {
            return logicalLeftBound.toStdString();
        }
        else
        {
            return std::string("0");
        }
    }

    return "";
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getLogicalRightBoundFormattedExpression()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getLogicalRightBoundFormattedExpression(std::string const& logicalPortName,
    int const& portMapIndex) const
{
    QSharedPointer<PortMap> selectedPortMap = getPortMap(logicalPortName, portMapIndex);
    if (selectedPortMap && selectedPortMap->getLogicalPort() && selectedPortMap->getLogicalPort()->range_)
    {
        QString logicalRightBound = selectedPortMap->getLogicalPort()->range_->getRight();
        return formattedValueFor(logicalRightBound).toStdString();
    }
    else if (portMapIndex < 0)
    {
        QString logicalLeftBound = getLogicalLeftForLogicalPort(logicalPortName);
        if (logicalLeftBound.isEmpty())
        {
            return logicalLeftBound.toStdString();
        }
        else
        {
            return std::string("0");
        }
    }

    return "";
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getLogicalRightBoundExpression()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getLogicalRightBoundExpression(std::string const& logicalPortName,
    int const& portMapIndex) const
{
    QSharedPointer<PortMap> selectedPortMap = getPortMap(logicalPortName, portMapIndex);
    if (selectedPortMap && selectedPortMap->getLogicalPort() && selectedPortMap->getLogicalPort()->range_)
    {
        QString logicalRightBound = selectedPortMap->getLogicalPort()->range_->getRight();
        return logicalRightBound.toStdString();
    }
    else if (portMapIndex < 0)
    {
        QString logicalLeftBound = getLogicalLeftForLogicalPort(logicalPortName);
        if (logicalLeftBound.isEmpty())
        {
            return logicalLeftBound.toStdString();
        }
        else
        {
            return std::string("0");
        }
    }

    return "";
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::setLogicalRightBound()
//-----------------------------------------------------------------------------
bool PortMapInterface::setLogicalRightBound(std::string const& logicalPortName, int const& portMapIndex,
    std::string const& newLogicalRight)
{
    QSharedPointer<PortMap> selectedPortMap = getPortMap(logicalPortName, portMapIndex);
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

    removeEmptyRange(logicalPort);

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getPhysicalLeftBoundValue()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getPhysicalLeftBoundValue(std::string const& logicalPortName, int const& portMapIndex,
    int const& baseNumber) const
{
    int modifiedPortMapIndex = portMapIndex;
    QSharedPointer<PortMapInterface::PortMapping> mapping = getPortMapping(logicalPortName);
    if (mapping && mapping->portMaps_.count() == 1 && portMapIndex < 0)
    {
        modifiedPortMapIndex = 0;
    }

    QSharedPointer<PortMap> selectedPortMap = getPortMap(logicalPortName, modifiedPortMapIndex);
    if (selectedPortMap && selectedPortMap->getPhysicalPort() && selectedPortMap->getPhysicalPort()->partSelect_)
    {
        QString physicalLeftBound = selectedPortMap->getPhysicalPort()->partSelect_->getLeftRange();
        return parseExpressionToBaseNumber(physicalLeftBound, baseNumber).toStdString();
    }

    return "";
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getPhysicalLeftBoundFormattedExpression()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getPhysicalLeftBoundFormattedExpression(std::string const& logicalPortName,
    int const& portMapIndex) const
{
    int modifiedPortMapIndex = portMapIndex;
    QSharedPointer<PortMapInterface::PortMapping> mapping = getPortMapping(logicalPortName);
    if (mapping && mapping->portMaps_.count() == 1 && portMapIndex < 0)
    {
        modifiedPortMapIndex = 0;
    }

    QSharedPointer<PortMap> selectedPortMap = getPortMap(logicalPortName, modifiedPortMapIndex);
    if (selectedPortMap && selectedPortMap->getPhysicalPort() && selectedPortMap->getPhysicalPort()->partSelect_)
    {
        QString physicalLeftBound = selectedPortMap->getPhysicalPort()->partSelect_->getLeftRange();
        return formattedValueFor(physicalLeftBound).toStdString();
    }

    return "";
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getPhysicalLefttBoundExpression()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getPhysicalLeftBoundExpression(std::string const& logicalPortName,
    int const& portMapIndex) const
{
    int modifiedPortMapIndex = portMapIndex;
    QSharedPointer<PortMapInterface::PortMapping> mapping = getPortMapping(logicalPortName);
    if (mapping && mapping->portMaps_.count() == 1 && portMapIndex < 0)
    {
        modifiedPortMapIndex = 0;
    }

    QSharedPointer<PortMap> selectedPortMap = getPortMap(logicalPortName, modifiedPortMapIndex);
    if (selectedPortMap && selectedPortMap->getPhysicalPort() && selectedPortMap->getPhysicalPort()->partSelect_)
    {
        QString physicalLeftBound = selectedPortMap->getPhysicalPort()->partSelect_->getLeftRange();
        return physicalLeftBound.toStdString();
    }

    return "";
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::setPhysicalLeftBound()
//-----------------------------------------------------------------------------
bool PortMapInterface::setPhysicalLeftBound(std::string const& logicalPortName, int const& portMapIndex,
    std::string const& newPhysicalLeft)
{
    QSharedPointer<PartSelect> physicalPart = createPhysicalPartSelect(logicalPortName, portMapIndex);
    if (physicalPart)
    {
        physicalPart->setLeftRange(QString::fromStdString(newPhysicalLeft));
    }

    if (newPhysicalLeft.empty())
    {
        removeEmptyPhysicalPort(getPortMap(logicalPortName, portMapIndex));
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getPhysicalPartSelect()
//-----------------------------------------------------------------------------
QSharedPointer<PartSelect> PortMapInterface::createPhysicalPartSelect(std::string const& logicalPortName,
    int const& portMapIndex)
{
    QSharedPointer<PortMap> portMap = getPortMap(logicalPortName, portMapIndex);
    if (!portMap)
    {
        portMap = createNewPortMap(QString::fromStdString(logicalPortName));
        if (!portMap)
        {
            return QSharedPointer<PartSelect>();
        }
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
std::string PortMapInterface::getPhysicalRightBoundValue(std::string const& logicalPortName,
    int const& portMapIndex, int const& baseNumber) const
{
    int modifiedPortMapIndex = portMapIndex;
    QSharedPointer<PortMapInterface::PortMapping> mapping = getPortMapping(logicalPortName);
    if (mapping && mapping->portMaps_.count() == 1 && portMapIndex < 0)
    {
        modifiedPortMapIndex = 0;
    }

    QSharedPointer<PortMap> selectedPortMap = getPortMap(logicalPortName, modifiedPortMapIndex);
    if (selectedPortMap && selectedPortMap->getPhysicalPort() && selectedPortMap->getPhysicalPort()->partSelect_)
    {
        QString physicalRightBound = selectedPortMap->getPhysicalPort()->partSelect_->getRightRange();
        return parseExpressionToBaseNumber(physicalRightBound, baseNumber).toStdString();
    }

    return "";
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getPhysicalRightBoundFormattedExpression()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getPhysicalRightBoundFormattedExpression(std::string const& logicalPortName,
    int const& portMapIndex) const
{
    int modifiedPortMapIndex = portMapIndex;
    QSharedPointer<PortMapInterface::PortMapping> mapping = getPortMapping(logicalPortName);
    if (mapping && mapping->portMaps_.count() == 1 && portMapIndex < 0)
    {
        modifiedPortMapIndex = 0;
    }

    QSharedPointer<PortMap> selectedPortMap = getPortMap(logicalPortName, modifiedPortMapIndex);
    if (selectedPortMap && selectedPortMap->getPhysicalPort() && selectedPortMap->getPhysicalPort()->partSelect_)
    {
        QString physicalRightBound = selectedPortMap->getPhysicalPort()->partSelect_->getRightRange();
        return formattedValueFor(physicalRightBound).toStdString();
    }

    return "";
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getPhysicalRightBoundExpression()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getPhysicalRightBoundExpression(std::string const& logicalPortName,
    int const& portMapIndex) const
{
    int modifiedPortMapIndex = portMapIndex;
    QSharedPointer<PortMapInterface::PortMapping> mapping = getPortMapping(logicalPortName);
    if (mapping && mapping->portMaps_.count() == 1 && portMapIndex < 0)
    {
        modifiedPortMapIndex = 0;
    }

    QSharedPointer<PortMap> selectedPortMap = getPortMap(logicalPortName, modifiedPortMapIndex);
    if (selectedPortMap && selectedPortMap->getPhysicalPort() && selectedPortMap->getPhysicalPort()->partSelect_)
    {
        QString physicalRightBound = selectedPortMap->getPhysicalPort()->partSelect_->getRightRange();
        return physicalRightBound.toStdString();
    }

    return "";
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::setPhysicalRightBound()
//-----------------------------------------------------------------------------
bool PortMapInterface::setPhysicalRightBound(std::string const& logicalPortName, int const& portMapIndex,
    std::string const& newPhysicalRight)
{
    QSharedPointer<PartSelect> physicalPart = createPhysicalPartSelect(logicalPortName, portMapIndex);
    if (physicalPart)
    {
        physicalPart->setRightRange(QString::fromStdString(newPhysicalRight));
    }

    if (newPhysicalRight.empty())
    {
        removeEmptyPhysicalPort(getPortMap(logicalPortName, portMapIndex));
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getLogicalPortIconPath()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getLogicalPortIconPath(std::string const& logicalPortName) const
{
    std::string mappingPath = "";

    QSharedPointer<PortMapping> mapping = getPortMapping(logicalPortName);
    if (mapping)
    {
        QSharedPointer<PortAbstraction> mappingPort = mapping->logicalPort_;
        if (mappingPort)
        {
            if (mappingPort->hasWire())
            {
                DirectionTypes::Direction direction = DirectionTypes::DIRECTION_INVALID;
                if (abstractionDef_ && mappingPort)
                {
                    direction =
                        abstractionDef_->getPortDirection(mappingPort->name(), interfaceMode_, systemGroup_);
                }

                mappingPath = portInterface_->getIconPathForDirection(direction);
            }
            else if (mappingPort->hasTransactional())
            {
                QString initiative("");
                if (abstractionDef_ && mappingPort)
                {
                    initiative =
                        abstractionDef_->getPortInitiative(mappingPort->name(), interfaceMode_, systemGroup_);
                }

                mappingPath = portInterface_->getIconPathForInitiative(initiative);
            }
        }
    }

    return mappingPath;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getPhysicalPortIconPath()
//-----------------------------------------------------------------------------
std::string PortMapInterface::getPhysicalPortIconPath(std::string const& logicalPortName, int const& portMapIndex)
const
{
    std::string iconPath = "";

    QSharedPointer<PortMap> portMap = getPortMap(logicalPortName, portMapIndex);
    if (portMap && portMap->getPhysicalPort())
    {
        QString portName = portMap->getPhysicalPort()->name_;
        if (!portName.isEmpty() && portName.compare(MULTIPLE_SELECTED, Qt::CaseSensitive) != 0)
        {
            iconPath = portInterface_->getIconPathForPort(portName.toStdString());
        }
    }

    return iconPath;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::logicalPortExists()
//-----------------------------------------------------------------------------
bool PortMapInterface::logicalPortExists(std::string const& logicalPortName) const
{
    bool portExists = false;
    if (abstractionDef_)
    {
        portExists = abstractionDef_->hasPort(QString::fromStdString(logicalPortName), interfaceMode_);
    }

    return portExists;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getAllReferencesToIdInItem()
//-----------------------------------------------------------------------------
int PortMapInterface::getAllReferencesToIdInItem(
    const std::string& itemName, std::string const&  valueID) const
{
    int totalReferencesToParameter = 0;

    QSharedPointer<PortMapInterface::PortMapping> selectedMapping = getPortMapping(itemName);
    for (auto portMap : selectedMapping->portMaps_)
    {
        QString idString = QString::fromStdString(valueID);

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
std::vector<std::string> PortMapInterface::getExpressionsInSelectedPortMap(std::string logicalPortName,
    int const& portMapIndex) const
{
    std::vector<std::string> expressionList;

    QSharedPointer<PortMap> portMap = getPortMap(logicalPortName, portMapIndex);
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
// Function: PortMapInterface::changeValidatorAbstractionDefinition()
//-----------------------------------------------------------------------------
void PortMapInterface::changeValidatorAbstractionDefinition()
{
    validator_->abstractionDefinitionChanged(abstractionDef_, interfaceMode_);
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::validateItems()
//-----------------------------------------------------------------------------
bool PortMapInterface::validateItems() const
{
    for (auto currentMapping : portMappings_)
    {
        for (auto portMap : currentMapping->portMaps_)
        {
            if (!validator_->validate(portMap))
            {
                return false;
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::logicalPortIsValid()
//-----------------------------------------------------------------------------
bool PortMapInterface::logicalPortIsValid(std::string const& logicalPortName, int const& portMapIndex) const
{
    QSharedPointer<PortMap> validatedPortMap = getPortMap(logicalPortName, portMapIndex);
    if (validatedPortMap)
    {
        return validator_->hasValidLogicalPort(validatedPortMap->getLogicalPort());
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::physicalPortIsValid()
//-----------------------------------------------------------------------------
bool PortMapInterface::physicalPortIsValid(std::string const& logicalPortName, int const& portMapIndex) const
{
    QSharedPointer<PortMap> validatedPortMap = getPortMap(logicalPortName, portMapIndex);
    if (validatedPortMap)
    {
        return validator_->hasValidPhysicalPort(validatedPortMap->getPhysicalPort());
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::physicalPortIsValid()
//-----------------------------------------------------------------------------
bool PortMapInterface::physicalMappingIsValid(std::string const& logicalPortName, int const& portMapIndex) const
{
    QSharedPointer<PortMap> validatedPortMap = getPortMap(logicalPortName, portMapIndex);
    if (validatedPortMap)
    {
        return validator_->hasValidPhysicalMapping(validatedPortMap);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::connectedPortsHaveValidPortTypes()
//-----------------------------------------------------------------------------
bool PortMapInterface::connectedPortsHaveValidPortTypes(std::string const& logicalPortName,
    int const& portMapIndex) const
{
    QSharedPointer<PortMap> validatedPortMap = getPortMap(logicalPortName, portMapIndex);
    if (validatedPortMap)
    {
        QSharedPointer<PortAbstraction> logicalPort = getPortMapping(logicalPortName)->logicalPort_;

        QSharedPointer<Port> portPointer =
            portInterface_->getPort(validatedPortMap->getPhysicalPort()->name_.toStdString());

        return validator_->connectedPortsHaveValidPortTypes(logicalPort, portPointer);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::connectedPortsHaveValidDirections()
//-----------------------------------------------------------------------------
bool PortMapInterface::connectedPortsHaveValidDirections(std::string const& logicalPortName,
    int const& portMapIndex) const
{
    QSharedPointer<PortMap> validatedPortMap = getPortMap(logicalPortName, portMapIndex);
    if (validatedPortMap)
    {
        QSharedPointer<PortAbstraction> logicalPort = getPortMapping(logicalPortName)->logicalPort_;
        QSharedPointer<Port> portPointer =
            portInterface_->getPort(validatedPortMap->getPhysicalPort()->name_.toStdString());

        return validator_->connectedPortsHaveValidDirections(logicalPort, portPointer);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::connectedPortsHaveValidInitiatives()
//-----------------------------------------------------------------------------
bool PortMapInterface::connectedPortsHaveValidInitiatives(std::string const& logicalPortName,
    int const& portMapIndex) const
{
    QSharedPointer<PortMap> validatedPortMap = getPortMap(logicalPortName, portMapIndex);
    if (validatedPortMap)
    {
        QSharedPointer<PortAbstraction> logicalPort = getPortMapping(logicalPortName)->logicalPort_;
        QSharedPointer<Port> portPointer =
            portInterface_->getPort(validatedPortMap->getPhysicalPort()->name_.toStdString());

        return validator_->connectedPortsHaveValidInitiatives(logicalPort, portPointer);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::connectedPortsHaveSameRange()
//-----------------------------------------------------------------------------
bool PortMapInterface::connectedPortsHaveSameRange(std::string const& logicalPortName, int const& portMapIndex)
const
{
    QSharedPointer<PortMap> validatedPortMap = getPortMap(logicalPortName, portMapIndex);
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
bool PortMapInterface::tieOffIsValid(std::string const& logicalPortName, int const& portMapIndex) const
{
    QSharedPointer<PortMap> validatedPortMap = getPortMap(logicalPortName, portMapIndex);
    if (validatedPortMap)
    {
        return validator_->hasValidTieOff(validatedPortMap->getLogicalTieOff());
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::logicalPortHasValidRange()
//-----------------------------------------------------------------------------
bool PortMapInterface::logicalPortHasValidRange(std::string const& logicalPortName, int const& portMapIndex) const
{
    QSharedPointer<PortMapping> mapping = getPortMapping(logicalPortName);
    QSharedPointer<PortMap> validatedPortMap = getPortMap(logicalPortName, portMapIndex);
    if (mapping && validatedPortMap && validatedPortMap->getLogicalPort())
    {
        validator_->logicalPortHasValidRange(validatedPortMap->getLogicalPort()->range_, mapping->logicalPort_);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getPortMapping()
//-----------------------------------------------------------------------------
QSharedPointer<PortMapInterface::PortMapping> PortMapInterface::getPortMapping(std::string const& logicalPortName)
    const
{
    QString logicalName = QString::fromStdString(logicalPortName);

    for (auto mapping : portMappings_)
    {
        if (mapping->logicalPort_->name() == logicalName)
        {
            return mapping;
        }
    }

    return QSharedPointer<PortMapInterface::PortMapping>();
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getComponentInstantiation()
//-----------------------------------------------------------------------------
QSharedPointer<PortMap> PortMapInterface::getPortMap(std::string const& logicalPortName, int const& portMapIndex)
    const
{
    QSharedPointer<PortMapInterface::PortMapping> selectedMapping = getPortMapping(logicalPortName);
    if (selectedMapping)
    {
        if (portMapIndex >= 0 && portMapIndex < selectedMapping->portMaps_.size())
        {
            return selectedMapping->portMaps_.at(portMapIndex);
        }
    }

    return QSharedPointer<PortMap>();
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::mapPorts()
//-----------------------------------------------------------------------------
bool PortMapInterface::connectPorts(std::string const& logicalPortName, std::string const& physicalPortName)
{
    QString logicalName = QString::fromStdString(logicalPortName);
    QString physicalName = QString::fromStdString(physicalPortName);

    if (logicalName.isEmpty() || physicalName.isEmpty() || !abstractionDef_->hasPort(logicalName) ||
        !portInterface_->portExists(physicalPortName))
    {
        return false;
    }

    QSharedPointer<PortMap::PhysicalPort> newMappedPhysical(
        new PortMap::PhysicalPort(QString::fromStdString(physicalPortName)));
    QSharedPointer<PortMap::LogicalPort> newMappedLogical(
        new PortMap::LogicalPort(QString::fromStdString(logicalPortName)));
    
    QSharedPointer<PortMapInterface::PortMapping> mapping = getPortMapping(logicalPortName);

    QSharedPointer<PortAbstraction> portAbstraction = mapping->logicalPort_;
    QSharedPointer<WireAbstraction> logicalWire = portAbstraction->getWire();
    if (logicalWire && !logicalWire->getWidth(interfaceMode_, systemGroup_).isEmpty())
    {
        int logicalSize = parseExpressionToDecimal(logicalWire->getWidth(interfaceMode_, systemGroup_)).toInt();
        int logicalLeft = logicalSize - 1;
        int logicalRight = 0;
        
        int physicalLeft = QString::fromStdString(portInterface_->getLeftBoundValue(physicalPortName)).toInt();
        int physicalRight = QString::fromStdString(portInterface_->getRightBoundValue(physicalPortName)).toInt();
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


    mapping->portMaps_.append(newPortMap);
    abstraction_->getPortMaps()->append(newPortMap);

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::addPortMap()
//-----------------------------------------------------------------------------
void PortMapInterface::addPortMap(int const& row, std::string const& newLogicalPortName)
{
    QSharedPointer<PortMapping> mapping = getPortMapping(newLogicalPortName);
    if (!mapping)
    {
        return;
    }

    QSharedPointer<PortMap::LogicalPort> logicalPort(new PortMap::LogicalPort(
        QString::fromStdString(newLogicalPortName)));
    QSharedPointer<PortMap> newPortMap(new PortMap());
    newPortMap->setLogicalPort(logicalPort);

    mapping->portMaps_.insert(row, newPortMap);
    abstraction_->getPortMaps()->append(newPortMap);
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::removePortMap()
//-----------------------------------------------------------------------------
bool PortMapInterface::removePortMap(std::string const& logicalPortName, int const& portMapIndex)
{
    QSharedPointer<PortMapping> mapping = getPortMapping(logicalPortName);
    QSharedPointer<PortMap> removedPortMap = getPortMap(logicalPortName, portMapIndex);
    if (mapping && removedPortMap)
    {
        mapping->portMaps_.removeAll(removedPortMap);
        abstraction_->getPortMaps()->removeAll(removedPortMap);

        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::removeAllPortMapsFromLogicalPort()
//-----------------------------------------------------------------------------
std::vector<std::string> PortMapInterface::removeAllPortMapsFromLogicalPort(std::string const& logicalPortName)
{
    std::vector<std::string> connectedPhysicalPorts;
    QSharedPointer<PortMapping> mapping = getPortMapping(logicalPortName);
    if (mapping)
    {
        QVector<QString> physicalPortNames;
        for (auto portMap : mapping->portMaps_)
        {
            if (portMap->getPhysicalPort() && !physicalPortNames.contains(portMap->getPhysicalPort()->name_))
            {
                physicalPortNames.append(portMap->getPhysicalPort()->name_);
            }

            abstraction_->getPortMaps()->removeAll(portMap);
        }

        for (auto portName : physicalPortNames)
        {
            connectedPhysicalPorts.push_back(portName.toStdString());
        }

        mapping->portMaps_.clear();
    }

    return connectedPhysicalPorts;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getPortMapPointer()
//-----------------------------------------------------------------------------
PortMap* PortMapInterface::getPortMapPointer(std::string const& logicalPortName, int const& portMapIndex) const
{
    PortMap* portMapPointer = nullptr;

    QSharedPointer<PortMap> selectedPortMap = getPortMap(logicalPortName, portMapIndex);
    if (selectedPortMap)
    {
        portMapPointer = selectedPortMap.data();
    }

    return portMapPointer;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getLogicalPortPointer()
//-----------------------------------------------------------------------------
PortAbstraction* PortMapInterface::getLogicalPortPointer(std::string const& logicalPortName) const
{
    PortAbstraction* logicalPortPointer = nullptr;

    QSharedPointer<PortMapInterface::PortMapping> portMapping = getPortMapping(logicalPortName);
    if (portMapping)
    {
        logicalPortPointer = portMapping->logicalPort_.data();
    }

    return logicalPortPointer;
}

//-----------------------------------------------------------------------------
// Function: PortMapInterface::getPortInterface()
//-----------------------------------------------------------------------------
PortsInterface* PortMapInterface::getPortInterface() const
{
    return portInterface_;
}
