//-----------------------------------------------------------------------------
// File: busifinterfacemodeeditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 7.4.2011
//
// Description:
// Base class for different interface mode editors of a bus interface.
//-----------------------------------------------------------------------------

#include "busifinterfacemodeeditor.h"

#include <KactusAPI/include/BusInterfaceInterface.h>

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceModeEditor::BusIfInterfaceModeEditor()
//-----------------------------------------------------------------------------
BusIfInterfaceModeEditor::BusIfInterfaceModeEditor(BusInterfaceInterface* busInterface, std::string const& busName,
    QString const& title, QWidget* parent):
QGroupBox(title, parent),
busInterface_(busInterface),
busName_(busName)
{
    Q_ASSERT(busInterface_);
}

//-----------------------------------------------------------------------------
// Function: busifinterfacemodeeditor::changeName()
//-----------------------------------------------------------------------------
void BusIfInterfaceModeEditor::changeName(std::string const& newName)
{
    busName_ = newName;
}

//-----------------------------------------------------------------------------
// Function: busifinterfacemodeeditor::getBusInterface()
//-----------------------------------------------------------------------------
BusInterfaceInterface* BusIfInterfaceModeEditor::getBusInterface() const
{
    return busInterface_;
}

//-----------------------------------------------------------------------------
// Function: busifinterfacemodeeditor::getBusName()
//-----------------------------------------------------------------------------
std::string BusIfInterfaceModeEditor::getBusName() const
{
    return busName_;
}
