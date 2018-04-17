//-----------------------------------------------------------------------------
// File: MasterSlavePathSearch.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 25.08.2016
//
// Description:
// Finds the paths from master interfaces to slave interfaces.
//-----------------------------------------------------------------------------

#ifndef MASTERSLAVEPATHSEARCH_H
#define MASTERSLAVEPATHSEARCH_H

#include <QSharedPointer>
#include <QVector>

class ConnectivityConnection;
class ConnectivityGraph;
class ConnectivityInterface;

//-----------------------------------------------------------------------------
//! Finds the paths from master interfaces to slave interfaces.
//-----------------------------------------------------------------------------
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
    QVector<QVector<QSharedPointer<ConnectivityInterface const> > > findMasterSlavePaths(
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
    QVector<QSharedPointer<ConnectivityInterface const> > findInitialMasterInterfaces(
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
    void findPaths(QSharedPointer<ConnectivityInterface const> startVertex,
        QSharedPointer<ConnectivityConnection const> previousEdge, 
        QVector<QSharedPointer<ConnectivityInterface const> > existingPath,
        QSharedPointer<const ConnectivityGraph> graph);

    /*!
     *  Find the interface connected to the selected interface.
     *
     *      @param [in] startInterface  The selected interface.
     *      @param [in] edge            The connection between the two interfaces.
     *
     *      @return The interface connected to the selected interface.
     */
    QSharedPointer<ConnectivityInterface const> findConnectedInterface(
        QSharedPointer<ConnectivityInterface const> startInterface,
        QSharedPointer<ConnectivityConnection const> edge) const;

    /*!
     *  Check if the selected interfaces can be connected.
     *
     *      @param [in] startVertex     The starting interface.
     *      @param [in] endVertex       The end interface.
     *
     *      @return True, if the interfaces can be connected, false otherwise.
     */
    bool canConnectInterfaces(QSharedPointer<ConnectivityInterface const> startVertex,
        QSharedPointer<ConnectivityInterface const> endVertex) const;

    /*!
     *  Remove paths that are contained within other paths.
     */
    void removeDuplicatePaths();

    /*!
     *  Check if the selected path should be kept in the master paths.
     *
     *      @param [in] currentPath     The selected path.
     *      @param [in] currentIndex    Index of the current path.
     *
     *      @return True, if the path is a master path, otherwise false.
     */
    bool pathIsFullPath(QVector<QSharedPointer<ConnectivityInterface const> > const& currentPath,
        int currentIndex);

    /*!
     *  Check if a path contains another path.
     *
     *      @param [in] pathOne     The first selected path.
     *      @param [in] pathTwo     The second selected path.
     *      @param [in] areaSize    Size of the smaller path.
     *
     *      @return True, if another of the selected paths contains the other, false otherwise.
     */
    bool pathContainsAnotherPath(QVector<QSharedPointer<ConnectivityInterface const> > const& pathOne,
        QVector<QSharedPointer<ConnectivityInterface const> > const& pathTwo, int areaSize) const;

    /*!
     *  Check if the path ends in a memory map.
     *
     *      @param [in] path    The selected path.
     *
     *      @return True, if the selected path ends in a memory map, false otherwise.
     */
    bool pathEndsInMemoryMap(QVector<QSharedPointer<ConnectivityInterface const> > const& path) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Connection paths from master interfaces.
    QVector<QVector<QSharedPointer<ConnectivityInterface const> > > masterPaths_;
};

#endif // MASTERSLAVEPATHSEARCH_H
