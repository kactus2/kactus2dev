//-----------------------------------------------------------------------------
// File: addressspacevisualizationitem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 20.12.2012
//
// Description:
// The base class to visualize objects in address space editor.
//-----------------------------------------------------------------------------

#ifndef ADDRESSSPACEVISUALIZATIONITEM_H
#define ADDRESSSPACEVISUALIZATIONITEM_H

#include <editors/ComponentEditor/visualization/AddressableItem.h>

#include <QSharedPointer>

class ExpressionParser;

//-----------------------------------------------------------------------------
//! The base class to visualize objects in address space editor.
//-----------------------------------------------------------------------------
class AddressSpaceVisualizationItem : public AddressableItem
{
	Q_OBJECT

public:

    //! The minimum and maximum bounds for address space items.
	enum Bounds
    {
        PEN_WIDTH = 2,
        MIN_HEIGHT = VisualizerItem::DEFAULT_HEIGHT/2 + PEN_WIDTH, //! Height for items with one address.
        SEGMENT_HEIGHT = 2*MIN_HEIGHT + PEN_WIDTH                  //! Height for items with address range.
	};

	/*!
     *  The constructor
	 *
	 *      @param [in] addrSpace           The address space being visualized.
   	 *      @param [in] expressionParser    The expression parser to use.
	 *      @param [in] parent              The parent graphics item.
	 */
	AddressSpaceVisualizationItem(QString const& addressSpaceWidth,
        QSharedPointer<ExpressionParser> expressionParser,
		QGraphicsItem* parent = nullptr);
	
	//! The destructor.
	virtual ~AddressSpaceVisualizationItem() = default;

    //! No copying. No assignment.
    AddressSpaceVisualizationItem(const AddressSpaceVisualizationItem& other) = delete;
    AddressSpaceVisualizationItem& operator=(const AddressSpaceVisualizationItem& other) = delete;

	/*!
     *  Get the bit width of the address space.
	 * 
	 *      @return The bit width of the address space.
	 */
	virtual int getBitWidth() const;

	/*!
     *  Set the coordinate that is the bottom y-coordinate for the item.
	 *
	 *      @param [in] yCoordinate The y-coordinate of the bottom.
	 */
	virtual void setBottomCoordinate(qreal yCoordinate);

	/*!
     *  Set the height for the item.
	 *
	 *      @param [in] height The height to set.
	 */
	virtual void setHeight(qreal height);

	/*!
     *  Sets the first non-overlapping address to display.
	 *
	 *      @param [in] The first address to set.
	 */
    virtual void setOverlappingTop(quint64 const& address);

	/*!
     *  Get the first non-overlapping address of the item.
	 *
	 *      @return The first non-overlapping address or -1 if completely overlapped.
	 */
    virtual quint64 getOverlappingTop();

	/*!
     *  Sets the last non-overlapping address to display.
	 *
	 *      @param [in] The last address to set.
	 */
    virtual void setOverlappingBottom(quint64 const& address);

    /*!
     *  Get the last non-overlapping address of the item.
     *
     *      @return The last non-overlapping address or -1 if completely overlapped.
     */
    virtual quint64 getOverlappingBottom();

    //! Tells the item that its memory block is shadowed by other memory blocks.
    virtual void setCompleteOverlap();
    
    /*!
     *  Tells if the item is completely overlapped by other items.
     *
     *      @return True, if the item's memory is completely under other memory blocks, otherwise false.
     */
    virtual bool isCompletelyOverlapped() const;

protected:

    //! Get the expression parser for solving expression values.
    QSharedPointer<ExpressionParser> getExpressionParser() const;

    //! The first non-overlapping address.
    quint64 firstFreeAddress_ = 0;

    //! The last non-overlapping address.
    quint64 lastFreeAddress_ = 0;

private:

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The bit width of the address space.
	QString addressSpaceWidth_;

    //! Tells if the item is completely overlapped by other items.
    bool overlapped_ = false;
};

#endif // ADDRESSSPACEVISUALIZATIONITEM_H
