/* 
 *  	Created on: 22.12.2012
 *      Author: Antti Kamppi
 * 		filename: addressspacescene.cpp
 *		Project: Kactus 2
 */

#include "addressspacescene.h"

AddressSpaceScene::AddressSpaceScene(QSharedPointer<AddressSpace> addrSpace,
									 QObject *parent):
QGraphicsScene(parent),
addrSpace_(addrSpace) {

}

AddressSpaceScene::~AddressSpaceScene() {
}
