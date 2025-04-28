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

    using Path = QVector<QSharedPointer<ConnectivityInterface const> >;

	/*!
     *  The constructor.
     */
	MasterSlavePathSearch() = default;

    /*!
     *  Finds all paths from master interfaces to slave interfaces.
     *
     *    @param [in] graph                   The connectivity graph to find the paths from.
     *    @param [in] allowOverlappingPaths   Flag for allowing overlapping paths in the path search.
     *
     *    @return All paths from master interfaces.
     */
    QVector<QVector<QSharedPointer<ConnectivityInterface const> > > findMasterSlavePaths(
        QSharedPointer<const ConnectivityGraph> graph, bool allowOverlappingPaths) const;

private:

    /*!
     *  Finds the bottom-most master interfaces for the starting points of the paths.
     *
     *    @param [in] graph   The connectivity graph to find the interfaces from.
     *
     *    @return The master interfaces for path start points.
     */
    QVector<QSharedPointer<ConnectivityInterface> > findInitialMasterInterfaces(
        QSharedPointer<const ConnectivityGraph> graph) const;

    /*!
     *  Check if the selected interface is a starting interface.
     *	
     *    @param [in] vertex  The selected interface.
     *
     *    @return True, if the interface is a starting interface, false otherwise.
     */
    bool isStartInterface(QSharedPointer<const ConnectivityInterface> vertex) const;

    /*!
     *  Check if the selected interface is connected to a memory item.
     *
     *    @param [in] vertex  The selected interface.
     *
     *    @return True, if the interface is a starting interface, false otherwise.
     */
    bool isConnectedToMemory(QSharedPointer<const ConnectivityInterface> vertex) const;

    /*!
     *  Check if the selected interface is connected to a subspace map.
     *	
     *    @param [in] vertex  The selected interface.
     *
     *    @return True, if the interface is connected to a subspace map, false otherwise.
     */
    bool isConnectedToSubspaceMap(QSharedPointer<const ConnectivityInterface> vertex) const;

    /*!
     *  Finds all the paths branching from the given start vertex.
     *
     *    @param [in] startVertex     The vertex to start the search from.
     *    @param [in] graph           The connectivity graph to find the paths from.
     */
    QVector<MasterSlavePathSearch::Path> findPaths(QSharedPointer<ConnectivityInterface > startVertex,
        QSharedPointer<const ConnectivityGraph> graph) const;

    /*!
     *  Find paths from the selected interface.
     *
     *    @param [in] currentVertex     The selected interface.
     *    @param [in] visitedVertices   List of already visited interfaces.
     *    @param [in] graph             The connectivity graph to find the paths from.
     *
     *    @return List of paths found from the selected interface.
     */
    QVector<MasterSlavePathSearch::Path> findPathsFromInterface(QSharedPointer<ConnectivityInterface> currentVertex,
        QVector<QSharedPointer<ConnectivityInterface>> visitedVertices,
        QSharedPointer<const ConnectivityGraph> graph) const;

    /*!
     *  Check if the direction from start to end is accepted.
     *
     *    @param [in] startVertex       Starting interface.
     *    @param [in] connectedVertex   Ending interface.
     *
     *    @return True, if the direction is accepted, false otherwise.
     */
    bool pathDirectionIsAccepted(QSharedPointer<ConnectivityInterface> startVertex, QSharedPointer<ConnectivityInterface> connectedVertex) const;

    /*!
     *  Check if the selected interface is an ending point for the path.
     *
     *    @param [in] possibleLeaf  The selected interface.
     *    @param [in] graph         The connectivity graph to find the paths from.
     *
     *    @return True, if the interface is an end point, false otherwise.
     */
    bool vertexIsLeaf(QSharedPointer<ConnectivityInterface> possibleLeaf, QSharedPointer<const ConnectivityGraph> graph) const;

    /*!
     *  Find the interface connected to the selected interface.
     *
     *    @param [in] startInterface  The selected interface.
     *    @param [in] edge            The connection between the two interfaces.
     *
     *    @return The interface connected to the selected interface.
     */
    QSharedPointer<ConnectivityInterface> findConnectedInterface(
        QSharedPointer<ConnectivityInterface const> startInterface,
        QSharedPointer<ConnectivityConnection const> edge) const;

    /*!
     *  Check if the selected interfaces can be connected.
     *
     *    @param [in] startVertex     The starting interface.
     *    @param [in] endVertex       The end interface.
     *
     *    @return True, if the interfaces can be connected, false otherwise.
     */
    bool canConnectInterfaces(QSharedPointer<ConnectivityInterface const> startVertex,
        QSharedPointer<ConnectivityInterface const> endVertex) const;

    /*!
     *  Finds the valid, longest paths in the given set of paths.
     *
     *    @param [in] paths                   The paths to search.
     *    @param [in] allowOverlappingPaths   Flag for allowing overlapping paths in the path search.
     *
     *    @return The valid paths found in the set.
     */
    QVector<MasterSlavePathSearch::Path> findValidPathsIn(QVector<MasterSlavePathSearch::Path> const& paths, bool allowOverlappingPaths) const;

    /*!
     *  Check if the selected path should be kept in the master paths.
     *
     *    @param [in] currentPath     The selected path.
     *    @param [in] availablePaths  All the available paths.
     *
     *    @return True, if the path is a master path, otherwise false.
     */
    bool pathIsFullPath(MasterSlavePathSearch::Path const& currentPath,
        QVector<MasterSlavePathSearch::Path> const& availablePaths) const;

    /*!
     *  Check if a path contains another path.
     *
     *    @param [in] pathOne     The first selected path.
     *    @param [in] pathTwo     The second selected path.
     *    @param [in] areaSize    Size of the smaller path.
     *
     *    @return True, if another of the selected paths contains the other, false otherwise.
     */
    bool pathContainsAnotherPath(QVector<QSharedPointer<ConnectivityInterface const> > const& pathOne,
        QVector<QSharedPointer<ConnectivityInterface const> > const& pathTwo, int areaSize) const;

    /*!
     *  Check if the path ends in a memory map.
     *
     *    @param [in] path    The selected path.
     *
     *    @return True, if the selected path ends in a memory map, false otherwise.
     */
    bool pathEndsInMemoryMap(QVector<QSharedPointer<ConnectivityInterface const> > const& path) const;

};

#endif // MASTERSLAVEPATHSEARCH_H
