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

//-----------------------------------------------------------------------------
// Function: MasterSlavePathSearch::MasterSlavePathSearch()
//-----------------------------------------------------------------------------
MasterSlavePathSearch::MasterSlavePathSearch(): masterPaths_()
{

}

//-----------------------------------------------------------------------------
// Function: MasterSlavePathSearch::~MasterSlavePathSearch()
//-----------------------------------------------------------------------------
MasterSlavePathSearch::~MasterSlavePathSearch()
{

}

//-----------------------------------------------------------------------------
// Function: MasterSlavePathSearch::findMasterSlavePaths()
//-----------------------------------------------------------------------------
QVector<QVector<QSharedPointer<ConnectivityInterface const> > > MasterSlavePathSearch::findMasterSlavePaths(
    QSharedPointer<const ConnectivityGraph> graph)
{
    masterPaths_.clear();

    foreach (QSharedPointer<ConnectivityInterface const> masterInterface, findInitialMasterInterfaces(graph))
    {
        findPaths(masterInterface, QSharedPointer<ConnectivityConnection const>(),
            QVector<QSharedPointer<ConnectivityInterface const> >(), graph);
    }

    removeDuplicatePaths();

    for (int i = 0; i < graph->getConnections().size(); ++i)
    {
        ConnectivityConnection const* edge = graph->getConnections().at(i).data();
        if (edge->getName().contains(QStringLiteral("_to_local_memory_map_")) &&
            edge->getFirstInterface() == edge->getSecondInterface())
        {
            QVector<QSharedPointer<ConnectivityInterface const> > localConnection;
            localConnection.append(edge->getFirstInterface());
            localConnection.append(edge->getSecondInterface());
            masterPaths_.append(localConnection);
        }
    }

    return masterPaths_;
}

//-----------------------------------------------------------------------------
// Function: MasterSlavePathSearch::findInitialMasterInterfaces()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<ConnectivityInterface const> > MasterSlavePathSearch::findInitialMasterInterfaces(
    QSharedPointer<const ConnectivityGraph> graph) const
{
    QVector<QSharedPointer<ConnectivityInterface const> > masterInterfaces;

    QVector<QSharedPointer<ConnectivityInterface> > graphInterfaces = graph->getInterfaces();
    for (int i = 0; i < graphInterfaces.size(); ++i)
    {
        QSharedPointer<ConnectivityInterface const> vertex = graphInterfaces[i];
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
void MasterSlavePathSearch::findPaths(QSharedPointer<ConnectivityInterface const> startVertex,
    QSharedPointer<ConnectivityConnection const> previousEdge,
    QVector<QSharedPointer<ConnectivityInterface const> > existingPath,
    QSharedPointer<const ConnectivityGraph> graph)
{
    existingPath.append(startVertex);

    QVector<QSharedPointer<ConnectivityConnection const> > connections =
        graph->getConnectionsFor(existingPath.last());

    if (connections.contains(previousEdge))
    {
        connections.remove(connections.indexOf(previousEdge));
    }

    if (connections.isEmpty())
    {
        masterPaths_.append(existingPath);
    }
    else
    {
        bool connectionFound = false;

        foreach (QSharedPointer<ConnectivityConnection const> nextEdge, connections)
        {
            QSharedPointer<ConnectivityInterface const> endVertex = findConnectedInterface(startVertex, nextEdge);
            if (!existingPath.contains(endVertex) && canConnectInterfaces(startVertex, endVertex))
            {
                findPaths(endVertex, nextEdge, existingPath, graph);
                connectionFound = true;
            }
        }

        if (!connectionFound && existingPath.size() > 1)
        {
            masterPaths_.append(existingPath);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MasterSlavePathSearch::findConnectedInterface()
//-----------------------------------------------------------------------------
QSharedPointer<ConnectivityInterface const> MasterSlavePathSearch::findConnectedInterface(
    QSharedPointer<ConnectivityInterface const> startInterface, QSharedPointer<ConnectivityConnection const> edge)
    const
{
    QSharedPointer<ConnectivityInterface const> firstInterface = edge->getFirstInterface();
    QSharedPointer<ConnectivityInterface const> secondInterface = edge->getSecondInterface();

    if (startInterface == firstInterface)
    {
        return secondInterface;
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

    QString startMode = startVertex->getMode();
    QSharedPointer<ConnectivityComponent const> startInstance = startVertex->getInstance();
    bool startIsMaster = startMode.compare(QLatin1String("master"), Qt::CaseInsensitive) == 0;
    bool startIsSlave = startMode.compare(QLatin1String("slave"), Qt::CaseInsensitive) == 0;
    bool startIsMirroredMaster = startMode.compare(QLatin1String("mirroredMaster"), Qt::CaseInsensitive) == 0;
    bool startIsMirroredSlave = startMode.compare(QLatin1String("mirroredSlave"), Qt::CaseInsensitive) == 0;
    bool startIsHierarhcical = startVertex->isHierarchical();
    bool startIsBridged = startVertex->isBridged();

    QString endMode = endVertex->getMode();
    QSharedPointer<ConnectivityComponent const> endInstance = endVertex->getInstance();
    bool endIsMaster = endMode.compare(QLatin1String("master"), Qt::CaseInsensitive) == 0;
    bool endIsSlave = endMode.compare(QLatin1String("slave"), Qt::CaseInsensitive) == 0;
    bool endIsMirroredMaster = endMode.compare(QLatin1String("mirroredMaster"), Qt::CaseInsensitive) == 0;
    bool endIsMirroredSlave = endMode.compare(QLatin1String("mirroredSlave"), Qt::CaseInsensitive) == 0;
    bool endIsBridged = endVertex->isBridged();
    bool endIsHierarchical = endVertex->isHierarchical();

    return (startVertex->getMode() == endVertex->getMode() && (startIsHierarhcical || endIsHierarchical)) ||
        (startIsMaster && ((endIsSlave && startInstance != endInstance) || endIsMirroredMaster)) ||
        (startIsSlave && (endIsMirroredSlave ||
            (endIsMaster && startIsBridged && endIsBridged && startInstance == endInstance))) ||
        (startIsMirroredMaster && endIsMirroredSlave && startInstance == endInstance) ||
        (startIsMirroredSlave && (endIsSlave || (endIsMirroredMaster && startInstance == endInstance)));
}

//-----------------------------------------------------------------------------
// Function: MasterSlavePathSearch::removeDuplicatePaths()
//-----------------------------------------------------------------------------
void MasterSlavePathSearch::removeDuplicatePaths()
{
    QVector<QVector<QSharedPointer<ConnectivityInterface const> > > highPaths;

    for (int currentIndex = 0; currentIndex < masterPaths_.size(); ++currentIndex)
    {
        QVector<QSharedPointer<ConnectivityInterface const> > currentPath = masterPaths_.at(currentIndex);
        if (currentPath.size() > 1 && pathEndsInMemoryMap(currentPath) &&
            pathIsFullPath(currentPath, currentIndex))
        {
            highPaths.append(currentPath);
        }
    }

    masterPaths_.clear();
    masterPaths_ = highPaths;
}

//-----------------------------------------------------------------------------
// Function: MasterSlavePathSearch::pathIsFullPath()
//-----------------------------------------------------------------------------
bool MasterSlavePathSearch::pathIsFullPath(
    QVector<QSharedPointer<ConnectivityInterface const> > const& currentPath, int currentIndex)
{
    for (int comparisonIndex = currentIndex + 1; comparisonIndex < masterPaths_.size(); ++comparisonIndex)
    {
        QVector<QSharedPointer<ConnectivityInterface const> > comparisonPath =
            masterPaths_.at(comparisonIndex);

        QList<QSharedPointer<ConnectivityInterface const> > currentList = currentPath.toList();
        QList<QSharedPointer<ConnectivityInterface const> > comparisonList = comparisonPath.toList();

        if (currentPath.last() == comparisonPath.last())
        {
            int pathLength = currentPath.size();
            int comparisonPathLength = comparisonPath.size();

            int examinationArea = pathLength;
            if (comparisonPathLength < pathLength)
            {
                examinationArea = comparisonPathLength;
            }

            if (pathContainsAnotherPath(currentPath, comparisonPath, examinationArea))
            {
                if (examinationArea == comparisonPathLength)
                {
                    masterPaths_.remove(masterPaths_.indexOf(comparisonPath));
                }
                else
                {
                    return false;
                }
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
        int pathOneIndex = pathOne.size() - 1 - areaIndex;
        int pathTwoIndex = pathTwo.size() - 1 - areaIndex;
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
    if (lastInterface && lastInterface->getConnectedMemory() &&
        lastInterface->getConnectedMemory()->getType().compare("memoryMap") == 0)
    {
        return true;
    }

    return false;
}
