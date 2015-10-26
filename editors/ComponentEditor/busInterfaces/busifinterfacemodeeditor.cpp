/* 
 *
 *  Created on: 7.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfacemodeeditor.cpp
 */

#include "busifinterfacemodeeditor.h"

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceModeEditor::BusIfInterfaceModeEditor()
//-----------------------------------------------------------------------------
BusIfInterfaceModeEditor::BusIfInterfaceModeEditor(QSharedPointer<BusInterface> busif,
    QSharedPointer<Component> component,
    QString const& title, 
    QWidget *parent):
QGroupBox(title, parent),
    busif_(busif),
    component_(component)
{
    Q_ASSERT(busif);
    Q_ASSERT(component);
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceModeEditor::~BusIfInterfaceModeEditor()
//-----------------------------------------------------------------------------
BusIfInterfaceModeEditor::~BusIfInterfaceModeEditor()
{
}
