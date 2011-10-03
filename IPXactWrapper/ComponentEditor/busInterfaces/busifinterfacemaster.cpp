/* 
 *
 *  Created on: 7.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfacemaster.cpp
 */

#include "busifinterfacemaster.h"

#include <models/businterface.h>
#include <models/component.h>
#include <models/generaldeclarations.h>
#include <models/masterinterface.h>

#include <QLabel>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QString>

BusIfInterfaceMaster::BusIfInterfaceMaster(General::InterfaceMode mode,
										   BusInterface* busif,
										   QSharedPointer<Component> component,
										   QWidget *parent):
BusIfInterfaceModeEditor(busif, parent), mode_(mode),
addressSpaceRef_(this), 
baseAddress_(this), 
prompt_(this), 
attributes_(this, tr("Base address attributes")),
component_(component) {

	connect(&addressSpaceRef_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onIndexChanged()), Qt::UniqueConnection);
	
	QFormLayout* addressLayout = new QFormLayout();
	addressLayout->addRow(tr("Address space:"), &addressSpaceRef_);

	QFormLayout* baseAddressLayout = new QFormLayout();
	baseAddressLayout->addRow(tr("Base address:"), &baseAddress_);

	QFormLayout* promptLayout = new QFormLayout();
	promptLayout->addRow(tr("Prompt:"), &prompt_);

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->addWidget(&attributes_);
	topLayout->addLayout(addressLayout);
	topLayout->addLayout(baseAddressLayout);
	topLayout->addLayout(promptLayout);
	
	restoreChanges();
}

BusIfInterfaceMaster::~BusIfInterfaceMaster() {
}

bool BusIfInterfaceMaster::isValid() const {
	
	// if address space ref is not defined but some other element is
	if (addressSpaceRef_.currentText().isEmpty() &&
		(!baseAddress_.text().isEmpty() || 
		!prompt_.text().isEmpty() ||
		!attributes_.isEmpty()))
		return false;

	return true;
}

void BusIfInterfaceMaster::restoreChanges() {

	updateDisplay();

	int index = -1;
	QString baseAddress;
	QString prompt;
	attributes_.clear();

	if (busIf()->getMaster()) {
		index = addressSpaceRef_.findText(busIf()->getMaster()->getAddressSpaceRef());
		baseAddress = busIf()->getMaster()->getBaseAddress();
		prompt = busIf()->getMaster()->getPrompt();
		attributes_.setAttributes(busIf()->getMaster()->getBaseAttributes());
	}
	addressSpaceRef_.setCurrentIndex(index);
	baseAddress_.setText(baseAddress);
	prompt_.setText(prompt);

	onIndexChanged();
}

void BusIfInterfaceMaster::applyChanges() {

	if (!isValid())
		return;

	busIf()->setInterfaceMode(mode_);

	// if address space ref is defined
	if (!addressSpaceRef_.currentText().isEmpty()) {
		busIf()->getMaster()->setAddressSpaceRef(addressSpaceRef_.currentText());
		busIf()->getMaster()->setBaseAddress(baseAddress_.text());
		busIf()->getMaster()->setPrompt(prompt_.text());
		busIf()->getMaster()->setBaseAttributes(attributes_.getAttributes());
	}

	// if address space ref was not defined then other elements must be empty

}

void BusIfInterfaceMaster::updateDisplay() {

	addressSpaceRef_.clear();

	addressSpaceRef_.addItems(component_->getAddressSpaceNames());
}

void BusIfInterfaceMaster::onIndexChanged() {

	// if address space is not referenced then other elements cant exist either
	if (addressSpaceRef_.currentText().isEmpty()) {
		baseAddress_.setDisabled(true);
		prompt_.setDisabled(true);
		attributes_.setDisabled(true);
	}
	else {
		baseAddress_.setEnabled(true);
		prompt_.setEnabled(true);
		attributes_.setEnabled(true);
	}
}

General::InterfaceMode BusIfInterfaceMaster::getInterfaceMode() const {
	return mode_;
}
