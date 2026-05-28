//-----------------------------------------------------------------------------
// File: MemoryBlockGraphItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 02.03.2022
//
// Description:
// The graphical item that represents one memory block.
//-----------------------------------------------------------------------------

#ifndef MEMORYBLOCKGRAPHITEM_H
#define MEMORYBLOCKGRAPHITEM_H

#include "ArrayableMemoryGraphItem.h"

#include <QColor>

class ExpressionParser;

//-----------------------------------------------------------------------------
//! The graphical item that represents one memory block.
//-----------------------------------------------------------------------------
class MemoryBlockGraphItem : public ArrayableMemoryGraphItem
{
	Q_OBJECT

public:

	/*!
     *  The constructor
	 *
     *    @param [in] expressionParser    The used expression parser.
     *    @param [in] parent              The parent graph item.
	 */
    MemoryBlockGraphItem(QSharedPointer<ExpressionParser> expressionParser,
		QGraphicsItem *parent);
	
	/*!
     *  The destructor.
     */
    virtual ~MemoryBlockGraphItem() = default;

    //! No copying. No assignment.
    MemoryBlockGraphItem(const MemoryBlockGraphItem& other) = delete;
    MemoryBlockGraphItem& operator=(const MemoryBlockGraphItem& other) = delete;

    /*!
     *  Updates the labels and tooltip for the item.
     */
    virtual void updateDisplay() override;

	/*!
     *  Get the last address contained in the item.
	 *
	 *    @return The last address.
	 */
	virtual quint64 getLastAddress() const override final;

    /*!
     *  Sets the addressable unit size.
     *
     *    @param [in] addressableUnitBits   The number of bits per address unit.
     */
    void setAddressableUnitBits(int addressableUnitBits);

	/*!
     *  Get number of bits the addressable unit contains.
	 *
	 *    @return The size of least addressable unit.
	 */
	virtual unsigned int getAddressUnitSize() const override final;

private:

    /*!
     *  Get the name of the memory block.
     *
     *    @return Name of the memory block.
     */
    virtual QString getName() const = 0;

    /*!
     *  Get the range of the memory block.
     *
     *    @return Range of the memory block.
     */
    virtual quint64 getRange() const = 0;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The number of bits in an address unit.
    int addressableUnitBits_;
};

#endif // MEMORYBLOCKGRAPHITEM_H
