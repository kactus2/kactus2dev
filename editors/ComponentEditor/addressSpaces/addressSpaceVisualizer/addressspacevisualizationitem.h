//-----------------------------------------------------------------------------
// File: addressspacevisualizationitem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 20.12.2012
//
// Description:
// The base class to visualize objects in address space editor.
//-----------------------------------------------------------------------------

#ifndef ADDRESSSPACEVISUALIZATIONITEM_H
#define ADDRESSSPACEVISUALIZATIONITEM_H

#include <common/graphicsItems/visualizeritem.h>
#include <IPXACTmodels/addressspace.h>

#include <QSharedPointer>
#include <QBrush>

class ExpressionParser;

//-----------------------------------------------------------------------------
//! The base class to visualize objects in address space editor.
//-----------------------------------------------------------------------------
class AddressSpaceVisualizationItem : public VisualizerItem
{
	Q_OBJECT

public:

    //! The minimum and maximum bounds for address space items.
	enum Bounds {
        PEN_WIDTH = 2,
        MIN_HEIGHT = VisualizerItem::DEFAULT_HEIGHT/2 + PEN_WIDTH, //! Height for items with one address.
        SEGMENT_HEIGHT = 2*MIN_HEIGHT + PEN_WIDTH                  //! Height for items with address range.
	};

	/*! The constructor
	 *
	 *      @param [in] addrSpace           The address space being visualized.
   	 *      @param [in] expressionParser    The expression parser to use.
	 *      @param [in] parent              The parent graphics item.
	 *
	*/
	AddressSpaceVisualizationItem(QString const& addressSpaceWidth,
        QSharedPointer<ExpressionParser> expressionParser,
		QGraphicsItem* parent = 0);
	
	//! The destructor
	virtual ~AddressSpaceVisualizationItem();

	//! Refresh the item.
	virtual void refresh() = 0;

	/*! Get the offset of the item. 
	 *
	 *      @return int The offset of the item.
	*/
	virtual quint64 getOffset() const = 0;

	/*! Get the last address contained in the item.
	 *
	 *      @return The last address.
	*/
	virtual quint64 getLastAddress() const = 0;

	/*! Get the bit width of the address space.
	 * 
	 *      @return The bit width of the address space.
	*/
	virtual int getBitWidth() const;

	/*! Set the coordinate that is the bottom y-coordinate for the item.
	 *
	 *      @param [in] yCoordinate The y-coordinate of the bottom.
	 *
	*/
	virtual void setBottomCoordinate(qreal yCoordinate);

	/*! Set the height for the item.
	 *
	 *      @param [in] height The height to set.
	 *
	*/
	virtual void setHeight(qreal height);

	/*! Sets the first non-overlapping address to display.
	 *
	 *      @param [in] The first address to set.
	*/
    virtual void setOverlappingTop(quint64 const& address);

	/*! Get the first non-overlapping address of the item.
	 *
	 *      @return The first non-overlapping address or -1 if completely overlapped.
	 */
    virtual quint64 getOverlappingTop();

	/*! Sets the last non-overlapping address to display.
	 *
	 *      @param [in] The last address to set.
	 */
    virtual void setOverlappingBottom(quint64 const& address);

    /*! Get the last non-overlapping address of the item.
     *
     *      @return The last non-overlapping address or -1 if completely overlapped.
     */
    virtual quint64 getOverlappingBottom();

    //! Tells the item that its memory block is shadowed by other memory blocks.
    virtual void setCompleteOverlap();
    
    /*! Tells if the item is completely overlapped by other items.
     *
     *      @return True, if the item's memory is completely under other memory blocks, otherwise false.
     */
    virtual bool isCompletelyOverlapped() const;

    /*!
     *  Sets the item to conflicted (overlaps with other memory blocks) or not-conflicted state.
     *
     *      @param [in] conflicted   Conflicted state to set.
     */
    virtual void setConflicted(bool conflicted);

    /*!
     *  Sets the item to conflicted or not-conflicted state.
     *
     *      @return True, if item is conflicted, otherwise false.
     */
    virtual bool isConflicted() const;

	/*! Convert the address to string presented in hexadecimal form.
	 *
	 *      @param [in] address  The address to be converted.
	 *      @param [in] bitWidth The bit width of the item.
	 *
	 *      @return QString containing the address in hexadecimal form.
	*/
    static QString addr2Str(quint64 const address, int const bitWidth);

protected:

	/*! Set the address to be shown on the top left corner.
	 *
	 *      @param [in] address The address to be shown in hexadecimal form.
	 *
	*/
	virtual void setLeftTopCorner(quint64 address);

	/*! Set the address to be shown on the top left corner.
	 *
	 *      @param [in] address The address to be shown in hexadecimal form.
	 *
	*/
	virtual void setRightTopCorner(quint64 address);

	/*! Set the address to be shown on the top left corner.
	 *
	 *      @param [in] address The address to be shown in hexadecimal form.
	 *
	*/
	virtual void setLeftBottomCorner(quint64 address);

	/*! Set the address to be shown on the top left corner.
	 *
	 *      @param [in] address The address to be shown in hexadecimal form.
	 *
	*/
	virtual void setRightBottomCorner(quint64 address);

    //! Get the expression parser for solving expression values.
    QSharedPointer<ExpressionParser> getExpressionParser() const;

    //! The first non-overlapping address.
    quint64 firstFreeAddress_;

    //! The last non-overlapping address.
    quint64 lastFreeAddress_;

private:
	
	//! No copying
	AddressSpaceVisualizationItem(const AddressSpaceVisualizationItem& other);
	AddressSpaceVisualizationItem& operator=(const AddressSpaceVisualizationItem& other);

    //! The bit width of the address space.
	QString addressSpaceWidth_;

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;

	//! Conflicted state. Item is conflicted if it overlaps with other items.
    bool conflicted_;

    //! Tells if the item is completely overlapped by other items.
    bool overlapped_;
};

#endif // ADDRESSSPACEVISUALIZATIONITEM_H
