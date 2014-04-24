/* 
 *
 *  Created on: 7.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfacemaster.cpp
 */

#include "busifinterfacemaster.h"

#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/masterinterface.h>

#include <QLabel>
#include <QGridLayout>
#include <QString>

BusIfInterfaceMaster::BusIfInterfaceMaster(General::InterfaceMode mode,
										   QSharedPointer<BusInterface> busif,
										   QSharedPointer<Component> component,
										   QWidget *parent):
BusIfInterfaceModeEditor(busif, component, tr("Master"), parent), 
master_(QSharedPointer<MasterInterface>(new MasterInterface())),
mode_(mode),
addressSpaceRef_(component, this), 
baseAddress_(this) {

	// set the title depending on the mode
	switch (mode) {
		case General::MASTER: {
			setTitle(tr("Master"));
			break;
							  }
		case General::MIRROREDMASTER: {
			setTitle(tr("Mirrored master"));
			break;
									  }
		// this editor should only be used for masters and mirrored masters
		default: {
			Q_ASSERT(false);
				 }
	}

	QLabel* addrSpaceLabel = new QLabel(tr("Address space"), this);
	QLabel* baseAddrLabel = new QLabel(tr("Base address"), this);

	QGridLayout* topLayout = new QGridLayout(this);
	topLayout->addWidget(addrSpaceLabel, 0, 0, Qt::AlignLeft);
	topLayout->addWidget(&addressSpaceRef_, 0, 1, Qt::AlignLeft);
	topLayout->addWidget(baseAddrLabel, 1, 0, Qt::AlignLeft);
	topLayout->addWidget(&baseAddress_, 1, 1, Qt::AlignLeft);
	topLayout->setColumnStretch(2, 1);
	topLayout->setRowStretch(2, 1);

	connect(&addressSpaceRef_, SIGNAL(addressSpaceSelected(const QString&)),
		this, SLOT(onAddressSpaceChange(const QString&)), Qt::UniqueConnection);
	connect(&baseAddress_, SIGNAL(textEdited(const QString&)),
		this, SLOT(onBaseAddressChange(const QString&)), Qt::UniqueConnection);
}

BusIfInterfaceMaster::~BusIfInterfaceMaster() {
}

bool BusIfInterfaceMaster::isValid() const {

	QString selectedAddrSpace = addressSpaceRef_.currentText();

	// address space ref is not mandatory
	if (selectedAddrSpace.isEmpty()) {
		return true;
	}
	
	// if the selected address space does not belong to component
	QStringList addrSpaceNames = component_->getAddressSpaceNames();
	if (!addrSpaceNames.contains(selectedAddrSpace)) {
		return false;
	}
	
	return true;
}

void BusIfInterfaceMaster::refresh() {

	// if the model contains master-element
	if (busif_->getMaster()) {
		master_ = busif_->getMaster();
	}
	else {
		master_.clear();
		master_ = QSharedPointer<MasterInterface>(new MasterInterface());
	}

	// update the selectable items
	addressSpaceRef_.refresh();

	QString addrSpaceRef = master_->getAddressSpaceRef();

	// if address space ref is empty then there can be no base address
	if (addrSpaceRef.isEmpty()) {
		baseAddress_.setDisabled(true);
	}
	else {
		baseAddress_.setEnabled(true);
		baseAddress_.setText(master_->getBaseAddress());
	}

	// select the address space ref and base address
	addressSpaceRef_.selectAddressSpace(addrSpaceRef);
}



General::InterfaceMode BusIfInterfaceMaster::getInterfaceMode() const {
	return mode_;
}

void BusIfInterfaceMaster::onAddressSpaceChange(const QString& addrSpaceName) {
	master_->setAddressSpaceRef(addrSpaceName);

	// if address space reference is empty then there can be no base address
	if (addrSpaceName.isEmpty()) {
		baseAddress_.clear();
		baseAddress_.setDisabled(true);
		master_->setBaseAddress("");
	}
	else {
		baseAddress_.setEnabled(true);
	}

	emit contentChanged();
}

void BusIfInterfaceMaster::onBaseAddressChange(const QString& newBase) {
	master_->setBaseAddress(newBase);
	emit contentChanged();
}	

void BusIfInterfaceMaster::saveModeSpecific() {
	busif_->setMaster(master_);
}
