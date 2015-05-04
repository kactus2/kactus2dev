/* 
 *  	Created on: 17.4.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapgraphitem.h
 *		Project: Kactus 2
 */

#ifndef MEMORYMAPGRAPHITEM_H
#define MEMORYMAPGRAPHITEM_H

#include <editors/ComponentEditor/visualization/memoryvisualizationitem.h>
#include <IPXACTmodels/memorymap.h>
#include <IPXACTmodels/AbstractMemoryMap.h>

#include <QSharedPointer>
#include <QGraphicsItem>

/*! \brief The graphical item that represents one memory map.
 *
 */
class MemoryMapGraphItem : public MemoryVisualizationItem {
	Q_OBJECT

public:

    /*!
	 *  The constructor.
	 *
	 *      @param [in] parentMemoryMap     The parent memory map of the memory remap.
	 *      @param [in] memoryRemap         The memory remap to be visualized.
	 *      @param [in] parent              Pointer to the owner of this graphics item.
	 */
	MemoryMapGraphItem(QSharedPointer<MemoryMap> parentMemoryMap, QSharedPointer<AbstractMemoryMap> memoryRemap,
        QGraphicsItem* parent = 0);

	//! \brief The destructor
	virtual ~MemoryMapGraphItem();

    //! \brief Refresh the item and re-layout the sub-items.
	virtual void refresh();
        
    //! Updates the labels and tooltip for the item.
    virtual void updateDisplay();

	/*! \brief Get the offset of the item. 
	 *
	 * \return int The offset of the item from the parent item's base address.
	*/
	virtual quint64 getOffset() const;

	/*! \brief Get the last address contained in the item.
	 *
	 * \return The last address.
	*/
	virtual quint64 getLastAddress() const;

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

	/*! \brief Get the width of the item.
	 *
	 * This width is affected by the item's children so if children grow this 
	 * width grows accordingly.
	 *
	 * \return The width of the item and it's sub-items.
	*/
	virtual qreal itemTotalWidth() const;

protected:

	//! Pointer to the memory remap that is shown
    QSharedPointer<AbstractMemoryMap> memoryMap_;

    //! Pointer to the parent of the shown memory remap.
    QSharedPointer<MemoryMap> parentMemoryMap_;

private:
	
	//! \brief No copying
	MemoryMapGraphItem(const MemoryMapGraphItem& other);

	//! \brief No assignment
	MemoryMapGraphItem& operator=(const MemoryMapGraphItem& other);
};

#endif // MEMORYMAPGRAPHITEM_H
