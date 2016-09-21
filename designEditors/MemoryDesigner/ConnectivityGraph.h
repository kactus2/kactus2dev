//-----------------------------------------------------------------------------
// File: ConnectivityGraph.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 12.08.2016
//
// Description:
// Graph for representing component connectivity through design hierarchies.
//-----------------------------------------------------------------------------

#ifndef CONNECTIVITYGRAPH_H
#define CONNECTIVITYGRAPH_H

#include <QSharedPointer>
#include <QVector>

class ConnectivityConnection;
class ConnectivityInterface;
class ConnectivityComponent;
class MemoryItem;

//-----------------------------------------------------------------------------
//! Graph for representing component connectivity through design hierarchies.
//-----------------------------------------------------------------------------
class ConnectivityGraph 
{
public:

	//! The constructor.
	ConnectivityGraph();

	//! The destructor.
	~ConnectivityGraph();

    /*!
     *  Adds an interface (=vertex) into the graph.
     *
     *      @param [in] vertex  The interface to add.
     */
    void addInterface(QSharedPointer<ConnectivityInterface> vertex);

    /*!
     *  Adds a connection (=edge) into the graph.
     *
     *      @param [in] connectionName      The name of the connection to add.
     *      @param [in] startPoint          The starting point for the edge.
     *      @param [in] endPoint            The end point for the edge.
     */
    void addConnection(QString const& connectionName, QSharedPointer<ConnectivityInterface> startPoint,
        QSharedPointer<ConnectivityInterface> endPoint);

    /*!
     *  Adds a component instance into the graph.
     *
     *      @param [in] instanceModel   The instance to add.
     */
    void addInstance(QSharedPointer<ConnectivityComponent> instanceModel);

    /*!
     *  Gets all interfaces (=vertices) in the graph.
     *
     *      @return All interfaces.
     */
    QVector<QSharedPointer<ConnectivityInterface> > getInterfaces() const;

    /*!
     *  Gets all the instances in the graph.
     *
     *      @return All instances.
     */
    QVector<QSharedPointer<ConnectivityComponent> > getInstances() const;
    
    /*!
     *  Finds all connections for the given interface.
     *
     *      @param [in] startPoint   The interface to find connections for.
     *
     *      @return All connections to/from the given interface.
     */
    QVector<QSharedPointer<ConnectivityConnection> > getConnectionsFor(QSharedPointer<ConnectivityInterface> vertex) const;

    /*!
     *  Finds the interface with the given name in the given instance.
     *
     *      @param [in] instanceName    The name of the instance containing the interface.
     *      @param [in] interfaceName   The name of the interface to find.
     *
     *      @return The found interface matching the given name and instance.
     */
    QSharedPointer<ConnectivityInterface> getInterface(QString const& instanceName, QString const& interfaceName) const;

private:

	// Disable copying.
	ConnectivityGraph(ConnectivityGraph const& rhs);
	ConnectivityGraph& operator=(ConnectivityGraph const& rhs);

    //! The vertices of the graph.
    QVector<QSharedPointer<ConnectivityInterface> > vertices_;

    //! The edges of the graph.
    QVector<QSharedPointer<ConnectivityConnection> > edges_;

    //! The component instances in the graph.
    QVector<QSharedPointer<ConnectivityComponent> > instances_;
};

#endif // CONNECTIVITYGRAPH_H
