/* 
 *  	Created on: 18.4.2012
 *      Author: Antti Kamppi
 * 		filename: addressblockgraphitem.h
 *		Project: Kactus 2
 */

#ifndef ADDRESSBLOCKGRAPHITEM_H
#define ADDRESSBLOCKGRAPHITEM_H

#include <IPXactWrapper/ComponentEditor/visualization/memoryvisualizationitem.h>
#include <models/addressblock.h>

#include <QGraphicsItem>
#include <QSharedPointer>

/*! \brief The graphical item that represents one address block.
 *
 */
class AddressBlockGraphItem : public MemoryVisualizationItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param addrBlock Pointer to the address block to visualize.
	 * \param parent Pointer to the parent graph item.
	 *
	*/
	AddressBlockGraphItem(QSharedPointer<AddressBlock> addrBlock, 
		QGraphicsItem *parent);
	
	//! \brief The destructor
	virtual ~AddressBlockGraphItem();

	//! \brief Refresh the item and sub-items.
	virtual void refresh();

	/*! \brief Get the offset of the item. 
	 *
	 * \return int The offset of the item from the parent item's base address.
	*/
	virtual int getOffset() const;

	/*! \brief Get the bit width of the item.
	 * 
	 * \return The bit width of the item.
	*/
	virtual int getBitWidth() const;

	/*! \brief Get number of bits the addressable unit contains.
	 *
	 * \return The size of least addressable unit.
	*/
	virtual unsigned int getAddressUnitSize() const;

private:
	
	//! \brief No copying
	AddressBlockGraphItem(const AddressBlockGraphItem& other);

	//! \brief No assignment
	AddressBlockGraphItem& operator=(const AddressBlockGraphItem& other);

	//! \brief Pointer to the address block being displayed.
	QSharedPointer<AddressBlock> addrBlock_;
};

#endif // ADDRESSBLOCKGRAPHITEM_H
