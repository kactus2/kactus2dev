/* 
 *
 *  Created on: 7.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfacemonitor.cpp
 */

#include "busifinterfacemonitor.h"
#include <IPXACTmodels/generaldeclarations.h>
#include "busifgeneraltab.h"
#include <library/LibraryManager/libraryinterface.h>
#include <library/LibraryManager/vlnv.h>
#include <IPXACTmodels/busdefinition.h>

#include <QGridLayout>
#include <QLabel>

BusIfInterfaceMonitor::BusIfInterfaceMonitor(QSharedPointer<BusInterface> busif, 
											 QSharedPointer<Component> component,
											 BusIfGeneralTab* generalTab,
											 LibraryInterface* libHandler,
											 QWidget *parent): 
BusIfInterfaceModeEditor(busif, component, tr("Monitor"), parent),
generalTab_(generalTab),
libHandler_(libHandler),
monitor_(QSharedPointer<BusInterface::MonitorInterface>(new BusInterface::MonitorInterface())),
interfaceMode_(this, QSharedPointer<BusInterface>(), false),
systemGroup_(this) {

	QLabel* modeLabel = new QLabel(tr("Interface mode"), this);
	QLabel* groupLabel = new QLabel(tr("Group"), this);

	QGridLayout* topLayout = new QGridLayout(this);
	topLayout->addWidget(modeLabel, 0, 0, Qt::AlignLeft);
	topLayout->addWidget(&interfaceMode_, 0, 1, Qt::AlignLeft);
	topLayout->addWidget(groupLabel, 1, 0, Qt::AlignLeft);
	topLayout->addWidget(&systemGroup_, 1, 1, Qt::AlignLeft);
	topLayout->setColumnStretch(2, 1);
	topLayout->setRowStretch(2, 1);

	connect(&interfaceMode_, SIGNAL(modeSelected(General::InterfaceMode)),
		this, SLOT(onInterfaceModeChange(General::InterfaceMode)), Qt::UniqueConnection);
	connect(&systemGroup_, SIGNAL(currentIndexChanged(const QString&)),
		this, SLOT(onSystemGroupChange(const QString&)), Qt::UniqueConnection);
}

BusIfInterfaceMonitor::~BusIfInterfaceMonitor() {
}

bool BusIfInterfaceMonitor::isValid() const {
	
	// if no interface mode is not defined
	if (interfaceMode_.currentText().isEmpty()) {
		return false;
	}

	// if interface mode is system or mirrored system but group is not defined
	else if ((interfaceMode_.selected() == General::SYSTEM ||
		interfaceMode_.selected() == General::MIRROREDSYSTEM) && 
		(systemGroup_.currentText().isEmpty())) {
	
		return false;
	}

	// all ok
	return true;
}

void BusIfInterfaceMonitor::refresh() {

	if (busif_->getMonitor()) {
		monitor_ = busif_->getMonitor();
	}
	else {
		monitor_ = QSharedPointer<BusInterface::MonitorInterface>(new BusInterface::MonitorInterface());
	}
	
	// ask the correct mode from the monitor
	General::InterfaceMode monitorMode = monitor_->interfaceMode_;
	
	// select the mode in the selector
	interfaceMode_.setMode(monitorMode);

	// disconnect the combo box to avoid emitting signals
	disconnect(&systemGroup_, SIGNAL(currentIndexChanged(const QString&)),
		this, SLOT(onSystemGroupChange(const QString&)));

	// clear the previous items from the group combo box
	systemGroup_.clear();

	// if selected interface mode is system or mirrored system then enable
	// the system group combo box
	switch (monitorMode) {
		case General::SYSTEM:
		case General::MIRROREDSYSTEM: {
			systemGroup_.setEnabled(true);

			// find the current bus definition
			VLNV busDefVLNV = generalTab_->getBusType();
			
			// if there is no bus definition
			if (!libHandler_->contains(busDefVLNV)) {
				break;
			}

			QSharedPointer<LibraryComponent> libComp = libHandler_->getModel(busDefVLNV);
			Q_ASSERT(libComp);
				
			// if the library component with given vlnv was not a bus definition
			if (libHandler_->getDocumentType(busDefVLNV) != VLNV::BUSDEFINITION) {
				break;
			}

			// get the system groups from the bus definition
			QSharedPointer<BusDefinition> busDef = libComp.staticCast<BusDefinition>();
			QStringList groupList = busDef->getSystemGroupNames();

			// add the system names to the list
			systemGroup_.addItems(groupList);

			// ask the monitor which group to select
			QString selectedGroup = monitor_->group_;

			// select the same text that was previously selected if it still can be found
			int index = systemGroup_.findText(selectedGroup);
			systemGroup_.setCurrentIndex(index);
			break;
									  }
		// if interface mode is not system or mirrored system then disable the
		// combo box and clear it's contents
		default: {
			systemGroup_.setDisabled(true);
			break;
				 }
	}

	// reconnect the combo box
	connect(&systemGroup_, SIGNAL(currentIndexChanged(const QString&)),
		this, SLOT(onSystemGroupChange(const QString&)), Qt::UniqueConnection);	
}

General::InterfaceMode BusIfInterfaceMonitor::getInterfaceMode() const {
	return General::MONITOR;
}

void BusIfInterfaceMonitor::saveModeSpecific() {
	busif_->setMonitor(monitor_);
}

void BusIfInterfaceMonitor::onSystemGroupChange( const QString& groupName ) {
	monitor_->group_ = groupName;
	emit contentChanged();
}

void BusIfInterfaceMonitor::onInterfaceModeChange( General::InterfaceMode mode ) {

	// update the mode to the monitor 
	monitor_->interfaceMode_ = mode;

	// disconnect the combo box to avoid emitting signals
	disconnect(&systemGroup_, SIGNAL(currentIndexChanged(const QString&)),
		this, SLOT(onSystemGroupChange(const QString&)));

	systemGroup_.clear();

	// if selected interface mode is system or mirrored system then enable
	// the system group combo box
	switch (mode) {
		case General::SYSTEM:
		case General::MIRROREDSYSTEM: {
			systemGroup_.setEnabled(true);
			// find the current bus definition
			VLNV busDefVLNV = generalTab_->getBusType();

			// if there is no bus definition
			if (!libHandler_->contains(busDefVLNV)) {
				break;
			}

			QSharedPointer<LibraryComponent> libComp = libHandler_->getModel(busDefVLNV);
			Q_ASSERT(libComp);

			// if the library component with given vlnv was not a bus definition
			if (libHandler_->getDocumentType(busDefVLNV) != VLNV::BUSDEFINITION) {
				break;
			}

			// get the system groups from the bus definition
			QSharedPointer<BusDefinition> busDef = libComp.staticCast<BusDefinition>();
			QStringList groupList = busDef->getSystemGroupNames();

			// add the system names to the list
			systemGroup_.addItems(groupList);

			// ask the monitor which group to select
			QString selectedGroup = monitor_->group_;

			// select the same text that was previously selected if it still can be found
			int index = systemGroup_.findText(selectedGroup);
			systemGroup_.setCurrentIndex(index);
			break;
									  }
									  // if interface mode is not system or mirrored system then disable the
									  // combo box
		default: {
			systemGroup_.setDisabled(true);
			systemGroup_.setCurrentIndex(-1);
			break;
				 }
	}

	// reconnect the combo box
	connect(&systemGroup_, SIGNAL(currentIndexChanged(const QString&)),
		this, SLOT(onSystemGroupChange(const QString&)), Qt::UniqueConnection);

	emit contentChanged();
}
