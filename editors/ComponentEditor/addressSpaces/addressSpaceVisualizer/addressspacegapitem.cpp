/* 
 *  	Created on: 4.1.2013
 *      Author: Antti Kamppi
 * 		filename: addressspacegapitem.cpp
 *		Project: Kactus 2
 */

#include "addressspacegapitem.h"
#include <common/KactusColors.h>

#include <QColor>
#include <QBrush>

//-----------------------------------------------------------------------------
// Function: AddressSpaceGapItem()
//-----------------------------------------------------------------------------
AddressSpaceGapItem::AddressSpaceGapItem(AddressSpaceGapItem::AddressPosition addrPos,
                                         QString const& addressSpaceWidth,
                                         QSharedPointer<ExpressionParser> expressionParser,
										 QGraphicsItem* parent):
AddressSpaceVisualizationItem(addressSpaceWidth, expressionParser, parent),
start_(0),
end_(0),
addrPosition_(addrPos) 
{
    setDefaultBrush(QBrush(Qt::white));
    if (addrPos == AddressSpaceGapItem::ALIGN_LEFT)
    {
        setNamePosition(VisualizerItem::NAME_LEFT_ALIGN, VisualizerItem::NAME_MIDDLE);
    }
    else //if (addrPos == AddressSpaceGapItem::ALIGN_RIGHT)
    {
        setNamePosition(VisualizerItem::NAME_RIGHT_ALIGN, VisualizerItem::NAME_MIDDLE);
    }
	setName("...");
    setToolTip("This memory block is unassigned.");
}

//-----------------------------------------------------------------------------
// Function: ~AddressSpaceGapItem()
//-----------------------------------------------------------------------------
AddressSpaceGapItem::~AddressSpaceGapItem() {
}

//-----------------------------------------------------------------------------
// Function: refresh()
//-----------------------------------------------------------------------------
void AddressSpaceGapItem::refresh() 
{
    if (addrPosition_ == AddressSpaceGapItem::ALIGN_LEFT) {
        setLeftTopCorner(start_);
        if (end_ != start_)
        {
            setLeftBottomCorner(end_);
        }
        else
        {
            VisualizerItem::setLeftBottomCorner("");
        }
    }

    else {
        setRightTopCorner(start_);
        if (end_ != start_)
        {
            setRightBottomCorner(end_);
        }
        else
        {
            VisualizerItem::setRightBottomCorner("");
        }
    }

    setOverlappingTop(start_);
    setOverlappingBottom(end_);

    VisualizerItem::reorganizeChildren();
}

//-----------------------------------------------------------------------------
// Function: getOffset()
//-----------------------------------------------------------------------------
quint64 AddressSpaceGapItem::getOffset() const {
	return start_;
}

//-----------------------------------------------------------------------------
// Function: getLastAddress()
//-----------------------------------------------------------------------------
quint64 AddressSpaceGapItem::getLastAddress() const {
	return end_;
}

//-----------------------------------------------------------------------------
// Function: setStartAddress()
//-----------------------------------------------------------------------------
void AddressSpaceGapItem::setStartAddress( quint64 address, bool contains /*= true*/ ) {
	if (contains) {
		start_ = address;
	}
	else {
		start_ = ++address;
	}
	refresh();
}

//-----------------------------------------------------------------------------
// Function: setEndAddress()
//-----------------------------------------------------------------------------
void AddressSpaceGapItem::setEndAddress( quint64 address, bool contains /*= true*/ ) {
	if (contains) {
		end_ = address;
	}
	else {
		end_ = --address;
	}
	refresh();
}

//-----------------------------------------------------------------------------
// Function: setAddressAlign()
//-----------------------------------------------------------------------------
void AddressSpaceGapItem::setAddressAlign(AddressSpaceGapItem::AddressPosition pos ) {
	addrPosition_ = pos;
	refresh();
}

//-----------------------------------------------------------------------------
// Function: setOverlappingTop()
//-----------------------------------------------------------------------------
void AddressSpaceGapItem::setOverlappingTop(quint64 const& address)
{
    firstFreeAddress_ = address;

    if (addrPosition_ == AddressSpaceGapItem::ALIGN_LEFT)
    {
        setLeftTopCorner(firstFreeAddress_);

        if (firstFreeAddress_ == lastFreeAddress_){        
            VisualizerItem::setLeftBottomCorner("");
        }
        else
        {
            setLeftBottomCorner(lastFreeAddress_);
        }
    }

    else //if (addrPosition == AddressSpaceGapItem::ALIGN_RIGHT)
    {
        setRightTopCorner(firstFreeAddress_);

        if (firstFreeAddress_ == lastFreeAddress_){        
            VisualizerItem::setRightBottomCorner("");
        }
        else
        {
            setRightBottomCorner(lastFreeAddress_);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: setOverlappingBottom()
//-----------------------------------------------------------------------------
void AddressSpaceGapItem::setOverlappingBottom(quint64 const& address)
{
    lastFreeAddress_ = address;

    if (addrPosition_ == AddressSpaceGapItem::ALIGN_LEFT)
    {
        if (firstFreeAddress_ == lastFreeAddress_){        
            VisualizerItem::setLeftBottomCorner("");
        }
        else
        {
            setLeftBottomCorner(lastFreeAddress_);
        }
    }

    else //if (addrPosition == AddressSpaceGapItem::ALIGN_RIGHT)
    {  
        if (firstFreeAddress_ == lastFreeAddress_){        
            VisualizerItem::setRightBottomCorner("");
        }
        else
        {
            setRightBottomCorner(lastFreeAddress_);
        }
    }
}