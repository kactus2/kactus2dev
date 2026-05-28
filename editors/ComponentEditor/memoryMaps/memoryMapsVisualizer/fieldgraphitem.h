//-----------------------------------------------------------------------------
// File: fieldgraphitem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 25.4.2012
//
// Description:
// The graphical item that represents a bit field within a register.
//-----------------------------------------------------------------------------

#ifndef FIELDGRAPHITEM_H
#define FIELDGRAPHITEM_H

#include "ArrayableMemoryGraphItem.h"

#include <IPXACTmodels/Component/Field.h>

#include <QSharedPointer>

class ExpressionParser;
//-----------------------------------------------------------------------------
//! The graphical item that represents a bit field within a register.
//-----------------------------------------------------------------------------
class FieldGraphItem : public ArrayableMemoryGraphItem
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *    @param [in] field               The field to visualize.
     *    @param [in] expressionParser    Parser for expressions in fields.
	 *    @param [in] parent              The parent graph item.
	 */
	FieldGraphItem(QSharedPointer<Field> field, QSharedPointer<ExpressionParser> expressionParser,
        QGraphicsItem* parent);

	//! The destructor
	virtual ~FieldGraphItem() = default;

    //! No copying
    FieldGraphItem(const FieldGraphItem& other) = delete;
    FieldGraphItem& operator=(const FieldGraphItem& other) = delete;

    //! Updates the labels and tooltip for the item.
    virtual void updateDisplay() override final;

	/*!
     *  Get the last bit contained in the field.
	 *
	 *    @return The last bit.
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
     *  Sets the first non-overlapping address to display.
	 *
	 *    @param [in] The first address to set.
	 */
    virtual void setDisplayOffset(quint64 const& address) override final;


    /*!
     *  Sets the last non-overlapping address to display.
     *
     *    @param [in] The last address to set.
     */
    virtual void setDisplayLastAddress(quint64 const& address) override final;
    
    /*!
     *  Checks if the item is to be used in the visualization.
     *
     *    @return True, if the item should be used, otherwise false.
     */
    virtual bool isPresent() const override final;

    //! Re-layouts the child items.
    virtual void redoChildLayout() override;

protected:
     
    //! Set the item into conflicted (overlapping memory) state.
     virtual void setConflicted(bool conflicted) final;

private:

	//! The field being visualized.
	QSharedPointer<Field> field_;
};

#endif // FIELDGRAPHITEM_H
