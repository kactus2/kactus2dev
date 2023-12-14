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
#include <editors/MemoryDesigner/ConnectivityGraph.h>
#include <editors/MemoryDesigner/ConnectivityInterface.h>
#include <editors/MemoryDesigner/MemoryItem.h>

#include <QQueue>

//-----------------------------------------------------------------------------
// Function: MasterSlavePathSearch::findMasterSlavePaths()
//-----------------------------------------------------------------------------
QVector<QVector<QSharedPointer<ConnectivityInterface const> > > MasterSlavePathSearch::findMasterSlavePaths(
    QSharedPointer<const ConnectivityGraph> graph) const
{
    QVector<MasterSlavePathSearch::Path> masterPaths;

    for (auto const& masterInterface : findInitialMasterInterfaces(graph))
    {
        masterPaths.append(findPaths(masterInterface,  graph));
    }

    masterPaths = findValidPathsIn(masterPaths);

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
        if (vertex->getMode().compare(QStringLiteral("master")) == 0 && vertex->getConnectedMemory() &&
            !vertex->isBridged())
        {
            masterInterfaces.append(vertex);
        }
    }

    return masterInterfaces;
}

//-----------------------------------------------------------------------------
// Function: MasterSlavePathSearch::findPaths()
//-----------------------------------------------------------------------------
QVector<MasterSlavePathSearch::Path> MasterSlavePathSearch::findPaths(
    QSharedPointer<ConnectivityInterface> startVertex,
    QSharedPointer<const ConnectivityGraph> graph) const
{
    QVector<MasterSlavePathSearch::Path> foundPaths;

    // Use breadth-first search (BFS) to find shortest paths and avoid loops.
    // The search spans a BFS tree in the graph using the parent-property of the vertices.
    QQueue<QSharedPointer<ConnectivityInterface> > verticesToSearch;
    QVector<QSharedPointer<ConnectivityInterface> > traversed;
    startVertex->setParent(nullptr);
    traversed.append(startVertex);
    verticesToSearch.enqueue(startVertex);

    while (verticesToSearch.isEmpty() == false)
    {
        auto const currentVertex = verticesToSearch.dequeue();
        auto const connections = graph->getConnectionsFor(currentVertex);

        bool isLeaf = true;
        for (auto const& nextEdge : connections)
        {
            auto nextVertex = findConnectedInterface(currentVertex, nextEdge);
            if (traversed.contains(nextVertex) == false && canConnectInterfaces(currentVertex, nextVertex))
            {
                traversed.append(nextVertex);
                nextVertex->setParent(currentVertex);
                verticesToSearch.enqueue(nextVertex);
                isLeaf = false;
            }
        }

        if (isLeaf)
        {
            foundPaths.append(findPathFromRoot(currentVertex));
        }
    }

    return foundPaths;
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

    QString startMode = startVertex->getMode();
    QSharedPointer<ConnectivityComponent const> startInstance = startVertex->getInstance();
    bool startIsMaster = startMode.compare(QLatin1String("master"), Qt::CaseInsensitive) == 0;
    bool startIsSlave = startMode.compare(QLatin1String("slave"), Qt::CaseInsensitive) == 0;
    bool startIsMirroredMaster = startMode.compare(QLatin1String("mirroredMaster"), Qt::CaseInsensitive) == 0;
    bool startIsMirroredSlave = startMode.compare(QLatin1String("mirroredSlave"), Qt::CaseInsensitive) == 0;
    bool startIsBridged = startVertex->isBridged();

    QString endMode = endVertex->getMode();
    QSharedPointer<ConnectivityComponent const> endInstance = endVertex->getInstance();
    bool endIsMaster = endMode.compare(QLatin1String("master"), Qt::CaseInsensitive) == 0;
    bool endIsSlave = endMode.compare(QLatin1String("slave"), Qt::CaseInsensitive) == 0;
    bool endIsMirroredMaster = endMode.compare(QLatin1String("mirroredMaster"), Qt::CaseInsensitive) == 0;
    bool endIsMirroredSlave = endMode.compare(QLatin1String("mirroredSlave"), Qt::CaseInsensitive) == 0;
    bool endIsBridged = endVertex->isBridged();

    return (startIsMaster && ((endIsSlave && startInstance != endInstance) || endIsMirroredMaster)) ||
        (startIsSlave && (endIsMirroredSlave ||
            (endIsMaster && startIsBridged && endIsBridged && startInstance == endInstance))) ||
        (startIsMirroredMaster && endIsMirroredSlave && startInstance == endInstance) ||
        (startIsMirroredSlave && (endIsSlave || (endIsMirroredMaster && startInstance == endInstance)));
}

//-----------------------------------------------------------------------------
// Function: MasterSlavePathSearch::findPathFromRoot()
//-----------------------------------------------------------------------------
MasterSlavePathSearch::Path MasterSlavePathSearch::findPathFromRoot(
    QSharedPointer<ConnectivityInterface const> endVertex) const
{
    QVector<QSharedPointer<ConnectivityInterface const> > path;
    path.append(endVertex);

    auto i = endVertex;
    while (i->getParent() != nullptr)
    {
        path.append(i->getParent());
        i = i->getParent();
    }

    std::reverse(path.begin(), path.end());
    return path;
}

//-----------------------------------------------------------------------------
// Function: MasterSlavePathSearch::removeDuplicatePaths()
//-----------------------------------------------------------------------------
QVector<MasterSlavePathSearch::Path> MasterSlavePathSearch::findValidPathsIn(
    QVector<MasterSlavePathSearch::Path> const& paths) const
{
    QVector<QVector<QSharedPointer<ConnectivityInterface const> > > highPaths;

    for (auto const& currentPath : paths)
    {
        if (currentPath.size() > 1 && pathEndsInMemoryMap(currentPath) &&
            pathIsFullPath(currentPath, paths))
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

            if (pathContainsAnotherPath(currentPath, comparisonPath, overlapLength) &&
                pathLength < comparisonPathLength)
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
        lastInterface->getConnectedMemory()->getType().compare("memoryMap") == 0;
}
