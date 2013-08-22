/* 
 *  	Created on: 20.12.2012
 *      Author: Antti Kamppi
 * 		filename: addressspacevisualizationitem.cpp
 *		Project: Kactus 2
 */

#include "addressspacevisualizationitem.h"

#include "common/kactuscolors.h"

#include <QRectF>
#include <QBrush>

AddressSpaceVisualizationItem::AddressSpaceVisualizationItem(QSharedPointer<AddressSpace> addrSpace,
															 QGraphicsItem* parent /*= 0*/):
VisualizerItem(parent),
addrSpace_(addrSpace),
defaultBrush_(QBrush()),
overlapped_(false)
{
	Q_ASSERT(addrSpace_);
}

AddressSpaceVisualizationItem::~AddressSpaceVisualizationItem() {
}

unsigned int AddressSpaceVisualizationItem::getAddressUnitSize() const {
	return addrSpace_->getAddressUnitBits();
}

int AddressSpaceVisualizationItem::getBitWidth() const {
	return addrSpace_->getWidth();
}

void AddressSpaceVisualizationItem::setLeftTopCorner( quint64 address ) {
	QString padded = addr2Str(address);
	VisualizerItem::setLeftTopCorner(padded);
}

void AddressSpaceVisualizationItem::setRightTopCorner( quint64 address ) {
	QString padded = addr2Str(address);
	VisualizerItem::setRightTopCorner(padded);
}

void AddressSpaceVisualizationItem::setLeftBottomCorner( quint64 address ) {
	QString padded = addr2Str(address);
	VisualizerItem::setLeftBottomCorner(padded);
}	

void AddressSpaceVisualizationItem::setRightBottomCorner( quint64 address ) {
	QString padded = addr2Str(address);
	VisualizerItem::setRightBottomCorner(padded);
}


void AddressSpaceVisualizationItem::setDefaultBrush(QBrush brush)
{
    defaultBrush_ = brush;
    setBrush(brush);
}

QBrush AddressSpaceVisualizationItem::defaultBrush()
{
    return defaultBrush_;
}

QString AddressSpaceVisualizationItem::addr2Str( quint64 address ) {
	// convert the number into hexadecimal form
	QString str = QString::number(address, 16);
	str = str.toUpper();

	int bitWidth = getBitWidth();

	// one hexadecimal number accounts for four bits
	int fieldSize = bitWidth / 4;
	QString padded = QString("%1").arg(str, fieldSize, QChar('0'));

	// group the string to groups of four characters
	int size = padded.size();
	for (int i = size; i > 0; i -= 4) {
		padded.insert(i, " ");
	}

	// add the identifier indicating a hexadecimal number
	padded.prepend("0x");

	return padded;
}

void AddressSpaceVisualizationItem::setBottomCoordinate( qreal yCoordinate ) {
	qreal width = rect().width();
	qreal height = yCoordinate - y();
	setRect(0, 0, width, height);
	VisualizerItem::reorganizeChildren();
}

void AddressSpaceVisualizationItem::setHeight( qreal height ) {
	qreal width = rect().width();
	setRect(0, 0, width, height);
	VisualizerItem::reorganizeChildren();
}

void AddressSpaceVisualizationItem::setOverlappingTop(quint64 const& address)
{
    firstFreeAddress_ = address;
    setLeftTopCorner(firstFreeAddress_);

    if (firstFreeAddress_ == lastFreeAddress_){        
        VisualizerItem::setLeftBottomCorner("");
    }
}

quint64 AddressSpaceVisualizationItem::getOverlappingTop()
{
    return firstFreeAddress_;
}

void AddressSpaceVisualizationItem::setOverlappingBottom(quint64 const& address)
{
    lastFreeAddress_ = address;
    setLeftBottomCorner(lastFreeAddress_);

    if (firstFreeAddress_ == lastFreeAddress_)
    {
        VisualizerItem::setLeftBottomCorner("");
    }
}

quint64 AddressSpaceVisualizationItem::getOverlappingBottom()
{
    return lastFreeAddress_;
}

//-----------------------------------------------------------------------------
// Function: setCompleteOverlap()
//-----------------------------------------------------------------------------
void AddressSpaceVisualizationItem::setCompleteOverlap()
{
    overlapped_ = true;
    hide();
}

//-----------------------------------------------------------------------------
// Function: isCompletelyOverlapped()
//-----------------------------------------------------------------------------
bool AddressSpaceVisualizationItem::isCompletelyOverlapped() const
{
    return overlapped_;
}

//-----------------------------------------------------------------------------
// Function: setConflicted()
//-----------------------------------------------------------------------------
void AddressSpaceVisualizationItem::setConflicted(bool conflicted)
{
    conflicted_ = conflicted;

    if (conflicted)
    {
        setBrush(KactusColors::MISSING_COMPONENT);
    }
    else
    {
        setBrush(defaultBrush());
    }
}

//-----------------------------------------------------------------------------
// Function: isConflicted()
//-----------------------------------------------------------------------------
bool AddressSpaceVisualizationItem::isConflicted() const
{
    return conflicted_;
}
