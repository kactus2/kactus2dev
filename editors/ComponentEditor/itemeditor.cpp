//-----------------------------------------------------------------------------
// File: itemeditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 02.02.2011
//
// Description:
// ItemEditor is a base class for editors in Component Editor module..
//-----------------------------------------------------------------------------

#include "itemeditor.h"

#include <IPXACTmodels/Component/Component.h>

#include <library/LibraryManager/libraryinterface.h>

//-----------------------------------------------------------------------------
// Function: itemeditor::ItemEditor()
//-----------------------------------------------------------------------------
ItemEditor::ItemEditor( QSharedPointer<Component> component, LibraryInterface* handler, QWidget *parent): 
QWidget(parent), 
component_(component),
handler_(handler)
{

}

//-----------------------------------------------------------------------------
// Function: itemeditor::~ItemEditor()
//-----------------------------------------------------------------------------
ItemEditor::~ItemEditor()
{

}

//-----------------------------------------------------------------------------
// Function: itemeditor::component()
//-----------------------------------------------------------------------------
QSharedPointer<Component> ItemEditor::component() const
{
	return component_;
}

//-----------------------------------------------------------------------------
// Function: confirmEditorChange()
//-----------------------------------------------------------------------------
bool ItemEditor::confirmEditorChange()
{
    // By default, we always allow the editor to be changed.
    return true;
}

//-----------------------------------------------------------------------------
// Function: ItemEditor::setProtection()
//-----------------------------------------------------------------------------
void ItemEditor::setProtection(bool locked)
{
    setDisabled(locked);
}

//-----------------------------------------------------------------------------
// Function: itemeditor::handler()
//-----------------------------------------------------------------------------
LibraryInterface* ItemEditor::handler() const
{
	return handler_;
}
