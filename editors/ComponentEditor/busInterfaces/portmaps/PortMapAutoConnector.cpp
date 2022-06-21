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

#include <KactusAPI/include/PortMapInterface.h>
#include <KactusAPI/include/ExpressionParser.h>
#include <KactusAPI/include/PortsInterface.h>
#include <KactusAPI/include/PortAbstractionInterface.h>
#include <KactusAPI/include/LibraryInterface.h>

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
PortMapAutoConnector::PortMapAutoConnector(PortMapInterface* portMapInterface, QObject* parent):
QObject(parent),
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
// Function: PortMapAutoConnector::onAutoConnect()
//-----------------------------------------------------------------------------
void PortMapAutoConnector::onAutoConnect()
{
    if (portMapInterface_ && portMapInterface_->itemCount() > 0)
    {
        QStringList selectedPorts;
        for (int i = 0; i < portMapInterface_->itemCount(); ++i)
        {
            if (portMapInterface_->getPhysicalPortName(i).empty() &&
                portMapInterface_->getLogicalTieOffValue(i).empty())
            {
                selectedPorts.append(QString::fromStdString(portMapInterface_->getLogicalPortName(i)));
            }
        }

        connectSelectedLogicalPorts(selectedPorts);
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapAutoConnector::onAutoConnectLogicalSignals()
//-----------------------------------------------------------------------------
void PortMapAutoConnector::onAutoConnectLogicalSignals(QStringList const& logicalSignals)
{
    connectSelectedLogicalPorts(logicalSignals);
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
void PortMapAutoConnector::connectSelectedLogicalPorts(QStringList const& logicalPorts)
{
    QList<PossiblePortMaps> possiblePairings;

    PortAbstractionInterface* logicalInterface = portMapInterface_->getLogicalPortInterface();
    for (auto logicalPort : logicalPorts)
    {
        std::string portName(logicalPort.toStdString());

        QString presenceString(QString::fromStdString(
            portMapInterface_->getLogicalPresenceWithLogicalPort(portName)));
        PresenceTypes::Presence presence = PresenceTypes::str2Presence(presenceString, PresenceTypes::OPTIONAL);

        if (presence != PresenceTypes::ILLEGAL)
        {
            QMap<double, QString> physicalPorts = getWeightedPhysicalPorts(portName, logicalInterface);

            PossiblePortMaps newPairing;
            newPairing.logicalPort_ = logicalPort;
            newPairing.possiblePhysicals_ = physicalPorts;

            possiblePairings.append(newPairing);
        }
    }

    QVector<QString> connectedPhysicals;

    for (int i = 0; i < possiblePairings.size(); i++)
    {
        QString logicalPort(possiblePairings.at(i).logicalPort_);

        QString bestMatchingPhysicalPort = getBestMatchingPhysicalPort(i, possiblePairings);

        if (!bestMatchingPhysicalPort.isEmpty())
        {
            portMapInterface_->connectPorts(logicalPort.toStdString(), bestMatchingPhysicalPort.toStdString());
            connectedPhysicals.append(bestMatchingPhysicalPort);
        }
    }

    if (!connectedPhysicals.isEmpty())
    {
        emit portMapsAutoConnected(connectedPhysicals);
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapAutoConnector::getWeightedPhysicalPorts()
//-----------------------------------------------------------------------------
QMap<double, QString> PortMapAutoConnector::getWeightedPhysicalPorts(std::string const& logicalPort,
    PortAbstractionInterface* logicalInterface) const
{
    QMap<double, QString> weightedPhysicalPorts;

    General::InterfaceMode busMode = portMapInterface_->getInterfaceMode();
    std::string systemGroup = portMapInterface_->getSystemGroup();

    DirectionTypes::Direction logicalDirection =
        logicalInterface->getDirection(logicalPort, busMode, systemGroup);

    QMap<QString, double> availablePorts = getPortsByDirection(logicalDirection);
    if (!availablePorts.isEmpty())
    {
        QMap<QString, double> availableWeightedPorts;

        bool isValidWidth = false;
        QString logicalWidth =
            QString::fromStdString(logicalInterface->getWidth(logicalPort, busMode, systemGroup));

        int logicalWidthInt = logicalWidth.toInt(&isValidWidth);

        if (isValidWidth)
        {
            availableWeightedPorts = getPortsByLogicalWidth(logicalWidthInt, availablePorts);
        }
        else
        {
            availableWeightedPorts = availablePorts;
        }

        if (!availableWeightedPorts.isEmpty())
        {
            availableWeightedPorts =
                weightPortsByLogicalName(QString::fromStdString(logicalPort), availableWeightedPorts);
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
            QString comparisonLogical = possiblePairings.at(j).logicalPort_;
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
    PortsInterface* portInterface = portMapInterface_->getPhysicalPortInterface();
    QMap<QString, double> availablePorts;

    for (auto portName : portInterface->getItemNames())
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
// Function: PortMapAutoConnector::getPortsByLogicalWidth()
//-----------------------------------------------------------------------------
QMap<QString, double> PortMapAutoConnector::getPortsByLogicalWidth(double logicalWidth,
    QMap<QString, double> portList) const
{
    const double WIDTH_WEIGHT = 0.2;
    PortsInterface* portInterface = portMapInterface_->getPhysicalPortInterface();

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
