/* 
 *
 *  Created on: 5.4.2011
 *      Author: Antti Kamppi
 * 		filename: businterfaceeditor.cpp
 */

#include "businterfaceeditor.h"

#include <models/component.h>
#include <models/businterface.h>

#include <QHBoxLayout>

BusInterfaceEditor::BusInterfaceEditor(LibraryInterface* libHandler,
									   QSharedPointer<Component> component, 
									   QSharedPointer<BusInterface> busif,
									   QWidget *parent): 
ItemEditor(component, parent),
busif_(busif),
tabs_(this), 
general_(libHandler, busif, component, &tabs_), 
portmaps_(libHandler, component, busif.data(), &tabs_) {

	Q_ASSERT(component);
	Q_ASSERT(libHandler);
	Q_ASSERT(busif_);

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addWidget(&tabs_);
	layout->setContentsMargins(0, 0, 0, 0);

	tabs_.addTab(&general_, tr("General"));
	tabs_.addTab(&portmaps_, tr("Port maps"));

	connect(&portmaps_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&portmaps_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(&portmaps_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	connect(&general_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&general_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(&general_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	connect(&tabs_, SIGNAL(currentChanged(int)),
		this, SLOT(onTabChange(int)), Qt::UniqueConnection);

	refresh();
}

BusInterfaceEditor::~BusInterfaceEditor() {
	tabs_.disconnect();
}

bool BusInterfaceEditor::isValid() const {
	
	if (!general_.isValid())
		return false;
	else if (!portmaps_.isValid())
		return false;

	// all was fine
	return true;
}

void BusInterfaceEditor::onTabChange( int index ) {

	// if port maps tab is selected
	if (index == 1) {
		// update the abstraction type
		portmaps_.setAbsType(general_.getAbsType(), busif_->getInterfaceMode());
	}
}

void BusInterfaceEditor::makeChanges() {
	
}

void BusInterfaceEditor::refresh() {
	general_.refresh();
	portmaps_.refresh();
}
