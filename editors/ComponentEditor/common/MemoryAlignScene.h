//-----------------------------------------------------------------------------
// File: MemoryAlignScene.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 22.12.2012
//
// Description:
// Base class for scenes visualizing address spaces or CPU regions.
//-----------------------------------------------------------------------------

#ifndef MEMORYALIGNSCENE_H
#define MEMORYALIGNSCENE_H

#include <QGraphicsScene>
#include <QSharedPointer>
#include <QMultiMap>

#include <common/graphicsItems/visualizeritem.h>

class AddressSpaceVisualizationItem;
class ExpressionParser;
class AddressSpace;
class AddressSpaceGapItem;


//-----------------------------------------------------------------------------
//! Base class for scenes visualizing address spaces or CPU regions..
//-----------------------------------------------------------------------------
class MemoryAlignScene : public QGraphicsScene
{
	Q_OBJECT

public:

    //! Margin between segments/blocks and segments/blocks outside address space. 
    static const unsigned int MARGIN = 10;

	/*!
     *  The constructor.
	 *
     *       @param [in] expressionParser  The expression parser to use for local memory maps.
	 *       @param [in] parent            The owner of the scene.
	 */
	MemoryAlignScene(QSharedPointer<ExpressionParser> expressionParser,
		QObject *parent);
	
	//! The destructor.
	virtual ~MemoryAlignScene() = default;

    //! No copying.
    MemoryAlignScene(const MemoryAlignScene& other) = delete;

    //! No assignment.
    MemoryAlignScene& operator=(const MemoryAlignScene& other) = delete;

	/*!
     *  Refresh the address space visualization.
	 */
	virtual void refresh();

    /*!
     *  Create the region/segment items placed on left.
     *
     */
    virtual void createRegionItems() = 0;

    /*!
     *  Create the address block items placed on right.
     *
     */
    virtual void createAddressBlockItems() = 0;
    
	/*!
     *  Get the last address contained in the item.
	 *
	 *      @return The last address.
	 */
    quint64 addressSpaceLastAddress() const;

	/*!
     *  Reposition the items on the visualization.
	 * 
	 *  The items are repositioned based on the offsets and new items are not created or old removed.
	 */
	virtual void rePosition();

protected:

    /*!
     *  Get the width of the visualized memory.
     *
     *      @return The width of the memory.
     */
    virtual QString getWidth() const = 0;

    /*!
     *  Get the range of the visualized memory.
     *
     *      @return The range of the memory.
     */
    virtual QString getRange() const = 0;

    //! Contains the segments and segments gaps ordered by offsets
    QMultiMap<quint64, AddressSpaceVisualizationItem*> segmentItems_;

    //! Contains the local address blocks and gaps ordered by offsets
    QMultiMap<quint64, AddressSpaceVisualizationItem*> addrBlockItems_;

    //! Contains the segments outside address space ordered by offsets
    QMultiMap<quint64, AddressSpaceVisualizationItem*> exceedingSegments_;

    //! Contains the local address blocks outside address space ordered by offsets
    QMultiMap<quint64, AddressSpaceVisualizationItem*> exceedingAddrBlocks_;

    //! The used expression parser.
    QSharedPointer<ExpressionParser> expressionParser_;

private:
	
    /*!
     *   Update the offsets and overlapping blocks of the segments/address blocks.
     *
     *      @param [in/out] itemMap             Map of segments/blocks to update.
     *      @param [in]     exceedingItemMap    Map of segments/blocks outside address space.
     *      @param [in]     align               Alignment of text on items.
     */
	void updateMaps(QMultiMap<quint64, AddressSpaceVisualizationItem*>& itemMap, 
        QMultiMap<quint64, AddressSpaceVisualizationItem*> const& exceedingItemMap, 
        VisualizerItem::LabelLayout const align);

    /*!
     *   Positions segments and address blocks outside address space.
     *
     *      @param [in] yStart             Minimum y-coordinate for items.
     */
    virtual void rePositionExceeding(qreal const yStart);

    /*!
     *   Checks if currentItem overlaps previous top-most item. Adds conflicted blocks
     *   if memories overlap and hides completely overlapped memory blocks.
     *
     *      @param [in]     currentItem     Item to check.
     *      @param [in/out] topItem         Top-most item so far.
     *      @param [in/out] prevConflict    Previous conflicting memory block.
     *      @param [in]     align           Alignment of text on items.
     *      @param [in/out] map             Map to which add conflicting blocks.
     */
    virtual void resolveConflicts(AddressSpaceVisualizationItem* currentItem, 
        AddressSpaceVisualizationItem*& topItem, 
        AddressSpaceVisualizationItem*& prevConflict,
        VisualizerItem::LabelLayout const align,
        QMultiMap<quint64, AddressSpaceVisualizationItem*>& map);

};

#endif // MEMORYALIGNSCENE_H
