//-----------------------------------------------------------------------------
// File: ConnectivityGraph.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
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
ConnectivityGraph::ConnectivityGraph(): vertices_(), edges_(), instances_()
{

}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraph::~ConnectivityGraph()
//-----------------------------------------------------------------------------
ConnectivityGraph::~ConnectivityGraph()
{

}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraph::addInterface()
//-----------------------------------------------------------------------------
void ConnectivityGraph::addInterface(QSharedPointer<ConnectivityInterface> vertex)
{
    vertices_.append(vertex);
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraph::addConnection()
//-----------------------------------------------------------------------------
void ConnectivityGraph::addConnection(QString const& connectionName, 
    QSharedPointer<ConnectivityInterface> startPoint, QSharedPointer<ConnectivityInterface> endPoint)
{
    if (!startPoint.isNull() && !endPoint.isNull())
    {
        QSharedPointer<ConnectivityConnection> connection(new ConnectivityConnection(connectionName, 
            startPoint, endPoint));
        edges_.append(connection);
    }
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraph::addInstance()
//-----------------------------------------------------------------------------
void ConnectivityGraph::addInstance(QSharedPointer<ConnectivityComponent> instanceModel)
{
    instances_.append(instanceModel);
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraph::getInterfaces()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<ConnectivityInterface> > ConnectivityGraph::getInterfaces() const
{
    return vertices_;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraph::getInstances()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<ConnectivityComponent> > ConnectivityGraph::getInstances() const
{
    return instances_;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraph::getConnectionsFor()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<ConnectivityConnection> > ConnectivityGraph::getConnectionsFor(
    QSharedPointer<ConnectivityInterface> startPoint) const
{
    QVector<QSharedPointer<ConnectivityConnection> > connections;

    foreach (QSharedPointer<ConnectivityConnection> edge, edges_)
    {
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
QSharedPointer<ConnectivityInterface> ConnectivityGraph::getInterface(QString const& instanceName, 
    QString const& interfaceName) const
{
    foreach (QSharedPointer<ConnectivityInterface> vertex, vertices_)
    {
        if (vertex->getName() == interfaceName && vertex->getInstance()->getName() == instanceName)
        {
            return vertex;
        }
    }

    return QSharedPointer<ConnectivityInterface>();
}
