/* 
 *  	Created on: 20.12.2012
 *      Author: Antti Kamppi
 * 		filename: addressspacevisualizationitem.h
 *		Project: Kactus 2
 */

#ifndef ADDRESSSPACEVISUALIZATIONITEM_H
#define ADDRESSSPACEVISUALIZATIONITEM_H

#include <common/graphicsItems/visualizeritem.h>
#include <models/addressspace.h>

#include <QSharedPointer>
#include <QBrush>

/*! \brief The base class to visualize objects in address space editor.
 *
 */
class AddressSpaceVisualizationItem : public VisualizerItem {
	Q_OBJECT

public:

    //! \brief The minimum and maximum bounds for address space items.
	enum Bounds {
        PEN_WIDTH = 2,
        MIN_HEIGHT = VisualizerItem::DEFAULT_HEIGHT/2 + PEN_WIDTH, //! Height for items with one address.
        SEGMENT_HEIGHT = 2*MIN_HEIGHT + PEN_WIDTH                  //! Height for items with address range.
	};

	/*! \brief The constructor
	 *
	 * \param addrSpace Pointer to the address space being visualized.
	 * \param parent Pointer to the parent graphics item.
	 *
	*/
	AddressSpaceVisualizationItem(QSharedPointer<AddressSpace> addrSpace,
		QGraphicsItem* parent = 0);
	
	//! \brief The destructor
	virtual ~AddressSpaceVisualizationItem();

	//! \brief Refresh the item.
	virtual void refresh() = 0;

	/*! \brief Get the offset of the item. 
	 *
	 * \return int The offset of the item.
	*/
	virtual quint64 getOffset() const = 0;

	/*! \brief Get the last address contained in the item.
	 *
	 * \return The last address.
	*/
	virtual quint64 getLastAddress() const = 0;

	/*! \brief Get number of bits the addressable unit contains.
	 *
	 * \return The size of least addressable unit.
	*/
	virtual unsigned int getAddressUnitSize() const;

	/*! \brief Get the bit width of the address space.
	 * 
	 * \return The bit width of the address space.
	*/
	virtual int getBitWidth() const;

	/*! \brief Set the coordinate that is the bottom y-coordinate for the item.
	 *
	 * \param yCoordinate The y-coordinate of the bottom.
	 *
	*/
	virtual void setBottomCoordinate(qreal yCoordinate);

	/*! \brief Set the height for the item.
	 *
	 * \param height The height to set.
	 *
	*/
	virtual void setHeight(qreal height);

	/*! \brief Sets the first non-overlapping address to display.
	 *
	 * \param The first address to set.
	*/
    virtual void setOverlappingTop(quint64 const& address);

	/*! \brief Get the first non-overlapping address of the item.
	 *
	 * \return The first non-overlapping address or -1 if completely overlapped.
	 */
    virtual quint64 getOverlappingTop();

	/*! \brief Sets the last non-overlapping address to display.
	 *
	 * \param The last address to set.
	 */
    virtual void setOverlappingBottom(quint64 const& address);

    /*! \brief Get the last non-overlapping address of the item.
     *
     * \return The last non-overlapping address or -1 if completely overlapped.
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

	/*! \brief Convert the address to string presented in hexadecimal form.
	 *
	 * \param address  The address to be converted.
	 * \param bitWidth The bit width of the item.
	 *
	 * \return QString containing the address in hexadecimal form.
	*/
    static QString addr2Str(quint64 const address, int const bitWidth);

protected:

	/*! \brief Set the address to be shown on the top left corner.
	 *
	 * \param address The address to be shown in hexadecimal form.
	 *
	*/
	virtual void setLeftTopCorner(quint64 address);

	/*! \brief Set the address to be shown on the top left corner.
	 *
	 * \param address The address to be shown in hexadecimal form.
	 *
	*/
	virtual void setRightTopCorner(quint64 address);

	/*! \brief Set the address to be shown on the top left corner.
	 *
	 * \param address The address to be shown in hexadecimal form.
	 *
	*/
	virtual void setLeftBottomCorner(quint64 address);

	/*! \brief Set the address to be shown on the top left corner.
	 *
	 * \param address The address to be shown in hexadecimal form.
	 *
	*/
	virtual void setRightBottomCorner(quint64 address);

    //! \brief The first non-overlapping address.
    quint64 firstFreeAddress_;

    //! \brief The last non-overlapping address.
    quint64 lastFreeAddress_;

private:
	
	//! \brief No copying
	AddressSpaceVisualizationItem(const AddressSpaceVisualizationItem& other);

	//! \brief No assignment
	AddressSpaceVisualizationItem& operator=(const AddressSpaceVisualizationItem& other);

	//! Pointer to the address space being edited.
	QSharedPointer<AddressSpace> addrSpace_;

	//! Conflicted state. Item is conflicted if it overlaps with other items.
    bool conflicted_;

    //! Tells if the item is completely overlapped by other items.
    bool overlapped_;
};

#endif // ADDRESSSPACEVISUALIZATIONITEM_H
