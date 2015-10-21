//-----------------------------------------------------------------------------
// File: memorygapitem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 20.11.2012
//
// Description:
// The graphical item that represents a gap in the memory between objects.
//-----------------------------------------------------------------------------

#ifndef MEMORYGAPITEM_H
#define MEMORYGAPITEM_H

#include "memoryvisualizationitem.h"

class ExpressionParser;

//-----------------------------------------------------------------------------
//! The graphical item that represents a gap in the memory between objects.
//-----------------------------------------------------------------------------
class MemoryGapItem : public MemoryVisualizationItem
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] parent  Pointer to the parent of the gap item.
	 */
	MemoryGapItem(QSharedPointer<ExpressionParser> expressionParser, QGraphicsItem* parent);
	
	//! The destructor.
	virtual ~MemoryGapItem();

	//! Refresh the gap item.
	virtual void refresh();

    //! Updates the labels and tooltip for the item.
    virtual void updateDisplay();

	/*!
     *  Get the offset of the item. 
	 *
	 *      @return int The offset of the item from the parent item's base address.
	 */
	virtual quint64 getOffset() const;

	/*!
     *  Get the last address contained in the gap.
	 *
	 *      @return The last address.
	 */
	virtual quint64 getLastAddress() const;

	/*!
     *  Get the bit width of the item.
	 * 
	 *      @return The bit width of the item.
	 */
	virtual int getBitWidth() const;

	/*!
     *  Get number of bits the addressable unit contains.
	 *
	 *      @return The size of least addressable unit.
	 */
	virtual unsigned int getAddressUnitSize() const;

	/*!
     *  Set start address for the gap.
	 *
	 *      @param [in] address     The address that limits the gap start.
	 */
	void setStartAddress(quint64 address);

	/*!
     *  Set end address for the gap.
	 *
	 *      @param [in] address     The address that limits the gap end.
	 */
	void setEndAddress(quint64 address);
    
protected:

    //! The start address of the gap.
    quint64 start_;

    //! The end address of the gap.
    quint64 end_;

private:
	
	//! No copying.
	MemoryGapItem(const MemoryGapItem& other);

	//! No assignment.
	MemoryGapItem& operator=(const MemoryGapItem& other);
};

#endif // MEMORYGAPITEM_H
