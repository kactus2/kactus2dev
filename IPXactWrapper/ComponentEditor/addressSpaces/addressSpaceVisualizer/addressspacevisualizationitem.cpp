/* 
 *  	Created on: 20.12.2012
 *      Author: Antti Kamppi
 * 		filename: addressspacevisualizationitem.cpp
 *		Project: Kactus 2
 */

#include "addressspacevisualizationitem.h"

AddressSpaceVisualizationItem::AddressSpaceVisualizationItem(QSharedPointer<AddressSpace> addrSpace,
															 QGraphicsItem* parent /*= 0*/):
VisualizerItem(parent),
addrSpace_(addrSpace) {

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
