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

#include <editors/ComponentEditor/visualization/memoryvisualizationitem.h>

#include <IPXACTmodels/Component/Register.h>

#include <QSharedPointer>

class ExpressionParser;

//-----------------------------------------------------------------------------
//! The graphical item that represents one register.
//-----------------------------------------------------------------------------
class RegisterGraphItem : public MemoryVisualizationItem
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] reg                 The register that this graph item visualizes.
     *      @param [in] expressionParser    Parser for expressions in register fields.
	 *      @param [in] parent              The parent of this graph item.
	 */
	RegisterGraphItem(QSharedPointer<Register> reg, QSharedPointer<ExpressionParser> expressionParser,
        QGraphicsItem* parent);
	
	//! The destructor.
	virtual ~RegisterGraphItem() = default;

    //! No copying
    RegisterGraphItem(const RegisterGraphItem& other) = delete;
    RegisterGraphItem& operator=(const RegisterGraphItem& other) = delete;

	//! Refresh the item, refresh and re-layout the sub-items and refresh parent item.
    virtual void refresh() override final;

    //! Updates the labels and tooltip for the item.
    virtual  void updateDisplay() override final;

    //! Add child item.
    virtual void addChild(MemoryVisualizationItem* childItem) override final;

	//! Remove the item.
    virtual void removeChild(MemoryVisualizationItem* childItem ) override final;

	/*!
     *  Get the offset of the item. 
	 *
	 *      @return The offset of the item from the parent item's base address.
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
     *  Get number of bits the addressable unit contains.
	 *
	 *      @return The size of least addressable unit.
	 */
	virtual unsigned int getAddressUnitSize() const override final;

    /*!
     *  Set the width for the item.
	 *
	 *       @param [in] width The new width of the item.
	 */
    virtual void setWidth(qreal width) override final;

    /*!
     *  Checks if the item is to be used in the visualization.
     *
     *      @return True, if the item should be used, otherwise false.
     */
    virtual bool isPresent() const override final;

    //! Re-layouts the child items.
     virtual void redoChildLayout() override final;

protected:

    //! Update the child items in the map. Field items are organized according to last address.
    virtual void updateChildMap() override final;

    /*!
     *  Repositions the child items and fills the empty gaps between them.
     */
    virtual void repositionChildren() override final;

private:
		
    /*!
     *  Gets the register size in AUB units.
     *
     *      @return The size of the register in AUBs.
     */
    quint64 getSizeInAUB() const;

    /*!
     *  Gets the MSB of the register.
     *
     *      @return The MSB of the register.
     */
    unsigned int getRegisterMSB() const;

    /*!
     * Finds the highest bit reserved by the register i.e. MSB or highest bit found in fields.
     *
     *     @return The highest bit index in the register.
     */
    unsigned int findHighestReservedBit();

    //! Removes current gaps between field items and re-sorts fields by offset.
    void removeGapsAndSortChildren();

    /*!
     * Fills the gaps between fields with items.
     *
     *     @param [in] highestReservedBit  The highest bit index reserved by the register.
     */
     void fillGapsBetweenChildren(unsigned int highestReservedBit);

    /*!
     *  Checks if there is empty memory space between the given child and the last known used bit index.
     *
     *      @param [in] current             The currently iterated child.
     *      @param [in] lastBitIndexInUse   The last known used bit index.
     *
     *      @return True, if there is empty space, otherwise false.
     */
    bool emptySpaceBeforeChild(MemoryVisualizationItem const* current, quint64 lastBitIndexInUse) const;

    /*!
     *  Creates a new child for representing a free memory slot.
     *
     *      @param [in] startAddress    The offset of the free memory slot.
     *      @param [in] lastAddress     The last address of the free memory slot.
     */
    QMap<quint64, MemoryVisualizationItem*>::iterator addMemoryGap(quint64 startAddress, quint64 endAddress);

    //! Mark all invalid fields outside register boundaries.
    void markConflictingChildren();

    /*!
     *  Checks if the two consecutive children overlap.
     *
     *      @param [in] current     The currently iterated child.
     *      @param [in] previous    The previously iterated child.
     *
     *      @return True, if the children overlap, otherwise false.
     */
    bool childrenOverlap(MemoryVisualizationItem const* current, MemoryVisualizationItem const* previous);

    /*!
     * Resizes and repositions the given child item.
     *
     *     @param [in] child               The child item to adjust.
     *     @param [in] highestReservedBit  The highest bit index used by the register.
     *
     *     @return 
     */
     void resizeAndPositionChild(MemoryVisualizationItem* child, unsigned int highestReservedBit) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Pointer to the register being visualized.
	QSharedPointer<Register> register_;

};

#endif // REGISTERGRAPHITEM_H
