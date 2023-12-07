//-----------------------------------------------------------------------------
// File: memoryvisualizationitem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
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
class ExpressionParser;
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

	/*!
     *  The constructor.
	 *
     *      @param [in] expressionParser    The used expression parser.
	 *      @param [in] parent              The owner of the item.
	 */
	MemoryVisualizationItem(QSharedPointer<ExpressionParser> expressionParser, QGraphicsItem* parent = 0);
	
   
    //! The destructor.
	virtual ~MemoryVisualizationItem() = default;
    
    virtual  QRectF itemTotalRect() const override final;

    //! No copying.
    MemoryVisualizationItem(const MemoryVisualizationItem& other) = delete;
    MemoryVisualizationItem& operator=(const MemoryVisualizationItem& other) = delete;

    //! Updates the labels and tooltip for the item.
    virtual void updateDisplay() = 0;

	/*!
     *  Get the offset of the item. 
	 *
	 *      @return The offset of the item from the parent item's base address.
	 */
	virtual quint64 getOffset() const = 0;

	/*!
     *  Get the last address contained in the item.
	 *
	 *      @return The last address.
	 */
	virtual quint64 getLastAddress() const = 0;

	/*!
     *  Get the bit width of the item.
	 * 
	 *      @return The bit width of the item.
	 */
	virtual int getBitWidth() const = 0;

	/*!
     *  Get number of bits the addressable unit contains.
	 *
	 *      @return The size of least addressable unit.
	 */
	virtual unsigned int getAddressUnitSize() const = 0;

    //! Re-layouts the child items.
    virtual void redoChildLayout();

	/*!
     *  Add a child visualization item for this item.
	 *
	 *       @param [in] childItem Pointer to the child to add.
	 */
	virtual void addChild(MemoryVisualizationItem* childItem);

    /*!
     *  Remove a child visualization item from this item.
	 *
	 *       @param [in] childItem Pointer to the child to remove.
	 */
	virtual void removeChild(MemoryVisualizationItem* childItem);

    int getChildCount() const;

    /*!
     *  Set the width for the item.
	 *
	 *       @param [in] width The new width of the item.
	 */
	virtual void setWidth(qreal width);

	/*!
     *  The bounding rect of the item.
	 *
	 *      @return The rectangle that bounds the item and possible sub items.
	 */
	virtual QRectF boundingRect() const;


signals:

	//! Emitted when this item's editor should be displayed.
	void selectEditor();

protected slots:

    
    /*!
     *  Repositions the child items within this item.
     */
    virtual void repositionChildren();

protected:
   
    /*!
     *  Parse a given expression.
     *
     *      @param [in] expression  The expression to be parsed.
     */
    quint64 parseExpression(QString const& expression) const;

    /*!
     *  Get the used expression parser.
     *
     *      @return Pointer to the expression parser.
     */
    QSharedPointer<ExpressionParser> getExpressionParser() const;

    //! Shows the expand/collapse icon if the item has any children. Otherwise the icon is hidden.
    void showExpandIconIfHasChildren();

    //! Update the offsets of the child items in the map and fills the empty gaps between them.
    void updateChildMap();

    //! Removes current gaps between child items and re-sorts items by offset.
    virtual void removeGapsAndSortChildren();

    /*!
     * Fills the gaps between child items with gap items.
     */
    virtual void fillGapsBetweenChildren();

    //! Mark all invalid children outside item boundaries.
    virtual void markConflictingChildren();

	//! Handler for mouse press events
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);

	//! Contains the child memory items. The offset of the child is the key.
	QMultiMap<quint64, MemoryVisualizationItem*> childItems_;

private:
    
    /*!
     *  Checks if there is empty memory space between the given child and the last known used address.
     *
     *      @param [in] current             The currently iterated child.
     *      @param [in] lastAddressInUse    The last known used address.
     *
     *      @return True, if there is empty space, otherwise false.
     */
    bool emptySpaceBeforeChild(MemoryVisualizationItem const* current, quint64 lastAddressInUse) const;

    /*!
     *	Mark child and its preceding children conflicting, if overlapping.
     *  
     *      @param [in] child     The child to check.   
     */
    void markChildIfConflicting(MemoryVisualizationItem* child);

    /*!
     *  Creates a new child for representing a free memory slot.
     *
     *      @param [in] offset          The offset of the free memory slot.
     *      @param [in] lastAddress     The last address of the free memory slot.
     *
     *      @return The created child item.
     */
    QMultiMap<quint64, MemoryVisualizationItem*>::iterator createMemoryGap(quint64 offset, quint64 lastAddress);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Width for child items.
    qreal childWidth_ = VisualizerItem::DEFAULT_WIDTH;

    //! The used expression parser.
    QSharedPointer<ExpressionParser> expressionParser_;

};

#endif // MEMORYVISUALIZATIONITEM_H
