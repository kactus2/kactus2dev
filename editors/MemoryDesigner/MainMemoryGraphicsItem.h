//-----------------------------------------------------------------------------
// File: MainMemoryGraphicsItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 16.09.2016
//
// Description:
// Parent class for memory map and address space graphics items in memory designer.
//-----------------------------------------------------------------------------

#ifndef MAINMEMORYGRAPHICSITEM_H
#define MAINMEMORYGRAPHICSITEM_H

#include <IPXACTmodels/common/VLNV.h>

#include <editors/common/diagramgrid.h>

#include <editors/MemoryDesigner/MemoryDesignerGraphicsItem.h>
#include <editors/MemoryDesigner/SubMemoryLayout.h>

class MemoryItem;
class MemoryExtensionGraphicsItem;
class ConnectivityComponent;

#include <QFontMetrics>
#include <QAction>

//-----------------------------------------------------------------------------
//! Parent class for memory map and address space graphics items in memory designer.
//-----------------------------------------------------------------------------
class MainMemoryGraphicsItem : public MemoryDesignerGraphicsItem, public SubMemoryLayout
{

public:

    /*!
     *  The constructor.
     *
     *    @param [in] memoryItem          Memory item data.
     *    @param [in] containingInstance  The containing instance.
     *    @param [in] subItemType         Type of the memory sub items.
     *    @param [in] filterSubItems      Value for filtering sub items.
     *    @param [in] identifierChain     Chain of strings identifying this item.
     *    @param [in] parent              The parent item.
     */
    MainMemoryGraphicsItem(QSharedPointer<MemoryItem const> memoryItem,
        QSharedPointer<ConnectivityComponent const> containingInstance, QString const& subItemType,
        bool filterSubItems, QVector<QString> identifierChain, QGraphicsItem* parent = 0);

	/*!
     *  The destructor.
     */
    virtual ~MainMemoryGraphicsItem() = default;

    /*!
     *  Add a memory connection to this item.
     *
     *    @param [in] connectionItem  The selected memory connection item.
     */
    virtual void addMemoryConnection(MemoryConnectionItem* connectionItem);

    /*!
     *  Get the memory collision items contained within this item.
     *
     *    @return Memory collision items contained within this item.
     */
    QVector<MemoryCollisionItem*> getMemoryCollisions() const;

    /*!
     *  Compress the item and the sub items.
     *	
     *    @param [in] compressMemoryItems     Flag for compressing the memory items.
     */
    void compressItemAndChildItems(bool compressMemoryItems);

    /*!
     *  Get the used coordinates of the item and all the connected items.
     *	
     *    @param [in] visitedItems        List of the visited memory items.
     *    @param [in] visitedConnections  List of the visited memory connections.
     *
     *    @return List of visible coordinates.
     */
    QVector<qreal> getUncutCoordinatesFromSet(QVector<MainMemoryGraphicsItem*>& visitedItems, QVector<MemoryConnectionItem*>& visitedConnections);

    /*!
     *  Move memory item and all the connected memory connections and the connected memory items.
     *
     *    @param [in] yTransfer   The amount of movement in the y-coordinate.
     */
    void moveItemAndConnectedItems(qreal yTransfer);

    /*!
     *  Get the last memory connection item connected to this memory graphics item.
     *
     *    @return The last memory connection item connected to this memory graphics item.
     */
    MemoryConnectionItem* getLastConnection() const;

    /*!
     *  Get the first memory connection item connected to this memory graphics item.
     *
     *    @return The memory connection item with the lowest base address.
     */
    MemoryConnectionItem* getFirstConnection() const;

    /*!
     *  Check if the memory graphics item has been compressed.
     *
     *    @return True, if the item has been compressed, false otherwise.
     */
    bool isCompressed() const;

    /*!
     *  Set a memory extension item for this graphics item.
     *
     *    @param [in] newExtensionItem    The new memory extension item.
     */
    void setExtensionItem(MemoryExtensionGraphicsItem* newExtensionItem);

    /*!
     *  Get the memory extension item contained within this graphics item.
     *
     *    @return The memory extension item contained within this graphics item.
     */
    MemoryExtensionGraphicsItem* getExtensionItem() const;

    /*!
     *  Check whether this item has a memory extension item.
     *
     *    @return True, if a memory extension item exists, false otherwise.
     */
    bool hasExtensionItem() const;

    /*!
     *  Get the memory item from which this graphics item has been constructed from.
     *
     *    @return The contained memory item.
     */
    QSharedPointer<MemoryItem const> getMemoryItem() const;

    /*!
     *  Create collision markers for overlapping connections.
     */
    void createOverlappingConnectionMarkers();

    /*!
     *  Check if the memory map is connected to any of the selected address spaces.
     *
     *    @param [in] spaceItems  List of the selected address space graphics items.
     *
     *    @return True, if the memory map is connected to any of the selected address spaces.
     */
    bool isConnectedToSpaceItems(QVector<MainMemoryGraphicsItem*> spaceItems) const;

    /*!
     *  Get the chained address space items connected to this item.
     *
     *    @return All the chained address space items connected to this item.
     */
    QVector<MainMemoryGraphicsItem*> getChainedSpaceItems() const;

    /*!
     *  Compress this item to the uncut coordinates.
     *
     *    @param [in] unCutCoordinates            List of the coordinates that are retained after compression.
     *    @param [in] CUTMODIFIER                 The modifier for the size of the cut area.
     *    @param [in] memoryItemsAreCompressed    Flag for condensing memory items.
     */
    virtual void compressToUnCutCoordinates(QVector<qreal> unCutCoordinates, const qreal CUTMODIFIER,
        bool memoryItemsAreCompressed);

    /*!
     *  Change the address range of this item and its sub items.
     *
     *    @param [in] offsetChange    The offset change of the item.
     */
    virtual void changeAddressRange(quint64 offsetChange);

    /*!
     *  Get the lowest point of all the connected memory items.
     *
     *    @return The lowest point of all the connected memory items.
     */
    qreal getLowestPointOfConnectedItems();

    /*!
     *  Get the lowest base address of all the connected memory items.
     *
     *    @return The lowest base address of all the connected memory items.
     */
    quint64 getLowestConnectedBaseAddress() const;

    /*!
     *  Get the highest last address of all the connected memory items.
     *
     *    @return The highest last address of all the connected memory items.
     */
    quint64 getHighestConnectedLastAddress() const;

    /*!
     *  Construct a memory graphics extension for this memory item.
     */
    void extendMemoryItem();

protected:

    /*!
     *  Get the instance name label.
     *
     *    @return Graphics text item containing the name of the containing component instance.
     */
    QGraphicsTextItem* getInstanceNameLabel() const;

    /*!
     *  Set a new compression value.
     *
     *    @param [in] newCompressValue    The new memory compression value.
     */
    void setCompressed(bool newCompressValue);

    /*!
     *  Get all the connections from connected memory items.
     *
     *    @param [in] visitedMemoryItems  List of all the memory items that have been checked.
     *
     *    @return All the connections from the connected memory items.
     */
    QMultiMap<quint64, MemoryConnectionItem*> getAllConnectionsFromConnectedItems(
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > visitedMemoryItems) const;

private:
    // Disable copying.
    MainMemoryGraphicsItem(MainMemoryGraphicsItem const& rhs);
    MainMemoryGraphicsItem& operator=(MainMemoryGraphicsItem const& rhs);

    /*!
     *  Get the available width for this item.
     *
     *    @return Available width for this item.
     */
    virtual qreal getItemWidth() const;

    /*!
     *  Check if the selected label collides with range labels.
     *
     *    @param [in] label   The selected label.
     *    @param [in] height  Height of the label text.
     *
     *    @return True, if the selected label collides with range labels, false otherwise.
     */
    virtual bool labelCollidesWithRangeLabels(QGraphicsTextItem* label, qreal fontHeight) const;

    /*!
     *  Get the minimum height required to display for the memory item.
     *
     *    @return The minimum height required to display the memory item.
     */
    virtual qreal getMinimumHeightForSubItems() const = 0;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The label containing the name of the containing component instance.
    QGraphicsTextItem* instanceNameLabel_;

    //! The memory item from which this graphics item has been constructed from.
    QSharedPointer<MemoryItem const> memoryItem_;

    //! A list of memory collisions connected to this memory item.
    QVector<MemoryCollisionItem*> memoryCollisions_;

    //! Holds whether the graphics item has been compressed or not.
    bool compressed_;

    //! The memory extension item.
    MemoryExtensionGraphicsItem* extensionItem_;
};

//-----------------------------------------------------------------------------

#endif // MAINMEMORYGRAPHICSITEM_H
