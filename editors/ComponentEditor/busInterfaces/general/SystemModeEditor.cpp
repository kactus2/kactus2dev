//-----------------------------------------------------------------------------
// File: SystemModeEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 7.4.2011
//
// Description:
// Editor to edit system or mirrored system details of a bus interface.
//-----------------------------------------------------------------------------

#include "SystemModeEditor.h"

#include "busifgeneraltab.h"

#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/BusDefinition/BusDefinition.h>
#include <IPXACTmodels/Component/BusInterface.h>

#include <KactusAPI/include/BusInterfaceInterface.h>

#include <QFormLayout>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: SystemModeEditor::SystemModeEditor()
//-----------------------------------------------------------------------------
SystemModeEditor::SystemModeEditor(BusInterfaceInterface* busInterface, std::string const& busName,
    LibraryInterface* libHandler, QWidget *parent):
ModeEditorBase(busInterface, busName, tr("System"), parent),
libHandler_(libHandler),
groupEditor_(this)
{
    if(busInterface->getMode(busName) == General::MIRRORED_SYSTEM) 
    {
        setTitle(tr("Mirrored system"));
    }

	groupEditor_.setProperty("mandatoryField", true);

	connect(&groupEditor_, SIGNAL(currentTextChanged(QString const&)),
		this, SLOT(onGroupChange(QString const&)), Qt::UniqueConnection);

    QFormLayout* groupLayout = new QFormLayout(this);
    groupLayout->addRow(tr("System group:"), &groupEditor_);
}

//-----------------------------------------------------------------------------
// Function: SystemModeEditor::isValid()
//-----------------------------------------------------------------------------
bool SystemModeEditor::isValid() const
{
	return !groupEditor_.currentText().isEmpty();
}

//-----------------------------------------------------------------------------
// Function: SystemModeEditor::refresh()
//-----------------------------------------------------------------------------
void SystemModeEditor::refresh()
{
	// when the combo box changes it must be disconnected to avoid emitting signals
	disconnect(&groupEditor_, SIGNAL(currentTextChanged(QString const&)), this, SLOT(onGroupChange(QString const&)));
	
	// update the combobox to display only the possible values

	// the text that was selected
	QString oldText = groupEditor_.currentText();

	// remove previous items in the combo box
	groupEditor_.clear();

    BusInterfaceInterface* busInterface = getBusInterface();
    std::string busName = getBusName();

	// the selected bus definition defines the groups that can be used
    VLNV busDefVLNV = busInterface->getBusType(busName);

	// if there is no bus definition specified there is nothing to select
	if (!busDefVLNV.isValid())
    {
		return;
	}

	// if the bus definition does not exist then it can't be read to find the groups
	if (!libHandler_->contains(busDefVLNV))
    {
		emit errorMessage(tr("The selected bus type does not exist in library."));
		return;
	}

	QSharedPointer<Document> libComp = libHandler_->getModel(busDefVLNV);
	Q_ASSERT(libComp);

	// if the library component with given vlnv was not a bus definition
	if (libHandler_->getDocumentType(busDefVLNV) != VLNV::BUSDEFINITION)
    {
		emit errorMessage(tr("Bus type VLNV was wrong type"));
		return;
	}

	QSharedPointer<BusDefinition> busDef = libComp.staticCast<BusDefinition>();
	QStringList groupList = busDef->getSystemGroupNames();

	// add the system names to the list
	groupEditor_.addItems(groupList);

    // select the same text that was previously selected if it still can be found
    int index;

    if (oldText.isEmpty())
    {
        index = groupEditor_.findText(QString::fromStdString(busInterface->getSystemGroup(busName)));
    }
    else
    {
        index = groupEditor_.findText(oldText);
    }

	groupEditor_.setCurrentIndex(index);

	// reconnect the combo box
	connect(&groupEditor_, SIGNAL(currentTextChanged(QString const&)),
        this, SLOT(onGroupChange(QString const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: SystemModeEditor::getInterfaceMode()
//-----------------------------------------------------------------------------
General::InterfaceMode SystemModeEditor::getInterfaceMode() const
{
    return getBusInterface()->getMode(getBusName());
}

//-----------------------------------------------------------------------------
// Function: SystemModeEditor::onGroupChange()
//-----------------------------------------------------------------------------
void SystemModeEditor::onGroupChange(QString const&)
{
    getBusInterface()->setSystemGroup(getBusName(), groupEditor_.currentText().toStdString());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SystemModeEditor::saveModeSpecific()
//-----------------------------------------------------------------------------
void SystemModeEditor::saveModeSpecific()
{
    getBusInterface()->setSystemGroup(getBusName(), groupEditor_.currentText().toStdString());
}
