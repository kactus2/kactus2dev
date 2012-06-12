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
									   void* dataPointer, 
									   QWidget *parent): 
ItemEditor(component, parent),
busif_(static_cast<BusInterface*>(dataPointer)),
tabs_(this), 
general_(libHandler, dataPointer, &tabs_), 
portmaps_(libHandler, component, dataPointer, &tabs_), 
interfaceMode_(libHandler, &general_, component, dataPointer, &tabs_), 
parameters_(dataPointer, &tabs_) {

	Q_ASSERT_X(component, "BusInterfaceEditor constructor",
		"Null Component-pointer given as parameter");
	Q_ASSERT_X(dataPointer, "BusInterfaceEditor constructor",
		"Null dataPointer given as parameter");

	initialize();
}

BusInterfaceEditor::BusInterfaceEditor(LibraryInterface* libHandler,
									   QSharedPointer<Component> component, 
									   QSharedPointer<BusInterface> busif,
									   QWidget *parent): 
ItemEditor(component, parent),
busif_(busif.data()),
tabs_(this), 
general_(libHandler, busif_, &tabs_), 
portmaps_(libHandler, component, busif_, &tabs_), 
interfaceMode_(libHandler, &general_, component, busif_, &tabs_), 
parameters_(busif_, &tabs_) {

	Q_ASSERT(component);
	Q_ASSERT(libHandler);
	Q_ASSERT(busif);

	initialize();
}

BusInterfaceEditor::~BusInterfaceEditor() {
	tabs_.disconnect();
}

void BusInterfaceEditor::initialize() {
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addWidget(&tabs_);

	tabs_.addTab(&general_, tr("General"));
	tabs_.addTab(&interfaceMode_, tr("Interface mode"));
	tabs_.addTab(&portmaps_, tr("Port maps"));
	tabs_.addTab(&parameters_, tr("Parameters"));

	connect(&portmaps_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&portmaps_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(&portmaps_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	connect(&general_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&general_, SIGNAL(nameChanged(const QString&)),
		this, SIGNAL(nameChanged(const QString&)), Qt::UniqueConnection);
	connect(&general_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(&general_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	connect(&interfaceMode_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&interfaceMode_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(&interfaceMode_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	connect(&parameters_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&parameters_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(&parameters_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	connect(&tabs_, SIGNAL(currentChanged(int)),
		this, SLOT(onTabChange(int)), Qt::UniqueConnection);

	refresh();
}

bool BusInterfaceEditor::isValid() const {
	
	if (!general_.isValid())
		return false;
	else if (!portmaps_.isValid())
		return false;
	else if (!interfaceMode_.isValid())
		return false;
	else if (!parameters_.isValid())
		return false;

	// all was fine
	return true;
}

void BusInterfaceEditor::removeModel() {
	component()->removeBusInterface(busif_);
}

void BusInterfaceEditor::onTabChange( int index ) {

	// if port maps tab is selected
	if (index == 2) {
		// update the abstraction type
		portmaps_.setAbsType(general_.getAbsType(), interfaceMode_.getInterfaceMode());
	}
}

void BusInterfaceEditor::makeChanges() {
	general_.applyChanges();
	portmaps_.applyChanges();
	interfaceMode_.applyChanges();

	component()->updateBusInterface(busif_);
}

void BusInterfaceEditor::refresh() {
	general_.restoreChanges();
	portmaps_.restoreChanges();
	interfaceMode_.restoreChanges();
	parameters_.refresh();
}
