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

/*! \brief The base class to visualize objects in address space editor.
 *
 */
class AddressSpaceVisualizationItem : public VisualizerItem {
	Q_OBJECT

public:

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

	virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);

private:
	
	//! \brief No copying
	AddressSpaceVisualizationItem(const AddressSpaceVisualizationItem& other);

	//! \brief No assignment
	AddressSpaceVisualizationItem& operator=(const AddressSpaceVisualizationItem& other);

	/*! \brief Convert the address to string presented in hexadecimal form.
	 *
	 * \param address The address to be converted.
	 *
	 * \return QString containing the address in hexadecimal form.
	*/
	QString addr2Str(quint64 address);

	//! \brief Pointer to the address space being edited.
	QSharedPointer<AddressSpace> addrSpace_;
};

#endif // ADDRESSSPACEVISUALIZATIONITEM_H
