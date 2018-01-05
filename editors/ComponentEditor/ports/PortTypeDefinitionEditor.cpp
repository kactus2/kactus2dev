//-----------------------------------------------------------------------------
// File: PortTypeDefinitionEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 02.01.2018
//
// Description:
// Editor for a list of port type definitions.
//-----------------------------------------------------------------------------

#include "PortTypeDefinitionEditor.h"

#include <editors/ComponentEditor/ports/PortTypeDefinitionDelegate.h>

//-----------------------------------------------------------------------------
// Function: PortTypeDefinitionEditor::PortTypeDefinitionEditor()
//-----------------------------------------------------------------------------
PortTypeDefinitionEditor::PortTypeDefinitionEditor(QModelIndex const& indexedRow, QWidget* parent):
ListEditor(parent),
indexedRow_(indexedRow)
{
    setItemDelegate(new PortTypeDefinitionDelegate(parent));
}

//-----------------------------------------------------------------------------
// Function: PortTypeDefinitionEditor::~PortTypeDefinitionEditor()
//-----------------------------------------------------------------------------
PortTypeDefinitionEditor::~PortTypeDefinitionEditor()
{
    emit closingEditor(indexedRow_);
}
