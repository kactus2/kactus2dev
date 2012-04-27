/* 
 *  	Created on: 18.4.2012
 *      Author: Antti Kamppi
 * 		filename: addressblockgraphitem.h
 *		Project: Kactus 2
 */

#ifndef ADDRESSBLOCKGRAPHITEM_H
#define ADDRESSBLOCKGRAPHITEM_H

#include "memoryvisualizeritem.h"
#include "registergraphitem.h"
#include <models/addressblock.h>

#include <QGraphicsItem>
#include <QSharedPointer>
#include <QList>
#include <QRectF>

/*! \brief The graphical item that represents one address block.
 *
 */
class AddressBlockGraphItem : public MemoryVisualizerItem {
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

	/*! \brief Set the width for the item.
	 *
	 * \param width The new width of the item.
	 *
	*/
	virtual void setWidth(qreal width);

	/*! \brief Get the offset of the address block.
	 *
	 * \return The offset from the base of the containing memory map.
	*/
	virtual int getOffset() const;

protected:

	//! \brief Set new positions for the register graph items
	virtual void reorganizeChildren();

private:
	
	//! \brief No copying
	AddressBlockGraphItem(const AddressBlockGraphItem& other);

	//! \brief No assignment
	AddressBlockGraphItem& operator=(const AddressBlockGraphItem& other);

	//! \brief Pointer to the address block being displayed.
	QSharedPointer<AddressBlock> addrBlock_;

	//! \brief Contains the registers of the address block.
	QList<QSharedPointer<RegisterGraphItem> > registers_;
};

#endif // ADDRESSBLOCKGRAPHITEM_H
