//-----------------------------------------------------------------------------
// File: TypeDefinitionsItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 13.10.2023
//
// Description:
// The Type Definitions-item in the component editor navigation tree.
//-----------------------------------------------------------------------------

#include "TypeDefinitionsItem.h"

#include <IPXACTmodels/Component/Component.h>

//-----------------------------------------------------------------------------
// Function: TypeDefinitionsItem::TypeDefinitionsItem()
//-----------------------------------------------------------------------------
TypeDefinitionsItem::TypeDefinitionsItem( ComponentEditorTreeModel* model,
    LibraryInterface* libHandler, QSharedPointer<Component> component, ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent)
{
  
}

//-----------------------------------------------------------------------------
// Function: TypeDefinitionsItem::text()
//-----------------------------------------------------------------------------
QString TypeDefinitionsItem::text() const
{
	return tr("Type definitions");
}

//-----------------------------------------------------------------------------
// Function: TypeDefinitionsItem::getFont()
//-----------------------------------------------------------------------------
QFont TypeDefinitionsItem::getFont() const
{
    QFont font(ComponentEditorItem::getFont());
    font.setBold(false);
    return font;
}

//-----------------------------------------------------------------------------
// Function: TypeDefinitionsItem::isValid()
//-----------------------------------------------------------------------------
bool TypeDefinitionsItem::isValid() const
{
	return true;
}

//-----------------------------------------------------------------------------
// Function: TypeDefinitionsItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* TypeDefinitionsItem::editor()
{
	return editor_;
}


//-----------------------------------------------------------------------------
// Function: TypeDefinitionsItem::refreshEditor()
//-----------------------------------------------------------------------------
void TypeDefinitionsItem::refreshEditor()
{

}

//-----------------------------------------------------------------------------
// Function: TypeDefinitionsItem::getTooltip()
//-----------------------------------------------------------------------------
QString TypeDefinitionsItem::getTooltip() const
{
	return tr("Contains the type definitions of the component");
}
