//-----------------------------------------------------------------------------
// File: fieldgapitem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
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
     *      @param [in] parser  Pointer to the used expression parser.
	 *      @param [in] parent  Pointer to the parent graphics item.
	 *
	*/
	FieldGapItem(QSharedPointer<ExpressionParser> parser, QGraphicsItem* parent);
	
	/*!
     *  The constructor.
	 *
     *      @param [in] name    Name to display for the gap item.
	 *      @param [in] parent  Pointer to the parent graphics item.
	 */
	FieldGapItem(QString const& name, QSharedPointer<ExpressionParser> expressionParser, QGraphicsItem* parent);

	//! The destructor.
	virtual ~FieldGapItem();

	//! Refresh the gap item.
	virtual void refresh();

	/*!
     *  Get the bit width of the item.
	 * 
	 *      @return The bit width of the item.
	 */
	virtual int getBitWidth() const;

	/*!
     *  Get the width of the item.
	 *
	 *      @return The width of the gap item.
	 */
	virtual qreal itemTotalWidth() const;

    /*!
     *  Set lowest bit for the gap.
	 *
	 *      @param [in] address     The lowest bit that limits the gap.
	 */
	void setStartAddress(quint64 address);

	/*!
     *  Set highest bit for the gap.
	 *
	 *      @param [in] address     The highest bit that limits the gap end.
	 */
	void setEndAddress(quint64 address);

	/*!
     *  Sets the first non-overlapping address to display.
	 *
	 *      @param [in] address     The first address to set.
	 */
    virtual void setDisplayOffset(quint64 const& address);

	/*!
     *  Sets the last non-overlapping address to display.
	 *
	 *      @param [in] The last address to set.
	 */
    virtual void setDisplayLastAddress(quint64 const& address);

private:

	//! No copying.
	FieldGapItem(const FieldGapItem& other);

	//! No assignment.
	FieldGapItem& operator=(const FieldGapItem& other);
};

#endif // FIELDGAPITEM_H
