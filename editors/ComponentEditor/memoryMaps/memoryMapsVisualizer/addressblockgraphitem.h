//-----------------------------------------------------------------------------
// File: addressblockgraphitem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 18.4.2012
//
// Description:
// The graphical item that represents one address block.
//-----------------------------------------------------------------------------

#ifndef ADDRESSBLOCKGRAPHITEM_H
#define ADDRESSBLOCKGRAPHITEM_H

#include <editors/ComponentEditor/visualization/memoryvisualizationitem.h>

#include <IPXACTmodels/Component/AddressBlock.h>

#include <QGraphicsItem>
#include <QSharedPointer>

class ExpressionParser;
//-----------------------------------------------------------------------------
//! The graphical item that represents one address block.
//-----------------------------------------------------------------------------
class AddressBlockGraphItem : public MemoryVisualizationItem
{
	Q_OBJECT

public:

	/*!
     *  The constructor
	 *
     *      @param [in] addrBlock           The address block to visualize.
     *      @param [in] expressionParser    The used expression parser.
     *      @param [in] parent              The parent graph item.
	 */
	AddressBlockGraphItem(QSharedPointer<AddressBlock> addrBlock, 
        QSharedPointer<ExpressionParser> expressionParser,
		QGraphicsItem *parent);
	
	//! The destructor
	virtual ~AddressBlockGraphItem() = default;

    //! No copying.
    AddressBlockGraphItem(const AddressBlockGraphItem& other) = delete;

    //! No assignment.
    AddressBlockGraphItem& operator=(const AddressBlockGraphItem& other) = delete;

    //! Refresh the item, re-layout the sub-items and refresh parent item.
	virtual void refresh() override final;

    //! Updates the labels and tooltip for the item.
    virtual void updateDisplay() override final;

	/*!
     *  Get the offset of the item. 
	 *
	 *      @return int The offset of the item from the parent item's base address.
	 */
	virtual quint64 getOffset() const override final;

	/*!
     *  Get the last address contained in the item.
	 *
	 *      @return The last address.
	 */
	virtual quint64 getLastAddress() const override final;

	/*!
     *  Get the bit width of the item.
	 * 
	 *      @return The bit width of the item.
	 */
	virtual int getBitWidth() const override final;

    /*!
     *  Sets the addressable unit size.
     *
     *      @param [in] addressableUnitBits   The number of bits per address unit.
     */
    void setAddressableUnitBits(int addressableUnitBits);

	/*!
     *  Get number of bits the addressable unit contains.
	 *
	 *      @return The size of least addressable unit.
	 */
	virtual unsigned int getAddressUnitSize() const override final;
           
    /*!
     *  Checks if the item is to be used in the visualization.
     *
     *      @return True, if the item should be used, otherwise false.
     */
    virtual bool isPresent() const override final;

private:
		
	//! Pointer to the address block being displayed.
	QSharedPointer<AddressBlock> addrBlock_;

    //! The number of bits in an address unit.
    int addrssableUnitBits_;
};

#endif // ADDRESSBLOCKGRAPHITEM_H
