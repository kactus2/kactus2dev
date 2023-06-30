//-----------------------------------------------------------------------------
// File: ModeEditorBase.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 7.4.2011
//
// Description:
// Base class for different interface mode editors of a bus interface.
//-----------------------------------------------------------------------------

#include "ModeEditorBase.h"

#include <KactusAPI/include/BusInterfaceInterface.h>

//-----------------------------------------------------------------------------
// Function: ModeEditorBase::ModeEditorBase()
//-----------------------------------------------------------------------------
ModeEditorBase::ModeEditorBase(BusInterfaceInterface* busInterface, std::string const& busName,
    QString const& title, QWidget* parent):
QGroupBox(title, parent),
busInterface_(busInterface),
busName_(busName)
{
    Q_ASSERT(busInterface_);

    setFlat(true);
}

//-----------------------------------------------------------------------------
// Function: ModeEditorBase::changeName()
//-----------------------------------------------------------------------------
void ModeEditorBase::changeName(std::string const& newName)
{
    busName_ = newName;
}

//-----------------------------------------------------------------------------
// Function: ModeEditorBase::getBusInterface()
//-----------------------------------------------------------------------------
BusInterfaceInterface* ModeEditorBase::getBusInterface() const
{
    return busInterface_;
}

//-----------------------------------------------------------------------------
// Function: ModeEditorBase::getBusName()
//-----------------------------------------------------------------------------
std::string ModeEditorBase::getBusName() const
{
    return busName_;
}
