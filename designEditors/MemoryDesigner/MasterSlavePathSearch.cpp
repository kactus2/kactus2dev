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

#include <designEditors/MemoryDesigner/ConnectivityConnection.h>
#include <designEditors/MemoryDesigner/ConnectivityGraph.h>
#include <designEditors/MemoryDesigner/ConnectivityInterface.h>

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
QVector<QVector<QSharedPointer<ConnectivityInterface> > > MasterSlavePathSearch::findMasterSlavePaths(
    QSharedPointer<const ConnectivityGraph> graph)
{
    masterPaths_.clear();

    foreach (QSharedPointer<ConnectivityInterface> masterInterface, findInitialMasterInterfaces(graph))
    {
        findPaths(masterInterface, QSharedPointer<ConnectivityConnection>(0), 
            QVector<QSharedPointer<ConnectivityInterface> >(), graph);
    }

    foreach (QSharedPointer<ConnectivityConnection> edge, graph->getConnections())
    {
        if (edge->getName().contains(QStringLiteral("_to_local_memory_map_")) &&
            edge->getFirstInterface() == edge->getSecondInterface())
        {
            QVector<QSharedPointer<ConnectivityInterface> > localConnection;
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
QVector<QSharedPointer<ConnectivityInterface> > MasterSlavePathSearch::findInitialMasterInterfaces(
    QSharedPointer<const ConnectivityGraph> graph) const
{
    QVector<QSharedPointer<ConnectivityInterface> > masterInterfaces;

    foreach (QSharedPointer<ConnectivityInterface> vertex, graph->getInterfaces())
    {
        if (vertex->getMode().compare(QStringLiteral("master")) == 0 && !vertex->isHierarchical() &&
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
void MasterSlavePathSearch::findPaths(QSharedPointer<ConnectivityInterface> startVertex, 
    QSharedPointer<ConnectivityConnection> previousEdge, 
    QVector<QSharedPointer<ConnectivityInterface> > existingPath, QSharedPointer<const ConnectivityGraph> graph)
{
    existingPath.append(startVertex);

    QVector<QSharedPointer<ConnectivityConnection> > connections = graph->getConnectionsFor(existingPath.last());

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

        foreach (QSharedPointer<ConnectivityConnection> nextEdge, connections)
        {
            QSharedPointer<ConnectivityInterface> endVertex = findConnectedInterface(startVertex, nextEdge);
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
QSharedPointer<ConnectivityInterface> MasterSlavePathSearch::findConnectedInterface(
    QSharedPointer<ConnectivityInterface> startInterface, QSharedPointer<ConnectivityConnection> edge) const
{
    QSharedPointer<ConnectivityInterface> firstInterface = edge->getFirstInterface();
    QSharedPointer<ConnectivityInterface> secondInterface = edge->getSecondInterface();

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
bool MasterSlavePathSearch::canConnectInterfaces(QSharedPointer<ConnectivityInterface> startVertex,
    QSharedPointer<ConnectivityInterface> endVertex) const
{
    if (startVertex == endVertex)
    {
        return false;
    }

    QString startMode = startVertex->getMode();
    QSharedPointer<ConnectivityComponent> startInstance = startVertex->getInstance();
    bool startIsMaster = startMode.compare(QLatin1String("master"), Qt::CaseInsensitive) == 0;
    bool startIsSlave = startMode.compare(QLatin1String("slave"), Qt::CaseInsensitive) == 0;
    bool startIsMirroredMaster = startMode.compare(QLatin1String("mirroredMaster"), Qt::CaseInsensitive) == 0;
    bool startIsMirroredSlave = startMode.compare(QLatin1String("mirroredSlave"), Qt::CaseInsensitive) == 0;

    QString endMode = endVertex->getMode();
    QSharedPointer<ConnectivityComponent> endInstance = endVertex->getInstance();
    bool endIsMaster = endMode.compare(QLatin1String("master"), Qt::CaseInsensitive) == 0;
    bool endIsSlave = endMode.compare(QLatin1String("slave"), Qt::CaseInsensitive) == 0;
    bool endIsMirroredMaster = endMode.compare(QLatin1String("mirroredMaster"), Qt::CaseInsensitive) == 0;
    bool endIsMirroredSlave = endMode.compare(QLatin1String("mirroredSlave"), Qt::CaseInsensitive) == 0;

    return (startVertex->getMode() == endVertex->getMode() &&
        startVertex->isHierarchical() || endVertex->isHierarchical()) ||
        (startIsMaster && (endIsSlave || endIsMirroredMaster)) ||
        (startIsSlave && (endIsMirroredSlave ||
            (endIsMaster && startVertex->isBridged() && endVertex->isBridged() && startInstance == endInstance))) ||
        (startIsMirroredMaster && endIsMirroredSlave && startInstance == endInstance) ||
        (startIsMirroredSlave && (endIsSlave || (endIsMirroredMaster && startInstance == endInstance)));
}
