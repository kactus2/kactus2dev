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

#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/MemoryBlockGraphItem.h>

#include <IPXACTmodels/Component/AddressBlock.h>

#include <QGraphicsItem>
#include <QSharedPointer>
#include <QColor>

class ExpressionParser;

//-----------------------------------------------------------------------------
//! The graphical item that represents one address block.
//-----------------------------------------------------------------------------
class AddressBlockGraphItem : public MemoryBlockGraphItem
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

    //! No copying. No assignment.
    AddressBlockGraphItem(const AddressBlockGraphItem& other) = delete;
    AddressBlockGraphItem& operator=(const AddressBlockGraphItem& other) = delete;

	/*!
     *  Get the offset of the item. 
	 *
	 *      @return int The offset of the item from the parent item's base address.
	 */
	virtual quint64 getOffset() const override final;

	/*!
     *  Get the bit width of the item.
	 * 
	 *      @return The bit width of the item.
	 */
	virtual int getBitWidth() const override final;
           
    /*!
     *  Checks if the item is to be used in the visualization.
     *
     *      @return True, if the item should be used, otherwise false.
     */
    virtual bool isPresent() const override final;

private:

    /*!
     *  Get the name of the memory block.
     *
     *      @return Name of the memory block.
     */
    virtual QString getName() const override final;

    /*!
     *  Get the range of the memory block.
     *
     *      @return Range of the memory block.
     */
    virtual quint64 getRange() const override final;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Pointer to the address block being displayed.
	QSharedPointer<AddressBlock> addrBlock_;
};

#endif // ADDRESSBLOCKGRAPHITEM_H
