//-----------------------------------------------------------------------------
// File: AddressableItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 11.10.2023
//
// Description:
// Common base class for address space and memory map visualization items.
//-----------------------------------------------------------------------------

#ifndef ADDRESSABLE_ITEM_H
#define ADDRESSABLE_ITEM_H

#include <common/graphicsItems/visualizeritem.h>

#include <QMultiMap>

//-----------------------------------------------------------------------------
//! Common base class for address space and memory map visualization items.
//-----------------------------------------------------------------------------
class AddressableItem : public VisualizerItem
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] parent              The owner of the item.
	 */
	AddressableItem(QGraphicsItem* parent = nullptr);
	
    //! The destructor.
	virtual ~AddressableItem() = default;
    
    //! No copying.
    AddressableItem(const AddressableItem& other) = delete;
    AddressableItem& operator=(const AddressableItem& other) = delete;

    //! Refresh the item.
	virtual void refresh() {};

	/*!
     *  Get the offset of the item. 
	 *
	 *      @return The offset of the item from the parent item's base address.
	 */
	virtual quint64 getOffset() const = 0;

	/*!
     *  Get the last address contained in the item.
	 *
	 *      @return The last address.
	 */
	virtual quint64 getLastAddress() const = 0;

	/*!
     *  Get the bit width of the item.
	 * 
	 *      @return The bit width of the item.
	 */
	virtual int getBitWidth() const = 0;

	/*!
     *  Get number of bits the addressable unit contains.
	 *
	 *      @return The size of least addressable unit.
	 */
	//virtual unsigned int getAddressUnitSize() const = 0;

	/*!
     *  Sets the first non-overlapping address to display.
	 *
	 *       @param [in] The first address to set.
	 */
    virtual void setDisplayOffset(quint64 const& address);

	/*!
     *  Get the first non-overlapping address of the item.
	 *
	 *      @return The first non-overlapping address.
	 */
    virtual quint64 getDisplayOffset();

	/*!
     *  Sets the last non-overlapping address to display.
	 *
	 *       @param [in] The last address to set.
	 */
    virtual void setDisplayLastAddress(quint64 const& address);

    /*!
     *  Get the last non-overlapping address of the item.
     *
     *      @return The last non-overlapping address.
     */
    virtual quint64 getDisplayLastAddress();

    //! Set the item into conflicted (overlapping memory) state.
    virtual void setConflicted(bool conflicted);

    /*!
     *  Checks if the item is conflicted (overlapping with others).
     *
     *      @return True, if the item is conflicted, otherwise false.
     */
    bool isConflicted() const;

protected:

	/*!
     *  Set the address to be shown on the top left corner.
	 *
	 *       @param [in] address The address to be shown in hexadecimal form.
	 */
	 void setTopLabelText(quint64 address);

	/*!
     *  Set the address to be shown on the bottom left corner.
	 *
	 *       @param [in] address The address to be shown in hexadecimal form.
	 */
	 void setBottomLabelText(quint64 address);

	/*!
	 *  Converts an address to hexadecimal string.
	 *
     *      @param [in] address   The address to convert.
     *      @param [in] bitWidth  The number of bits used in the representation.
	 *
	 *      @return The hexadecimal representation.
	 */
	static QString toHexString(quint64 address, int bitWidth);

    //! comparison function for two equal offsets.
    static bool compareItems(const AddressableItem* s1, const AddressableItem* s2);

    //! The first address of the item.
    quint64 firstAddress_ = 0;

    //! The last address of the item.
    quint64 lastAddress_ = 0;

private:
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Conflicted state. Item is conflicted if it overlaps with other items.
    bool conflicted_ = false;

};

#endif // MEMORYVISUALIZATIONITEM_H
