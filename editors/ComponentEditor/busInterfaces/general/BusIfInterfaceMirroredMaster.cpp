//-----------------------------------------------------------------------------
// File: BusIfInterfaceMirroredMaster.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 06.03.2017
//
// Description:
// Display for mirrored master bus interface.
//-----------------------------------------------------------------------------

#include "BusIfInterfaceMirroredMaster.h"

#include <QLabel>
#include <QString>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceMirroredMaster::BusIfInterfaceMirroredMaster()
//-----------------------------------------------------------------------------
BusIfInterfaceMirroredMaster::BusIfInterfaceMirroredMaster(BusInterfaceInterface* busInterface,
    std::string const& busName, QWidget* parent):
BusIfInterfaceModeEditor(busInterface, busName, tr("Mirrored Master"), parent)
{
    QLabel* textLabel = new QLabel(tr("No mode specific options."));

    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    verticalLayout->addWidget(textLabel, 0, Qt::AlignTop);
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceMirroredMaster::~BusIfInterfaceMirroredMaster()
//-----------------------------------------------------------------------------
BusIfInterfaceMirroredMaster::~BusIfInterfaceMirroredMaster()
{

}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceMirroredMaster::isValid()
//-----------------------------------------------------------------------------
bool BusIfInterfaceMirroredMaster::isValid() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceMirroredMaster::refresh()
//-----------------------------------------------------------------------------
void BusIfInterfaceMirroredMaster::refresh()
{

}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceMirroredMaster::getInterfaceMode()
//-----------------------------------------------------------------------------
General::InterfaceMode BusIfInterfaceMirroredMaster::getInterfaceMode() const
{
    return General::MIRROREDMASTER;
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceMirroredMaster::saveModeSpecific()
//-----------------------------------------------------------------------------
void BusIfInterfaceMirroredMaster::saveModeSpecific()
{

}
