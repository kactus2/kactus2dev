/* 
 *  	Created on: 15.10.2012
 *      Author: Antti Kamppi
 * 		filename: memoryvisualizationitem.h
 *		Project: Kactus 2
 */

#ifndef MEMORYVISUALIZATIONITEM_H
#define MEMORYVISUALIZATIONITEM_H

#include <common/graphicsItems/expandableitem.h>
#include <common/graphicsItems/graphicsexpandcollapseitem.h>

#include <QMultiMap>
#include <QGraphicsSceneMouseEvent>

/*! \brief The base class for graphics items to visualize memory objects.
 *
 */
class MemoryVisualizationItem : public ExpandableItem {
	Q_OBJECT

public:

	//! \brief The indentation of each child under its parent.
	enum Indentation {
		CHILD_INDENTATION = GraphicsExpandCollapseItem::SIDE
	};

	/*! \brief The constructor.
	 *
	 * \param parent Pointer to the owner of the item.
	 *
	*/
	MemoryVisualizationItem(QGraphicsItem* parent = 0);
	
	//! \brief The destructor
	virtual ~MemoryVisualizationItem();

	//! \brief Set the item brush and store it as original.
    void setBrush(const QBrush& brush);



	//! \brief Refresh the item and sub-items.
	virtual void refresh() = 0;

	/*! \brief Get the offset of the item. 
	 *
	 * \return int The offset of the item from the parent item's base address.
	*/
	virtual quint64 getOffset() const = 0;

	/*! \brief Get the last address contained in the item.
	 *
	 * \return The last address.
	*/
	virtual quint64 getLastAddress() const = 0;

	/*! \brief Get the bit width of the item.
	 * 
	 * \return The bit width of the item.
	*/
	virtual int getBitWidth() const = 0;

	/*! \brief Get number of bits the addressable unit contains.
	 *
	 * \return The size of least addressable unit.
	*/
	virtual unsigned int getAddressUnitSize() const = 0;

	/*! \brief Add a child visualization item for this item.
	 *
	 * \param childItem Pointer to the child to add.
	 *
	*/
	virtual void addChild(MemoryVisualizationItem* childItem);

	/*! \brief Remove a child visualization item from this item.
	 *
	 * \param childItem Pointer to the child to remove.
	 *
	*/
	virtual void removeChild(MemoryVisualizationItem* childItem);

	/*! \brief Set the width for the item.
	 *
	 * \param width The new width of the item.
	 *
	*/
	virtual void setWidth(qreal width);

	/*! \brief The bounding rect of the item.
	 *
	 * \return The rectangle that bounds the item and possible sub items.
	*/
	virtual QRectF boundingRect() const;

	/*! \brief Sets the first non-overlapping address to display.
	 *
	 * \param The first address to set.
	*/
    virtual void setOverlappingTop(quint64 const& address);

    virtual quint64 getOverlappingTop();

	/*! \brief Sets the last non-overlapping address to display.
	 *
	 * \param The last address to set.
	*/
    virtual void setOverlappingBottom(quint64 const& address);

    virtual quint64 getOverlappingBottom();

	/*! \brief Sets the item to be completely overlapped by adjacent items.
	*/
    virtual void setCompleteOverlap();

    //! \brief Set the item into conflicted (overlapping memory) state.
    virtual void setConflicted();

    //! \brief Remove the item from conflicted state.
    virtual void setNotConflicted();


    qreal getChildWidth() const;

    qreal itemTotalWidth() const;


signals:

	//! \brief Emitted when the item changes.
	void contentChanged();

	//! \brief Emitted when this item's editor should be displayed.
	void selectEditor();

protected:

	//! \brief Handler for mouse press events
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);

	/*! \brief Set new positions for child items.
	 * 
	 * The child items are organized in the order of their offset.
	*/
	virtual void reorganizeChildren();

	//! \brief Update the offsets of the child items in the map.
	void updateChildMap();

 static bool offsetLessThan(const MemoryVisualizationItem* s1, const MemoryVisualizationItem* s2);

	/*! \brief Set text to the top left corner.
	 *
	 * The function groups the hexadecimal digit into groups of 4 digits.
	 *
	 * \param text The text to display in the corner.
	 *
	*/
	virtual void setLeftTopCorner(const QString& text);

	/*! \brief Set the address to be shown on the top left corner.
	 *
	 * \param address The address to be shown in hexadecimal form.
	 *
	*/
	virtual void setLeftTopCorner(quint64 address);

	/*! \brief Set text to the bottom left corner.
	 *
	 * The function groups the hexadecimal digit into groups of 4 digits.
	 *
	 * \param text The text to display in the corner.
	 *
	*/
	virtual void setLeftBottomCorner(const QString& text);

	/*! \brief Set the address to be shown on the bottom left corner.
	 *
	 * \param address The address to be shown in hexadecimal form.
	 *
	*/
	virtual void setLeftBottomCorner(quint64 address);



	//! \brief Contains the child memory items. The offset of the child is the key.
	QMultiMap<quint64, MemoryVisualizationItem*> childItems_;

protected:
    
    //! \brief The first non-overlapping address.
    quint64 firstFreeAddress_;

    //! \brief The last non-overlapping address.
    quint64 lastFreeAddress_;

    //! \brief Width for child items.
    qreal childWidth_;

private:
	
	//! \brief No copying
	MemoryVisualizationItem(const MemoryVisualizationItem& other);

	//! \brief No assignment
	MemoryVisualizationItem& operator=(const MemoryVisualizationItem& other);
   
	//! \brief Original coloring for the item.
    QBrush originalBrush_;
};

#endif // MEMORYVISUALIZATIONITEM_H
