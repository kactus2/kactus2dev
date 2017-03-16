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
        QSharedPointer<ConnectivityComponent> startInstance = startVertex->getInstance();
        bool startVertexIsMirroredSlave =
            startVertex->getMode().compare("mirroredSlave", Qt::CaseInsensitive) == 0;

        foreach (QSharedPointer<ConnectivityConnection> nextEdge, connections)
        {
            QSharedPointer<ConnectivityInterface> firstInterface = nextEdge->getFirstInterface();
            QSharedPointer<ConnectivityInterface> secondInterface = nextEdge->getSecondInterface();

            if (!existingPath.contains(firstInterface) &&
                (!startVertexIsMirroredSlave ||
                (startVertexIsMirroredSlave && startInstance != firstInterface->getInstance())))
            {
                findPaths(firstInterface, nextEdge, existingPath, graph);
            }
            else if (!existingPath.contains(secondInterface) &&
                (!startVertexIsMirroredSlave ||
                (startVertexIsMirroredSlave && startInstance != secondInterface->getInstance())))
            {
                findPaths(secondInterface, nextEdge, existingPath, graph);
            }
        }
    }
} 
