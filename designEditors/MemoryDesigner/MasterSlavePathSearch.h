//-----------------------------------------------------------------------------
// File: MasterSlavePathSearch.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.08.2016
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#ifndef MASTERSLAVEPATHSEARCH_H
#define MASTERSLAVEPATHSEARCH_H

#include <QSharedPointer>
#include <QVector>

class ConnectivityConnection;
class ConnectivityGraph;
class ConnectivityInterface;

class MasterSlavePathSearch 
{
public:

	//! The constructor.
	MasterSlavePathSearch();

	//! The destructor.
	~MasterSlavePathSearch();

    /*!
     *  Finds all paths from master interfaces to slave interfaces.
     *
     *      @param [in] graph   The connectivity graph to find the paths from.
     *
     *      @return All paths from master interfaces.
     */
    QVector<QVector<QSharedPointer<ConnectivityInterface> > > findMasterSlavePaths(
        QSharedPointer<const ConnectivityGraph> graph);

private:

	// Disable copying.
	MasterSlavePathSearch(MasterSlavePathSearch const& rhs);
	MasterSlavePathSearch& operator=(MasterSlavePathSearch const& rhs);

    /*!
     *  Finds the bottom-most master interfaces for the starting points of the paths.
     *
     *      @param [in] graph   The connectivity graph to find the interfaces from.
     *
     *      @return The master interfaces for path start points.
     */
    QVector<QSharedPointer<ConnectivityInterface> > findInitialMasterInterfaces(
        QSharedPointer<const ConnectivityGraph> graph) const;

    /*!
     *  Finds all the paths branching from the given start vertex.
     *
     *      @param [in] startVertex     The vertex to start the search from.
     *      @param [in] previousEdge    The edge traveled to the starting vertex and to exclude from the search.
     *      @param [in] existingPath    The traveled path to the start vertex.
     *      @param [in] graph           The connectivity graph to find the paths from.
     *
     *      @return <Description>.
     */
    void findPaths(QSharedPointer<ConnectivityInterface> startVertex, 
        QSharedPointer<ConnectivityConnection> previousEdge, 
        QVector<QSharedPointer<ConnectivityInterface> > existingPath, 
        QSharedPointer<const ConnectivityGraph> graph);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Connection paths from master interfaces.
    QVector<QVector<QSharedPointer<ConnectivityInterface> > > masterPaths_;
};

#endif // MASTERSLAVEPATHSEARCH_H
