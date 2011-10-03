/* 
 *
 *  Created on: 7.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfacemonitor.cpp
 */

#include "busifinterfacemonitor.h"
#include <models/generaldeclarations.h>
#include <models/businterface.h>
#include "busifgeneraltab.h"
#include <LibraryManager/libraryinterface.h>
#include <LibraryManager/vlnv.h>
#include <models/busdefinition.h>

#include <QFormLayout>
#include <QVBoxLayout>

BusIfInterfaceMonitor::BusIfInterfaceMonitor(BusInterface* busif, 
											 BusIfGeneralTab* generalTab,
											 LibraryInterface* libHandler,
											 QWidget *parent): 
BusIfInterfaceModeEditor(busif, parent),
generalTab_(generalTab),
libHandler_(libHandler),
interfaceMode_(this),
systemGroup_(this) {

	// set up the items for interface mode combo box
	interfaceMode_.addItem(QString("master"));  // index = 0
	interfaceMode_.addItem(QString("slave"));	// index = 1
	interfaceMode_.addItem(QString("system"));	// index = 2
	interfaceMode_.addItem(QString("mirroredSlave"));	// index = 3
	interfaceMode_.addItem(QString("mirroredMaster"));	// index = 4
	interfaceMode_.addItem(QString("mirroredSystem"));	// index = 5

	connect(&interfaceMode_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onInterfaceModeChange(int)), Qt::UniqueConnection);

	// set up the layout of the GUI items
	QFormLayout* interfaceLayout = new QFormLayout();
	interfaceLayout->addRow(tr("Interface mode"), &interfaceMode_);

	QFormLayout* groupLayout = new QFormLayout();
	groupLayout->addRow(tr("Group"), &systemGroup_);

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->addLayout(interfaceLayout);
	topLayout->addLayout(groupLayout);

	restoreChanges();
}

BusIfInterfaceMonitor::~BusIfInterfaceMonitor() {
}

bool BusIfInterfaceMonitor::isValid() const {
	
	// if no interface mode is not defined
	if (interfaceMode_.currentText().isEmpty())
		return false;

	// if interface mode is system or mirrored system but group is not defined
	else if ((interfaceMode_.currentIndex() == 2 ||
		interfaceMode_.currentIndex() == 5) && 
		(systemGroup_.currentText().isEmpty()))
		return false;

	// all ok
	return true;
}

void BusIfInterfaceMonitor::restoreChanges() {
	
	updateDisplay();

	int groupIndex = -1;
	int interfaceIndex = -1;
	// select the value specified in the monitor element
	if (busIf()->getMonitor()) {
		
		// get the index to be selected for system group
		groupIndex = systemGroup_.findText(busIf()->getMonitor()->group_);

		// get the index to be selected for interface mode
		interfaceIndex = interfaceMode_.findText(General::interfaceMode2Str(
			busIf()->getMonitor()->interfaceMode_));

	}
	
	systemGroup_.setCurrentIndex(groupIndex);
	interfaceMode_.setCurrentIndex(interfaceIndex);

	onInterfaceModeChange(interfaceIndex);
}

void BusIfInterfaceMonitor::applyChanges() {

	busIf()->setInterfaceMode(General::MONITOR);
	busIf()->getMonitor()->interfaceMode_ = General::str2Interfacemode(
		interfaceMode_.currentText(), General::SYSTEM);

	// set the right value for monitor's group-element
	switch (interfaceMode_.currentIndex()) {
		// if selected interface mode is system or mirrored system
		case 2:
		case 5: {
			busIf()->getMonitor()->group_ = systemGroup_.currentText();
			return;
				}
				// if interface mode is not system or mirrored system then disable the
				// combo box
		default: {
			busIf()->getMonitor()->group_ = QString();
			return;
				 }
	}
}

void BusIfInterfaceMonitor::onInterfaceModeChange( int index ) {

	// if selected interface mode is system or mirrored system then enable
	// the system group combo box
	switch (index) {
		case 2:
		case 5: {
			systemGroup_.setEnabled(true);
			return;
				}
		// if interface mode is not system or mirrored system then disable the
		// combo box
		default: {
			systemGroup_.setDisabled(true);
			return;
				 }
	}
}

void BusIfInterfaceMonitor::updateDisplay() {

	// the text that was selected
	QString oldText = systemGroup_.currentText();

	systemGroup_.clear();

	VLNV busDefVLNV = generalTab_->getBusType();
	if (!busDefVLNV.isValid())
		return;

	if (!libHandler_->contains(busDefVLNV)) {
		emit errorMessage(tr("The selected bus type does not exist in library."));
		return;
	}

	QSharedPointer<LibraryComponent> libComb = libHandler_->getModel(busDefVLNV);

	// if library did not contain given bus definition
	if (!libComb) {
		emit errorMessage(tr("No Bus Definition with given VLNV was found."));
		return;
	}

	// if the library component with given vlnv was not a bus definition
	else if (libComb->getVlnv()->getType() != VLNV::BUSDEFINITION) {
		emit errorMessage(tr("Bus type VLNV was wrong type"));
		return;
	}

	QSharedPointer<BusDefinition> busDef = libComb.staticCast<BusDefinition>();
	QStringList groupList = busDef->getSystemGroupNames();

	// add the system names to the list
	systemGroup_.addItems(groupList);

	// select the same text that was previously selected if it still can be found
	int index = systemGroup_.findText(oldText);
	systemGroup_.setCurrentIndex(index);
}

General::InterfaceMode BusIfInterfaceMonitor::getInterfaceMode() const {
	return General::MONITOR;
}
