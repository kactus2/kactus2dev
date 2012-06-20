/* 
 *
 *  Created on: 7.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfacemslave.cpp
 */

#include "busifinterfacemslave.h"

#include <models/businterface.h>
#include <models/generaldeclarations.h>

BusIfInterfaceMSlave::BusIfInterfaceMSlave(QSharedPointer<BusInterface> busif, 
										   QSharedPointer<Component> component,
										   QWidget *parent):
BusIfInterfaceModeEditor(busif, component, tr("Mirrored slave"), parent),
mirroredSlave_(busif->getMirroredSlave()) {

}

BusIfInterfaceMSlave::~BusIfInterfaceMSlave() {
}

bool BusIfInterfaceMSlave::isValid() const {
	return true;
}

void BusIfInterfaceMSlave::refresh() {

}

General::InterfaceMode BusIfInterfaceMSlave::getInterfaceMode() const {
	return General::MIRROREDSLAVE;
}

void BusIfInterfaceMSlave::saveModeSpecific() {

}
