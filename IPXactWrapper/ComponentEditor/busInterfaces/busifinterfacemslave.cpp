/* 
 *
 *  Created on: 7.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfacemslave.cpp
 */

#include "busifinterfacemslave.h"

#include <models/businterface.h>
#include <models/generaldeclarations.h>


BusIfInterfaceMSlave::BusIfInterfaceMSlave(BusInterface* busif, 
										   QWidget *parent): 
BusIfInterfaceModeEditor(busif, parent) {
}

BusIfInterfaceMSlave::~BusIfInterfaceMSlave() {
}

bool BusIfInterfaceMSlave::isValid() const {
	return true;
}

void BusIfInterfaceMSlave::restoreChanges() {

}

void BusIfInterfaceMSlave::applyChanges() {

	busIf()->setInterfaceMode(General::MIRROREDSLAVE);
}

General::InterfaceMode BusIfInterfaceMSlave::getInterfaceMode() const {
	return General::MIRROREDSLAVE;
}
