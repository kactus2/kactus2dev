//-----------------------------------------------------------------------------
// File: ParameterItemEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 06.10.2017
//
// Description:
// ItemEditor for component items containing parameters.
//-----------------------------------------------------------------------------

#include "ParameterItemEditor.h"

//-----------------------------------------------------------------------------
// Function: ParameterItemEditor::ParameterItemEditor()
//-----------------------------------------------------------------------------
ParameterItemEditor::ParameterItemEditor( QSharedPointer<Component> component, LibraryInterface* handler,
    QWidget *parent):
ItemEditor(component, handler, parent)
{

}

//-----------------------------------------------------------------------------
// Function: ParameterItemEditor::~ParameterItemEditor()
//-----------------------------------------------------------------------------
ParameterItemEditor::~ParameterItemEditor()
{

}
