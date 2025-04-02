//-----------------------------------------------------------------------------
// File: MemoryConnectionHandler.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 03.01.2017
//
// Description:
// Constructs the memory connections for the memory design diagram.
//-----------------------------------------------------------------------------

#ifndef MEMORYCONNECTIONHANDLER_H
#define MEMORYCONNECTIONHANDLER_H

#include <QSharedPointer>
#include <QVector>
#include <QQueue>
#include <QGraphicsItem>

#include <editors/MemoryDesigner/MemoryConnectionAddressCalculator.h>

class ConnectivityGraph;
class ConnectivityInterface;
class MemoryColumn;
class MainMemoryGraphicsItem;
class MemoryConnectionItem;
class MemoryCollisionItem;
class MemoryColumnHandler;
class MemoryItem;
class MemoryGraphicsItemHandler;

//-----------------------------------------------------------------------------
//! Constructs the memory connections for the memory design diagram.
//-----------------------------------------------------------------------------
class MemoryConnectionHandler
{

public:

    //! Used for simplifying the interface path.
    using Path = QVector<QSharedPointer<ConnectivityInterface const> >;

    /*!
     *  The constructor.
     *
     *    @param [in] columnHandler     Handler for memory columns.
     *    @param [in] itemHandler       Handler for graphics items.
     */
    MemoryConnectionHandler(QSharedPointer<MemoryColumnHandler> columnHandler, QSharedPointer<MemoryGraphicsItemHandler> itemHandler);

    /*!
     *  The destructor.
     */
    ~MemoryConnectionHandler() = default;

    /*!
     *  Change the condensing of memory graphics items.
     *
     *    @param [in] condenseMemoryItems     Value for condensing memory graphics items.
     */
    void setCondenseMemoryItems(bool condenseMemoryItems);

    /*!
     *  Check whether the memory graphics items are condensed or not.
     *
     *    @return True, if the memory graphics items are condensed, otherwise false.
     */
    bool memoryItemsAreCondensed() const;

    /*!
     *  Set the filtering for chained address space memory connections.
     *
     *    @param [in] filterChains    Value for the chained address space memory connection filtering.
     */
    void setFilterAddressSpaceChains(bool filterChains);

    /*!
     *  Check whether the chained address space memory connections are filtered or not.
     *
     *    @return True, if the chained address space memory connections are filtered, false otherwise.
     */
    bool addressSpaceChainsAreFiltered() const;

    /*!
     *  Clear the memory connection and memory connection collision lists.
     */
    void clearConnectionLists();

    /*!
     *  Redraw the memory connections and memory collisions.
     */
    void reDrawConnectionsAndCollisions();

    /*!
     *  Create the connections and place the connected items.
     *	
     *    @param [in] connectionGraph     Graph containing the memory connection paths.
     *    @param [in] spaceColumn         Column containing the address space graphics items.
     *    @param [in] memoryMapColumn     Column containing the memory map graphics items.
     */
    void createConnectedItems(QSharedPointer<ConnectivityGraph> connectionGraph,
        MemoryColumn* spaceColumn,
        MemoryColumn* memoryMapColumn);

private:
    // Disable copying.
    MemoryConnectionHandler(MemoryConnectionHandler const& rhs);
    MemoryConnectionHandler& operator=(MemoryConnectionHandler const& rhs);

    //! Collection of memory items connected to each other.
    struct ConnectedItemSet 
    {
        //! List of address space items connected to this set.
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > connectedSpaces_;

        //! List of memory map items connected to this set.
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > connectedMaps_;
    };

    /*!
     *  Combine paths containing the same memory items.
     *
     *    @param [in] masterSlavePaths    Interface paths from master to slave.
     *
     *    @return A list of combined path sets.
     */
    QVector<QSharedPointer<QVector<Path> > > findPathSets(QVector<Path> const& masterSlavePaths) const;

    /*!
     *  Find the indexes of the path sets that the selected path belongs to.
     *
     *    @param [in] currentPath     The selected path.
     *    @param [in] pathSets        List of the path sets.
     *
     *    @return List of path set indexes.
     */
    QQueue<int> findPathSetIndexes(Path const& currentPath,
        QVector<QSharedPointer<QVector<Path> > > const& pathSets) const;

    /*!
     *  Check if an interface of the selected path is contained with the selected path set.
     *	
     *    @param [in] currentPath     The selected path.
     *    @param [in] pathSet         The selected comparison path set.
     *
     *    @return True, if an interface is contained within the path set, false otherwise.
     */
    bool pathIsContainedWithPathSet(Path const& currentPath, QSharedPointer<QVector<Path> > pathSet) const;

    /*!
     *  Check if the selected interface is contained within the selected path.
     *	
     *    @param [in] pathInterface   The selected interface.
     *    @param [in] comparisonPath  The selected comparison path.
     *
     *    @return True, if the interface is contained within the path, false otherwise.
     */
    bool interfacedItemIsWithinPath(QSharedPointer<const ConnectivityInterface> pathInterface,
        Path const& comparisonPath) const;

    /*!
     *  Create a combined connection set from the selected path set.
     *
     *    @param [in] connectionSet       The selected path set.
     *    @param [in] placedMapItems      List of already placed memory map items.
     *    @param [in] memoryMapColumn     The memory map column.
     *    @param [in] spaceYPlacement     Placement of the top item in the connection set.
     *    @param [in] placedSpaceItems    List of already placed address space items.
     *
     *    @return List of the created memory connections.
     */
    QVector<MemoryConnectionItem*> createConnectionSet(QSharedPointer<QVector<Path> > connectionSet,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems,
        MemoryColumn* memoryMapColumn,
        qreal& spaceYPlacement,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems);

    /*!
     *  Create a connection displaying only the start and end items.
     *
     *    @param [in] connectionPath      The connection path.
     *    @param [in] placedMapItems      List of the already placed memory maps.
     *    @param [in] memoryMapColumn     The memory map column.
     *    @param [in] placedSpaceItems    List of the already placed address spaces.
     */
    void createOnlyEndsConnection(Path const& connectionPath,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems,
        MemoryColumn* memoryMapColumn,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems);

    /*!
     *  Check if the selected memory connection already exists.
     *
     *    @param [in] startItem                 Start item of the connection.
     *    @param [in] endItem                   End item of the connection.
     *    @param [in] connectionBaseAddress     Base address of the connection.
     *    @param [in] connectionEndAddress      Last address of the connection.
     *
     *    @return True, if the connection exists, false otherwise.
     */
    bool connectionExists(MainMemoryGraphicsItem* startItem,
        MainMemoryGraphicsItem* endItem,
        quint64 const& connectionBaseAddress,
        quint64 const& connectionEndAddress) const;

    /*!
     *  Check if the connection start item should be moved, instead of the end item.
     *
     *    @param [in] endItem                   The connection end item.
     *    @param [in] connectionBaseAddress     Base address of the connection.
     *
     *    @return True, if the start item should be moved, false otherwise.
     */
    bool moveStartItemNotEndItem(MainMemoryGraphicsItem* endItem,
        quint64 const& connectionBaseAddress) const;

    /*!
     *  Create a set for each separate memory item group.
     *
     *    @param [in] placedSpaceItems  List of the available address spaces, used as starting points for checking.
     *
     *    @return List of grouped memory items.
     */
    QVector<QSharedPointer<ConnectedItemSet> > separateBrokenPathSet(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems) const;

    /*!
     *  Get connected items for the selected address space item.
     *
     *    @param [in] spaceItem     The selected address space item.
     *    @param [in] visitedItems  List of already visited items.
     *
     *    @return A group of memory items connected to each other.
     */
    QSharedPointer<ConnectedItemSet> getConnectedItemsForSpaceItem(MainMemoryGraphicsItem* spaceItem, QSharedPointer<QVector<MainMemoryGraphicsItem*> > visitedItems) const;

    /*!
     *  Get connected items for the selected memory item.
     *
     *    @param [in] memoryItem            The selected memory item.
     *    @param [in] itemsConnectedToItem  List of items connected to this item.
     *    @param [in] visitedItems          List of already visited items.
     */
    void getConnectedItems(MainMemoryGraphicsItem* memoryItem, QVector<MainMemoryGraphicsItem*>& itemsConnectedToItem, QSharedPointer<QVector<MainMemoryGraphicsItem*> > visitedItems) const;

    /*!
     *  Create a connection displaying the full connection.
     *
     *    @param [in] connectionPath      The connection path.
     *    @param [in] placedMapItems      List of the already placed memory maps.
     *    @param [in] memoryMapColumn     The memory map column.
     *    @param [in] placedSpaceItems    List of the already placed address spaces.
     */
    void createFullConnection(Path const& connectionPath,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems,
        MemoryColumn* memoryMapColumn,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems);

    /*!
     *  Create connection to a local memory map.
     *
     *    @param [in] localSpace            The space whose local memory map is being connected to.
     *    @param [in] localMapInterface     Interface of the local memory map.
     *    @param [in] placedMapItems        List of placed memory map items.
     *    @param [in] mapColumn             The column for memory maps.
     *
     *    @return 
     */
    void createFullConnectionToLocalMap(MainMemoryGraphicsItem* localSpace,
        QSharedPointer<const ConnectivityInterface> localMapInterface,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems,
        MemoryColumn* mapColumn);

    /*!
     *  Create a connection to an address space item.
     *
     *    @param [in] spaceItem                 The connected address space item.
     *    @param [in] previousItem              The item that is being connected to the address space item.
     *    @param [in] calculatedBaseAddress     Base address of the connection.
     *    @param [in] calculatedEndAddress      End address of the connection.
     *    @param [in] yTransfer                 Value for moving the connection relative to the previous item.
     *    @param [in] placedSpaceItems          List of placed address space items.
     */
    void createFullConnectionToSpace(MainMemoryGraphicsItem* spaceItem,
        MainMemoryGraphicsItem* previousItem,
        quint64 const& calculatedBaseAddress,
        quint64 const& calculatedEndAddress,
        qreal const& yTransfer,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems);

    /*!
     *  Create a connection to a memory map item.
     *
     *    @param [in] mapItem                   The connected memory map item.
     *    @param [in] previousItem              The item that is being connected to the memory map item.
     *    @param [in] calculatedBaseAddress     Calculated base address of the memory item.
     *    @param [in] connectionBaseAddress     Base address of the connection.
     *    @param [in] connectionEndAddress      End address of the connection.
     *    @param [in] connectionTransfer        Value for moving the connection relative to the previous item.
     *    @param [in] mapColumn                 Column for memory maps.
     *    @param [in] placedMapItems            List of placed memory map items.
     */
    void createFullConnectionToMemoryMap(MainMemoryGraphicsItem* mapItem,
        MainMemoryGraphicsItem* previousItem,
        quint64 const& calculatedBaseAddress,
        quint64 const& connectionBaseAddress,
        quint64 const& connectionEndAddress,
        qreal const& connectionTransfer,
        MemoryColumn* mapColumn,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems);

    /*!
     *  Get the memory graphics item corresponding to an interface.
     *
     *    @param [in] pathInterface     The selected interface.
     *
     *    @return Memory graphics item of the selected interface.
     */
    MainMemoryGraphicsItem* getPathGraphicsItem(QSharedPointer<const ConnectivityInterface> pathInterface) const;

    /*!
     *  Get the main graphics item corresponding to the selected connection interface.
     *
     *    @param [in] connectionInterface     The selected connection interface.
     *    @param [in] columnType              The column type of the main graphics item.
     *
     *    @return The found main graphics item.
     */
    MainMemoryGraphicsItem* getMainGraphicsItem(QSharedPointer<ConnectivityInterface const> connectionInterface,
        QString const& columnType) const;

    /*!
     *  Get the first initiator interface connected to an address space for the selected connection path.
     *
     *    @param [in] connectionPath    The selected connection path.
     *
     *    @return The first initiator interface for the selected connection path.
     */
    QSharedPointer<ConnectivityInterface const> getFirstInitiatorInterface(QVector<QSharedPointer<ConnectivityInterface const> > connectionPath) const;

    /*!
     *  Get the end item for the memory connection.
     *
     *    @param [in] startInterface  Start interface of the memory connection path.
     *    @param [in] endInterface    End interface of the memory connection path.
     *
     *    @return The end item of the memory connection.
     */
    MainMemoryGraphicsItem* getConnectionEndItem(QSharedPointer<ConnectivityInterface const> startInterface,
        QSharedPointer<ConnectivityInterface const> endInterface) const;

    /*!
     *  Get the local memory map graphics item of the selected address space interface.
     *
     *    @param [in] spaceInterface  Interface node referencing the selected address space item.
     *
     *    @return The local memory map graphics item of contained within the selected interface node.
     */
    MainMemoryGraphicsItem* getLocalMemoryMapItem(QSharedPointer<ConnectivityInterface const> spaceInterface) const;

    /*!
     *  Place the memory map item.
     *	
     *    @param [in] mapItem               The connected memory map item.
     *    @param [in] startItem             The connection starting item.
     *    @param [in] yTransfer             The initial position of the connection from the starting memory item.
     *    @param [in] placedMapItems        List of the placed memory map items.
     *    @param [in] remappedBaseAddress   Calculated base address of the memory map.
     *    @param [in] remappedEndAddress    Calculated end address of the memory map.
     *    @param [in] mapColumn             The memory map column.
     */
    void placeMemoryMap(MainMemoryGraphicsItem* mapItem,
        MainMemoryGraphicsItem* startItem,
        qreal const& yTransfer,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems,
        quint64 const& remappedBaseAddress,
        quint64 const& remappedEndAddress,
        MemoryColumn* mapColumn);

    /*!
     *  Check and reposition a memory map to a memory map overlap column if needed.
     *
     *    @param [in] connectionBaseAddress   Lowest base address connected to the selected memory item.
     *    @param [in] connectionLastAddress   Highest last address connected to the selected memory item.
     *    @param [in] placedMaps              A list of placed memory map items.
     *    @param [in] memoryItem              The selected memory map item.
     *    @param [in] originalColumn          The original column of the memory map item.
     *    @param [in] connectionStartItem     The start item of the memory map connection.
     */
    void checkMemoryMapRepositionToOverlapColumn(quint64 connectionBaseAddress,
        quint64 connectionLastAddress,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMaps,
        MainMemoryGraphicsItem* memoryItem,
        MemoryColumn* originalColumn,
        MainMemoryGraphicsItem* connectionStartItem);

    /*!
     *  Place the address space item.
     *	
     *    @param [in] spaceItem               The selected address space item.
     *    @param [in] placedSpaceItems        List of the placed address space items.
     *    @param [in] originalColumn          Original column of the address space items.
     *    @param [in] targetItem              End item of the connection.
     *    @param [in] connectionBaseAddress   Base address of the connection.
     */
    void positionSpaceItem(MainMemoryGraphicsItem* spaceItem, qreal ypos);

    /*!
     *  Place the address space item to a column.
     *	
     *    @param [in] spaceItem           The selected address space item.
     *    @param [in] spaceRectangle      Scene rectangle of the new position for the address space item.
     *    @param [in] spaceLineWidth      Line width of the address space item.
     *    @param [in] placedSpaceItems    List of the placed address space items.
     */
    void placeSpaceItemToColumn(MainMemoryGraphicsItem* spaceItem,
        QRectF const& spaceRectangle,
        int spaceLineWidth,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems) const;

    /*!
     *  Create the connection item.
     *	
     *    @param [in] startItem               The starting item of the connection.
     *    @param [in] endItem                 The end item of the connection.
     *    @param [in] remappedAddress         Base address for the connection.
     *    @param [in] remappedEndAddress      End address for the connection.
     *    @param [in] yTransfer               Position of the connection relative to the starting item.
     */
    void createConnectionItem(MainMemoryGraphicsItem* startItem,
        MainMemoryGraphicsItem* endItem,
        quint64 const& remappedAddress,
        quint64 const& remappedEndAddress,
        qreal const& yTransfer);

    /*!
     *  Change the address range of the selected memory graphics item.
     *
     *    @param [in] connectionEndItem       The selected memory graphics item.
     *    @param [in] remappedAddress         Remapped address of the memory connection.
     */
    void changeConnectionEndItemRanges(MainMemoryGraphicsItem* connectionEndItem,
        quint64 const& remappedAddress) const;

    /*!
     *  Get the memory item in the highest position of the set.
     *	
     *    @param [in] currentHighestItem  The currently highest memory item.
     *    @param [in] itemSet             The selected set.
     *
     *    @return The highest memory item.
     */
    MainMemoryGraphicsItem* getHighestPlacedItemInSet(MainMemoryGraphicsItem* currentHighestItem,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > itemSet) const;

    /*!
     *  Compress and reposition the connection set.
     *	
     *    @param [in] placedSpaceItems    List of the address space items of the connection set.
     *    @param [in] placedMapItems      List of the memory map items of the connection set.
     *
     *    @return The memory connections of the set.
     */
    QVector<MemoryConnectionItem*> compressConnectedMemoryItems(
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems) const;

    /*!
     *  Calculate the area movements of the compressed set.
     *	
     *    @param [in] uncutCoordinates    Coordinates that must be visible in the compressed set.
     *    @param [in] CUTMODIFIER         Maximum height of the compressed set.
     *
     *    @return Map with Y-coordinate as the key and total movement up to this coordinate as the value.
     */
    QMap<qreal, qreal> calculateAreaMovements(QVector<qreal> uncutCoordinates, qreal const& CUTMODIFIER) const;

    /*!
     *  Check if an address space item should be repositioned to another column.
     *	
     *    @param [in] placedSpaceItems    List of the placed address space items.
     */
    void repositionCompressedAddressSpaces(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems);

    /*!
     *  Check if the selected memory item collides with other placed memory items.
     *	
     *    @param [in] memoryItem      The selected memory item.
     *    @param [in] placedItems     List of the placed memory items.
     *
     *    @return True, if the item collides, false otherwise.
     */
    bool itemCollidesWithOtherPlacedItems(MainMemoryGraphicsItem const* memoryItem,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedItems) const;

    /*!
     *  Place the space item to a column.
     *	
     *    @param [in] spaceItem           The space item.
     *    @param [in] placedSpaceItems    List of the placed space items.
     */
    void placeCompressedSpaceToColumn(MainMemoryGraphicsItem* spaceItem,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems);

    /*!
     *  Reposition compressed memory maps to previous columns.
     *
     *    @param [in] placedMapItems      A list of the placed memory map items.
     *    @param [in] memoryMapColumn     The main memory map column.
     */
    void repositionCompressedMemoryMaps(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems,
        MemoryColumn const* memoryMapColumn);

    /*!
     *  Redraw the memory connections.
     *	
     *    @param [in] connectionItems     List of the memory connection items.
     *
     *    @return 
     */
    void redrawMemoryConnections(QVector<MemoryConnectionItem*> connectionItems) const;

    /*!
     *  Create markers for the overlapping connections.
     *
     *    @param [in] placedSpaceItems    List of the placed address space items.
     */
    void createOverlappingConnectionMarkers(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Value for displaying condensed memory connections.
    bool condenseMemoryItems_ = true;

    //! Value for filtering the chained address space memory connections.
    bool filterAddressSpaceChains_ = true;

    //! A list of memory connections made to memory maps.
    QVector<MemoryConnectionItem*> connectionsToMemoryMaps_;

    //! A list of memory collisions.
    QVector<MemoryCollisionItem*> memoryCollisions_;

    //! Handler for memory columns.
    QSharedPointer<MemoryColumnHandler> columnHandler_;

    //! Handler for memory items.
    QSharedPointer<MemoryGraphicsItemHandler> itemHandler_;
};

//-----------------------------------------------------------------------------

#endif // MEMORYCONNECTIONHANDLER_H
