/* 
 *  	Created on: 17.12.2012
 *      Author: Antti Kamppi
 * 		filename: localmemorymapgraphitem.h
 *		Project: Kactus 2
 */

#ifndef LOCALMEMORYMAPGRAPHITEM_H
#define LOCALMEMORYMAPGRAPHITEM_H

#include <IPXactWrapper/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapgraphitem.h>
#include <models/addressspace.h>

class LocalMemoryMapGraphItem : public MemoryMapGraphItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param addrSpace Pointer to the address space that contains the local memory map.
	 * \param localMemoryMap Pointer to the local memory map.
	 * \param parent Pointer to the owner of the graphics item.
	 *
	*/
	LocalMemoryMapGraphItem(QSharedPointer<AddressSpace> addrSpace,
		QSharedPointer<MemoryMap> localMemoryMap, 
		QGraphicsItem* parent = 0);
	
	//! \brief The destructor
	virtual ~LocalMemoryMapGraphItem();

	//! \brief Refresh the item and sub-items.
	virtual void refresh();

	/*! \brief Get number of bits the addressable unit contains.
	 *
	 * \return The size of least addressable unit.
	*/
	virtual unsigned int getAddressUnitSize() const;

private:
	
	//! \brief No copying
	LocalMemoryMapGraphItem(const LocalMemoryMapGraphItem& other);

	//! \brief No assignment
	LocalMemoryMapGraphItem& operator=(const LocalMemoryMapGraphItem& other);

	//! \brief Pointer to the address space containing the local memory map.
	QSharedPointer<AddressSpace> addrSpace_;
};

#endif // LOCALMEMORYMAPGRAPHITEM_H
