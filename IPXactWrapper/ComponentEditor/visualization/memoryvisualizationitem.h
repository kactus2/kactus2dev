/* 
 *  	Created on: 15.10.2012
 *      Author: Antti Kamppi
 * 		filename: memoryvisualizationitem.h
 *		Project: Kactus 2
 */

#ifndef MEMORYVISUALIZATIONITEM_H
#define MEMORYVISUALIZATIONITEM_H

#include <common/graphicsItems/expandableitem.h>

#include <QMap>

/*! \brief The base class for graphics items to visualize memory objects.
 *
 */
class MemoryVisualizationItem : public ExpandableItem {
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * \param parent Pointer to the owner of the item.
	 *
	*/
	MemoryVisualizationItem(QGraphicsItem* parent = 0);
	
	//! \brief The destructor
	virtual ~MemoryVisualizationItem();

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

signals:

	//! \brief Emitted when the item changes.
	void contentChanged();

protected:

	/*! \brief Set new positions for child items.
	 * 
	 * The child items are organized in the order of their offset.
	*/
	virtual void reorganizeChildren();

	//! \brief Contains the child memory items. The offset of the child is the key.
	QMap<int, MemoryVisualizationItem*> childItems_;

private:
	
	//! \brief No copying
	MemoryVisualizationItem(const MemoryVisualizationItem& other);

	//! \brief No assignment
	MemoryVisualizationItem& operator=(const MemoryVisualizationItem& other);
};

#endif // MEMORYVISUALIZATIONITEM_H
