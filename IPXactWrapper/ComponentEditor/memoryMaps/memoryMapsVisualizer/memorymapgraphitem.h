/* 
 *  	Created on: 17.4.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapgraphitem.h
 *		Project: Kactus 2
 */

#ifndef MEMORYMAPGRAPHITEM_H
#define MEMORYMAPGRAPHITEM_H

#include "memoryvisualizeritem.h"
#include "addressblockgraphitem.h"
#include <models/memorymap.h>

#include <QSharedPointer>
#include <QList>
#include <QRectF>

/*! \brief The graphical item that represents one memory map.
 *
 */
class MemoryMapGraphItem : public MemoryVisualizerItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param memoryMap Pointer to the memory map to visualize
	 * \param parent Pointer to the owner of this graphics item.
	 *
	*/
	MemoryMapGraphItem(QSharedPointer<MemoryMap> memoryMap, QGraphicsItem* parent = 0);
	
	//! \brief The destructor
	virtual ~MemoryMapGraphItem();

protected:

	//! \brief Set new positions for the address block graph items.
	virtual void reorganizeChildren();

private:
	
	//! \brief No copying
	MemoryMapGraphItem(const MemoryMapGraphItem& other);

	//! \brief No assignment
	MemoryMapGraphItem& operator=(const MemoryMapGraphItem& other);

	//! \brief Pointer to the memory map that is shown
	QSharedPointer<MemoryMap> memoryMap_;

	//! \brief Contains the graphical items for the address blocks.
	QList<QSharedPointer<AddressBlockGraphItem> > adBlocks_;
};

#endif // MEMORYMAPGRAPHITEM_H
