//-----------------------------------------------------------------------------
// File: MasterSlavePathSearch.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 25.08.2016
//
// Description:
// Finds the paths from master interfaces to slave interfaces.
//-----------------------------------------------------------------------------

#include "MasterSlavePathSearch.h"

#include <editors/MemoryDesigner/ConnectivityConnection.h>
#include <editors/MemoryDesigner/ConnectivityComponent.h>
#include <editors/MemoryDesigner/ConnectivityGraph.h>
#include <editors/MemoryDesigner/ConnectivityInterface.h>
#include <editors/MemoryDesigner/MemoryItem.h>
#include <editors/MemoryDesigner/MemoryDesignerConstants.h>

#include <QQueue>

//-----------------------------------------------------------------------------
// Function: MasterSlavePathSearch::findMasterSlavePaths()
//-----------------------------------------------------------------------------
QVector<QVector<QSharedPointer<ConnectivityInterface const> > > MasterSlavePathSearch::
    findMasterSlavePaths(QSharedPointer<const ConnectivityGraph> graph, bool allowOverlappingPaths) const
{
    QVector<MasterSlavePathSearch::Path> masterPaths;

    for (auto const& masterInterface : findInitialMasterInterfaces(graph))
    {
        masterPaths.append(findPaths(masterInterface,  graph));
    }

    masterPaths = findValidPathsIn(masterPaths, allowOverlappingPaths);

    for (auto const& edge : graph->getConnections())
    {
        if (edge->getName().contains(QStringLiteral("_to_local_memory_map_")) &&
            edge->getFirstInterface() == edge->getSecondInterface())
        {
            QVector<QSharedPointer<ConnectivityInterface const> > localConnection;
            localConnection.append(edge->getFirstInterface());
            localConnection.append(edge->getSecondInterface());
            masterPaths.append(localConnection);
        }
    }

    return masterPaths;
}

//-----------------------------------------------------------------------------
// Function: MasterSlavePathSearch::findInitialMasterInterfaces()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<ConnectivityInterface> > MasterSlavePathSearch::findInitialMasterInterfaces(
    QSharedPointer<const ConnectivityGraph> graph) const
{
    QVector<QSharedPointer<ConnectivityInterface> > masterInterfaces;

    for (auto const& vertex : graph->getInterfaces())
    {
        if (isStartInterface(vertex) && isConnectedToMemory(vertex))
        {
            masterInterfaces.append(vertex);
        }
    }

    return masterInterfaces;
}

//-----------------------------------------------------------------------------
// Function: MasterSlavePathSearch::isStartInterface()
//-----------------------------------------------------------------------------
bool MasterSlavePathSearch::isStartInterface(QSharedPointer<const ConnectivityInterface> vertex) const
{
    return vertex->getMode() == General::MASTER || vertex->getMode() == General::INITIATOR;
}

//-----------------------------------------------------------------------------
// Function: MasterSlavePathSearch::isConnectedToMemory()
//-----------------------------------------------------------------------------
bool MasterSlavePathSearch::isConnectedToMemory(QSharedPointer<const ConnectivityInterface> vertex) const
{
    if (vertex->getMode() == General::MASTER)
    {
        return vertex->isConnectedToMemory();
    }
    else if (vertex->getMode() == General::INITIATOR && vertex->getInstance())
    {
        return vertex->isConnectedToMemory() || isConnectedToSubspaceMap(vertex);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MasterSlavePathSearch::isConnectedToSubspaceMap()
//-----------------------------------------------------------------------------
bool MasterSlavePathSearch::isConnectedToSubspaceMap(QSharedPointer<const ConnectivityInterface> vertex) const
{
    for (auto memoryItem : vertex->getInstance()->getMemories())
    {
        if (memoryItem->getType() == MemoryDesignerConstants::MEMORYMAP_TYPE)
        {
            for (auto memoryBlock : memoryItem->getChildItems())
            {
                if (memoryBlock->getType() == MemoryDesignerConstants::SUBSPACEMAP_TYPE && memoryBlock->getInitiatorReference() == vertex->getName())
                {
                    return true;
                }
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MasterSlavePathSearch::findPaths()
//-----------------------------------------------------------------------------
QVector<MasterSlavePathSearch::Path> MasterSlavePathSearch::findPaths(QSharedPointer<ConnectivityInterface > startVertex, QSharedPointer<const ConnectivityGraph> graph) const
{
    QVector<MasterSlavePathSearch::Path> foundPaths;
    QVector<QSharedPointer<ConnectivityInterface> > visitedVertices;

    for (auto const& path : findPathsFromInterface(startVertex, visitedVertices, graph))
    {
        foundPaths.append(path);
    }

    return foundPaths;
}

//-----------------------------------------------------------------------------
// Function: MasterSlavePathSearch::findPathsFromInterface()
//-----------------------------------------------------------------------------
QVector<MasterSlavePathSearch::Path> MasterSlavePathSearch::findPathsFromInterface(
    QSharedPointer<ConnectivityInterface> currentVertex,
    QVector<QSharedPointer<ConnectivityInterface>> visitedVertices,
    QSharedPointer<const ConnectivityGraph> graph) const
{
    visitedVertices.append(currentVertex);
    QVector<MasterSlavePathSearch::Path> foundPaths;
    QVector<MasterSlavePathSearch::Path> pathsFromVertices;

    for (auto const& nextEdge : graph->getConnectionsFor(currentVertex))
    {
        auto nextVertex = findConnectedInterface(currentVertex, nextEdge);

        if (visitedVertices.contains(nextVertex) == false && canConnectInterfaces(currentVertex, nextVertex) && pathDirectionIsAccepted(currentVertex, nextVertex))
        {
            if (visitedVertices.contains(nextVertex) == false)
            {
                for (auto const& path : findPathsFromInterface(nextVertex, visitedVertices, graph))
                {
                    pathsFromVertices.append(path);
                }
            }
        }
    }

    if (pathsFromVertices.isEmpty())
    {
        MasterSlavePathSearch::Path newPath{ currentVertex };
        foundPaths.append(newPath);
    }
    else
    {
        for (auto path : pathsFromVertices)
        {
            path.prepend(currentVertex);
            foundPaths.append(path);
        }
    }

    return foundPaths;
}

//-----------------------------------------------------------------------------
// Function: MasterSlavePathSearch::pathDirectionIsAccepted()
//-----------------------------------------------------------------------------
bool MasterSlavePathSearch::pathDirectionIsAccepted(QSharedPointer<ConnectivityInterface> startVertex, QSharedPointer<ConnectivityInterface> connectedVertex) const
{
    General::InterfaceMode startMode = startVertex->getMode();
    General::InterfaceMode connectedMode = connectedVertex->getMode();
    bool acceptDirection = true;

    if ((startMode == General::MASTER || startMode == General::INITIATOR) &&
        (connectedMode == General::SLAVE || connectedMode == General::TARGET) &&
        startVertex->getInstance() == connectedVertex->getInstance())
    {
        acceptDirection = false;
    }
    else if (startMode == General::SLAVE || startMode == General::TARGET)
    {
        if (connectedMode == General::MIRRORED_SLAVE || connectedMode == General::MIRRORED_TARGET)
        {
            acceptDirection = false;
        }
        else if ((connectedMode == General::MASTER || connectedMode == General::INITIATOR) &&
            (startVertex->getInstance() != connectedVertex->getInstance()))
        {
            acceptDirection = false;
        }
    }
    else if ((startMode == General::MIRRORED_MASTER || startMode == General::MIRRORED_INITIATOR) &&
        (connectedMode == General::MASTER || connectedMode == General::INITIATOR))
    {
        return false;
    }
    else if ((startMode == General::MIRRORED_SLAVE || startMode == General::MIRRORED_TARGET) &&
        (connectedMode == General::MIRRORED_MASTER || connectedMode == General::MIRRORED_INITIATOR))
    {
        return false;
    }

    return acceptDirection;
}

//-----------------------------------------------------------------------------
// Function: MasterSlavePathSearch::vertexIsLeaf()
//-----------------------------------------------------------------------------
bool MasterSlavePathSearch::vertexIsLeaf(QSharedPointer<ConnectivityInterface> possibleLeaf, QSharedPointer<const ConnectivityGraph> graph) const
{
    General::InterfaceMode leafMode = possibleLeaf->getMode();

    return (leafMode == General::SLAVE || leafMode == General::TARGET) && possibleLeaf->isBridged() == false;
}

//-----------------------------------------------------------------------------
// Function: MasterSlavePathSearch::findConnectedInterface()
//-----------------------------------------------------------------------------
QSharedPointer<ConnectivityInterface> MasterSlavePathSearch::findConnectedInterface(
    QSharedPointer<ConnectivityInterface const> startInterface, QSharedPointer<ConnectivityConnection const> edge)
    const
{
    QSharedPointer<ConnectivityInterface> firstInterface = edge->getFirstInterface();
    if (startInterface == firstInterface)
    {
        return edge->getSecondInterface();
    }
    else
    {
        return firstInterface;
    }
}

//-----------------------------------------------------------------------------
// Function: MasterSlavePathSearch::canConnectInterfaces()
//-----------------------------------------------------------------------------
bool MasterSlavePathSearch::canConnectInterfaces(QSharedPointer<ConnectivityInterface const> startVertex,
    QSharedPointer<ConnectivityInterface const> endVertex) const
{
    if (startVertex == endVertex)
    {
        return false;
    }
    
    if (startVertex->getMode() == endVertex->getMode() && (startVertex->isHierarchical() || endVertex->isHierarchical()))
    {
        return true;
    }

    General::InterfaceMode initiatorMode = General::INITIATOR;
    General::InterfaceMode mirroredInitiatorMode = General::MIRRORED_INITIATOR;
    General::InterfaceMode targetMode = General::TARGET;
    General::InterfaceMode mirroredTargetMode = General::MIRRORED_TARGET;

    General::InterfaceMode startMode = startVertex->getMode();
    bool startModeIsRevision2022 = General::modeIsRevision2022(startMode);
    if (startModeIsRevision2022 == false)
    {
        initiatorMode = General::MASTER;
        mirroredInitiatorMode = General::MIRRORED_MASTER;
        targetMode = General::SLAVE;
        mirroredTargetMode = General::MIRRORED_SLAVE;
    }

    bool startIsInitiator = startMode == initiatorMode;
    bool startIsTarget = startMode == targetMode;
    bool startIsMirroredInitiator = startMode == mirroredInitiatorMode;
    bool startIsMirroredTarget = startMode == mirroredTargetMode;
    bool startIsBridged = startVertex->isBridged();

    General::InterfaceMode endMode = endVertex->getMode();
    bool endModeIsRevision2022 = General::modeIsRevision2022(endMode);
    bool endIsInitiator = endMode == initiatorMode;
    bool endIsTarget = endMode == targetMode;
    bool endIsMirroredInitiator = endMode == mirroredInitiatorMode;
    bool endIsMirroredTarget = endMode == mirroredTargetMode;
    bool endIsBridged = endVertex->isBridged();

    QSharedPointer<ConnectivityComponent const> startInstance = startVertex->getInstance();
    QSharedPointer<ConnectivityComponent const> endInstance = endVertex->getInstance();

    return
        (startModeIsRevision2022 == endModeIsRevision2022) && (
        (startIsInitiator && ((endIsTarget && startInstance != endInstance) || endIsMirroredInitiator)) ||
        (startIsTarget && (endIsMirroredTarget || (endIsInitiator && startIsBridged && endIsBridged && startInstance == endInstance))) ||
        (startIsMirroredInitiator && endIsMirroredTarget && startInstance == endInstance) ||
        (startIsMirroredTarget && (endIsTarget || (endIsMirroredInitiator && startInstance == endInstance)))
        );
}

//-----------------------------------------------------------------------------
// Function: MasterSlavePathSearch::removeDuplicatePaths()
//-----------------------------------------------------------------------------
QVector<MasterSlavePathSearch::Path> MasterSlavePathSearch::findValidPathsIn(QVector<MasterSlavePathSearch::Path> const& paths,
    bool allowOverlappingPaths) const
{
    QVector<QVector<QSharedPointer<ConnectivityInterface const> > > highPaths;

    for (auto const& currentPath : paths)
    {
        if ((currentPath.size() > 1 && pathEndsInMemoryMap(currentPath)) &&
            (allowOverlappingPaths == true || (allowOverlappingPaths == false && pathIsFullPath(currentPath, paths))))
        {
            highPaths.append(currentPath);
        }
    }

    return highPaths;
}

//-----------------------------------------------------------------------------
// Function: MasterSlavePathSearch::pathIsFullPath()
//-----------------------------------------------------------------------------
bool MasterSlavePathSearch::pathIsFullPath(MasterSlavePathSearch::Path const& currentPath, 
    QVector<MasterSlavePathSearch::Path> const& availablePaths) const
{
    for (int comparisonIndex = 0; comparisonIndex < availablePaths.size(); ++comparisonIndex)
    {
        QVector<QSharedPointer<ConnectivityInterface const> > comparisonPath =
            availablePaths.at(comparisonIndex);

        if (currentPath != comparisonPath && currentPath.last() == comparisonPath.last())
        {
            auto const pathLength = currentPath.size();
            auto const comparisonPathLength = comparisonPath.size();

            auto overlapLength = qMin(pathLength, comparisonPathLength);

            // Exclude path only if the start interface isn't bridged (e.g. if there is a CPU in middle 
            // of a path) or the path is contained within another path.
            if (pathLength < comparisonPathLength &&
                pathContainsAnotherPath(currentPath, comparisonPath, overlapLength))
            {
                return false;
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: MasterSlavePathSearch::pathContainsAnotherPath()
//-----------------------------------------------------------------------------
bool MasterSlavePathSearch::pathContainsAnotherPath(
    QVector<QSharedPointer<ConnectivityInterface const> > const& pathOne,
    QVector<QSharedPointer<ConnectivityInterface const> > const& pathTwo, int areaSize) const
{
    for (int areaIndex = 0; areaIndex < areaSize; areaIndex++)
    {
        auto const pathOneIndex = pathOne.size() - 1 - areaIndex;
        auto const pathTwoIndex = pathTwo.size() - 1 - areaIndex;
        if (pathOne.at(pathOneIndex) != pathTwo.at(pathTwoIndex))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: MasterSlavePathSearch::pathEndsInMemoryMap()
//-----------------------------------------------------------------------------
bool MasterSlavePathSearch::pathEndsInMemoryMap(QVector<QSharedPointer<ConnectivityInterface const> > const& path)
    const
{
    QSharedPointer<ConnectivityInterface const> lastInterface = path.last();

    return lastInterface && lastInterface->getConnectedMemory() &&
        lastInterface->getConnectedMemory()->getType().compare(MemoryDesignerConstants::MEMORYMAP_TYPE) == 0;
}
