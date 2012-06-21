/* 
 *
 *  Created on: 7.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfaceslave.cpp
 */

#include "busifinterfaceslave.h"

#include <models/generaldeclarations.h>

#include <QFormLayout>

BusIfInterfaceSlave::BusIfInterfaceSlave(QSharedPointer<BusInterface> busif,
										 QSharedPointer<Component> component,
										 QWidget *parent):
BusIfInterfaceModeEditor(busif, component, tr("Slave"), parent),
slave_(QSharedPointer<SlaveInterface>(new SlaveInterface())),
memSelector_(component, this) {

	Q_ASSERT(slave_);

	QFormLayout* memRefLayout = new QFormLayout(this);
	memRefLayout->addRow(tr("Memory map"), &memSelector_);

	connect(&memSelector_, SIGNAL(memoryMapSelected(const QString&)),
		this, SLOT(onMemoryMapChange(const QString&)), Qt::UniqueConnection);
}

BusIfInterfaceSlave::~BusIfInterfaceSlave() {
}

bool BusIfInterfaceSlave::isValid() const {
	return true;
}

void BusIfInterfaceSlave::refresh() {
	// if the model contains master-element
	if (busif_->getSlave()) {
		slave_ = busif_->getSlave();
	}
	else {
		slave_.clear();
		slave_ = QSharedPointer<SlaveInterface>(new SlaveInterface());
	}

	// update the selectable items
	memSelector_.refresh();

	// select the correct memory map in the combo box
	QString memMapRef = slave_->getMemoryMapRef();
	memSelector_.selectMemoryMap(memMapRef);
}

General::InterfaceMode BusIfInterfaceSlave::getInterfaceMode() const {
	return General::SLAVE;
}

void BusIfInterfaceSlave::onMemoryMapChange( const QString& newMemoryMapName ) {
	slave_->setMemoryMapRef(newMemoryMapName);
	emit contentChanged();
}

void BusIfInterfaceSlave::saveModeSpecific() {
	busif_->setSlave(slave_);
}
