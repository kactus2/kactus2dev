//-----------------------------------------------------------------------------
// File: ConnectivityGraph.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 12.08.2016
//
// Description:
// Graph for representing component connectivity through design hierarchies.
//-----------------------------------------------------------------------------

#include "ConnectivityGraph.h"

#include "ConnectivityConnection.h"
#include "ConnectivityInterface.h"
#include "ConnectivityComponent.h"

//-----------------------------------------------------------------------------
// Function: ConnectivityGraph::ConnectivityGraph()
//-----------------------------------------------------------------------------
ConnectivityGraph::ConnectivityGraph():
vertices_(),
edges_(),
instances_()
{

}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraph::~ConnectivityGraph()
//-----------------------------------------------------------------------------
ConnectivityGraph::~ConnectivityGraph()
{

}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraph::getInterfaces()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<ConnectivityInterface> >& ConnectivityGraph::getInterfaces() const
{
    return vertices_;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraph::getInstances()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<ConnectivityComponent> >& ConnectivityGraph::getInstances() const
{
    return instances_;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraph::getConnectionsFor()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<ConnectivityConnection const> > ConnectivityGraph::getConnectionsFor(
    QSharedPointer<ConnectivityInterface const> startPoint) const
{
    QVector<QSharedPointer<ConnectivityConnection const> > connections;

    for (int i = 0; i< edges_.size(); ++i)
    {
        QSharedPointer<ConnectivityConnection const> edge = edges_.at(i);
        if (edge->getFirstInterface() == startPoint || edge->getSecondInterface() == startPoint)
        {
            connections.append(edge);
        }
    }

    return connections;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraph::getInterface()
//-----------------------------------------------------------------------------
QSharedPointer<ConnectivityInterface const> ConnectivityGraph::getInterface(QString const& interfaceName,
    QString const& instanceName) 
{
    for (int i = 0; i < vertices_.size(); ++i)
    {
        QSharedPointer<ConnectivityInterface const> vertex = vertices_[i];
        if (vertex->getName().compare(interfaceName) == 0 &&
            vertex->getInstance()->getName().compare(instanceName) == 0)            
        {
            return vertex;
        }
    }

    return QSharedPointer<ConnectivityInterface const>();
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraph::getConnections()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<ConnectivityConnection> >& ConnectivityGraph::getConnections() const
{
    return edges_;
}
