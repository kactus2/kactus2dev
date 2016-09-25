//-----------------------------------------------------------------------------
// File: MasterSlavePathSearch.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.08.2016
//
// Description:
// <Short description of the class/file contents>
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
        foreach (QSharedPointer<ConnectivityConnection> nextEdge, connections)
        {
            if (!existingPath.contains(nextEdge->getFirstInterface()))
            {
                findPaths(nextEdge->getFirstInterface(), nextEdge, existingPath, graph);
            }
            else if (!existingPath.contains(nextEdge->getSecondInterface()))
            {
                findPaths(nextEdge->getSecondInterface(), nextEdge, existingPath, graph);
            }
        }
    }
} 
