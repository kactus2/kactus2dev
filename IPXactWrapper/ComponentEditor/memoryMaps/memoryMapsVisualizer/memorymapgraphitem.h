/* 
 *  	Created on: 17.4.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapgraphitem.h
 *		Project: Kactus 2
 */

#ifndef MEMORYMAPGRAPHITEM_H
#define MEMORYMAPGRAPHITEM_H

#include <IPXactWrapper/ComponentEditor/visualization/memoryvisualizationitem.h>
#include <models/memorymap.h>

#include <QSharedPointer>
#include <QGraphicsItem>

/*! \brief The graphical item that represents one memory map.
 *
 */
class MemoryMapGraphItem : public MemoryVisualizationItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param memoryMap Pointer to the memory map to visualize
	 * \param parent Pointer to the owner of this graphics item.
	 *
	*/
	MemoryMapGraphItem(QSharedPointer<MemoryMap> memoryMap, QGraphicsItem* parent = 0);
	
	//! \brief The destructor
	virtual ~MemoryMapGraphItem();

	//! \brief Refresh the item and sub-items.
	virtual void refresh();

	/*! \brief Get the offset of the item. 
	 *
	 * \return int The offset of the item from the parent item's base address.
	*/
	virtual quint64 getOffset() const;

	/*! \brief Get the last address contained in the item.
	 *
	 * \return The last address.
	*/
	virtual quint64 getLastAddress() const;

	/*! \brief Get the bit width of the item.
	 * 
	 * \return The bit width of the item.
	*/
	virtual int getBitWidth() const;

	/*! \brief Get number of bits the addressable unit contains.
	 *
	 * \return The size of least addressable unit.
	*/
	virtual unsigned int getAddressUnitSize() const;

private:
	
	//! \brief No copying
	MemoryMapGraphItem(const MemoryMapGraphItem& other);

	//! \brief No assignment
	MemoryMapGraphItem& operator=(const MemoryMapGraphItem& other);

	//! \brief Pointer to the memory map that is shown
	QSharedPointer<MemoryMap> memoryMap_;
};

#endif // MEMORYMAPGRAPHITEM_H
