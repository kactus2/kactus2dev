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

#include <designEditors/common/diagramgrid.h>

#include <designEditors/MemoryDesigner/MemoryDesignerGraphicsItem.h>
#include <designEditors/MemoryDesigner/SubMemoryLayout.h>

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
     *      @param [in] memoryItem          Memory item data.
     *      @param [in] containingInstance  The containing instance.
     *      @param [in] subItemType         Type of the memory sub items.
     *      @param [in] filterSubItems      Value for filtering sub items.
     *      @param [in] identifierChain     Chain of strings identifying this item.
     *      @param [in] parent              The parent item.
     */
    MainMemoryGraphicsItem(QSharedPointer<MemoryItem> memoryItem,
        QSharedPointer<ConnectivityComponent> containingInstance, QString const& subItemType, bool filterSubItems,
        QVector<QString> identifierChain, QGraphicsItem* parent = 0);

	/*!
     *  The destructor.
     */
    virtual ~MainMemoryGraphicsItem();

    /*!
     *  Add a memory connection to this item.
     *
     *      @param [in] connectionItem  The selected memory connection item.
     */
    virtual void addMemoryConnection(MemoryConnectionItem* connectionItem);

    /*!
     *  Get the memory collision items contained within this item.
     *
     *      @return Memory collision items contained within this item.
     */
    QVector<MemoryCollisionItem*> getMemoryCollisions() const;

    /*!
     *  Change the ranges of the child items.
     *
     *      @param [in] offset  Offset of this memory item.
     */
    virtual void changeChildItemRanges(quint64 offset);

    /*!
     *  Redraw the connected memory connections.
     */
    void reDrawConnections();

    /*!
     *  Compress the item and the contained sub items.
     *
     *      @param [in] movedConnections    Connection items that have already been moved.
     */
    virtual void condenseItemAndChildItems(QSharedPointer<QVector<MemoryConnectionItem*> > movedConnections) = 0;

    /*!
     *  Move memory item and all the connected memory connections and the connected memory items.
     *
     *      @param [in] yTransfer   The amount of movement in the y-coordinate.
     */
    void moveItemAndConnectedItems(qreal yTransfer);

    /*!
     *  Get lowest point of the item, connection or connected item.
     *
     *      @return The lowest point.
     */
    quint64 getSceneEndPoint() const;

    /*!
     *  Get the last memory connection item connected to this memory graphics item.
     *
     *      @return The last memory connection item connected to this memory graphics item.
     */
    MemoryConnectionItem* getLastConnection() const;

    /*!
     *  Get the first memory connection item connected to this memory graphics item.
     *
     *      @return The memory connection item with the lowest base address.
     */
    MemoryConnectionItem* getFirstConnection() const;

    /*!
     *  Check if the memory graphics item has been compressed.
     *
     *      @return True, if the item has been compressed, false otherwise.
     */
    bool isCompressed() const;

    /*!
     *  Set a memory extension item for this graphics item.
     *
     *      @param [in] newExtensionItem    The new memory extension item.
     */
    void setExtensionItem(MemoryExtensionGraphicsItem* newExtensionItem);

    /*!
     *  Get the memory extension item contained within this graphics item.
     *
     *      @return The memory extension item contained within this graphics item.
     */
    MemoryExtensionGraphicsItem* getExtensionItem() const;

    /*!
     *  Check whether this item has a memory extension item.
     *
     *      @return True, if a memory extension item exists, false otherwise.
     */
    bool hasExtensionItem() const;

    /*!
     *  Hide the base address label of the first sub item and the last address label of the last sub item.
     */
    void hideFirstAndLastSegmentRange();

    /*!
     *  Get the memory item from which this graphics item has been constructed from.
     *
     *      @return The contained memory item.
     */
    QSharedPointer<MemoryItem> getMemoryItem() const;

    /*!
     *  Create collision markers for overlapping connections.
     */
    void createOverlappingConnectionMarkers();

    /*!
     *  Check if the memory map is connected to any of the selected address spaces.
     *
     *      @param [in] spaceItems  List of the selected address space graphics items.
     *
     *      @return True, if the memory map is connected to any of the selected address spaces.
     */
    bool isConnectedToSpaceItems(QVector<MainMemoryGraphicsItem*> spaceItems) const;

    /*!
     *  Get the chained address space items connected to this item.
     *
     *      @return All the chained address space items connected to this item.
     */
    QVector<MainMemoryGraphicsItem*> getChainedSpaceItems() const;

    /*!
     *  Compress this item to the uncut addresses.
     *
     *      @param [in] unCutAddresses  List of the addresses that are retained after compression.
     *      @param [in] CUTMODIFIER     The modifier for the size of the cut area.
     */
    virtual void compressToUnCutAddresses(QVector<quint64> unCutAddresses, const int CUTMODIFIER);

    /*!
     *  Compress this item to the uncut coordinates.
     *
     *      @param [in] unCutCoordinates    List of the coordinates that are retained after compression.
     *      @param [in] CUTMODIFIER         The modifier for the size of the cut area.
     */
    virtual void compressToUnCutCoordinates(QVector<qreal> unCutCoordinates, const qreal CUTMODIFIER);

    /*!
     *  Change the address range of this item and its sub items.
     *
     *      @param [in] offsetChange    The offset change of the item.
     */
    virtual void changeAddressRange(quint64 offsetChange);

    /*!
     *  Get the lowest point of all the connected memory items.
     *
     *      @return The lowest point of all the connected memory items.
     */
    qreal getLowestPointOfConnectedItems();

    /*!
     *  Get the lowest base address of all the connected memory items.
     *
     *      @return The lowest base address of all the connected memory items.
     */
    quint64 getLowestConnectedBaseAddress() const;

    /*!
     *  Get the highest last address of all the connected memory items.
     *
     *      @return The highest last address of all the connected memory items.
     */
    quint64 getHighestConnectedLastAddress() const;

protected:

    /*!
     *  Get the instance name label.
     *
     *      @return Graphics text item containing the name of the containing component instance.
     */
    QGraphicsTextItem* getInstanceNameLabel() const;

    /*!
     *  Set a new compression value.
     *
     *      @param [in] newCompressValue    The new memory compression value.
     */
    void setCompressed(bool newCompressValue);

    /*!
     *  Get all the connections from connected memory items.
     *
     *      @param [in] visitedMemoryItems  List of all the memory items that have been checked.
     *
     *      @return All the connections from the connected memory items.
     */
    QMap<quint64, MemoryConnectionItem*> getAllConnectionsFromConnectedItems(
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > visitedMemoryItems) const;

private:
    // Disable copying.
    MainMemoryGraphicsItem(MainMemoryGraphicsItem const& rhs);
    MainMemoryGraphicsItem& operator=(MainMemoryGraphicsItem const& rhs);

    /*!
     *  Get the available width for this item.
     *
     *      @return Available width for this item.
     */
    virtual qreal getItemWidth() const;

    /*!
     *  Check if the selected label collides with range labels.
     *
     *      @param [in] label   The selected label.
     *      @param [in] height  Height of the label text.
     *
     *      @return True, if the selected label collides with range labels, false otherwise.
     */
    virtual bool labelCollidesWithRangeLabels(QGraphicsTextItem* label, qreal fontHeight) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The label containing the name of the containing component instance.
    QGraphicsTextItem* instanceNameLabel_;

    //! The memory item from which this graphics item has been constructed from.
    QSharedPointer<MemoryItem> memoryItem_;

    //! A list of memory collisions connected to this memory item.
    QVector<MemoryCollisionItem*> memoryCollisions_;

    //! Holds whether the graphics item has been compressed or not.
    bool compressed_;

    //! The memory extension item.
    MemoryExtensionGraphicsItem* extensionItem_;
};

//-----------------------------------------------------------------------------

#endif // MAINMEMORYGRAPHICSITEM_H
