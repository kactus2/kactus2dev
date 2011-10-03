/* 
 *
 *  Created on: 11.3.2011
 *      Author: Antti Kamppi
 * 		filename: generalinterfacetab.cpp
 */

#include "generalinterfacetab.h"


#include <designwidget/diagramcomponent.h>
#include <designwidget/DiagramChangeCommands.h>
#include <designwidget/blockdiagram.h>

#include <common/GenericEditProvider.h>

#include <models/generaldeclarations.h>
#include <models/businterface.h>
#include <models/masterinterface.h>
#include <models/slaveinterface.h>
#include <models/component.h>

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QSharedPointer>

GeneralInterfaceTab::GeneralInterfaceTab(DiagramConnectionEndPoint* interface, QWidget *parent):
QWidget(parent), interface_(interface), 
busType_(this, interface->getBusInterface()->getBusType()),
absType_(this, interface->getBusInterface()->getAbstractionType()),
instanceName_(interface->name(), this), interfaceMode_(this) {

	busType_.setFlat(false);
	busType_.setTitle(tr("Bus type"));
	absType_.setFlat(false);
	absType_.setTitle(tr("Abstraction type"));

	// If the interface is not hierarchical and the underlying component is packaged, then it can't be edited.
	if (!interface->isHierarchical() && interface->encompassingComp()->componentModel()->getVlnv()->isValid()) {
		instanceName_.setDisabled(true);
	}

    interfaceMode_.setDisabled(true);

	interfaceMode_.addItem(tr("master"));
	interfaceMode_.addItem(tr("slave"));
	interfaceMode_.addItem(tr("system"));
	interfaceMode_.addItem(tr("mirroredMaster"));
	interfaceMode_.addItem(tr("mirroredSlave"));
	interfaceMode_.addItem(tr("mirroredSystem"));
	interfaceMode_.addItem(tr("monitor"));

	BusInterface* busInterface = interface->getBusInterface().data();
	General::InterfaceMode mode = busInterface->getInterfaceMode();
	interfaceMode_.setCurrentIndex(interfaceMode_.findText(
		General::interfaceMode2Str(mode), Qt::MatchFixedString));

	QLabel* intName = new QLabel(tr("Interface name:"), this);
	QLabel* intMode = new QLabel(tr("Interface mode:"), this);
	
	QHBoxLayout* nameLayout = new QHBoxLayout();
	nameLayout->addWidget(intName);
	nameLayout->addWidget(&instanceName_, 1);
	
	QHBoxLayout* modeLayout = new QHBoxLayout();
	modeLayout->addWidget(intMode);
	modeLayout->addWidget(&interfaceMode_, 1);

	QVBoxLayout* editLayout = new QVBoxLayout();
	editLayout->addLayout(nameLayout);
	editLayout->addLayout(modeLayout);
	editLayout->addStretch();

	QHBoxLayout* topLayout = new QHBoxLayout(this);
	topLayout->addWidget(&busType_, 0, Qt::AlignTop | Qt::AlignLeft);
	topLayout->addWidget(&absType_, 0, Qt::AlignTop | Qt::AlignLeft);
	topLayout->addLayout(editLayout, Qt::AlignTop | Qt::AlignLeft);
	topLayout->addStretch();

	connect(&instanceName_, SIGNAL(editingFinished()),
		this, SLOT(onSave()), Qt::UniqueConnection);
	connect(&interfaceMode_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onSave()), Qt::UniqueConnection);

    connect(interface_, SIGNAL(contentChanged()), this, SLOT(onContentChanged()));
}

GeneralInterfaceTab::~GeneralInterfaceTab() {
	disconnect(&instanceName_, SIGNAL(editingFinished()),
		this, SLOT(onSave()));
	disconnect(&interfaceMode_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onSave()));
}

void GeneralInterfaceTab::onSave() {

	if (!interface_->isHierarchical() &&
        interface_->encompassingComp()->componentModel()->getVlnv()->isValid())
    {
        return;
    }

	QSharedPointer<BusInterface> busIf = interface_->getBusInterface();

    // Check if the name or the interface mode has changed.
    if (instanceName_.text() != busIf->getName() ||
	    General::interfaceMode2Str(busIf->getInterfaceMode()) != interfaceMode_.currentText())
    {
        General::InterfaceMode newMode =
            General::str2Interfacemode(interfaceMode_.currentText(), General::MASTER);

        QSharedPointer<QUndoCommand> cmd(new EndPointChangeCommand(interface_, instanceName_.text(), newMode));
        static_cast<BlockDiagram*>(interface_->scene())->getEditProvider().addCommand(cmd);
        emit contentChanged();
	}
}

//-----------------------------------------------------------------------------
// Function: onContentChanged()
//-----------------------------------------------------------------------------
void GeneralInterfaceTab::onContentChanged()
{
    // Set the name and interface mode.
    instanceName_.setText(interface_->getBusInterface()->getName());

    BusInterface* busInterface = interface_->getBusInterface().data();
    General::InterfaceMode mode = busInterface->getInterfaceMode();
    interfaceMode_.setCurrentIndex(interfaceMode_.findText(
        General::interfaceMode2Str(mode), Qt::MatchFixedString));
}
