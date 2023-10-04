//-----------------------------------------------------------------------------
// File: segmentgraphitem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 20.12.2012
//
// Description:
// The graph item to visualize segments in address space editor.
//-----------------------------------------------------------------------------

#ifndef SEGMENTGRAPHITEM_H
#define SEGMENTGRAPHITEM_H

#include <editors/ComponentEditor/addressSpaces/addressSpaceVisualizer/addressspacevisualizationitem.h>

#include <QSharedPointer>

class ExpressionParser;
class Region;
//-----------------------------------------------------------------------------
//! The graph item to visualize segments in address space editor.
//-----------------------------------------------------------------------------
class SegmentGraphItem : public AddressSpaceVisualizationItem
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] segment             Pointer to the segment represented by this graph item.
	 *      @param [in] addressSpaceWidth   Width of the address space.
	 *      @param [in] expressionParser    Pointer to the expression parser.
	 *      @param [in] parent              Pointer to the parent graph item.
	 */
	SegmentGraphItem(QSharedPointer<Region> segment, QString const& addressSpaceWidth,
        QSharedPointer<ExpressionParser> expressionParser, QGraphicsItem* parent = 0);
	
	//! The destructor.
	virtual ~SegmentGraphItem() = default;

    //! No copying.
    SegmentGraphItem(const SegmentGraphItem& other) = delete;

    //! No assignment.
    SegmentGraphItem& operator=(const SegmentGraphItem& other) = delete;

	//! Refresh the item.
	virtual void refresh() override final;

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
	
	//! Pointer to the segment being visualized.
	QSharedPointer<Region> segment_;
};

#endif // SEGMENTGRAPHITEM_H
