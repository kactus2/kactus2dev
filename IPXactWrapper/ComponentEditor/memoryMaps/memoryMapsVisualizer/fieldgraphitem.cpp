/* 
 *  	Created on: 25.4.2012
 *      Author: Antti Kamppi
 * 		filename: fieldgraphitem.cpp
 *		Project: Kactus 2
 */

#include "fieldgraphitem.h"
#include "registergraphitem.h"
#include <common/KactusColors.h>

#include <QBrush>
#include <QColor>

//-----------------------------------------------------------------------------
// Function: FieldGraphItem::FieldGraphItem()
//-----------------------------------------------------------------------------
FieldGraphItem::FieldGraphItem( QSharedPointer<Field> field,
							   QGraphicsItem* parent):
MemoryVisualizationItem(parent),
field_(field) {
	Q_ASSERT(field_);
	QBrush brush(KactusColors::FIELD_COLOR);
	setDefaultBrush(brush);

	// fields show name in the middle
	setNamePosition(VisualizerItem::NAME_CENTERED, VisualizerItem::NAME_BOTTOM);

	setShowExpandableItem(false);
	setExpansionRectVisible(false);
    setOverlappingTop(field->getMSB());
    setOverlappingBottom(field->getBitOffset());
}

//-----------------------------------------------------------------------------
// Function: FieldGraphItem::~FieldGraphItem()
//-----------------------------------------------------------------------------
FieldGraphItem::~FieldGraphItem() {	
}

//-----------------------------------------------------------------------------
// Function: FieldGraphItem::refresh()
//-----------------------------------------------------------------------------
void FieldGraphItem::refresh() {
	setRect(0, 0, VisualizerItem::MIN_WIDTH, VisualizerItem::DEFAULT_HEIGHT);

	// the name depends on the size of the rectangle (if too small then name is chopped)
	setName(field_->getName());
	setLeftTopCorner(QString::number(field_->getMSB()));
	VisualizerItem::setRightTopCorner(QString::number(field_->getBitOffset()));
	ExpandableItem::reorganizeChildren();

    setOverlappingTop(field_->getMSB());
    setOverlappingBottom(field_->getBitOffset());
    setToolTip("<b>" + getName() + "</b> [" + QString::number(field_->getMSB()) + ".." + 
        QString::number(field_->getBitOffset()) + "]");

	MemoryVisualizationItem* parentGraphItem = static_cast<MemoryVisualizationItem*>(parentItem());
	Q_ASSERT(parentGraphItem);
	parentGraphItem->refresh();
}

//-----------------------------------------------------------------------------
// Function: FieldGraphItem::getOffset()
//-----------------------------------------------------------------------------
quint64 FieldGraphItem::getOffset() const {
	return field_->getBitOffset();
}

//-----------------------------------------------------------------------------
// Function: FieldGraphItem::getBitWidth()
//-----------------------------------------------------------------------------
int FieldGraphItem::getBitWidth() const {
	return field_->getBitWidth();
}

//-----------------------------------------------------------------------------
// Function: FieldGraphItem::getAddressUnitSize()
//-----------------------------------------------------------------------------
unsigned int FieldGraphItem::getAddressUnitSize() const {
	RegisterGraphItem* regItem = static_cast<RegisterGraphItem*>(parentItem());
	Q_ASSERT(regItem);
	return regItem->getAddressUnitSize();
}

//-----------------------------------------------------------------------------
// Function: FieldGraphItem::getLastAddress()
//-----------------------------------------------------------------------------
quint64 FieldGraphItem::getLastAddress() const {
	return field_->getMSB();
}

//-----------------------------------------------------------------------------
// Function: FieldGraphItem::setOverlappingTop()
//-----------------------------------------------------------------------------
void FieldGraphItem::setOverlappingTop(quint64 const& address)
{
    firstFreeAddress_ = address;
    setLeftTopCorner(QString::number(firstFreeAddress_));

}

//-----------------------------------------------------------------------------
// Function: FieldGraphItem::setOverlappingBottom()
//-----------------------------------------------------------------------------
void FieldGraphItem::setOverlappingBottom(quint64 const& address)
{
    lastFreeAddress_ = address;
    setRightTopCorner(QString::number(lastFreeAddress_));
}

//-----------------------------------------------------------------------------
// Function: FieldGraphItem::setCompleteOverlap()
//-----------------------------------------------------------------------------
void FieldGraphItem::setCompleteOverlap()
{
    MemoryVisualizationItem::setCompleteOverlap();
    setWidth(0);
    ExpandableItem::reorganizeChildren();
}