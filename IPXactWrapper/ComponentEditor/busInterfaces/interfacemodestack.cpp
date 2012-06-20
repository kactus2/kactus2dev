/* 
 *  	Created on: 20.6.2012
 *      Author: Antti Kamppi
 * 		filename: interfacemodestack.cpp
 *		Project: Kactus 2
 */

#include "interfacemodestack.h"
#include "busifgeneraltab.h"

InterfaceModeStack::InterfaceModeStack( QSharedPointer<BusInterface> busif,
									   QSharedPointer<Component> component,
									   LibraryInterface* handler,
									   BusIfGeneralTab* parent):
QStackedWidget(parent),
busif_(busif),
parent_(parent),
mode_(busif->getInterfaceMode()),
master_(General::MASTER, busif, component, this),
slave_(busif, component, this),
system_(General::SYSTEM, parent, handler, busif, component, this),
mirroredMaster_(General::MIRROREDMASTER, busif, component, this),
mirroredSlave_(busif, component, this),
mirroredSystem_(General::MIRROREDSYSTEM, parent, handler, busif, component, this),
monitor_(busif, component, parent, handler, this) {

	Q_ASSERT(busif_);
	Q_ASSERT(component);
	Q_ASSERT(parent);

	insertWidget(General::MASTER, &master_);
	insertWidget(General::SLAVE, &slave_);
	insertWidget(General::SYSTEM, &system_);
	insertWidget(General::MIRROREDMASTER, &mirroredMaster_);
	insertWidget(General::MIRROREDSLAVE, &mirroredSlave_);
	insertWidget(General::MIRROREDSYSTEM, &mirroredSystem_);
	insertWidget(General::MONITOR, &monitor_);

	connect(&master_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&slave_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&system_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&mirroredMaster_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&mirroredSlave_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&mirroredSystem_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&monitor_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

InterfaceModeStack::~InterfaceModeStack() {
}

void InterfaceModeStack::setMode( General::InterfaceMode mode ) {
	// update the current mode
	mode_ = mode;

	// update the mode also for bus interface
	busif_->setInterfaceMode(mode);

	// refresh the editor
	refresh();

	emit contentChanged();
}

void InterfaceModeStack::refresh() {
	mode_ = busif_->getInterfaceMode();

	// select the correct editor
	setCurrentIndex(mode_);

	switch (mode_) {
		case General::MASTER: {
			master_.refresh();
			master_.saveModeSpecific();
			break;
							  }
		case General::SLAVE: {
			slave_.refresh();
			slave_.saveModeSpecific();
			break;
							 }
		case General::SYSTEM: {
			system_.refresh();
			system_.saveModeSpecific();
			break;
							  }
		case General::MIRROREDMASTER: {
			mirroredMaster_.refresh();
			mirroredMaster_.saveModeSpecific();
			break;
									  }
		case General::MIRROREDSLAVE: {
			mirroredSlave_.refresh();
			mirroredSlave_.saveModeSpecific();
			break;
									 }
		case General::MIRROREDSYSTEM: {
			mirroredSystem_.refresh();
			mirroredSystem_.saveModeSpecific();
			break;
									  }
		case General::MONITOR: {
			monitor_.refresh();
			monitor_.saveModeSpecific();
			break;
							   }
		default: {
			Q_ASSERT(false);
				 }
	}
}
