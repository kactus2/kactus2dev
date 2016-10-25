//-----------------------------------------------------------------------------
// File: PortMapAutoConnector.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 03.06.2016
//
// Description:
// Automatically forms port maps between logical and physical ports.
//-----------------------------------------------------------------------------

#include "PortMapAutoConnector.h"

#include "JaroWinklerAlgorithm.h"

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/PortMap.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>

#include <QtMath>

//-----------------------------------------------------------------------------
// Function: PortMapAutoConnector::PortMapAutoConnector()
//-----------------------------------------------------------------------------
PortMapAutoConnector::PortMapAutoConnector(QSharedPointer<Component> component,
    QSharedPointer<BusInterface> busInterface, QSharedPointer<ExpressionParser> parser,
    LibraryInterface* libraryHandler, QObject* parent):
QObject(parent),
    component_(component),
    busInterface_(busInterface),
    absDef_(),
    parser_(parser),
    libraryHandler_(libraryHandler)
{

}

//-----------------------------------------------------------------------------
// Function: PortMapAutoConnector::~PortMapAutoConnector()
//-----------------------------------------------------------------------------
PortMapAutoConnector::~PortMapAutoConnector()
{

}

//-----------------------------------------------------------------------------
// Function: PortMapAutoConnector::setAbstractionDefinition()
//-----------------------------------------------------------------------------
void PortMapAutoConnector::setAbstractionDefinition(VLNV const& abstractionDefinitionVLNV,
    General::InterfaceMode newMode)
{
    interfaceMode_ = newMode;
    absDef_.clear();

    if (abstractionDefinitionVLNV.isValid())
    {
        QSharedPointer<Document const> libComb = libraryHandler_->getModelReadOnly(abstractionDefinitionVLNV);
        if (libComb && libraryHandler_->getDocumentType(abstractionDefinitionVLNV) == VLNV::ABSTRACTIONDEFINITION)
        {
            absDef_ = libComb.dynamicCast<AbstractionDefinition const>();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapAutoConnector::onAutoConnect()
//-----------------------------------------------------------------------------
void PortMapAutoConnector::onAutoConnect()
{
    if (absDef_ && !absDef_->getLogicalPorts()->isEmpty())
    {
        connectSelectedLogicalPorts(*absDef_->getLogicalPorts());
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapAutoConnector::connectSelectedLogicalPorts()
//-----------------------------------------------------------------------------
void PortMapAutoConnector::connectSelectedLogicalPorts(QList<QSharedPointer<PortAbstraction> > logicalPorts)
{
    QList<PossiblePortMaps> possiblePairings;

    foreach (QSharedPointer<PortAbstraction> logicalPort, logicalPorts)
    {
        if (!logicalPortHasReferencingPortMap(logicalPort->name()) &&
            logicalPort->getPresence(interfaceMode_) != PresenceTypes::ILLEGAL)
        {
            QMap<double, QString> physicalPorts = getWeightedPhysicalPorts(logicalPort);

            PossiblePortMaps newPairing;
            newPairing.logicalPort_ = logicalPort;
            newPairing.possiblePhysicals_ = physicalPorts;

            possiblePairings.append(newPairing);
        }
    }

    for (int i = 0; i < possiblePairings.size(); i++)
    {
        QSharedPointer<PortAbstraction> logicalPort = possiblePairings.at(i).logicalPort_;

        QString bestMatchingPhysicalPort = getBestMatchingPhysicalPort(logicalPort, i, possiblePairings);

        if (!bestMatchingPhysicalPort.isEmpty())
        {
            QSharedPointer<Port> physicalPort = component_->getPort(bestMatchingPhysicalPort);
            connectPorts(logicalPort, physicalPort);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapAutoConnector::getBestMatchingPhysicalPort()
//-----------------------------------------------------------------------------
QString PortMapAutoConnector::getBestMatchingPhysicalPort(QSharedPointer<PortAbstraction> logicalPort, 
    int logicalIndex, QList<PossiblePortMaps> const& possiblePairings) const
{
    QMapIterator<double, QString> physicalIterator(possiblePairings.at(logicalIndex).possiblePhysicals_);
    physicalIterator.toBack();
    while (physicalIterator.hasPrevious())
    {
        physicalIterator.previous();

        QString physicalPort = physicalIterator.value();
        double comparisonWeight = physicalIterator.key();

        bool physicalFound = true;

        for (int j = logicalIndex + 1; j < possiblePairings.size() && physicalFound; j++)
        {
            QString comparisonLogical = possiblePairings.at(j).logicalPort_->name();
            double comparisonValue = possiblePairings.at(j).possiblePhysicals_.key(physicalPort);
            if (comparisonValue > comparisonWeight)
            {
                physicalFound = false;
            }
        }

        if (physicalFound)
        {
            return physicalPort;
        }
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: PortMapAutoConnector::logicalPortHasReferencingPortMap()
//-----------------------------------------------------------------------------
bool PortMapAutoConnector::logicalPortHasReferencingPortMap(QString const& logicalName) const
{
    foreach (QSharedPointer<PortMap> portMap, *busInterface_->getPortMaps())
    {
        if (portMap->getLogicalPort() && portMap->getLogicalPort()->name_.compare(logicalName) == 0)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapAutoConnector::getWeightedPhysicalPorts()
//-----------------------------------------------------------------------------
QMap<double, QString> PortMapAutoConnector::getWeightedPhysicalPorts(QSharedPointer<PortAbstraction> logicalPort)
    const
{
    QMap<double, QString> weightedPhysicalPorts;

    DirectionTypes::Direction logicalDirection = absDef_->getPortDirection(logicalPort->name(), interfaceMode_);
    
    QMap<QSharedPointer<Port>, double> availablePorts = getPortsByDirection(logicalDirection);
    if (!availablePorts.isEmpty())
    {
        QMap<QString, double> availableWeightedPorts;

        QString logicalWidth = getLogicalPortWidth(logicalPort);
        if (!logicalWidth.isEmpty() && parser_->isValidExpression(logicalWidth))
        {
            int logicalWidthInt = parser_->parseExpression(logicalWidth).toInt();
            availableWeightedPorts = getPortsByLogicalWidth(logicalWidthInt, availablePorts);
        }
        else
        {
            availableWeightedPorts = getPortNames(availablePorts);
        }

        if (!availableWeightedPorts.isEmpty())
        {
            availableWeightedPorts = weightPortsByLogicalName(logicalPort->name(), availableWeightedPorts);
        }

        QMapIterator<QString, double> weightIterator(availableWeightedPorts);
        while (weightIterator.hasNext())
        {
            weightIterator.next();

            weightedPhysicalPorts.insertMulti(weightIterator.value(), weightIterator.key());
        }
    }

    return weightedPhysicalPorts;
}

//-----------------------------------------------------------------------------
// Function: PortMapAutoConnector::getPortsByDirection()
//-----------------------------------------------------------------------------
QMap<QSharedPointer<Port>, double> PortMapAutoConnector::getPortsByDirection(
    DirectionTypes::Direction logicalDirection) const
{
    QMap<QSharedPointer<Port>, double> availablePorts;

    foreach (QSharedPointer<Port> physicalPort, *component_->getPorts())
    {
        if (physicalPort->getDirection() == logicalDirection)
        {
            availablePorts.insert(physicalPort, 2);
        }
        else if ((logicalDirection == DirectionTypes::IN || logicalDirection == DirectionTypes::OUT) &&
            physicalPort->getDirection() == DirectionTypes::INOUT)
        {
            availablePorts.insert(physicalPort, 0);
        }
    }

    return availablePorts;
}

//-----------------------------------------------------------------------------
// Function: PortMapAutoConnector::getLogicalPortWidth()
//-----------------------------------------------------------------------------
QString PortMapAutoConnector::getLogicalPortWidth(QSharedPointer<PortAbstraction> logicalPort) const
{
    QSharedPointer<WireAbstraction> abstractWire = logicalPort->getWire();
    if (abstractWire)
    {
        return abstractWire->getWidth(interfaceMode_);
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: PortMapAutoConnector::getPortsByLogicalWidth()
//-----------------------------------------------------------------------------
QMap<QString, double> PortMapAutoConnector::getPortsByLogicalWidth(double logicalWidth, 
    QMap<QSharedPointer<Port>, double> portList) const
{
    const double WIDTH_WEIGHT = 0.2;

    QMap<QString, double> widthMatchingPorts;

    QMapIterator<QSharedPointer<Port>, double> portIterator(portList);
    while (portIterator.hasNext())
    {
        portIterator.next();

        QSharedPointer<Port> physicalPort = portIterator.key();

        int calculatedLeftBound = parser_->parseExpression(physicalPort->getLeftBound()).toInt();
        int calculatedRightBound = parser_->parseExpression(physicalPort->getRightBound()).toInt();
        double portWidth = abs(calculatedLeftBound - calculatedRightBound) + 1;

        double widthSimilarity = WIDTH_WEIGHT * (qMin(logicalWidth, portWidth)/(qMax(logicalWidth, portWidth)));

        widthMatchingPorts.insert(physicalPort->name(), widthSimilarity + portIterator.value());
    }

    return widthMatchingPorts;
}

//-----------------------------------------------------------------------------
// Function: PortMapAutoConnector::getPortsByLogicalName()
//-----------------------------------------------------------------------------
QMap<QString, double> PortMapAutoConnector::weightPortsByLogicalName(QString logicalName,
    QMap<QString, double> portList) const
{
    const double JARO_WINKLER_THRESHOLD = 0.75;

    QMap<QString, double> jaroDistancedPorts;

    QMapIterator<QString, double> portIterator(portList);
    while (portIterator.hasNext())
    {
        portIterator.next();        
        QString physicalName = portIterator.key();

        double jaroDistance = JaroWinklerAlgorithm::calculateJaroWinklerDistance(physicalName, logicalName);
        
        if (jaroDistance >= JARO_WINKLER_THRESHOLD)
        {
            jaroDistancedPorts.insert(portIterator.key(), jaroDistance + portIterator.value());
        }
    }

    return jaroDistancedPorts;
}

//-----------------------------------------------------------------------------
// Function: PortMapAutoConnector::getPortNames()
//-----------------------------------------------------------------------------
QMap<QString, double> PortMapAutoConnector::getPortNames(QMap<QSharedPointer<Port>, double> portList) const
{
    QMap<QString, double> portNameList;

    QMapIterator<QSharedPointer<Port>, double> portIterator(portList);
    while (portIterator.hasNext())
    {
        portIterator.next();

        QSharedPointer<Port> physicalPort = portIterator.key();

        portNameList.insert(physicalPort->name(), portIterator.value());
    }

    return portNameList;
}

//-----------------------------------------------------------------------------
// Function: PortMapAutoConnector::reorderPortsToWeight()
//-----------------------------------------------------------------------------
QStringList PortMapAutoConnector::reorderPortsToWeight(QMap<QString, double> portList) const
{
    int portCount = portList.count();

    QStringList reorderedPorts;

    for (int i = 0; i < portCount; ++i)
    {
        double bestWeight = portList.first();
        QString bestPort = portList.firstKey();

        QMapIterator<QString, double> portIterator(portList);
        while(portIterator.hasNext())
        {
            portIterator.next();

            if (portIterator.value() > bestWeight)
            {
                bestWeight = portIterator.value();
                bestPort = portIterator.key();
            }
        }

        reorderedPorts.append(bestPort);
        portList.remove(bestPort);
    }

    return reorderedPorts;
}

//-----------------------------------------------------------------------------
// Function: PortMapAutoConnector::connectPorts()
//-----------------------------------------------------------------------------
void PortMapAutoConnector::connectPorts(QSharedPointer<PortAbstraction> portAbstraction,
    QSharedPointer<Port> componentPort)
{
    if (portAbstraction && componentPort)
    {
        QSharedPointer<PortMap::PhysicalPort> newMappedPhysical(new PortMap::PhysicalPort(componentPort->name()));
        
        QSharedPointer<PortMap::LogicalPort> newMappedLogical(new PortMap::LogicalPort(portAbstraction->name()));
        
        QSharedPointer<WireAbstraction> logicalWire = portAbstraction->getWire();
        if (logicalWire && !logicalWire->getWidth(interfaceMode_).isEmpty())
        {
            int logicalSize = parser_->parseExpression(logicalWire->getWidth(interfaceMode_)).toInt();
            int logicalLeft = logicalSize - 1;
            int logicalRight = 0;

            int physicalLeft = parser_->parseExpression(componentPort->getLeftBound()).toInt();
            int physicalRight = parser_->parseExpression(componentPort->getRightBound()).toInt();
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

        emit portMapCreated(newPortMap);
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapAutoConnector::onAutoConnectLogicalSignals()
//-----------------------------------------------------------------------------
void PortMapAutoConnector::onAutoConnectLogicalSignals(QStringList const& logicalSignals)
{
    if (absDef_)
    {
        QList<QSharedPointer<PortAbstraction> > selectedLogicalSignals;
        foreach (QString const& logicalName, logicalSignals)
        {
            selectedLogicalSignals.append(getLogicalPort(logicalName));
        }

        connectSelectedLogicalPorts(selectedLogicalSignals);
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapAutoConnector::getLogicalPort()
//-----------------------------------------------------------------------------
QSharedPointer<PortAbstraction> PortMapAutoConnector::getLogicalPort(QString const& logicalName) const
{
    if (absDef_)
    {
        foreach (QSharedPointer<PortAbstraction> logicalPort, *absDef_->getLogicalPorts())
        {
            if (logicalPort->name().compare(logicalName) == 0)
            {
                return logicalPort;
            }
        }
    }

    return QSharedPointer<PortAbstraction> ();
}
