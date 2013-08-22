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

AddressSpaceGapItem::AddressSpaceGapItem(QSharedPointer<AddressSpace> addrSpace,
										 AddressSpaceGapItem::AddressPosition addrPos,
										 QGraphicsItem* parent):
AddressSpaceVisualizationItem(addrSpace, parent),
start_(0),
end_(0),
addrPosition_(addrPos) 
{
	setDefaultBrush(QBrush(Qt::white));
	setNamePosition(VisualizerItem::NAME_CENTERED, VisualizerItem::NAME_MIDDLE);
	setName("...");
    setToolTip("This memory block is unassigned.");
}

AddressSpaceGapItem::~AddressSpaceGapItem() {
}

void AddressSpaceGapItem::refresh() {

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

quint64 AddressSpaceGapItem::getOffset() const {
	return start_;
}

quint64 AddressSpaceGapItem::getLastAddress() const {
	return end_;
}

void AddressSpaceGapItem::setStartAddress( quint64 address, bool contains /*= true*/ ) {
	if (contains) {
		start_ = address;
	}
	else {
		start_ = ++address;
	}
	refresh();
}

void AddressSpaceGapItem::setEndAddress( quint64 address, bool contains /*= true*/ ) {
	if (contains) {
		end_ = address;
	}
	else {
		end_ = --address;
	}
	refresh();
}

void AddressSpaceGapItem::setAddressAlign(AddressSpaceGapItem::AddressPosition pos ) {
	addrPosition_ = pos;
	refresh();
}

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