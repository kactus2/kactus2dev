//-----------------------------------------------------------------------------
// File: AddressSpaceGraphicsItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 08.07.2016
//
// Description:
// Graphics item for visualizing an address space in the memory designer.
//-----------------------------------------------------------------------------

#ifndef ADDRESSSPACEGRAPHICSITEM_H
#define ADDRESSSPACEGRAPHICSITEM_H

#include <common/graphicsItems/GraphicsItemTypes.h>

#include <editors/MemoryDesigner/MainMemoryGraphicsItem.h>

class AddressSpace;
class MemoryItem;
class ConnectivityComponent;
class AddressSegmentGraphicsItem;

#include <QSharedPointer>
#include <QGraphicsPixmapItem>

//-----------------------------------------------------------------------------
//! Graphics item for visualizing address spaces in the memory designer.
//-----------------------------------------------------------------------------
class AddressSpaceGraphicsItem : public MainMemoryGraphicsItem
{

public:
    //! The type of the address space graphics item.
    enum { Type = GFX_TYPE_ADDRESS_SPACE_ITEM };

    /*!
     *  The constructor.
     *
     *      @param [in] memoryItem          Memory item containing address space data.
     *      @param [in] identifierChain     Chain of strings identifying this item.
     *      @param [in] containingInstance  Data of the containing component instance.
     *      @param [in] filterSegments      Value for filtering address space segments.
     *      @param [in] parent              The parent item.
     */
    AddressSpaceGraphicsItem(QSharedPointer<MemoryItem const> memoryItem, QVector<QString> identifierChain,
        QSharedPointer<ConnectivityComponent const> containingInstance, bool filterSegments,
        QGraphicsItem* parent = 0);

    // Disable copying.
    AddressSpaceGraphicsItem(AddressSpaceGraphicsItem const& rhs) = delete;
    AddressSpaceGraphicsItem& operator=(AddressSpaceGraphicsItem const& rhs) = delete;

	/*!
     *  The destructor.
     */
    virtual ~AddressSpaceGraphicsItem() = default;

    /*!
     *  Get the type of the address space graphics item.
     */
    int type() const { return Type; }

    /*!
     *  Compress the sub items contained within the address space and the space item.
     *
     *      @param [in] movedConnections        Connection items that have already been moved.
     *      @param [in] condenseMemoryItems     Flag for condensing memory items.
     */
    virtual void condenseItemAndChildItems(QSharedPointer<QVector<MemoryConnectionItem*> > movedConnections,
        bool condenseMemoryItems) override;

    /*!
     *  Get all the address space graphics items connected to the origin item.
     */
    QVector<MainMemoryGraphicsItem*> getAllConnectedSpaceItems() const;

private:

    /*!
     *  Set the positions of the labels.
     */
    virtual void setLabelPositions() override;

    /*!
     *  Create a new address segment graphics item.
     *
     *      @param [in] subMemoryItem   Memory item containing address segment data.
     *      @param [in] isEmpty         Boolean value for an empty address segment.
     *
     *      @return The created address segment graphics item.
     */
    virtual MemoryDesignerChildGraphicsItem* createNewSubItem(QSharedPointer<MemoryItem const> subMemoryItem,
        bool isEmpty) override;

    /*!
     *  Create an empty address segment graphics item.
     *
     *      @param [in] beginAddress    Base address of the empty address segment graphics item.
     *      @param [in] rangeEnd        End address of the empty address segment graphics item.
     *
     *      @return The created address segment graphics item.
     */
    virtual MemoryDesignerChildGraphicsItem* createEmptySubItem(quint64 beginAddress, quint64 rangeEnd) override;

    /*!
     *  Get the compressed height of an address space graphics item.
     *
     *      @param [in] memoryItemsAreCompressed    Flag for condensing memory items.
     *
     *      @return The compressed height of an address space graphics item.
     */
    quint64 getFilteredCompressedHeight(bool memoryItemsAreCompressed);

    /*!
     *  Get the compressed height of an address space graphics item using coordinate positions of the connected
     *  items.
     *
     *      @param [in] memoryItemsAreCompressed    Flag for condensing memory items.
     *      @param [in] visitedMemoryItems          List of handled memory items.
     *      @param [in] connectionIterator          Iterator that goes through the memory connections.
     *
     *      @return The compressed height of an address space graphics item.
     */
    qreal getFilteredCompressedHeightByCoordinates(bool memoryItemsAreCompressed,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > visitedMemoryItems,
        QMapIterator<quint64, MemoryConnectionItem*> connectionIterator);

    /*!
     *  Get the address space items connected to the selected memory item.
     *
     *      @param [in] memoryItem          The selected memory graphics item.
     *
     *      @return All the address space items connected to the selected item.
     */
    QVector<MainMemoryGraphicsItem*> getSpaceItemsConnectedToSpaceItem(MainMemoryGraphicsItem* memoryItem) const;

    /*!
     *  Get all the chained memory connection items.
     *
     *      @return The chained memory connection items.
     */
    QMap<quint64, MemoryConnectionItem*> getChainedMemoryConnections() const;

    /*!
     *  Get all the addresses that must be retained after compression.
     *
     *      @param [in] connectionIterator  Iterator holding all the required memory connections.
     *
     *      @return All the addresses that are retained after compression.
     */
    QVector<quint64> getUnCutAddressesFromConnections(
        QMapIterator<quint64, MemoryConnectionItem*> connectionIterator) const;

    /*!
     *  Get the list of coordinates that must be retained after compression.
     *
     *      @param [in] connectionIterator  Iterator holding all the connected memory connections.
     *
     *      @return All the coordinates that are retained after compression.
     */
    QVector<qreal> getUnCutCoordinatesFromConnections(
        QMapIterator<quint64, MemoryConnectionItem*> connectionIterator) const;

    /*!
     *  Check if an address space has connections pointing to the same memory item with different address ranges.
     *
     *      @param [in] connectionIterator  Iterator holding all the connected memory connections.
     *
     *      @return True, if a problem connection is found, false otherwise.
     */
    bool hasProblemConnection(QMapIterator<quint64, MemoryConnectionItem*> connectionIterator) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The icon for an address space with a CPU.
    QGraphicsPixmapItem* cpuIcon_;
};

//-----------------------------------------------------------------------------

#endif // ADDRESSSPACEGRAPHICSITEM_H
