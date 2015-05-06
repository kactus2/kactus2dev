//-----------------------------------------------------------------------------
// File: addressspacescene.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 22.12.2012
//
// Description:
// The graphics scene containing the segments and local memory map of an address space.
//-----------------------------------------------------------------------------

#ifndef ADDRESSSPACESCENE_H
#define ADDRESSSPACESCENE_H

#include <IPXACTmodels/addressspace.h>
#include "addressspacegapitem.h"

#include <QGraphicsScene>
#include <QSharedPointer>
#include <QMultiMap>

class AddressSpaceVisualizationItem;
class ExpressionParser;

//-----------------------------------------------------------------------------
//! The graphics scene containing the segments and local memory map of an address space.
//-----------------------------------------------------------------------------
class AddressSpaceScene : public QGraphicsScene
{
	Q_OBJECT

public:

    //! Margin between segments/blocks and segments/blocks outside address space. 
    static const unsigned int MARGIN = 10;

	/*! The constructor.
	 *
	 *       @param [in] addrSpace         The address space being visualized.
     *       @param [in] expressionParser  The expression parser to use for local memory maps.
	 *       @param [in] parent            The owner of the scene.
	 *
	*/
	AddressSpaceScene(QSharedPointer<AddressSpace> addrSpace, QSharedPointer<ExpressionParser> expressionParser,
		QObject *parent);
	
	//! The destructor
	virtual ~AddressSpaceScene();

	/*! Refresh the address space visualization.
	 *
	*/
	virtual void refresh();

	/*! Reposition the items on the visualization.
	 * 
	 * The items are repositioned based on the offsets and new items are not 
	 * created or old removed.
	*/
	virtual void rePosition();

private:
	
	//! No copying
	AddressSpaceScene(const AddressSpaceScene& other);

	//! No assignment
	AddressSpaceScene& operator=(const AddressSpaceScene& other);

    /*!
     *   Update the offsets and overlapping blocks of the segments/address blocks.
     *
     *      @param [in/out] itemMap             Map of segments/blocks to update.
     *      @param [in]     exceedingItemMap    Map of segments/blocks outside address space.
     *      @param [in]     align               Alignment of text on items.
     */
	void updateMaps(QMultiMap<quint64, AddressSpaceVisualizationItem*>& itemMap, 
        QMultiMap<quint64, AddressSpaceVisualizationItem*> const& exceedingItemMap, 
        AddressSpaceGapItem::AddressPosition const align);

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
        AddressSpaceGapItem::AddressPosition const align,
        QMultiMap<quint64, AddressSpaceVisualizationItem*>& map);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The address space being visualized.
	QSharedPointer<AddressSpace> addrSpace_;

	//! Contains the segments and segments gaps ordered by offsets
	QMultiMap<quint64, AddressSpaceVisualizationItem*> segmentItems_;

	//! Contains the local address blocks and gaps ordered by offsets
	QMultiMap<quint64, AddressSpaceVisualizationItem*> addrBlockItems_;

	//! Contains the segments outside address space ordered by offsets
	QMultiMap<quint64, AddressSpaceVisualizationItem*> exceedingSegments_;

	//! Contains the local address blocks outside address space ordered by offsets
	QMultiMap<quint64, AddressSpaceVisualizationItem*> exceedingAddrBlocks_;

    QSharedPointer<ExpressionParser> expressionParser_;
};

#endif // ADDRESSSPACESCENE_H
