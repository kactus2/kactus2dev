//-----------------------------------------------------------------------------
// File: interfacemodeselector.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 15.06.2012
//
// Description:
// The editor to select an interface mode for bus interface within a component.
//-----------------------------------------------------------------------------

#include "interfacemodeselector.h"

#include <IPXACTmodels/Component/BusInterface.h>

//-----------------------------------------------------------------------------
// Function: InterfaceModeSelector::InterfaceModeSelector()
//-----------------------------------------------------------------------------
InterfaceModeSelector::InterfaceModeSelector(Document::Revision docRevision, General::InterfaceMode mode, 
	bool showMonitor, QWidget* parent):
QComboBox(parent),
showMonitor_(showMonitor)
{
	initialize(docRevision);

	setCurrentText(General::interfaceMode2Str(mode));
}

//-----------------------------------------------------------------------------
// Function: InterfaceModeSelector::initialize()
//-----------------------------------------------------------------------------
void InterfaceModeSelector::initialize(Document::Revision docRevision)
{
	// mode names can not be edited
	setEditable(false);

	setSizeAdjustPolicy(QComboBox::AdjustToContents);

	QStringList items;
	if (docRevision == Document::Revision::Std14)
	{
        items = General::INTERFACE_MODES.values();
	}
	else if (docRevision == Document::Revision::Std22)
    {
        items = General::INTERFACE_MODES_2022.values();
	}

    items.prepend("");

    // if monitor should not be displayed then remove it
    if (showMonitor_ == false)
    {
		items.removeOne(General::interfaceMode2Str(General::MONITOR));
    }
    addItems(items);

	connect(this, SIGNAL(currentTextChanged(const QString&)),
		this, SLOT(setMode(const QString&)), Qt::UniqueConnection);

	// the interface mode is mandatory by default
	setProperty("mandatoryField", true);
}

//-----------------------------------------------------------------------------
// Function: InterfaceModeSelector::setMode()
//-----------------------------------------------------------------------------
void InterfaceModeSelector::setMode(const General::InterfaceMode mode)
{
	// if the mode the set is monitor and it is not shown
	if (!showMonitor_ && mode == General::MONITOR)
    {
		return;
	}

	disconnect(this, SIGNAL(currentTextChanged(const QString&)), this, SLOT(setMode(const QString&)));
	
	setCurrentText(General::interfaceMode2Str(mode));
	
	connect(this, SIGNAL(currentTextChanged(const QString&)),
        this, SLOT(setMode(const QString&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: InterfaceModeSelector::setMode()
//-----------------------------------------------------------------------------
void InterfaceModeSelector::setMode(QString const& modeName)
{
	disconnect(this, SIGNAL(currentTextChanged(const QString&)), this, SLOT(setMode(const QString&)));

	// find the item for the text
	int index = findText(modeName, Qt::MatchFixedString);

	// if text was found
	if (index >= 0)
    {
		setCurrentIndex(index);
		emit modeSelected(General::str2Interfacemode(modeName, General::INTERFACE_MODE_COUNT));
	}

	connect(this, SIGNAL(currentTextChanged(const QString&)),
        this, SLOT(setMode(const QString&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: InterfaceModeSelector::selected()
//-----------------------------------------------------------------------------
General::InterfaceMode InterfaceModeSelector::selected() const
{
	return General::str2Interfacemode(currentText(), General::INTERFACE_MODE_COUNT);
}
