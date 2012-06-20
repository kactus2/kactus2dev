/* 
 *
 *  Created on: 7.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfaceslave.cpp
 */

#include "busifinterfaceslave.h"

#include <models/generaldeclarations.h>

BusIfInterfaceSlave::BusIfInterfaceSlave(QSharedPointer<BusInterface> busif,
										 QSharedPointer<Component> component,
										 QWidget *parent):
BusIfInterfaceModeEditor(busif, component, tr("Slave"), parent),
slave_(QSharedPointer<SlaveInterface>(new SlaveInterface())) {

	Q_ASSERT(slave_);
}

BusIfInterfaceSlave::~BusIfInterfaceSlave() {
}

bool BusIfInterfaceSlave::isValid() const {
	return true;
}

void BusIfInterfaceSlave::refresh() {

}

General::InterfaceMode BusIfInterfaceSlave::getInterfaceMode() const {
	return General::SLAVE;
}

void BusIfInterfaceSlave::onMemoryMapChange( const QString& newMemoryMapName ) {

	busif_->setInterfaceMode(General::SLAVE);
	emit contentChanged();
}

void BusIfInterfaceSlave::saveModeSpecific() {
	busif_->setSlave(slave_);
}
