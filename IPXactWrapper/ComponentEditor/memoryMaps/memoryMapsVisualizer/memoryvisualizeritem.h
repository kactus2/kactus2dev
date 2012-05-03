/* 
 *  	Created on: 17.4.2012
 *      Author: Antti Kamppi
 * 		filename: memoryvisualizeritem.h
 *		Project: Kactus 2
 */

#ifndef MEMORYVISUALIZERITEM_H
#define MEMORYVISUALIZERITEM_H

#include <QGraphicsRectItem>
#include <QObject>
#include <QGraphicsTextItem>
#include <QGraphicsSimpleTextItem>

/*! \brief The base class for graphics items for memory map visualization.
 *
 */
class MemoryVisualizerItem : public QObject, public QGraphicsRectItem {
	Q_OBJECT

public:

	//! \brief The maximum width of an item
	static const int MAX_ITEM_WIDTH = 200;

	//! \brief The height of an item
	static const int ITEM_HEIGHT = 20;
	
	//! \brief The minimum width of a memory item that represents only one bit.
	static const int MIN_BIT_WIDTH = 50;

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the parent graphics item.
	 * \param maxWidth The width of the item in bits.
	 *
	*/
	MemoryVisualizerItem(QGraphicsItem* parent = 0, 
		int maxWidth = 0);
	
	//! \brief The destructor
	virtual ~MemoryVisualizerItem();

	/*! \brief Get the rectangle reserved by this item and it's sub-items recursively.
	 *
	 * \return QRectF which contains the rectangles of this item and sub-items and their
	 * children recursively.
	*/
	virtual QRectF itemTotalRect() const;

	/*! \brief Get the width of the item.
	 *
	 * This width is affected by the item's children so if children grow this 
	 * width grows accordingly.
	 *
	 * \return The width of the item and it's sub-items.
	*/
	virtual qreal itemTotalWidth() const;

	/*! \brief Set the width for the item.
	 *
	 * \param width The new width of the item.
	 *
	*/
	virtual void setWidth(qreal width);

	/*! \brief Get the offset of the item. 
	 * 
	 * The default implementation returns always 0. 
	 * All sub classes must implement this function if they contain an offset value.
	 *
	 * \return int The offset of the item from the parent item's base address.
	*/
	virtual int getOffset() const;

	/*! \brief Get the width of the item.
	 * 
	 * The default implementation returns the maximum width.
	 *
	 * \return The width of the item.
	*/
	virtual int getWidth() const;

signals:

	//! \brief This signal is emitted when the item is changed.
	void contentChanged();

protected:

	/*! \brief Set the display name of the item.
	 *
	 * \param name The name to display.
	 *
	*/
	void setName(const QString& name);

	/*! \brief Set the text located in the left corner of the item.
	 *
	 * \param text The text to display.
	 *
	*/
	void setLeftCornerText(const QString& text);

	/*! \brief Set the text located in the right corner of the item.
	 *
	 * \param text The text to display.
	 *
	*/
	void setRightCornerText(const QString& text);

	/*! \brief Get the minimum rectangle of the item.
	 *
	 * \return QRectF that limits the minimum size needed by this item.
	*/
	virtual QRectF minimumRect() const;

	/*! \brief Set new positions for child items.
	 * 
	 * The base class implementation only updates the position of the name.
	*/
	virtual void reorganizeChildren();

	//! \brief The maximum width of the item in bits.
	int maxWidth_;

private:
	
	//! \brief No copying
	MemoryVisualizerItem(const MemoryVisualizerItem& other);

	//! \brief No assignment
	MemoryVisualizerItem& operator=(const MemoryVisualizerItem& other);

	//! \brief The label to display the name of the memory map.
	QGraphicsTextItem nameLabel_;

	//! \brief The label to display text in the left top corner.
	QGraphicsSimpleTextItem leftTopText_;

	//! \brief The label to display text in the right top corner.
	QGraphicsSimpleTextItem rightTopText_;
};

#endif // MEMORYVISUALIZERITEM_H
