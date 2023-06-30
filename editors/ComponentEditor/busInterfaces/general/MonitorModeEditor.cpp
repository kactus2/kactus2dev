//-----------------------------------------------------------------------------
// File: MonitorModeEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 7.4.2011
//
// Description:
// Editor the edit a monitor settings of a bus interface.
//-----------------------------------------------------------------------------

#include "MonitorModeEditor.h"

#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/BusDefinition/BusDefinition.h>
#include <IPXACTmodels/Component/Component.h>

#include <KactusAPI/include/BusInterfaceInterface.h>

#include <QGridLayout>
#include <QLabel>

//-----------------------------------------------------------------------------
// Function: MonitorModeEditor::MonitorModeEditor()
//-----------------------------------------------------------------------------
MonitorModeEditor::MonitorModeEditor(BusInterfaceInterface* busInterface, std::string const& busName,
	Document::Revision docRevision,
    LibraryInterface* libHandler, QWidget *parent):
ModeEditorBase(busInterface, busName, tr("Monitor"), parent),
libHandler_(libHandler),
interfaceMode_(docRevision, General::INTERFACE_MODE_COUNT, false, this), 
groupLabel_(tr("Group:"), this),
systemGroup_(this)
{
	setupLayout();

	connect(&interfaceMode_, SIGNAL(modeSelected(General::InterfaceMode)),
		this, SLOT(onInterfaceModeChange(General::InterfaceMode)), Qt::UniqueConnection);
	connect(&systemGroup_, SIGNAL(currentTextChanged(QString const&)),
		this, SLOT(onSystemGroupChange(QString const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: MonitorModeEditor::isValid()
//-----------------------------------------------------------------------------
bool MonitorModeEditor::isValid() const
{
	// if no interface mode is not defined
	if (interfaceMode_.currentText().isEmpty())
    {
		return false;
	}

	// if interface mode is system or mirrored system but group is not defined
	else if ((interfaceMode_.selected() == General::SYSTEM ||
		interfaceMode_.selected() == General::MIRRORED_SYSTEM) && 
		(systemGroup_.currentText().isEmpty()))
    {
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: MonitorModeEditor::refresh()
//-----------------------------------------------------------------------------
void MonitorModeEditor::refresh()
{
    BusInterfaceInterface* busInterface = getBusInterface();
    std::string busName = getBusName();

    General::InterfaceMode monitorMode = busInterface->getMonitorMode(busName);

	// select the mode in the selector
	interfaceMode_.setMode(monitorMode);

	// disconnect the combo box to avoid emitting signals
	disconnect(&systemGroup_, SIGNAL(currentTextChanged(QString const&)),
		this, SLOT(onSystemGroupChange(QString const&)));

	// clear the previous items from the group combo box
	systemGroup_.clear();

	// if selected interface mode is system or mirrored system then enable
	// the system group combo box
    if (monitorMode == General::SYSTEM || monitorMode == General::MIRRORED_SYSTEM)
    {
        systemGroup_.setEnabled(true);

        // find the current bus definition
        VLNV busDefVLNV = busInterface->getBusType(busName);

        if (libHandler_->getDocumentType(busDefVLNV) == VLNV::BUSDEFINITION)
        {
            QSharedPointer<Document> libComp = libHandler_->getModel(busDefVLNV);
            Q_ASSERT(libComp);

            // get the system groups from the bus definition
            QSharedPointer<BusDefinition> busDef = libComp.staticCast<BusDefinition>();
            QStringList groupList = busDef->getSystemGroupNames();

            // add the system names to the list
            systemGroup_.addItems(groupList);

            // ask the monitor which group to select
            QString selectedGroup = QString::fromStdString(busInterface->getMonitorGroup(busName));

            // select the same text that was previously selected if it still can be found
            int index = systemGroup_.findText(selectedGroup);
            systemGroup_.setCurrentIndex(index);
        }
    }
    // if interface mode is not system or mirrored system then disable the
    // combo box and clear it's contents
    else
    {
        systemGroup_.setDisabled(true);

    }

	// reconnect the combo box
	connect(&systemGroup_, SIGNAL(currentTextChanged(QString const&)),
		this, SLOT(onSystemGroupChange(QString const&)), Qt::UniqueConnection);	
}

//-----------------------------------------------------------------------------
// Function: MonitorModeEditor::getInterfaceMode()
//-----------------------------------------------------------------------------
General::InterfaceMode MonitorModeEditor::getInterfaceMode() const
{
	return General::MONITOR;
}

//-----------------------------------------------------------------------------
// Function: MonitorModeEditor::saveModeSpecific()
//-----------------------------------------------------------------------------
void MonitorModeEditor::saveModeSpecific()
{
    onSystemGroupChange(systemGroup_.currentText());

    General::InterfaceMode monitorMode =
        General::str2Interfacemode(interfaceMode_.currentText(), General::INTERFACE_MODE_COUNT);

    onInterfaceModeChange(monitorMode);
}

//-----------------------------------------------------------------------------
// Function: MonitorModeEditor::onSystemGroupChange()
//-----------------------------------------------------------------------------
void MonitorModeEditor::onSystemGroupChange( QString const& groupName )
{
    getBusInterface()->setMonitorGroup(getBusName(), groupName.toStdString());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: MonitorModeEditor::onInterfaceModeChange()
//-----------------------------------------------------------------------------
void MonitorModeEditor::onInterfaceModeChange( General::InterfaceMode mode )
{
	// update the mode to the monitor 
    BusInterfaceInterface* busInterface = getBusInterface();
    std::string busName = getBusName();
    std::string modeString = General::interfaceMode2Str(mode).toStdString();

    busInterface->setMonitorMode(busName, modeString);

	// disconnect the combo box to avoid emitting signals
	disconnect(&systemGroup_, SIGNAL(currentTextChanged(QString const&)),
		this, SLOT(onSystemGroupChange(QString const&)));

	systemGroup_.clear();

	// if selected interface mode is system or mirrored system then enable
	// the system group combo box
    if (mode == General::SYSTEM || mode == General::MIRRORED_SYSTEM)
    {
        systemGroup_.setEnabled(true);
        systemGroup_.setVisible(true);
        groupLabel_.setVisible(true);

        // find the current bus definition
        VLNV busDefVLNV = busInterface->getBusType(busName);

			// if there is no bus definition
        if (libHandler_->contains(busDefVLNV))
        {
            QSharedPointer<Document> libComp = libHandler_->getModel(busDefVLNV);
            Q_ASSERT(libComp);

            // if the library component with given vlnv was not a bus definition
            if (libHandler_->getDocumentType(busDefVLNV) == VLNV::BUSDEFINITION)
            {
                // get the system groups from the bus definition
                QSharedPointer<BusDefinition> busDef = libComp.staticCast<BusDefinition>();
                QStringList groupList = busDef->getSystemGroupNames();

                // add the system names to the list
                systemGroup_.addItems(groupList);

                // ask the monitor which group to select
                QString selectedGroup = QString::fromStdString(busInterface->getMonitorGroup(busName));

                // select the same text that was previously selected if it still can be found
                int index = systemGroup_.findText(selectedGroup);
                systemGroup_.setCurrentIndex(index);
            }
        }
    }
    // if interface mode is not system or mirrored system then disable the combo box
    else
    {
        busInterface->setMonitorGroup(busName, std::string());

        systemGroup_.setDisabled(true);
        systemGroup_.setHidden(true);
        systemGroup_.setCurrentIndex(-1);

        groupLabel_.setHidden(true);
    }

	// reconnect the combo box
	connect(&systemGroup_, SIGNAL(currentTextChanged(QString const&)),
		this, SLOT(onSystemGroupChange(QString const&)), Qt::UniqueConnection);

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: MonitorModeEditor::setupLayout()
//-----------------------------------------------------------------------------
void MonitorModeEditor::setupLayout()
{
    QLabel* modeLabel = new QLabel(tr("Interface mode:"), this);

    QGridLayout* topLayout = new QGridLayout(this);
    topLayout->addWidget(modeLabel, 0, 0, 1, 1);
    topLayout->addWidget(&interfaceMode_, 0, 1, 1, 1);
    topLayout->addWidget(&groupLabel_, 1, 0, 1, 1);
    topLayout->addWidget(&systemGroup_, 1, 1, 1, 1);
    topLayout->setColumnStretch(1, 1);
    topLayout->setRowStretch(2, 10);
}
