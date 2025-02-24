//-----------------------------------------------------------------------------
// File: memorygapitem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
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
	 *    @param [in] parent  Pointer to the parent of the gap item.
	 */
	MemoryGapItem(QSharedPointer<ExpressionParser> expressionParser, QGraphicsItem* parent);
	
	//! The destructor.
	virtual ~MemoryGapItem() = default;

    //! No copying.
    MemoryGapItem(const MemoryGapItem& other) = delete;

    //! No assignment.
    MemoryGapItem& operator=(const MemoryGapItem& other) = delete;

    //! Updates the labels and tooltip for the item.
    void updateDisplay() override;

	/*!
     *  Get the offset of the item. 
	 *
	 *    @return int The offset of the item from the parent item's base address.
	 */
	quint64 getOffset() const override;

	/*!
     *  Get the last address contained in the gap.
	 *
	 *    @return The last address.
	 */
	quint64 getLastAddress() const override;

	/*!
     *  Get the bit width of the item.
	 * 
	 *    @return The bit width of the item.
	 */
	int getBitWidth() const override;

	/*!
     *  Get number of bits the addressable unit contains.
	 *
	 *    @return The size of least addressable unit.
	 */
	unsigned int getAddressUnitSize() const override;

    //! Re-layouts the child items.
    void redoChildLayout() override final;
};

#endif // MEMORYGAPITEM_H
