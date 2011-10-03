/* 
 *
 *  Created on: 7.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfaceslave.cpp
 */

#include "busifinterfaceslave.h"

#include <models/generaldeclarations.h>
#include <models/businterface.h>

BusIfInterfaceSlave::BusIfInterfaceSlave(BusInterface* busif, QWidget *parent): 
BusIfInterfaceModeEditor(busif, parent) {
}

BusIfInterfaceSlave::~BusIfInterfaceSlave() {
}

bool BusIfInterfaceSlave::isValid() const {
	return true;
}

void BusIfInterfaceSlave::restoreChanges() {

}

void BusIfInterfaceSlave::applyChanges() {
	busIf()->setInterfaceMode(General::SLAVE);
}

General::InterfaceMode BusIfInterfaceSlave::getInterfaceMode() const {
	return General::SLAVE;
}
