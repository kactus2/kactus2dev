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
#include <QPointF>

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

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the parent graphics item.
	 *
	*/
	MemoryVisualizerItem(QGraphicsItem* parent = 0);
	
	//! \brief The destructor
	virtual ~MemoryVisualizerItem();

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

	/*! \brief Set new positions for child items.
	 *
	*/
	virtual void reorganizeChildren() = 0;

private:
	
	//! \brief No copying
	MemoryVisualizerItem(const MemoryVisualizerItem& other);

	//! \brief No assignment
	MemoryVisualizerItem& operator=(const MemoryVisualizerItem& other);

	//! \brief The label to display the name of the memory map.
	QGraphicsTextItem nameLabel_;
};

#endif // MEMORYVISUALIZERITEM_H
