//-----------------------------------------------------------------------------
// File: fieldgapitem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 30.11.2012
//
// Description:
// The graphical item that represents a gap between fields.
//-----------------------------------------------------------------------------

#ifndef FIELDGAPITEM_H
#define FIELDGAPITEM_H

#include <editors/ComponentEditor/visualization/memorygapitem.h>

#include <QGraphicsItem>

class ExpressionParser;

//-----------------------------------------------------------------------------
//! The graphical item that represents a gap between fields.
//-----------------------------------------------------------------------------
class FieldGapItem : public MemoryGapItem
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
     *      @param [in] name    Name to display for the gap item.
	 *      @param [in] parent  The parent graphics item.
	 */
	FieldGapItem(QString const& name, QSharedPointer<ExpressionParser> expressionParser, QGraphicsItem* parent);

	//! The destructor.
	virtual ~FieldGapItem() = default;

    //! No copying.
    FieldGapItem(const FieldGapItem& other) = delete;

    //! No assignment.
    FieldGapItem& operator=(const FieldGapItem& other) = delete;

	//! Refresh the gap item.
	virtual void updateDisplay() override final;

	/*!
     *  Get the bit width of the item.
	 * 
	 *      @return The bit width of the item.
	 */
	virtual int getBitWidth() const override final;

	/*!
     *  Get the width of the item.
	 *
	 *      @return The width of the gap item.
	 */
	virtual qreal itemTotalWidth() const override final;

    //! Set the item into conflicted (overlapping memory) state.
    virtual void setConflicted(bool conflicted) override final;
};

#endif // FIELDGAPITEM_H
