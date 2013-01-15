/* 
 *  	Created on: 14.1.2013
 *      Author: Antti Kamppi
 * 		filename: localaddrblockgraphitem.h
 *		Project: Kactus 2
 */

#ifndef LOCALADDRBLOCKGRAPHITEM_H
#define LOCALADDRBLOCKGRAPHITEM_H

#include <IPXactWrapper/ComponentEditor/addressSpaces/addressSpaceVisualizer/addressspacevisualizationitem.h>
#include <models/addressblock.h>

#include <QSharedPointer>

/*! \brief The graph item to visualize address blocks in local memory map.
 *
 */
class LocalAddrBlockGraphItem : public AddressSpaceVisualizationItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param addrSpace Pointer to the address space containing the local address block.
	 * \param block Pointer to the address block being visualized.
	 * \param parent Pointer to the parent graph item.
	 *
	*/
	LocalAddrBlockGraphItem(QSharedPointer<AddressSpace> addrSpace,
		QSharedPointer<AddressBlock> block,
		QGraphicsItem* parent = 0);
	
	//! \brief The destructor
	virtual ~LocalAddrBlockGraphItem();

	//! \brief Refresh the item.
	virtual void refresh();

	/*! \brief Get the offset of the item. 
	 *
	 * \return int The offset of the item.
	*/
	virtual quint64 getOffset() const;

	/*! \brief Get the last address contained in the item.
	 *
	 * \return The last address.
	*/
	virtual quint64 getLastAddress() const;

private:
	
	//! \brief No copying
	LocalAddrBlockGraphItem(const LocalAddrBlockGraphItem& other);

	//! \brief No assignment
	LocalAddrBlockGraphItem& operator=(const LocalAddrBlockGraphItem& other);

	//! \brief Pointer to the address block being visualized.
	QSharedPointer<AddressBlock> addrBlock_;
};

#endif // LOCALADDRBLOCKGRAPHITEM_H
