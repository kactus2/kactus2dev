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
     *      @param [in] columnHandler   Handler for memory columns.
     */
    MemoryConnectionHandler(QSharedPointer<MemoryColumnHandler> columnHandler);

    /*!
     *  The destructor.
     */
    ~MemoryConnectionHandler();

    /*!
     *  Change the condensing of memory graphics items.
     *
     *      @param [in] condenseMemoryItems     Value for condensing memory graphics items.
     */
    void setCondenseMemoryItems(bool condenseMemoryItems);

    /*!
     *  Check whether the memory graphics items are condensed or not.
     *
     *      @return True, if the memory graphics items are condensed, otherwise false.
     */
    bool memoryItemsAreCondensed() const;

    /*!
     *  Set the filtering for chained address space memory connections.
     *
     *      @param [in] filterChains    Value for the chained address space memory connection filtering.
     */
    void setFilterAddressSpaceChains(bool filterChains);

    /*!
     *  Check whether the chained address space memory connections are filtered or not.
     *
     *      @return True, if the chained address space memory connections are filtered, false otherwise.
     */
    bool addressSpaceChainsAreFiltered() const;

    /*!
     *  Clear the memory connection and memory connection collision lists.
     */
    void clearConnectionLists();

    /*!
     *  Create the memory connections.
     *
     *      @param [in] connectionGraph     Graph containing the memory connection paths.
     *      @param [in] spaceColumn         Column containing the address space graphics items.
     *      @param [in] memoryMapColumn     Column containing the memory map graphics items.
     */
    void createMemoryConnections(QSharedPointer<ConnectivityGraph> connectionGraph, MemoryColumn* spaceColumn,
        MemoryColumn* memoryMapColumn);

    /*!
     *  Redraw the memory connections and memory collisions.
     */
    void reDrawConnectionsAndCollisions();

    /*!
     *  Create the connections and place the connected items.
     *	
     *      @param [in] connectionGraph     Graph containing the memory connection paths.
     *      @param [in] spaceColumn         Column containing the address space graphics items.
     *      @param [in] memoryMapColumn     Column containing the memory map graphics items.
     */
    void createConnectedItems(QSharedPointer<ConnectivityGraph> connectionGraph, MemoryColumn* spaceColumn, MemoryColumn* memoryMapColumn);

private:
    // Disable copying.
    MemoryConnectionHandler(MemoryConnectionHandler const& rhs);
    MemoryConnectionHandler& operator=(MemoryConnectionHandler const& rhs);

    /*!
     *  Create a memory connection between an address space and a memory map.
     *
     *      @param [in] connectionPath          The path of the memory connection.
     *      @param [in] placedMapItems          A list of placed memory map items.
     *      @param [in] memoryMapColumn         The column containing the memory maps.
     *      @param [in] spaceYPlacement         The Y placement of the next address space item.
     *      @param [in] placedSpaceItems        A list of placed address space items.
     *      @param [in] spaceColumn             The column containing the address spaces.
     */
    void createConnection(QVector<QSharedPointer<ConnectivityInterface const> > const& connectionPath,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems, MemoryColumn* memoryMapColumn,
        qreal& spaceYPlacement, QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
        MemoryColumn* spaceColumn);

    /*!
     *  Get the starting interface that references an address space for the connection path.
     *
     *      @param [in] connectionPath  The selected connection path.
     *
     *      @return The starting interface for the connection.
     */
    QSharedPointer<ConnectivityInterface const> getStartInterface(
        QVector<QSharedPointer<ConnectivityInterface const> > connectionPath);

    /*!
     *  Get the end item for the memory connection.
     *
     *      @param [in] startInterface  Start interface of the memory connection path.
     *      @param [in] endInterface    End interface of the memory connection path.
     *
     *      @return The end item of the memory connection.
     */
    MainMemoryGraphicsItem* getConnectionEndItem(QSharedPointer<ConnectivityInterface const> startInterface,
        QSharedPointer<ConnectivityInterface const> endInterface) const;

    /*!
     *  Get the starting base address of the memory connection.
     *
     *      @param [in] startInterface  Start interface of the memory connection path.
     *      @param [in] endInterface    End interface of the memory connection path.
     *
     *      @return Starting base address of the memory connection.
     */
    quint64 getStartingBaseAddress(QSharedPointer<ConnectivityInterface const> startInterface,
        QSharedPointer<ConnectivityInterface const> endInterface) const;

    /*!
     *  Get the initial transfer in y-coordinate for the memory connection.
     *
     *      @param [in] baseAddressNumber                   Base address of the memory connection.
     *      @param [in] mirroredSlaveAddressChange          Address change caused by the mirrored slave interfaces.
     *      @param [in] hasRemapRange                       Flag for remapped connection range.
     *      @param [in] memoryMapBaseAddress                Base address of the connected memory map item.
     *      @param [in] spaceChainConnectionBaseAddress     Base address modified caused by the bridged interfaces.
     *
     *      @return The initial transfer value for the memory connection item.
     */
    qreal getConnectionInitialTransferY(quint64 baseAddressNumber, quint64 mirroredSlaveAddressChange,
        bool hasRemapRange, quint64 memoryMapBaseAddress, quint64 spaceChainConnectionBaseAddress) const;

    /*!
     *  Place the memory map item.
     *
     *      @param [in] connectionBaseAddress       Base address of the current memory connection.
     *      @param [in] connectionLastAddress       Last address of the current memory connection.
     *      @param [in] connectionEndItem           End item of the memory connection.
     *      @param [in] connectionStartItem         Start item of the memory connection.
     *      @param [in] yTransfer                   Memory connection transfer in the y-coordinate.
     *      @param [in] hasRemapRange               Flag for remapped connection range.
     *      @param [in] memoryMapBaseAddress        Base address of the connected memory map.
     *      @param [in] spaceItemPlaced             Flag for placed address space item.
     *      @param [in] isChainedSpaceConnection    Flag for bridged bus interfaces on the path.
     *      @param [in] placedMapItems              List of all the placed memory map items.
     *      @param [in] placedSpaceItems            List of all the placed address space items.
     *      @param [in] memoryMapColumn             Column containing the memory map items.
     *      @param [in] spaceColumn                 Column containing the address space items.
     *      @param [in] spaceYPlacement             Y-coordinate for the address space item.
     */
    void placeMemoryMapItem(quint64 connectionBaseAddress, quint64 connectionLastAddress,
        MainMemoryGraphicsItem* connectionEndItem, MainMemoryGraphicsItem* connectionStartItem, qreal yTransfer,
        bool hasRemapRange, quint64 memoryMapBaseAddress, bool spaceItemPlaced, bool isChainedSpaceConnection,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems, MemoryColumn* memoryMapColumn,
        MemoryColumn* spaceColumn, qreal& spaceYPlacement);

    /*!
     *  Create the memory connection item.
     *
     *      @param [in] connectionPath          The path of the memory connection.
     *      @param [in] connectionStartItem     Start item of the memory connection.
     *      @param [in] connectionEndItem       End item of the memory connection.
     *      @param [in] remappedAddress         Remapped base address of the memory connection.
     *      @param [in] remappedEndAddress      Remapped last address of the memory connection.
     *      @param [in] memoryMapBaseAddress    Base address of the memory map item.
     *      @param [in] hasRemapRange           Flag for remapped connection range.
     *      @param [in] yTransfer               Memory connection transfer in y-coordinate.
     *      @param [in] spaceYPlacement         Y-coordinate for the address space item.
     */
    void createMemoryConnectionItem(QVector<QSharedPointer<ConnectivityInterface const> > connectionPath,
        MainMemoryGraphicsItem* connectionStartItem, MainMemoryGraphicsItem* connectionEndItem,
        quint64 remappedAddress, quint64 remappedEndAddress, quint64 memoryMapBaseAddress, bool hasRemapRange,
        qreal yTransfer, qreal& spaceYPlacement);

    /*!
     *  Check and reposition a memory map to a memory map overlap column if needed.
     *
     *      @param [in] connectionBaseAddress   Lowest base address connected to the selected memory item.
     *      @param [in] connectionLastAddress   Highest last address connected to the selected memory item.
     *      @param [in] placedMaps              A list of placed memory map items.
     *      @param [in] memoryItem              The selected memory map item.
     *      @param [in] originalColumn          The original column of the memory map item.
     *      @param [in] connectionStartItem     The start item of the memory map connection.
     */
    void checkMemoryMapRepositionToOverlapColumn(quint64 connectionBaseAddress, quint64 connectionLastAddress,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMaps, MainMemoryGraphicsItem* memoryItem,
        MemoryColumn* originalColumn, MainMemoryGraphicsItem* connectionStartItem);

    /*!
     *  Reposition the selected memory map item.
     *
     *      @param [in] placedMapItems          List of the placed memory map items.
     *      @param [in] placedSpaceItems        List of the placed address space items.
     *      @param [in] startConnectionItem     The connection start item.
     *      @param [in] addressSpaceColumn      Column containing the address spaces.
     *      @param [in] endConnectionItem       The connection end item.
     *      @param [in] memoryMapYTransfer      Y transfer of the memory map.
     */
    void repositionMemoryMap(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
        MainMemoryGraphicsItem* startConnectionItem, MemoryColumn* addressSpaceColumn,
        MainMemoryGraphicsItem* endConnectionItem, int memoryMapYTransfer);

    /*!
     *  Reposition an address space item to match a selected memory map.
     *
     *      @param [in] placedSpaceItems    List of the placed address space items.
     *      @param [in] startItem           The connection start item.
     *      @param [in] endItem             The connection end item.
     *      @param [in] spaceColumn         Column containing the address spaces.
     *      @param [in] memoryMapYTransfer  Y transfer of the memory map.
     */
    void repositionSpaceItemToMemoryMap(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
        MainMemoryGraphicsItem* startItem, MainMemoryGraphicsItem* endItem, MemoryColumn* spaceColumn,
        int memoryMapYTransfer);

    /*!
     *  Redraw the memory connections.
     *
     *      @param [in] placedSpaceItems    List of the space items that have been placed in the diagram.
     */
    void reDrawConnections(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems);

    /*!
     *  Place the address space item to another address space column.
     *
     *      @param [in] spaceItem               The selected address space item.
     *      @param [in] spaceChain              Connected address space items.
     *      @param [in] originalColumn          The original column of the selected address space item.
     *      @param [in] targetItem              The other item of the memory connection.
     *      @param [in] connectionBaseAddress   Base address of the current memory connection item.
     */
    void placeSpaceItemToOtherColumn(MainMemoryGraphicsItem* spaceItem,
        QVector<MainMemoryGraphicsItem*> spaceChain, MemoryColumn* originalColumn,
        MainMemoryGraphicsItem* targetItem, quint64 connectionBaseAddress);

    /*!
     *  Move the unconnected memory items to the bottom of the selected column.
     *
     *      @param [in] memoryItems     List of the placed memory items.
     *      @param [in] memoryColumn    Target column for the unconnected memory items.
     */
    void moveUnconnectedMemoryItems(QSharedPointer<QVector<MainMemoryGraphicsItem*> > memoryItems, MemoryColumn* memoryColumn);

    /*!
     *  Create markers for the overlapping connections.
     *
     *      @param [in] placedSpaceItems    List of the placed address space items.
     */
    void createOverlappingConnectionMarkers(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems);

    /*!
     *  Compress the graphics items.
     *
     *      @param [in] placedSpaceItems    A list of the placed address space items.
     *      @param [in] spaceYPlacement     The last y-coordinate of the address space items.
     *      @param [in] spaceColumn         Column containing address space items.
     */
    void compressGraphicsItems(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
        qreal& spaceYPlacement, MemoryColumn* spaceColumn);

    /*!
     *  Reposition compressed memory maps to previous columns.
     *
     *      @param [in] placedMapItems      A list of the placed memory map items.
     *      @param [in] memoryMapColumn     The main memory map column.
     */
    void repositionCompressedMemoryMaps(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems,
        MemoryColumn* memoryMapColumn);

    /*!
     *  Get the main graphics item corresponding to the selected connection interface.
     *
     *      @param [in] connectionInterface     The selected connection interface.
     *      @param [in] columnType              The column type of the main graphics item.
     *
     *      @return The found main graphics item.
     */
    MainMemoryGraphicsItem* getMainGraphicsItem(QSharedPointer<ConnectivityInterface const> connectionInterface,
        QString columnType) const;

    /*!
     *  Get the local memory map graphics item of the selected address space interface.
     *
     *      @param [in] spaceInterface  Interface node referencing the selected address space item.
     *
     *      @return The local memory map graphics item of contained within the selected interface node.
     */
    MainMemoryGraphicsItem* getLocalMemoryMapItem(QSharedPointer<ConnectivityInterface const> spaceInterface) const;

    /*!
     *	Create the connections between the selected address space item.
     *
     *		@param [in]	connectionStartItem     First address space item of the connection path.
     *		@param [in]	spaceColumn             The address space column.
     *		@param [in]	spaceChain              Address space items to be connected.
     *		@param [in]	connectionPath          Path of the memory connection.
     *		@param [in]	placedSpaceItems        List of the placed address space items.
     *		@param [in]	spaceYPlacement         Y coordinate of the address space placement.
     *
     *		@return	A list of the connected address space graphics items.
     */
    QVector<MainMemoryGraphicsItem*> createSpaceChain(MainMemoryGraphicsItem* connectionStartItem,
        MemoryColumn* spaceColumn, QVector<MemoryConnectionAddressCalculator::ChainedSpace> spaceChain,
        QVector<QSharedPointer<const ConnectivityInterface> > connectionPath,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems, qreal& spaceYPlacement);

    /*!
     *  Create a connection between two address space items.
     *
     *      @param [in] connectionPath          Path of the memory connection.
     *      @param [in] connectionStartItem     Start item of the connection.
     *      @param [in] connectionBaseAddress   Base address of the connection.
     *      @param [in] connectionMiddleItem    The second address space item.
     *      @param [in] newSpaceInterface       Interface of the second address space item.
     *      @param [in] spaceColumn             The address space column.
     *      @param [in] placedSpaceItems        List of the placed address space items.
     *      @param [in] spaceItemChain          Connection chain of address space items.
     *      @param [in] spaceYPlacement         Y coordinate of the address space placement.
     */
    void createSpaceConnection(QVector<QSharedPointer<ConnectivityInterface const> > connectionPath,
        MainMemoryGraphicsItem* connectionStartItem, quint64 connectionBaseAddress,
        MainMemoryGraphicsItem* connectionMiddleItem, MemoryColumn* spaceColumn,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
        QVector<MainMemoryGraphicsItem*> spaceItemChain, qreal& spaceYPlacement);

    /*!
     *  Place the selected address space item.
     *
     *      @param [in] spaceItem               The selected address space item.
     *      @param [in] positionY               The new y-coordinate for the address space item.
     *      @param [in] originalColumn          Original column of the address space item.
     *      @param [in] placedSpaceItems        List of all the address space items that have been placed.
     *      @param [in] connectionStartItem     Start item of the connection.
     */
    void placeSpaceItem(MainMemoryGraphicsItem* spaceItem, qreal positionY,
        MemoryColumn* originalColumn, QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
        MainMemoryGraphicsItem* connectionStartItem);

    /*!
     *  Change the column of the master address space item.
     *
     *      @param [in] masterSpaceItem     The selected master address space item.
     *      @param [in] spaceItemY          Y-coordinate of the address space item.
     *      @param [in] originalColumn      Original column of the master address space item.
     *      @param [in] spaceItemChain      Connection chain of address space items.
     */
    void changeMasterAddressSpaceColumn(MainMemoryGraphicsItem* masterSpaceItem, qreal spaceItemY,
        MemoryColumn* originalColumn, QVector<MainMemoryGraphicsItem*> spaceItemChain);

    /*!
     *  Change the column of a colliding master address space item.
     *
     *      @param [in] currentColumn   The current column of the master address space item.
     *      @param [in] spaceItemChain  Connection chain of address space items.
     */
    void changeCollidingMasterAddressSpaceColumn(MemoryColumn* currentColumn,
        QVector<MainMemoryGraphicsItem*> spaceItemChain);

    /*!
     *  Reposition the overlapping address space items.
     *
     *      @param [in] placedSpaceItems    List of all the placed address space items.
     */
    void repositionOverlappingSpaceItems(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems);

    /*!
     *  Get the rectangle of selected item with adjustments made from the connected memory connections.
     *
     *      @param [in] memoryItem  The selected memory item.
     *
     *      @return Rectangle with the height to contain the selected memory item and all its connection items.
     */
    QRectF getConnectionAdjustedItemRectangle(MainMemoryGraphicsItem* memoryItem) const;

    /*!
     *  Change the column for the selected address space item.
     *
     *      @param [in] spaceItem       The selected address space item.
     *      @param [in] spaceRectangle  Rectangle of the selected address space item.
     *      @param [in] spaceLineWidth  Line width of the selected address space item.
     *      @param [in] newPositionY    New y-coordinate for the address space item.
     *      @param [in] originalColumn  Original column of the address space item.
     */
    void changeSpaceItemColumn(MainMemoryGraphicsItem* spaceItem, QRectF spaceRectangle, int spaceLineWidth,
        qreal newPositionY, MemoryColumn* originalColumn);

    /*!
     *  Get the memory item colliding with the selected item.
     *
     *      @param [in] spaceRectangle  Bounding rectangle of the selected item.
     *      @param [in] spaceLineWidth  Line width of the selected item.
     *      @param [in] currentColumn   Current column of the selected item.
     *
     *      @return The memory item colliding with the selected item.
     */
    MainMemoryGraphicsItem* getCollidingSpaceItem(QRectF spaceRectangle, int spaceLineWidth,
        MemoryColumn* currentColumn) const;

    /*!
     *  Change the address range of the selected memory graphics item.
     *
     *      @param [in] connectionEndItem       The selected memory graphics item.
     *      @param [in] remappedAddress         Remapped address of the memory connection.
     *      @param [in] memoryMapBaseAddress    Base address of the memory graphics item.
     *      @param [in] hasRemappedRange        Value for connections through mirrored slaves.
     */
    void changeConnectionEndItemRanges(MainMemoryGraphicsItem* connectionEndItem, quint64 remappedAddress,
        quint64 memoryMapBaseAddress, bool hasRemappedRange);

    /*!
     *  Combine paths containing the same memory items.
     *	
     *      @param [in] masterSlavePaths    Interface paths from master to slave.
     *
     *      @return A list of combined path sets.
     */
    QVector<QSharedPointer<QVector<Path> > > findPathSets(QVector<Path> masterSlavePaths) const;

    /*!
     *  Find the indexes of the path sets that the selected path belongs to.
     *	
     *      @param [in] currentPath     The selected path.
     *      @param [in] pathSets        List of the path sets.
     *
     *      @return List of path set indexes.
     */
    QQueue<int> findPathSetIndexes(Path currentPath, QVector<QSharedPointer<QVector<Path> > > pathSets) const;

    /*!
     *  Check if an interface of the selected path is contained with the selected path set.
     *	
     *      @param [in] currentPath     The selected path.
     *      @param [in] pathSet         The selected compraison path set.
     *
     *      @return True, if an interface is contained within the path set, false otherwise.
     */
    bool pathIsContainedWithPathSet(Path currentPath, QSharedPointer<QVector<Path> > pathSet) const;

    /*!
     *  Check if the selected interface is contained within the selected path.
     *	
     *      @param [in] pathInterface   The selected interface.
     *      @param [in] comparisonPath  The selected comparison path.
     *
     *      @return True, if the interface is contained within the path, false otherwise.
     */
    bool interfacedItemIsWithinPath(QSharedPointer<const ConnectivityInterface> pathInterface, MemoryConnectionHandler::Path comparisonPath) const;

    /*!
     *  Create a combined connection set from the selected path set.
     *	
     *      @param [in] connectionSet       The selected path set.
     *      @param [in] placedMapItems      List of already placed memory map items.
     *      @param [in] memoryMapColumn     The memory map column.
     *      @param [in] spaceYPlacement     Placement of the top item in the connection set.
     *      @param [in] placedSpaceItems    List of already placed address space items.
     *      @param [in] spaceColumn         The address space column.
     *
     *      @return List of the created memory connections.
     */
    QVector<MemoryConnectionItem*> createConnectionSet(QSharedPointer<QVector<Path> > connectionSet,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems,
        MemoryColumn* memoryMapColumn,
        qreal& spaceYPlacement,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
        MemoryColumn* spaceColumn);

    /*!
     *  Create a connection from the set.
     *	
     *      @param [in] connectionPath      The connection path.
     *      @param [in] placedMapItems      List of the already placed memory maps.
     *      @param [in] memoryMapColumn     The memory map column.
     *      @param [in] placedSpaceItems    List of the already placed address spaces.
     *      @param [in] spaceColumn         The address space column.
     */
    void createConnectionFromSet(Path const& connectionPath,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems,
        MemoryColumn* memoryMapColumn,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
        MemoryColumn* spaceColumn);

    /*!
     *  Place the memory map item.
     *	
     *      @param [in] mapItem         The connected memory map item.
     *      @param [in] startItem       The connection starting item.
     *      @param [in] yTransfer       The initial position of the connection from the starting memory item.
     *      @param [in] pathVariables   The calculated variables for the connection.
     *      @param [in] placedMapItems  List of the placed memory map items.
     *      @param [in] mapColumn       The memory map column.
     */
    void placeMemoryMap(MainMemoryGraphicsItem* mapItem, MainMemoryGraphicsItem* startItem, qreal yTransfer, MemoryConnectionAddressCalculator::ConnectionPathVariables const& pathVariables,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems, MemoryColumn* mapColumn);

    /*!
     *  Get the memory item in the highest position of the set.
     *	
     *      @param [in] currentHighestItem  The currently highest memory item.
     *      @param [in] itemSet             The selected set.
     *
     *      @return The highest memory item.
     */
    MainMemoryGraphicsItem* getHighestPlacedItemInSet(MainMemoryGraphicsItem* currentHighestItem, QSharedPointer<QVector<MainMemoryGraphicsItem*> > itemSet);

    /*!
     *  Create the connection item.
     *	
     *      @param [in] startItem               The starting item of the connection.
     *      @param [in] endItem                 The end item of the connection.
     *      @param [in] remappedAddress         Base address for the connection.
     *      @param [in] remappedEndAddress      End address for the connection.
     *      @param [in] memoryMapBaseAddress    Base address of the connected memory map.
     *      @param [in] hasRemapRange           Flag for the remap range.
     *      @param [in] yTransfer               Position of the connection relative to the starting item.
     */
    void createConnectionItem(MainMemoryGraphicsItem* startItem, MainMemoryGraphicsItem* endItem, quint64 remappedAddress, quint64 remappedEndAddress, quint64 memoryMapBaseAddress, bool hasRemapRange, qreal yTransfer);

    /*!
     *  Place the address space item.
     *	
     *      @param [in] spaceItem               The selected address space item.
     *      @param [in] placedSpaceItems        List of the placed address space items.
     *      @param [in] originalColumn          Original column of the address space items.
     *      @param [in] targetItem              End item of the connection.
     *      @param [in] connectionBaseAddress   Base address of the connection.
     */
    void positionSpaceItem(MainMemoryGraphicsItem* spaceItem,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
        MemoryColumn* originalColumn,
        MainMemoryGraphicsItem const* targetItem,
        quint64 connectionBaseAddress);

    /*!
     *  Place the address space item to a column.
     *	
     *      @param [in] spaceItem           The selected address space item.
     *      @param [in] spaceRectangle      Scene rectangle of the new position for the address space item.
     *      @param [in] spaceLineWidth      Line width of the address space item.
     *      @param [in] placedSpaceItems    List of the placed address space items.
     */
    void placeSpaceItemToColumn(MainMemoryGraphicsItem* spaceItem, QRectF const& spaceRectangle, int spaceLineWidth, QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems);

    /*!
     *  Compress and reposition the connection set.
     *	
     *      @param [in] placedSpaceItems    List of the address space items of the connection set.
     *      @param [in] placedMapItems      List of the memory map items of the connection set.
     *
     *      @return The memory connections of the set.
     */
    QVector<MemoryConnectionItem*> compressConnectedMemoryItems(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems, QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems);

    /*!
     *  Calculate the area movements of the compressed set.
     *	
     *      @param [in] uncutCoordinates    Coordinates that must be visible in the compressed set.
     *      @param [in] CUTMODIFIER         Maximum height of the compressed set.
     *
     *      @return Map with Y-coordinate as the key and total movement up to this coordinate as the value.
     */
    QMap<qreal, qreal> calculateAreaMovements(QVector<qreal> uncutCoordinates, qreal const& CUTMODIFIER) const;

    /*!
     *  Redraw the memory connections.
     *	
     *      @param [in] connectionItems     List of the memory connection items.
     *
     *      @return 
     */
    void redrawMemoryConnections(QVector<MemoryConnectionItem*> connectionItems) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Value for displaying condensed memory connections.
    bool condenseMemoryItems_;

    //! Value for filtering the chained address space memory connections.
    bool filterAddressSpaceChains_;

    //! A list of memory connections made to memory maps.
    QVector<MemoryConnectionItem*> connectionsToMemoryMaps_;

    //! A list of memory collisions.
    QVector<MemoryCollisionItem*> memoryCollisions_;

    //! Handler for memory columns.
    QSharedPointer<MemoryColumnHandler> columnHandler_;
};

//-----------------------------------------------------------------------------

#endif // MEMORYCONNECTIONHANDLER_H
