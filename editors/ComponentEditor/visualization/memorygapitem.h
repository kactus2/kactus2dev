/* 
 *  	Created on: 20.11.2012
 *      Author: Antti Kamppi
 * 		filename: memorygapitem.h
 *		Project: Kactus 2
 */

#ifndef MEMORYGAPITEM_H
#define MEMORYGAPITEM_H

#include "memoryvisualizationitem.h"

/*! \brief The graphical item that represents a gap in the memory between objects.
 *
 */
class MemoryGapItem : public MemoryVisualizationItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the parent of the gap item.
	 *
	*/
	MemoryGapItem(QGraphicsItem* parent);
	
	//! \brief The destructor
	virtual ~MemoryGapItem();

	//! \brief Refresh the gap item.
	virtual void refresh();

	/*! \brief Get the offset of the item. 
	 *
	 * \return int The offset of the item from the parent item's base address.
	*/
	virtual quint64 getOffset() const;

	/*! \brief Get the last address contained in the gap.
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

	/*! \brief Set start address for the gap.
	 *
	 * \param address The address that limits the gap start.
	 * \param contains If true then the address is contained in the gap, 
	 * otherwise the gap starts from next address.
	 *
	*/
	void setStartAddress(quint64 address, bool contains = true);

	/*! \brief Set end address for the gap.
	 *
	 * \param address The address that limits the gap end.
	 * \param contains If true then the address is contained in the gap,
	 * otherwise the gap ends before the specified address.
	 *
	*/
	void setEndAddress(quint64 address, bool contains = true);

protected:

	//! \brief The start address of the gap.
	quint64 start_;

	//! \brief The end address of the gap.
	quint64 end_;

private:
	
	//! \brief No copying
	MemoryGapItem(const MemoryGapItem& other);

	//! \brief No assignment
	MemoryGapItem& operator=(const MemoryGapItem& other);
   
};

#endif // MEMORYGAPITEM_H
