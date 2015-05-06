//-----------------------------------------------------------------------------
// File: fieldgraphitem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 25.4.2012
//
// Description:
// The graphical item that represents a bit field within a register.
//-----------------------------------------------------------------------------

#ifndef FIELDGRAPHITEM_H
#define FIELDGRAPHITEM_H

#include <editors/ComponentEditor/visualization/memoryvisualizationitem.h>
#include <IPXACTmodels/field.h>

#include <QSharedPointer>

class ExpressionParser;
//-----------------------------------------------------------------------------
//! The graphical item that represents a bit field within a register.
//-----------------------------------------------------------------------------
class FieldGraphItem : public MemoryVisualizationItem
{
	Q_OBJECT

public:

	/*! The constructor.
	 *
	 *      @param [in] field               The field to visualize.
     *      @param [in] expressionParser    Parser for expressions in fields.
	 *      @param [in] parent              The parent graph item.
	 *
	*/
	FieldGraphItem(QSharedPointer<Field> field, QSharedPointer<ExpressionParser> expressionParser,
        QGraphicsItem* parent);

	//! The destructor
	virtual ~FieldGraphItem();

    //! Refresh the item and the parent item.
	virtual void refresh();

    //! Updates the labels and tooltip for the item.
    virtual void updateDisplay();

	/*! Get the offset of the item. 
	 *
	 *      @return int The offset of the item from the parent item's base address.
	*/
	virtual quint64 getOffset() const;

	/*! Get the last bit contained in the field.
	 *
	 *      @return The last bit.
	*/
	virtual quint64 getLastAddress() const;

	/*! Get the bit width of the item.
	 * 
	 *      @return The bit width of the item.
	*/
	virtual int getBitWidth() const;

	/*! Get number of bits the addressable unit contains.
	 *
	 *      @return The size of least addressable unit.
	*/
	virtual unsigned int getAddressUnitSize() const;
 
	/*! Set the width for the item.
	 *
	 *      @param [in] width The new width of the item.
	 *
	*/
    void setWidth(qreal width);

	/*! Sets the first non-overlapping address to display.
	 *
	 *      @param [in] The first address to set.
	*/
    virtual void setDisplayOffset(quint64 const& address);


    /*! Sets the last non-overlapping address to display.
    *
    *      @param [in] The last address to set.
    */
    virtual void setDisplayLastAddress(quint64 const& address);
    
    /*!
     *  Checks if the item is to be used in the visualization.
     *
     *      @return True, if the item should be used, otherwise false.
     */
    virtual bool isPresent() const;

protected:
     
    //! Set the item into conflicted (overlapping memory) state.
     virtual void setConflicted(bool conflicted);

private:

	//! No copying
	FieldGraphItem(const FieldGraphItem& other);
	FieldGraphItem& operator=(const FieldGraphItem& other);

	//! The field being visualized.
	QSharedPointer<Field> field_;

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;
};

#endif // FIELDGRAPHITEM_H
