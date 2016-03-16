//-----------------------------------------------------------------------------
// File: componenteditorswpropertiesitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 24.5.2012
//
// Description:
// The Software Properties-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditorswpropertiesitem.h"

#include <editors/ComponentEditor/software/SWPropertiesEditor.h>

#include <IPXACTmodels/Component/Component.h>

//-----------------------------------------------------------------------------
// Function: ComponentEditorSWPropertiesItem::ComponentEditorSWPropertiesItem()
//-----------------------------------------------------------------------------
ComponentEditorSWPropertiesItem::ComponentEditorSWPropertiesItem(ComponentEditorTreeModel* model,
    LibraryInterface* libHandler,
    QSharedPointer<Component> component,
    ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent)
{

}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSWPropertiesItem::~ComponentEditorSWPropertiesItem()
//-----------------------------------------------------------------------------
ComponentEditorSWPropertiesItem::~ComponentEditorSWPropertiesItem()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSWPropertiesItem::getFont()
//-----------------------------------------------------------------------------
QFont ComponentEditorSWPropertiesItem::getFont() const
{
    QFont font(ComponentEditorItem::getFont());
    font.setBold(!component_->getSWProperties()->isEmpty());
    return font;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSWPropertiesItem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorSWPropertiesItem::getTooltip() const
{
	return tr("Contains the software properties of the component");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSWPropertiesItem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorSWPropertiesItem::text() const
{
	return tr("Software properties");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSWPropertiesItem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorSWPropertiesItem::isValid() const
{
	foreach (QSharedPointer<ComProperty> property, *component_->getSWProperties())
    {
		if (!property->validate())
        {
			return false;
		}
	}
	return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSWPropertiesItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorSWPropertiesItem::editor()
{
	if (!editor_)
    {
		editor_ = new SWPropertiesEditor(component_, libHandler_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));
	}
	return editor_;
}
