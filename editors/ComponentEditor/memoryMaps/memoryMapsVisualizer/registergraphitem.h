//-----------------------------------------------------------------------------
// File: registergraphitem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 24.04.2012
//
// Description:
// The graphical item that represents one register.
//-----------------------------------------------------------------------------

#ifndef REGISTERGRAPHITEM_H
#define REGISTERGRAPHITEM_H

#include "ArrayableMemoryGraphItem.h"

#include <IPXACTmodels/Component/Register.h>

#include <QSharedPointer>

class ExpressionParser;

//-----------------------------------------------------------------------------
//! The graphical item that represents one register.
//-----------------------------------------------------------------------------
class RegisterGraphItem : public ArrayableMemoryGraphItem
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *    @param [in] reg                 The register that this graph item visualizes.
     *    @param [in] expressionParser    Parser for expressions in register fields.
	 *    @param [in] parent              The parent of this graph item.
	 */
	RegisterGraphItem(QSharedPointer<Register> reg, QSharedPointer<ExpressionParser> expressionParser,
        QGraphicsItem* parent);
	
	//! The destructor.
	virtual ~RegisterGraphItem() = default;

    //! No copying
    RegisterGraphItem(const RegisterGraphItem& other) = delete;
    RegisterGraphItem& operator=(const RegisterGraphItem& other) = delete;

    //! Updates the labels and tooltip for the item.
    virtual  void updateDisplay() override final;

    //! Add child item.
    virtual void addChild(MemoryVisualizationItem* childItem) override final;

	//! Remove the item.
    virtual void removeChild(MemoryVisualizationItem* childItem ) override final;

	/*!
     *  Get the last address contained in the item.
	 *
	 *    @return The last address.
	 */
	virtual quint64 getLastAddress() const override final;

	/*!
     *  Get the bit width of the item.
	 * 
	 *    @return The bit width of the item.
	 */
	virtual int getBitWidth() const override final;

	/*!
     *  Get number of bits the addressable unit contains.
	 *
	 *    @return The size of least addressable unit.
	 */
	virtual unsigned int getAddressUnitSize() const override final;

    /*!
     *  Set the width for the item.
	 *
	 *    @param [in] width The new width of the item.
	 */
    virtual void setWidth(qreal width) override final;

    /*!
     *  Checks if the item is to be used in the visualization.
     *
     *    @return True, if the item should be used, otherwise false.
     */
    virtual bool isPresent() const override final;


protected:

    /*!
     *  Repositions the child items and fills the empty gaps between them.
     */
    virtual void repositionChildren() override final;

    //! Removes current gaps between field items and re-sorts fields by offset.
    virtual void removeGapsAndSortChildren() override final;

    /*!
     * Fills the gaps between fields with gap items.
     */
    virtual void fillGapsBetweenChildren() override final;

    //! Mark all invalid fields outside register boundaries.
    virtual void markConflictingChildren() override final;

private:
		
    /*!
     *  Gets the register size in AUB units.
     *
     *    @return The size of the register in AUBs.
     */
    quint64 getSizeInAUB() const;

    /*!
     *  Gets the MSB of the register.
     *
     *    @return The MSB of the register.
     */
    unsigned int getRegisterMSB() const;

    /*!
     * Finds the highest bit reserved by the register i.e. MSB or highest bit found in fields.
     *
     *    @return The highest bit index in the register.
     */
    unsigned int findHighestReservedBit() const;


    /*!
     *  Checks if there is empty memory space between the given child and the last known used bit index.
     *
     *    @param [in] current             The currently iterated child.
     *    @param [in] lastBitIndexInUse   The last known used bit index.
     *
     *    @return True, if there is empty space, otherwise false.
     */
    bool emptySpaceBeforeChild(MemoryVisualizationItem const* current, quint64 lastBitIndexInUse) const;

    /*!
     *  Creates a new child for representing a free memory slot.
     *
     *    @param [in] startAddress    The offset of the free memory slot.
     *    @param [in] lastAddress     The last address of the free memory slot.
     */
    QMultiMap<quint64, MemoryVisualizationItem*>::iterator addMemoryGap(quint64 startAddress, quint64 endAddress);

    /*!
     *  Creates a new child that represents a gap in memory, when the gap is outside the register.
     *
     *    @param [in] startAddress    The offset of the memory gap.
     *    @param [in] lastAddress     The last address of the memory gap.
     */
    QMultiMap<quint64, MemoryVisualizationItem*>::iterator addOutOfBoundsMemoryGap(quint64 startAddress, quint64 endAddress);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Pointer to the register being visualized.
	QSharedPointer<Register> register_;
};

#endif // REGISTERGRAPHITEM_H
