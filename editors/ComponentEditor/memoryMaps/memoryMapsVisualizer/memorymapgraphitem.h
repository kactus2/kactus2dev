//-----------------------------------------------------------------------------
// File: memorymapgraphitem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 17.04.2012
//
// Description:
// The graphical item that represents one memory map.
//-----------------------------------------------------------------------------

#ifndef MEMORYMAPGRAPHITEM_H
#define MEMORYMAPGRAPHITEM_H

#include <editors/ComponentEditor/visualization/memoryvisualizationitem.h>

#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/MemoryMapBase.h>

#include <QSharedPointer>
#include <QGraphicsItem>

class ExpressionParser;

//-----------------------------------------------------------------------------
//! The graphical item that represents one memory map.
//-----------------------------------------------------------------------------
class MemoryMapGraphItem : public MemoryVisualizationItem
{
	Q_OBJECT

public:

    /*!
	 *  The constructor.
	 *
	 *    @param [in] parentMemoryMap     The parent memory map of the memory remap.
	 *    @param [in] memoryRemap         The memory remap to be visualized.
	 *    @param [in] parent              Pointer to the owner of this graphics item.
	 */
	MemoryMapGraphItem(QSharedPointer<MemoryMap> parentMemoryMap,
        QSharedPointer<MemoryMapBase> memoryRemap,
        QSharedPointer<ExpressionParser> expressionParser,
        QGraphicsItem* parent = 0);

	//! The destructor.
	virtual ~MemoryMapGraphItem() = default;

    //! No copying.
    MemoryMapGraphItem(const MemoryMapGraphItem& other) = delete;

    //! No assignment.
    MemoryMapGraphItem& operator=(const MemoryMapGraphItem& other) = delete;
        
    //! Updates the labels and tooltip for the item.
    virtual void updateDisplay() override final;

	/*!
     *  Get the offset of the item. 
	 *
	 *    @return int The offset of the item from the parent item's base address.
	 */
	virtual quint64 getOffset() const override final;

	/*!
     *  Get the last address contained in the item.
	 *
	 *    @return The last address.
	 */
	virtual quint64 getLastAddress() const override final;

	/*!
     *  Get the bit width of the item.
	 * 
	 *    @return The bit width of the item.
	 */
	virtual int getBitWidth() const override final;

	/*!
     *  Get number of bits the addressable unit contains.
	 *
	 *    @return The size of least addressable unit.
	 */
	virtual unsigned int getAddressUnitSize() const override;

	/*!
     *  Get the width of the item.
	 *
	 * This width is affected by the item's children so if children grow this width grows accordingly.
	 *
	 *    @return The width of the item and it's sub-items.
	 */
	virtual qreal itemTotalWidth() const override final;
       
    /*!
     *  Checks if the item is to be used in the visualization.
     *
     *    @return True, if the item should be used, otherwise false.
     */
    virtual bool isPresent() const override final;


protected:

    /*!
     *  Get the displayed memory remap.
     *
     *    @return The displayed memory remap.
     */
    QSharedPointer<MemoryMapBase> getMemoryMap() const;

private:

	//! Pointer to the memory remap that is shown
    QSharedPointer<MemoryMapBase> memoryMap_;

    //! Pointer to the parent of the shown memory remap.
    QSharedPointer<MemoryMap> parentMemoryMap_;

};

#endif // MEMORYMAPGRAPHITEM_H
