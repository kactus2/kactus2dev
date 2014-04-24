/* 
 *
 *  Created on: 7.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfacesystem.cpp
 */

#include "busifinterfacesystem.h"

#include "busifgeneraltab.h"
#include <library/LibraryManager/libraryinterface.h>
#include <library/LibraryManager/vlnv.h>
#include <IPXACTmodels/busdefinition.h>
#include <IPXACTmodels/librarycomponent.h>
#include <IPXACTmodels/businterface.h>

#include <QFormLayout>
#include <QStringList>

BusIfInterfaceSystem::BusIfInterfaceSystem(General::InterfaceMode mode, 
										   BusIfGeneralTab* generalTab,
										   LibraryInterface* libHandler,
										   QSharedPointer<BusInterface> busif,
										   QSharedPointer<Component> component,
										   QWidget *parent):
BusIfInterfaceModeEditor(busif, component, tr("System"), parent),
mode_(mode),
monitor_(busif->getMonitor()),
generalTab_(generalTab), 
libHandler_(libHandler), 
group_(this) {

	Q_ASSERT(mode == General::SYSTEM || mode == General::MIRROREDSYSTEM);

	switch (mode) {
		case General::SYSTEM: {
			setTitle(tr("System"));
			break;
							  }
		case General::MIRROREDSYSTEM: {
			setTitle(tr("Mirrored system"));
			break;
									  }
		// This editor should only be used for system and mirrored system.
		default: {
			Q_ASSERT(false);
				 }
	}

	connect(&group_, SIGNAL(currentIndexChanged(const QString&)),
		this, SLOT(onGroupChange(const QString&)), Qt::UniqueConnection);

	QFormLayout* layout = new QFormLayout(this);
	layout->addRow(tr("Group"), &group_);
}

BusIfInterfaceSystem::~BusIfInterfaceSystem() {
}

bool BusIfInterfaceSystem::isValid() const {
	return !group_.currentText().isEmpty();
}

void BusIfInterfaceSystem::refresh() {

	// when the combo box changes it must be disconnected to avoid emitting signals
	disconnect(&group_, SIGNAL(currentIndexChanged(const QString&)),
		this, SLOT(onGroupChange(const QString&)));
	
	// update the combobox to display only the possible values

	// the text that was selected
	QString oldText = group_.currentText();

	// remove previous items in the combo box
	group_.clear();

	// the selected bus definition defines the groups that can be used
	VLNV busDefVLNV = generalTab_->getBusType();
	
	// if there is no bus definition specified there is nothing to select
	if (!busDefVLNV.isValid()) {
		return;
	}

	// if the bus definition does not exist then it can't be read to find the groups
	if (!libHandler_->contains(busDefVLNV)) {
		emit errorMessage(tr("The selected bus type does not exist in library."));
		return;
	}

	QSharedPointer<LibraryComponent> libComp = libHandler_->getModel(busDefVLNV);
	Q_ASSERT(libComp);

	// if the library component with given vlnv was not a bus definition
	if (libHandler_->getDocumentType(busDefVLNV) != VLNV::BUSDEFINITION) {
		emit errorMessage(tr("Bus type VLNV was wrong type"));
		return;
	}

	QSharedPointer<BusDefinition> busDef = libComp.staticCast<BusDefinition>();
	QStringList groupList = busDef->getSystemGroupNames();

	// add the system names to the list
	group_.addItems(groupList);

	// select the same text that was previously selected if it still can be found
	int index = group_.findText(oldText);
	group_.setCurrentIndex(index);

	// reconnect the combo box
	connect(&group_, SIGNAL(currentIndexChanged(const QString&)),
		this, SLOT(onGroupChange(const QString&)), Qt::UniqueConnection);
}

General::InterfaceMode BusIfInterfaceSystem::getInterfaceMode() const {
	return mode_;
}

void BusIfInterfaceSystem::onGroupChange( const QString& /*newGroup*/ ) {
	busif_->setSystem(group_.currentText());
	emit contentChanged();
}

void BusIfInterfaceSystem::saveModeSpecific() {
	busif_->setSystem(group_.currentText());
}
