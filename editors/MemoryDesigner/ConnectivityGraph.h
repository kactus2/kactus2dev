//-----------------------------------------------------------------------------
// File: ConnectivityGraph.h
//-----------------------------------------------------------------------------
// Project: Kactus2
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
     *  Gets all interfaces (=vertices) in the graph.
     *
     *    @return All interfaces.
     */
    QVector<QSharedPointer<ConnectivityInterface> >& getInterfaces() const;

    /*!
     *  Gets all the instances in the graph.
     *
     *    @return All instances.
     */
    QVector<QSharedPointer<ConnectivityComponent> >& getInstances() const;
    
    /*!
     *  Finds all connections for the given interface.
     *
     *    @param [in] startPoint   The interface to find connections for.
     *
     *    @return All connections to/from the given interface.
     */
    QVector<QSharedPointer<ConnectivityConnection const> > getConnectionsFor(
        QSharedPointer<ConnectivityInterface const> startPoint) const;

    /*!
     *  Finds the interface with the given name in the given instance.
     *
     *    @param [in] instanceName    The name of the instance containing the interface.
     *    @param [in] interfaceName   The name of the interface to find.
     *
     *    @return The found interface matching the given name and instance.
     */
    QSharedPointer<ConnectivityInterface const> getInterface(QString const& interfaceName,
        QString const& instanceName);

    /*!
     *  Get all connections in the graph.
     *
     *    @return All connections in the graph.
     */
    QVector<QSharedPointer<ConnectivityConnection> >& getConnections() const;

private:

	// Disable copying.
	ConnectivityGraph(ConnectivityGraph const& rhs);
	ConnectivityGraph& operator=(ConnectivityGraph const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The vertices of the graph.
    mutable QVector<QSharedPointer<ConnectivityInterface> > vertices_;

    //! The edges of the graph.
    mutable QVector<QSharedPointer<ConnectivityConnection> > edges_;

    //! The component instances in the graph.
    mutable QVector<QSharedPointer<ConnectivityComponent> > instances_;
};

#endif // CONNECTIVITYGRAPH_H
