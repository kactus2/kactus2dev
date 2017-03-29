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

private:
    // Disable copying.
    MemoryConnectionHandler(MemoryConnectionHandler const& rhs);
    MemoryConnectionHandler& operator=(MemoryConnectionHandler const& rhs);

    //! Path variables determined during connection path examination.
    struct ConnectionPathVariables
    {
        //! The base address number of the path interfaces.
        quint64 baseAddressNumber_;

        //! Address change caused by mirrored slave bus interfaces.
        quint64 mirroredSlaveAddressChange_;

        //! End address of the memory map modified by mirrored slave interfaces.
        quint64 memoryMapEndAddress_;

        //! Base address modified by the interface bridges.
        quint64 spaceChainBaseAddress_;

        //! Flag for paths that have gone through a mirrored slave with a remapped address range.
        bool hasRemapRange_;

        //! Flag for paths that have gone through a bridge.
        bool isChainedSpaceConnection_;

        //! Address space items referenced by the bridged master bus interfaces of the path.
        QVector<MainMemoryGraphicsItem*> spaceChain_;

        //! The last address space item that will be connected to the memory map item.
        MainMemoryGraphicsItem* spaceItemConnectedToMapItem_;
    };

    /*!
     *  Create a memory connection between an address space and a memory map.
     *
     *      @param [in] connectionPath          The path of the memory connection.
     *      @param [in] placedMapItems          A list of placed memory map items.
     *      @param [in] memoryMapColumn         The column containing the memory maps.
     *      @param [in] spaceYPlacement         The Y placement of the next address space item.
     *      @param [in] placedSpaceItems        A list of placed address space items.
     *      @param [in] spaceColumn             The column containing the address spaces.
     *      @param [in] previousConnectionLow   Lowest point of the previous memory connection and the connected
     *                                          items.
     */
    void createConnection(QVector<QSharedPointer<ConnectivityInterface> > connectionPath,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems, MemoryColumn* memoryMapColumn,
        int& spaceYPlacement, QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
        MemoryColumn* spaceColumn, qreal& previousConnectionLow);

    /*!
     *  Get the end item for the memory connection.
     *
     *      @param [in] startInterface  Start interface of the memory connection path.
     *      @param [in] endInterface    End interface of the memory connection path.
     *
     *      @return The end item of the memory connection.
     */
    MainMemoryGraphicsItem* getConnectionEndItem(QSharedPointer<ConnectivityInterface> startInterface,
        QSharedPointer<ConnectivityInterface> endInterface) const;

    /*!
     *  Get the starting base address of the memory connection.
     *
     *      @param [in] startInterface  Start interface of the memory connection path.
     *      @param [in] endInterface    End interface of the memory connection path.
     *
     *      @return Starting base address of the memory connection.
     */
    quint64 getStartingBaseAddress(QSharedPointer<ConnectivityInterface> startInterface,
        QSharedPointer<ConnectivityInterface> endInterface) const;

    /*!
     *  Examine the connection path for master or mirrored slave interfaces..
     *
     *      @param [in] baseAddressNumber       Base address number of the memory connection.
     *      @param [in] connectionStartItem     Start item of the memory connection.
     *      @param [in] connectionEndItem       End item of the memory connection.
     *      @param [in] startInterface          Start interface of the connection path.
     *      @param [in] endInterface            End interface of the connection path.
     *      @param [in] connectionPath          The connection path.
     *      @param [in] spaceColumn             Memory column for address space items.
     *      @param [in] placedSpaceItems        List of all the placed address space items.
     *      @param [in] spaceYPlacement         Y-coordinate of the next address space item.
     *
     *      @return List of variable changes caused by the interfaces on the connection path.
     */
    ConnectionPathVariables examineConnectionPath(quint64 baseAddressNumber,
        MainMemoryGraphicsItem* connectionStartItem, MainMemoryGraphicsItem* connectionEndItem,
        QSharedPointer<ConnectivityInterface> startInterface, QSharedPointer<ConnectivityInterface> endInterface,
        QVector<QSharedPointer<ConnectivityInterface> > connectionPath, MemoryColumn* spaceColumn,
        QSharedPointer<QVector<MainMemoryGraphicsItem* > > placedSpaceItems, int& spaceYPlacement);

    /*!
     *  Calculate the remapped base address for the memory connection.
     *
     *      @param [in] memoryMapBaseAddress                Base address of the memory map item.
     *      @param [in] baseAddressNumber                   Base address of the memory connection.
     *      @param [in] spaceChainConnectionBaseAddress     Base address modifier caused by the bridged interfaces.
     *      @param [in] mirroredSlaveAddressChange          Address change caused by the mirrored slave interfaces.
     *      @param [in] hasRemapRange                       Flag for remapped connection range.
     *
     *      @return The remapped base address of the memory connection.
     */
    quint64 getRemappedBaseAddress(quint64 memoryMapBaseAddress, quint64 baseAddressNumber,
        quint64 spaceChainConnectionBaseAddress, quint64 mirroredSlaveAddressChange, bool hasRemapRange) const;

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
    void placeMemoryMapItem(MainMemoryGraphicsItem* connectionEndItem, MainMemoryGraphicsItem* connectionStartItem,
        qreal yTransfer, bool hasRemapRange, quint64 memoryMapBaseAddress, bool spaceItemPlaced,
        bool isChainedSpaceConnection, QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems, MemoryColumn* memoryMapColumn,
        MemoryColumn* spaceColumn, int& spaceYPlacement);

    /*!
     *  Create the memory connection item.
     *
     *      @param [in] connectionStartItem     Start item of the memory connection.
     *      @param [in] connectionEndItem       End item of the memory connection.
     *      @param [in] remappedAddress         Remapped base address of the memory connection.
     *      @param [in] remappedEndAddress      Remapped last address of the memory connection.
     *      @param [in] memoryMapBaseAddress    Base address of the memory map item.
     *      @param [in] hasRemapRange           Flag for remapped connection range.
     *      @param [in] yTransfer               Memory connection transfer in y-coordinate.
     *      @param [in] previousConnectionLow   Lowest point of the previous memory connection.
     *      @param [in] spaceYPlacement         Y-coordinate for the address space item.
     */
    void createMemoryConnectionItem(MainMemoryGraphicsItem* connectionStartItem,
        MainMemoryGraphicsItem* connectionEndItem, quint64 remappedAddress, quint64 remappedEndAddress,
        quint64 memoryMapBaseAddress, bool hasRemapRange, qreal yTransfer, qreal& previousConnectionLow,
        int& spaceYPlacement);

    /*!
     *  Check and reposition a memory map to a memory map overlap column if needed.
     *
     *      @param [in] placedMaps              A list of placed memory map items.
     *      @param [in] memoryItem              The selected memory map item.
     *      @param [in] originalColumn          The original column of the memory map item.
     *      @param [in] connectionStartItem     The start item of the memory map connection.
     */
    void checkMemoryMapRepositionToOverlapColumn(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMaps,
        MainMemoryGraphicsItem* memoryItem, MemoryColumn* originalColumn, MainMemoryGraphicsItem* connectionStartItem);

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
     *      @param [in] originalColumn          The original column of the selected address space item.
     *      @param [in] targetItem              The other item of the memory connection.
     *      @param [in] connectionBaseAddress   Base address of the current memory connection item.
     */
    void placeSpaceItemToOtherColumn(MainMemoryGraphicsItem* spaceItem, MemoryColumn* originalColumn,
        MainMemoryGraphicsItem* targetItem, quint64 connectionBaseAddress);

    /*!
     *  Move the unconnected address space items to the bottom of the address space column.
     *
     *      @param [in] placedSpaceItems    List of the placed address space items.
     *      @param [in] spaceColumn         Column containing the address space items.
     */
    void moveUnconnectedAddressSpaces(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
        MemoryColumn* spaceColumn);

    /*!
     *  Move the unconnected memory map items to the bottom of the memory map column.
     *
     *      @param [in] placedMapItems      List of the placed memory map items.
     *      @param [in] memoryMapColumn     Column containing the memory map items.
     */
    void moveUnconnectedMemoryMaps(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems,
        MemoryColumn* memoryMapColumn);

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
        int& spaceYPlacement, MemoryColumn* spaceColumn);

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
    MainMemoryGraphicsItem* getMainGraphicsItem(QSharedPointer<ConnectivityInterface> connectionInterface,
        QString columnType) const;

    /*!
     *  Get the local memory map graphics item of the selected address space interface.
     *
     *      @param [in] spaceInterface  Interface node referencing the selected address space item.
     *
     *      @return The local memory map graphics item of contained within the selected interface node.
     */
    MainMemoryGraphicsItem* getLocalMemoryMapItem(QSharedPointer<ConnectivityInterface> spaceInterface) const;

    /*!
     *  Get the memory item node referencing the selected local memory map.
     *
     *      @param [in] spaceInterface  Interface node containing the selected local memory map.
     */
    QSharedPointer<MemoryItem> getMemoryItemForLocalMap(QSharedPointer<ConnectivityInterface> spaceInterface)
        const;

    /*!
     *  Create a connection between two address space items.
     *
     *      @param [in] connectionStartItem     Start item of the connection.
     *      @param [in] connectionBaseAddress   Base address of the connection.
     *      @param [in] connectionMiddleItem    The second address space item.
     *      @param [in] newSpaceInterface       Interface of the second address space item.
     *      @param [in] spaceColumn             The address space column.
     *      @param [in] placedSpaceItems        List of the placed address space items.
     *      @param [in] spaceItemChain          Connection chain of address space items.
     *      @param [in] spaceYPlacement         Y coordinate of the address space placement.
     */
    void createSpaceConnection(MainMemoryGraphicsItem* connectionStartItem, quint64 connectionBaseAddress,
        MainMemoryGraphicsItem* connectionMiddleItem, QSharedPointer<ConnectivityInterface> newSpaceInterface,
        MemoryColumn* spaceColumn, QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
        QVector<MainMemoryGraphicsItem*> spaceItemChain, int& spaceYPlacement);

    /*!
     *  Place the selected address space item.
     *
     *      @param [in] spaceItem               The selected address space item.
     *      @param [in] positionY               The new y-coordinate for the address space item.
     *      @param [in] originalColumn          Original column of the address space item.
     *      @param [in] placedSpaceItems        List of all the address space items that have been placed.
     *      @param [in] connectionStartItem     Start item of the connection.
     */
    void placeSpaceItem(MainMemoryGraphicsItem* spaceItem, int positionY,
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
