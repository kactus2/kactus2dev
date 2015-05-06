//-----------------------------------------------------------------------------
// File: memoryvisualizationitem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 15.10.2012
//
// Description:
// A base class for graphics items to visualize memory objects
//-----------------------------------------------------------------------------

#ifndef MEMORYVISUALIZATIONITEM_H
#define MEMORYVISUALIZATIONITEM_H

#include <common/graphicsItems/expandableitem.h>
#include <common/graphicsItems/graphicsexpandcollapseitem.h>

#include <QMultiMap>
#include <QGraphicsSceneMouseEvent>

class MemoryGapItem;

//-----------------------------------------------------------------------------
//! A base class for graphics items to visualize memory objects.
//-----------------------------------------------------------------------------
class MemoryVisualizationItem : public ExpandableItem
{
	Q_OBJECT

public:

	//! The indentation of each child under its parent.
	enum Indentation 
    {
		CHILD_INDENTATION = GraphicsExpandCollapseItem::SIDE
	};

	/*! The constructor.
	 *
	 *      @param [in] parent The owner of the item.
	*/
	MemoryVisualizationItem(QGraphicsItem* parent = 0);
	
	//! The destructor
	virtual ~MemoryVisualizationItem();

    /*!
     *  Set the item brush and store it as default brush.
     *
     *      @param [in] brush   The brush to set.
     */
    void setBrush(QBrush const& brush);

	//! Refresh the item.
	virtual void refresh() = 0;

    //! Updates the labels and tooltip for the item.
    virtual void updateDisplay() = 0;

	/*! Get the offset of the item. 
	 *
	 *      @return The offset of the item from the parent item's base address.
	*/
	virtual quint64 getOffset() const = 0;

	/*! Get the last address contained in the item.
	 *
	 *      @return The last address.
	*/
	virtual quint64 getLastAddress() const = 0;

	/*! Get the bit width of the item.
	 * 
	 *      @return The bit width of the item.
	*/
	virtual int getBitWidth() const = 0;

	/*! Get number of bits the addressable unit contains.
	 *
	 *      @return The size of least addressable unit.
	*/
	virtual unsigned int getAddressUnitSize() const = 0;

	/*! Add a child visualization item for this item.
	 *
	 *       @param [in] childItem Pointer to the child to add.
	 *
	*/
	virtual void addChild(MemoryVisualizationItem* childItem);

	/*! Remove a child visualization item from this item.
	 *
	 *       @param [in] childItem Pointer to the child to remove.
	 *
	*/
	virtual void removeChild(MemoryVisualizationItem* childItem);

	/*! Set the width for the item.
	 *
	 *       @param [in] width The new width of the item.
	 *
	*/
	virtual void setWidth(qreal width);

	/*! The bounding rect of the item.
	 *
	 *      @return The rectangle that bounds the item and possible sub items.
	*/
	virtual QRectF boundingRect() const;

	/*! Sets the first non-overlapping address to display.
	 *
	 *       @param [in] The first address to set.
	*/
    virtual void setDisplayOffset(quint64 const& address);

	/*! Get the first non-overlapping address of the item.
	 *
	 *      @return The first non-overlapping address.
	 */
    virtual quint64 getDisplayOffset();

	/*! Sets the last non-overlapping address to display.
	 *
	 *       @param [in] The last address to set.
	 */
    virtual void setDisplayLastAddress(quint64 const& address);

    /*! Get the last non-overlapping address of the item.
     *
     *      @return The last non-overlapping address.
     */
    virtual quint64 getDisplayLastAddress();

	//! Sets the item to be completely overlapped by adjacent items.
    virtual void setCompleteOverlap();

    /*! Tells if the item is completely overlapped by other items.
     *
     *      @return True, if the item's memory is completely under other memory blocks, otherwise false.
     */
    virtual bool isCompletelyOverlapped() const;

    //! Set the item into conflicted (overlapping memory) state.
    virtual void setConflicted(bool conflicted);

    /*!
     *  Checks if the item is conflicted (overlapping with others).
     *
     *      @return True, if the item is conflicted, otherwise false.
     */
    bool isConflicted() const;
   
    /*! Set new positions for child items.
	 * 
	 * The child items are organized in the order of their offset.
	*/
	virtual void reorganizeChildren();

    /*!
     *  Checks if the item is to be used in the visualization.
     *
     *      @return True, if the item should be used, otherwise false.
     */
    virtual bool isPresent() const;

public slots:

    //! Refresh the item and all the sub items.
    virtual void recursiveRefresh();

signals:

	//! Emitted when this item's editor should be displayed.
	void selectEditor();

protected slots:

    
    /*!
     *  Repositions the child items within this item.
     */
    virtual void repositionChildren();

protected:
   
    //! Shows the expand/collapse icon if the item has any children. Otherwise the icon is hidden.
    void showExpandIconIfHasChildren();

    //! Update the offsets of the child items in the map and fills the empty gaps between them..
    virtual void updateChildMap();
        
    /*!
     *  Checks if the children must be repositioned inside this item.
     *
     *      @return True, if the repositioning is required.
     */
    bool mustRepositionChildren();
 
	//! Handler for mouse press events
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);

	/*! Set text to the top left corner.
	 *
	 * The function groups the hexadecimal digit into groups of 4 digits.
	 *
	 *       @param [in] text The text to display in the corner.
	*/
	virtual void setLeftTopCorner(QString const& text);

	/*! Set the address to be shown on the top left corner.
	 *
	 *       @param [in] address The address to be shown in hexadecimal form.
	*/
	virtual void setLeftTopCorner(quint64 address);

    /*!
     *  Converts an address to hexadecimal string.
     *
     *      @param [in] address   The address to convert.
     *
     *      @return The hexadecimal representation.
     */
    QString toHexString(quint64 address);

	/*! Set text to the bottom left corner.
	 *
	 * The function groups the hexadecimal digit into groups of 4 digits.
	 *
	 *       @param [in] text The text to display in the corner.
	*/
	virtual void setLeftBottomCorner(QString const& text);

	/*! Set the address to be shown on the bottom left corner.
	 *
	 *       @param [in] address The address to be shown in hexadecimal form.
	*/
	virtual void setLeftBottomCorner(quint64 address);

	//! Contains the child memory items. The offset of the child is the key.
	QMultiMap<quint64, MemoryVisualizationItem*> childItems_;

    //! The first non-overlapping address.
    quint64 firstFreeAddress_;

    //! The last non-overlapping address.
    quint64 lastFreeAddress_;

    //! Width for child items.
    qreal childWidth_;

private:
	
	//! No copying.
	MemoryVisualizationItem(const MemoryVisualizationItem& other);
	MemoryVisualizationItem& operator=(const MemoryVisualizationItem& other);

    /*!
     *  Checks if there is empty memory space between the beginning of the item and its first child.
     *
     *      @param [in] current   The currently iterated child.
     *
     *      @return True, if there is empty space, otherwise false.
     */
    bool emptySpaceBeforeFirstChild(MemoryVisualizationItem* current) const;
    
    /*!
     *  Checks if there is empty memory space between the given child and the last known used address.
     *
     *      @param [in] current             The currently iterated child.
     *      @param [in] lastAddressInUse    The last known used address.
     *
     *      @return True, if there is empty space, otherwise false.
     */
    bool emptySpaceBeforeChild(MemoryVisualizationItem* current, quint64 lastAddressInUse) const;

    /*!
     *  Checks if the two consecutive children overlap.
     *
     *      @param [in] current     The currently iterated child.
     *      @param [in] previous    The previously iterated child.
     *
     *      @return True, if the children overlap, otherwise false.
     */
    bool childrenOverlap(MemoryVisualizationItem* current, MemoryVisualizationItem* previous) const;

    /*!
     *  Creates a new child for representing a free memory slot.
     *
     *      @param [in] offset          The offset of the free memory slot.
     *      @param [in] lastAddress     The last address of the free memory slot.
     *
     *      @return The created child item.
     */
    MemoryGapItem* createMemoryGap(quint64 offset, quint64 lastAddress);
    
    /*!
     *  Creates a new child for representing an overlapping memory slot.
     *
     *      @param [in] offset          The offset of the overlapping memory slot.
     *      @param [in] lastAddress     The last address of the overlapping memory slot.
     *
     *      @return The created child item.
     */
    MemoryGapItem* createConflictItem(qint64 offset, qint64 lastAddress);

    //! comparison function for two equal offsets.
    static bool compareItems(const MemoryVisualizationItem* s1, const MemoryVisualizationItem* s2);

    /*!
     *  Groups a given address text to groups of four digits.
     *
     *      @param [in] text   The address to group.
     *
     *      @return The grouped address text.
     */
    QString groupByFourDigits(QString const& text) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Conflicted state. Item is conflicted if it overlaps with other items.
    bool conflicted_;

    //! Tells if the item is completely overlapped by other items.
    bool overlapped_;
};

#endif // MEMORYVISUALIZATIONITEM_H
