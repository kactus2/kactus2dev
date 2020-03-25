//-----------------------------------------------------------------------------
// File: addressspacegapitem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 04.01.2013
//
// Description:
// The graphical item that represents a gap in between address space objects.
//-----------------------------------------------------------------------------

#ifndef ADDRESSSPACEGAPITEM_H
#define ADDRESSSPACEGAPITEM_H

#include <editors/ComponentEditor/addressSpaces/addressSpaceVisualizer/addressspacevisualizationitem.h>

class ExpressionParser;

//-----------------------------------------------------------------------------
//! The graphical item that represents a gap in between address space objects.
//-----------------------------------------------------------------------------
class AddressSpaceGapItem : public AddressSpaceVisualizationItem
{
	Q_OBJECT

public:

	//! Define where the address limits of the gap are being drawn.
	enum AddressPosition
    {
		ALIGN_LEFT = 0,		// Align the address on the left side
		ALIGN_RIGHT			// Align the address on the right side
	};

	/*!
	 *  The constructor.
	 *
	 *      @param [in] addrPos             Position of the address space.
	 *      @param [in] addressSpaceWidth   Width of the address space.
	 *      @param [in] expressionParser    The used expression parser.
	 *      @param [in] parent              Pointer to the parent graph item.
	 */
	AddressSpaceGapItem(VisualizerItem::LabelLayout addrPos,
        QString const& addressSpaceWidth,
        QSharedPointer<ExpressionParser> expressionParser,
		QGraphicsItem* parent = 0);
	
	//! The destructor.
	virtual ~AddressSpaceGapItem() = default;

    //! No copying.
    AddressSpaceGapItem(const AddressSpaceGapItem& other) = delete;

    //! No assignment.
    AddressSpaceGapItem& operator=(const AddressSpaceGapItem& other) = delete;

	//! Refresh the item.
	virtual void refresh() override;

	/*!
     *  Get the offset of the item. 
	 *
	 *      @return int The offset of the item.
	 */
	virtual quint64 getOffset() const override final;

	/*!
     *  Get the last address contained in the item.
	 *
	 *      @return The last address.
	 */
	virtual quint64 getLastAddress() const override final;

	/*!
     *  Set start address for the gap.
	 *
	 *      @param [in] address     The address that limits the gap start.
	 *      @param [in] contains    If true then the address is contained in the gap, otherwise the gap starts
     *                              from next address.
	 */
	void setStartAddress(quint64 address, bool contains = true);

	/*!
     *  Set end address for the gap.
	 *
	 *      @param [in] address     The address that limits the gap end.
	 *      @param [in] contains    If true then the address is contained in the gap, otherwise the gap ends
     *                              before the specified address.
	 */
	void setEndAddress(quint64 address, bool contains = true);

    /*!
     *  Sets the first non-overlapping address to display.
	 *
	 *      @param [in] address     The first address to set.
	 */
    virtual void setOverlappingTop(quint64 const& address) override final;

	/*!
     *  Sets the last non-overlapping address to display.
	 *
	 *      @param [in] address     The last address to set.
	 */
    virtual void setOverlappingBottom(quint64 const& address) override final;

private:
		
	//! The start address of the gap.
	quint64 start_;

	//! The end address of the gap.
	quint64 end_;

	//! Defines where the address limits are aligned.
    VisualizerItem::LabelLayout addrPosition_;
};

#endif // ADDRESSSPACEGAPITEM_H
