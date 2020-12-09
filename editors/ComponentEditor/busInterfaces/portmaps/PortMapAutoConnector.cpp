//-----------------------------------------------------------------------------
// File: PortMapAutoConnector.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 03.06.2016
//
// Description:
// Automatically forms port maps between logical and physical ports.
//-----------------------------------------------------------------------------

#include "PortMapAutoConnector.h"

#include "JaroWinklerAlgorithm.h"

#include <editors/ComponentEditor/busInterfaces/portmaps/interfaces/PortMapInterface.h>
#include <editors/ComponentEditor/common/ExpressionParser.h>
#include <editors/ComponentEditor/ports/PortsInterface.h>

#include <library/LibraryInterface.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/PortMap.h>

#include <QtMath>

//-----------------------------------------------------------------------------
// Function: PortMapAutoConnector::PortMapAutoConnector()
//-----------------------------------------------------------------------------
PortMapAutoConnector::PortMapAutoConnector(QSharedPointer<Component> component,
    QSharedPointer<ExpressionParser> parser, PortMapInterface* portMapInterface, LibraryInterface* libraryHandler,
    QObject* parent):
QObject(parent),
absDef_(),
parser_(parser),
libraryHandler_(libraryHandler),
interfaceMode_(General::INTERFACE_MODE_COUNT),
physicalPrefix_(),
portMapInterface_(portMapInterface)
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
    General::InterfaceMode newMode, QString const& systemGroup)
{
    interfaceMode_ = newMode;
    systemGroup_ =  systemGroup;
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
// Function: PortMapAutoConnector::setPrefix()
//-----------------------------------------------------------------------------
void PortMapAutoConnector::setPrefix(QString const& prefix)
{
    physicalPrefix_ = prefix;
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
            logicalPort->getPresence(interfaceMode_, systemGroup_) != PresenceTypes::ILLEGAL)
        {
            QMap<double, QString> physicalPorts = getWeightedPhysicalPorts(logicalPort);

            PossiblePortMaps newPairing;
            newPairing.logicalPort_ = logicalPort;
            newPairing.possiblePhysicals_ = physicalPorts;

            possiblePairings.append(newPairing);
        }
    }

    QVector<QString> connectedPhysicals;

    for (int i = 0; i < possiblePairings.size(); i++)
    {
        QSharedPointer<PortAbstraction> logicalPort = possiblePairings.at(i).logicalPort_;

        QString bestMatchingPhysicalPort = getBestMatchingPhysicalPort(i, possiblePairings);

        if (!bestMatchingPhysicalPort.isEmpty())
        {
            portMapInterface_->connectPorts(
                logicalPort->name().toStdString(), bestMatchingPhysicalPort.toStdString());
            connectedPhysicals.append(bestMatchingPhysicalPort);
        }
    }

    if (!connectedPhysicals.isEmpty())
    {
        emit portMapsAutoConnected(connectedPhysicals);
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapAutoConnector::logicalPortHasReferencingPortMap()
//-----------------------------------------------------------------------------
bool PortMapAutoConnector::logicalPortHasReferencingPortMap(QString const& logicalName) const
{
    for(auto mappingName : portMapInterface_->getItemNames())
    {
        if (QString::fromStdString(mappingName).compare(logicalName) == 0 && portMapInterface_->portMapCount(mappingName) > 0)
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

    DirectionTypes::Direction logicalDirection = absDef_->getPortDirection(logicalPort->name(), interfaceMode_, systemGroup_);

    QMap<QString, double> availablePorts = getPortsByDirection(logicalDirection);
    if (!availablePorts.isEmpty())
    {
        QMap<QString, double> availableWeightedPorts;

        bool isValidWidth = false;
        QString logicalWidth = parser_->parseExpression(getLogicalPortWidth(logicalPort), &isValidWidth);
        if (isValidWidth)
        {
            int logicalWidthInt = parser_->parseExpression(logicalWidth).toInt();
            availableWeightedPorts = getPortsByLogicalWidth(logicalWidthInt, availablePorts);
        }
        else
        {
            availableWeightedPorts = availablePorts;
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
// Function: PortMapAutoConnector::getBestMatchingPhysicalPort()
//-----------------------------------------------------------------------------
QString PortMapAutoConnector::getBestMatchingPhysicalPort(int logicalIndex,
    QList<PossiblePortMaps> const& possiblePairings) const
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
// Function: PortMapAutoConnector::getPortsByDirection()
//-----------------------------------------------------------------------------
QMap<QString, double> PortMapAutoConnector::getPortsByDirection(DirectionTypes::Direction logicalDirection) const
{
    PortsInterface* portInterface = portMapInterface_->getPortInterface();
    QMap<QString, double> availablePorts;

    for (auto portName : portMapInterface_->getPortInterface()->getItemNames())
    {
        if (portInterface->getDirectionType(portName) == logicalDirection)
        {
            availablePorts.insert(QString::fromStdString(portName), 2);
        }
        else if ((logicalDirection == DirectionTypes::IN || logicalDirection == DirectionTypes::OUT) &&
            portInterface->getDirectionType(portName) == DirectionTypes::INOUT)
        {
            availablePorts.insert(QString::fromStdString(portName), 0);
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
        return abstractWire->getWidth(interfaceMode_, systemGroup_);
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: PortMapAutoConnector::getPortsByLogicalWidth()
//-----------------------------------------------------------------------------
QMap<QString, double> PortMapAutoConnector::getPortsByLogicalWidth(double logicalWidth,
    QMap<QString, double> portList) const
{
    const double WIDTH_WEIGHT = 0.2;
    PortsInterface* portInterface = portMapInterface_->getPortInterface();

    QMap<QString, double> widthMatchingPorts;

    QMapIterator<QString, double> portIterator(portList);
    while (portIterator.hasNext())
    {
        portIterator.next();

        std::string portName = portIterator.key().toStdString();

        int calculatedLeftBound = QString::fromStdString(portInterface->getLeftBoundValue(portName)).toInt();
        int calculatedRightBound = QString::fromStdString(portInterface->getRightBoundValue(portName)).toInt();
        double portWidth = abs(calculatedLeftBound - calculatedRightBound) + 1;
        double widthSimilarity = WIDTH_WEIGHT * (qMin(logicalWidth, portWidth)/(qMax(logicalWidth, portWidth)));

        widthMatchingPorts.insert(portIterator.key(), widthSimilarity + portIterator.value());
    }

    return widthMatchingPorts;
}

//-----------------------------------------------------------------------------
// Function: PortMapAutoConnector::getPortsByLogicalName()
//-----------------------------------------------------------------------------
QMap<QString, double> PortMapAutoConnector::weightPortsByLogicalName(QString const& logicalName,
    QMap<QString, double> portList) const
{
    const double JARO_WINKLER_THRESHOLD = 0.75;

    QMap<QString, double> jaroDistancedPorts;

    QMapIterator<QString, double> portIterator(portList);
    while (portIterator.hasNext())
    {
        portIterator.next();        
        QString physicalName = portIterator.key();

        double jaroDistance = JaroWinklerAlgorithm::calculateJaroWinklerDistance(physicalName, physicalPrefix_ + logicalName);

        if (jaroDistance >= JARO_WINKLER_THRESHOLD)
        {
            jaroDistancedPorts.insert(portIterator.key(), jaroDistance + portIterator.value());
        }
    }

    return jaroDistancedPorts;
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
