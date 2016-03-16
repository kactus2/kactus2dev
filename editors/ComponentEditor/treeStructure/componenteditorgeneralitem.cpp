//-----------------------------------------------------------------------------
// File: componenteditortreemodel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 09.05.2012
//
// Description:
// The general-item in the component editor navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditorgeneralitem.h"

#include <editors/ComponentEditor/general/generaleditor.h>

#include <IPXACTmodels/Component/Component.h>

//-----------------------------------------------------------------------------
// Function: componenteditorgeneralitem::ComponentEditorGeneralItem()
//-----------------------------------------------------------------------------
ComponentEditorGeneralItem::ComponentEditorGeneralItem( ComponentEditorTreeModel* model,
    LibraryInterface* libHandler, QSharedPointer<Component> component, ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent)
{

}

//-----------------------------------------------------------------------------
// Function: componenteditorgeneralitem::~ComponentEditorGeneralItem()
//-----------------------------------------------------------------------------
ComponentEditorGeneralItem::~ComponentEditorGeneralItem()
{

}

//-----------------------------------------------------------------------------
// Function: componenteditorgeneralitem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorGeneralItem::text() const
{
	return tr("General");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorGeneralItem::getFont()
//-----------------------------------------------------------------------------
QFont ComponentEditorGeneralItem::getFont() const
{
    QFont font(ComponentEditorItem::getFont());
    font.setBold(true);
    return font;
}

//-----------------------------------------------------------------------------
// Function: componenteditorgeneralitem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorGeneralItem::isValid() const
{
	// general editor is always valid because it contains no mandatory elements
	return true;
}

//-----------------------------------------------------------------------------
// Function: componenteditorgeneralitem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorGeneralItem::editor()
{
	if (!editor_)
    {
		editor_ = new GeneralEditor(libHandler_, component_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: componenteditorgeneralitem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorGeneralItem::getTooltip() const
{
	return tr("Contains the general settings of a component");
}
