/* 
 *
 *  Created on: 5.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfacemodetab.cpp
 */

#include "busifinterfacemodetab.h"

#include "busifinterfacemaster.h"
#include "busifinterfacemonitor.h"
#include "busifinterfacemslave.h"
#include "busifinterfaceslave.h"
#include "busifinterfacesystem.h"
#include "busifinterfacemodeeditor.h"

#include <models/businterface.h>
#include <models/generaldeclarations.h>

#include <QFormLayout>
#include <QVBoxLayout>

BusIfInterfaceModeTab::BusIfInterfaceModeTab(LibraryInterface* libHandler,
	BusIfGeneralTab* generalTab,
	QSharedPointer<Component> component,
	void* dataPointer, 
	QWidget *parent): 
QWidget(parent), busif_(static_cast<BusInterface*>(dataPointer)),
modeSelector_(this),
stack_(this) {

	Q_ASSERT_X(dataPointer, "BusIfInterfaceModeTab constructor",
		"Null dataPointer given as parameter");

	// set up the selector box
	modeSelector_.addItem(tr("master"));
	modeSelector_.addItem(tr("slave"));
	modeSelector_.addItem(tr("system"));
	modeSelector_.addItem(tr("mirrored slave"));
	modeSelector_.addItem(tr("mirrored master"));
	modeSelector_.addItem(tr("mirrored system"));
	modeSelector_.addItem(tr("monitor"));
	connect(&modeSelector_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onIndexChanged(int)), Qt::UniqueConnection);

	modeSelector_.setProperty("mandatoryField", true);

	// set up the different editors for different interface modes.
	setupEditors(generalTab, libHandler, component);

	QFormLayout* selectorLayout = new QFormLayout();
	selectorLayout->addRow(tr("Select interface mode"), &modeSelector_);

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->addLayout(selectorLayout);
	topLayout->addWidget(&stack_);
}

BusIfInterfaceModeTab::~BusIfInterfaceModeTab() {
}

bool BusIfInterfaceModeTab::isValid() const {
	// get pointer to the editor
	QWidget* widget = stack_.currentWidget();
	BusIfInterfaceModeEditor* editor = static_cast<BusIfInterfaceModeEditor*>(
		widget);

	return editor->isValid();
}

void BusIfInterfaceModeTab::restoreChanges() {

	// select the correct editor that matches the interface mode
	switch (busif_->getInterfaceMode()) {
		case General::MASTER: {
			stack_.setCurrentIndex(0);
			modeSelector_.setCurrentIndex(0);
			break;
							  }
		case General::SLAVE: {
			stack_.setCurrentIndex(1);
			modeSelector_.setCurrentIndex(1);
			break;
							 }
		case General::SYSTEM: {
			stack_.setCurrentIndex(2);
			modeSelector_.setCurrentIndex(2);
			break;
							  }
		case General::MIRROREDSLAVE: {
			stack_.setCurrentIndex(3);
			modeSelector_.setCurrentIndex(3);
			break;
									 }
		case General::MIRROREDMASTER: {
			stack_.setCurrentIndex(4);
			modeSelector_.setCurrentIndex(4);
			break;
									  }
		case General::MIRROREDSYSTEM: {
			stack_.setCurrentIndex(5);
			modeSelector_.setCurrentIndex(5);
			break;
									  }
		case General::MONITOR: {
			stack_.setCurrentIndex(6);
			modeSelector_.setCurrentIndex(6);
			break;
							   }
		default:
			return;
	}

	// get pointer to the editor
	QWidget* widget = stack_.currentWidget();
	BusIfInterfaceModeEditor* editor = static_cast<BusIfInterfaceModeEditor*>(
		widget);
	// and restore changes
	editor->restoreChanges();
}

void BusIfInterfaceModeTab::applyChanges() {

	// get pointer to the editor
	QWidget* widget = stack_.currentWidget();
	BusIfInterfaceModeEditor* editor = static_cast<BusIfInterfaceModeEditor*>(
		widget);

	// if editor is valid then apply changes
	if (editor->isValid())
		editor->applyChanges();
}

void BusIfInterfaceModeTab::onIndexChanged( int index ) {
	// change the widget to match the selected
	stack_.setCurrentIndex(index);

	// get pointer to the editor
	QWidget* widget = stack_.currentWidget();
	BusIfInterfaceModeEditor* editor = static_cast<BusIfInterfaceModeEditor*>(
		widget);
	// and update the editor 
	editor->updateDisplay();

	emit contentChanged();
}

void BusIfInterfaceModeTab::setupEditors(BusIfGeneralTab* generalTab,
										 LibraryInterface* libHandler,
										 QSharedPointer<Component> component) {
	BusIfInterfaceMaster* master = new BusIfInterfaceMaster(General::MASTER, 
		busif_, component, &stack_);
	connect(master, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(master, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(master, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	BusIfInterfaceSlave* slave = new BusIfInterfaceSlave(busif_,&stack_);
	connect(slave, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(slave, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(slave, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	BusIfInterfaceSystem* system = new BusIfInterfaceSystem(General::SYSTEM,
		generalTab, libHandler, busif_, &stack_);
	connect(system, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(system, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(system, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	BusIfInterfaceMSlave* mirSlave = new BusIfInterfaceMSlave(busif_, &stack_);
	connect(mirSlave, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(mirSlave, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(mirSlave, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	BusIfInterfaceMaster* mirMaster = new BusIfInterfaceMaster(General::MIRROREDMASTER, 
		busif_, component, &stack_);
	connect(mirMaster, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(mirMaster, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(mirMaster, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	BusIfInterfaceSystem* mirSystem = new BusIfInterfaceSystem(
		General::MIRROREDSYSTEM, generalTab, libHandler, busif_, &stack_);
	connect(mirSystem, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(mirSystem, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(mirSystem, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	BusIfInterfaceMonitor* monitor = new BusIfInterfaceMonitor(busif_, 
		generalTab, libHandler, &stack_);
	connect(monitor, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(monitor, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(monitor, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	stack_.addWidget(master);
	stack_.addWidget(slave);
	stack_.addWidget(system);
	stack_.addWidget(mirSlave);
	stack_.addWidget(mirMaster);
	stack_.addWidget(mirSystem);
	stack_.addWidget(monitor);
}

General::InterfaceMode BusIfInterfaceModeTab::getInterfaceMode() {
	QWidget* widget = stack_.currentWidget();
	BusIfInterfaceModeEditor* editor = static_cast<BusIfInterfaceModeEditor*>(widget);

	return editor->getInterfaceMode();
}
