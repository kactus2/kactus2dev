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
        QSharedPointer<Document> libComb = libraryHandler_->getModel(abstractionDefinitionVLNV);
        if (libComb && libraryHandler_->getDocumentType(abstractionDefinitionVLNV) == VLNV::ABSTRACTIONDEFINITION)
        {
            absDef_ = libComb.dynamicCast<AbstractionDefinition>();
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
        QList<QSharedPointer<PortAbstraction> > logicalPorts;
        foreach (QSharedPointer<PortAbstraction> abstractPort, *absDef_->getLogicalPorts())
        {
            logicalPorts.append(abstractPort);
        }

        connectSelectedLogicalPorts(logicalPorts);
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
QString PortMapAutoConnector::getBestMatchingPhysicalPort(QSharedPointer<PortAbstraction> logicalPort, int
    logicalIndex, QList<PossiblePortMaps> possiblePairings) const
{
    int physicalPortRank = 0;

    QMapIterator<double, QString> physicalIterator(possiblePairings.at(logicalIndex).possiblePhysicals_);
    physicalIterator.toBack();
    while (physicalIterator.hasPrevious())
    {
        physicalIterator.previous();

        physicalPortRank++;

        QString physicalPort = physicalIterator.value();
        double comparisonWeight = physicalIterator.key();

        bool physicalFound = true;

        for (int j = logicalIndex + 1; j < possiblePairings.size(); j++)
        {
            QString comparisonLogical = possiblePairings.at(j).logicalPort_->name();
            double comparisonValue = possiblePairings.at(j).possiblePhysicals_.key(physicalPort);
            if (comparisonValue > comparisonWeight)
            {
                physicalFound = false;
                break;
            }
        }

        if (physicalFound)
        {
            return physicalPort;
        }
    }

    return QString("");
}

//-----------------------------------------------------------------------------
// Function: PortMapAutoConnector::logicalPortHasReferencingPortMap()
//-----------------------------------------------------------------------------
bool PortMapAutoConnector::logicalPortHasReferencingPortMap(QString const& logicalName) const
{
    foreach (QSharedPointer<PortMap> portMap, *busInterface_->getPortMaps())
    {
        if (portMap->getLogicalPort() && portMap->getLogicalPort()->name_ == logicalName)
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

    QMap<QSharedPointer<Port>, double> availablePorts = getPortsByDirection(logicalPort);
    if (!availablePorts.isEmpty())
    {
        QMap<QString, double> availableWeightedPorts;
        QString logicalWidth = getLogicalPortWidth(logicalPort);
        if (parser_->isValidExpression(logicalWidth) && !logicalWidth.isEmpty())
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
    QSharedPointer<PortAbstraction> logicalPort) const
{
    DirectionTypes::Direction logicalDirection =
        absDef_->getPortDirection(logicalPort->name(), interfaceMode_);

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
        QString logicalWidthString = abstractWire->getWidth(interfaceMode_);
        return logicalWidthString;
    }

    return QString("");
}

//-----------------------------------------------------------------------------
// Function: PortMapAutoConnector::getPortsByLogicalWidth()
//-----------------------------------------------------------------------------
QMap<QString, double> PortMapAutoConnector::getPortsByLogicalWidth(int logicalWidth,
    QMap<QSharedPointer<Port>, double> portList) const
{
    QMap<QString, double> widthMatchingPorts;

    QMapIterator<QSharedPointer<Port>, double> portIterator(portList);
    while (portIterator.hasNext())
    {
        portIterator.next();

        QSharedPointer<Port> physicalPort = portIterator.key();

        double logicalDouble = logicalWidth;
        int calculatedLeftBound = parser_->parseExpression(physicalPort->getLeftBound()).toInt();
        int calculatedRightBound = parser_->parseExpression(physicalPort->getRightBound()).toInt();

        double portWidth = abs(calculatedLeftBound - calculatedRightBound) + 1;

        double widthSimilarity = (1/3) * (qMin(logicalDouble, portWidth)/(qMax(logicalDouble, portWidth)));

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
    QMap<QString, double> jaroDistancedPorts;

    QMapIterator<QString, double> portIterator(portList);
    while (portIterator.hasNext())
    {
        portIterator.next();

        QString logicalLower = logicalName.toLower();
        QString physicalLower = portIterator.key().toLower();

        int physicalNameSize = physicalLower.size();
        int logicalNameSize = logicalLower.size();

        int matchingDistance = qMax(qFloor(qMax(physicalNameSize, logicalNameSize)/2) - 1, 1);

        QString firstMatch = getMatchingCharacters(physicalLower, logicalLower, matchingDistance);
        QString secondMatch = getMatchingCharacters(logicalLower, physicalLower, matchingDistance);
        
        double jaroDistance = 0;

        if (!firstMatch.isEmpty() && !secondMatch.isEmpty() && firstMatch.size() == secondMatch.size())
        {
            int matchingCharacters = firstMatch.size();
            int transpositions = getTranspositions(firstMatch, secondMatch);

            jaroDistance =
                calculateJaroDistance(physicalNameSize, logicalNameSize, matchingCharacters, transpositions);

            jaroDistance = calculateWinklerDistance(jaroDistance);
        }
        
        if (jaroDistance > 0.65)
        {
            jaroDistancedPorts.insert(portIterator.key(), jaroDistance + portIterator.value());
        }
    }

    return jaroDistancedPorts;
}

//-----------------------------------------------------------------------------
// Function: PortMapAutoConnector::getMatchingCharacters()
//-----------------------------------------------------------------------------
QString PortMapAutoConnector::getMatchingCharacters(QString const& shortName, QString const& longName,
    int matchingDistance) const
{
    QString matchingCharacters = "";

    if (matchingDistance > 0)
    {
        for (int i = 0; i < shortName.size(); ++i)
        {
            if (!matchingCharacters.contains(shortName.at(i)))
            {
                int startPosition = 0;
                int recalculatedMatchingDistance = matchingDistance;
                if (i - matchingDistance > 0)
                {
                    startPosition = i - matchingDistance;
                    recalculatedMatchingDistance = matchingDistance * 2;
                }
                else
                {
                    recalculatedMatchingDistance = qMax(1, recalculatedMatchingDistance + i);
                }

                QString longNameSection = longName.mid(startPosition, recalculatedMatchingDistance + 1);
                if (longNameSection.contains(shortName.at(i)))
                {
                    matchingCharacters.append(shortName.at(i));
                }
            }
        }
    }

    return matchingCharacters;
}

//-----------------------------------------------------------------------------
// Function: PortMapAutoConnector::getTranspositions()
//-----------------------------------------------------------------------------
int PortMapAutoConnector::getTranspositions(QString const& firstMatch, QString const& secondMatch) const
{
    double transpositions = 0;
    for (int i = 0; i < firstMatch.size(); i++)
    {
        if (firstMatch.at(i) != secondMatch.at(i))
        {
            transpositions++;
        }
    }

    return transpositions;
}

//-----------------------------------------------------------------------------
// Function: PortMapAutoConnector::calculateJaroDistance()
//-----------------------------------------------------------------------------
double PortMapAutoConnector::calculateJaroDistance(int physicalNameSize, int logicalNameSize,
    int matchingCharacters, int transpositions) const
{
    double jaroDistance = 0;

    if (matchingCharacters != 0)
    {
        double physicalNameDouble = physicalNameSize;
        double logicalNameDouble = logicalNameSize;
        double matchingDouble = matchingCharacters;
        double transpositionsDouble = transpositions;

        double physicalPart = matchingDouble / physicalNameDouble;
        double logicalPart = matchingDouble / logicalNameDouble;
        double transpositionPart = (matchingDouble - transpositionsDouble) / matchingDouble;

        double jaroDistance = (physicalPart + logicalPart + transpositionPart) / 3;
        return jaroDistance;
    }

    return jaroDistance;
}

//-----------------------------------------------------------------------------
// Function: PortMapAutoConnector::calculateWinklerDistance()
//-----------------------------------------------------------------------------
double PortMapAutoConnector::calculateWinklerDistance(double jaroDistance) const
{
    double prefixLength = 3;
    double scalingFactor = 0.1;

    double winklerDistance = jaroDistance + (prefixLength*scalingFactor*(1-jaroDistance));
    return winklerDistance;
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

            QSharedPointer<Range> newLogicalRange
                (new Range(QString::number(logicalLeft), QString::number(logicalRight)));
            newMappedLogical->range_ = newLogicalRange;

            QSharedPointer<PartSelect> newPhysicalPartSelect
                (new PartSelect(QString::number(physicalLeft), QString::number(physicalRight)));
            newMappedPhysical->partSelect_ = newPhysicalPartSelect;
        }

        QSharedPointer<PortMap> newPortMap (new PortMap());
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
        foreach (QString logicalName, logicalSignals)
        {
            QSharedPointer<PortAbstraction> logicalPort = getLogicalPort(logicalName);
            selectedLogicalSignals.append(logicalPort);
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
            if (logicalPort->name() == logicalName)
            {
                return logicalPort;
            }
        }
    }

    return QSharedPointer<PortAbstraction> ();
}
