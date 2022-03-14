//-----------------------------------------------------------------------------
// File: BusReferenceComboBox.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 24.02.2022
//
// Description:
// The combo box to select one of the defined bus interfaces.
//-----------------------------------------------------------------------------

#include "BusReferenceComboBox.h"

#include <editors/ComponentEditor/busInterfaces/interfaces/BusInterfaceInterface.h>

//-----------------------------------------------------------------------------
// Function: BusReferenceComboBox::BusReferenceComboBox()
//-----------------------------------------------------------------------------
BusReferenceComboBox::BusReferenceComboBox(BusInterfaceInterface* busInterface, QWidget *parent):
QComboBox(parent),
busInterface_(busInterface)
{
    setEditable(false);
}

//-----------------------------------------------------------------------------
// Function: BusReferenceComboBox::setBusInterfaces()
//-----------------------------------------------------------------------------
void BusReferenceComboBox::setupSelection(QString const& currentValue, General::InterfaceMode busMode)
{
    clear();

    addItem("");

    for (auto busName : busInterface_->getItemNames())
    {
        if (busMode == General::INTERFACE_MODE_COUNT || busInterface_->getMode(busName) == busMode)
        {
            addItem(QString::fromStdString(busName));
        }
    }

    if (findText(currentValue) < 0)
    {
        addItem(currentValue);
    }

    setCurrentText(currentValue);
}
