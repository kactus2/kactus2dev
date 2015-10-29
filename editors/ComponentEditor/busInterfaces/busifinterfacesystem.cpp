/* 
 *
 *  Created on: 7.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfacesystem.cpp
 */

#include "busifinterfacesystem.h"

#include "busifgeneraltab.h"

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <IPXACTmodels/Component/BusInterface.h>

#include <IPXACTmodels/common/VLNV.h>

#include <QFormLayout>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceSystem::BusIfInterfaceSystem()
//-----------------------------------------------------------------------------
BusIfInterfaceSystem::BusIfInterfaceSystem(General::InterfaceMode mode, 
    LibraryInterface* libHandler,
    QSharedPointer<BusInterface> busif,
    QSharedPointer<Component> component,
    QWidget *parent):
BusIfInterfaceModeEditor(busif, component, tr("System"), parent),
    mode_(mode),
    monitor_(busif->getMonitor()),
    libHandler_(libHandler), 
    groupEditor_(this)
{
    Q_ASSERT(mode == General::SYSTEM || mode == General::MIRROREDSYSTEM);

    if (mode == General::SYSTEM)
    {
        setTitle(tr("System"));
    }
    else if(mode == General::MIRROREDSYSTEM) 
    {
        setTitle(tr("Mirrored system"));
    }

	groupEditor_.setProperty("mandatoryField", true);

	connect(&groupEditor_, SIGNAL(currentIndexChanged(QString const&)),
		this, SLOT(onGroupChange(QString const&)), Qt::UniqueConnection);

    QFormLayout* groupLayout = new QFormLayout(this);
    groupLayout->addRow(tr("System group"), &groupEditor_);
    groupLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceSystem::~BusIfInterfaceSystem()
//-----------------------------------------------------------------------------
BusIfInterfaceSystem::~BusIfInterfaceSystem()
{
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceSystem::isValid()
//-----------------------------------------------------------------------------
bool BusIfInterfaceSystem::isValid() const
{
	return !groupEditor_.currentText().isEmpty();
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceSystem::refresh()
//-----------------------------------------------------------------------------
void BusIfInterfaceSystem::refresh()
{
	// when the combo box changes it must be disconnected to avoid emitting signals
	disconnect(&groupEditor_, SIGNAL(currentIndexChanged(QString const&)), this, SLOT(onGroupChange(QString const&)));
	
	// update the combobox to display only the possible values

	// the text that was selected
	QString oldText = groupEditor_.currentText();

	// remove previous items in the combo box
	groupEditor_.clear();

	// the selected bus definition defines the groups that can be used
	VLNV busDefVLNV = busif_->getBusType();
	
	// if there is no bus definition specified there is nothing to select
	if (!busDefVLNV.isValid()) {
		return;
	}

	// if the bus definition does not exist then it can't be read to find the groups
	if (!libHandler_->contains(busDefVLNV)) {
		emit errorMessage(tr("The selected bus type does not exist in library."));
		return;
	}

	QSharedPointer<Document> libComp = libHandler_->getModel(busDefVLNV);
	Q_ASSERT(libComp);

	// if the library component with given vlnv was not a bus definition
	if (libHandler_->getDocumentType(busDefVLNV) != VLNV::BUSDEFINITION) {
		emit errorMessage(tr("Bus type VLNV was wrong type"));
		return;
	}

	QSharedPointer<BusDefinition> busDef = libComp.staticCast<BusDefinition>();
	QStringList groupList = busDef->getSystemGroupNames();

	// add the system names to the list
	groupEditor_.addItems(groupList);

	// select the same text that was previously selected if it still can be found
	int index = groupEditor_.findText(oldText);
	groupEditor_.setCurrentIndex(index);

	// reconnect the combo box
	connect(&groupEditor_, SIGNAL(currentIndexChanged(QString const&)),
        this, SLOT(onGroupChange(QString const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceSystem::getInterfaceMode()
//-----------------------------------------------------------------------------
General::InterfaceMode BusIfInterfaceSystem::getInterfaceMode() const
{
	return mode_;
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceSystem::onGroupChange()
//-----------------------------------------------------------------------------
void BusIfInterfaceSystem::onGroupChange(QString const&)
{
	busif_->setSystem(groupEditor_.currentText());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceSystem::saveModeSpecific()
//-----------------------------------------------------------------------------
void BusIfInterfaceSystem::saveModeSpecific()
{
	busif_->setSystem(groupEditor_.currentText());
}
