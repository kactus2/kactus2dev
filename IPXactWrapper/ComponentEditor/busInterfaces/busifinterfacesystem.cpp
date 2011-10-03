/* 
 *
 *  Created on: 7.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfacesystem.cpp
 */

#include "busifinterfacesystem.h"

#include "busifgeneraltab.h"
#include <LibraryManager/libraryinterface.h>
#include <LibraryManager/vlnv.h>
#include <models/busdefinition.h>
#include <models/librarycomponent.h>
#include <models/businterface.h>

#include <QFormLayout>
#include <QStringList>

BusIfInterfaceSystem::BusIfInterfaceSystem(General::InterfaceMode mode,
										   BusIfGeneralTab* generalTab,
										   LibraryInterface* libHandler,
										   BusInterface* busif,
										   QWidget *parent): 
BusIfInterfaceModeEditor(busif, parent), 
mode_(mode),
generalTab_(generalTab), 
libHandler_(libHandler), 
group_(this) {

	Q_ASSERT_X((mode == General::SYSTEM || mode == General::MIRROREDSYSTEM),
		"BusIfInterfaceSystem constructor",
		"Invalid mode given for the editor");

	group_.setProperty("mandatoryField", true);

	QFormLayout* layout = new QFormLayout(this);
	layout->addRow(tr("Group name of the system"), &group_);

	restoreChanges();
}

BusIfInterfaceSystem::~BusIfInterfaceSystem() {
}

bool BusIfInterfaceSystem::isValid() const {
	return !group_.currentText().isEmpty();
}

void BusIfInterfaceSystem::restoreChanges() {
	
	// update the combobox to display only the possible values
	updateDisplay();

	// select the value 
	int index = group_.findText(busIf()->getSystem());
	group_.setCurrentIndex(index);
}

void BusIfInterfaceSystem::applyChanges() {
	busIf()->setInterfaceMode(mode_);
	busIf()->setSystem(group_.currentText());
}

void BusIfInterfaceSystem::updateDisplay() {
	// the text that was selected
	QString oldText = group_.currentText();
	
	group_.clear();

	
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
	group_.addItems(groupList);

	// select the same text that was previously selected if it still can be found
	int index = group_.findText(oldText);
	group_.setCurrentIndex(index);
}

General::InterfaceMode BusIfInterfaceSystem::getInterfaceMode() const {
	return mode_;
}

