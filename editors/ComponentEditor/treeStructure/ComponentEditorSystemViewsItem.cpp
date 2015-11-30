//-----------------------------------------------------------------------------
// File: ComponentEditorSystemViewsItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 13.7.2012
//
// Description:
// The System Views-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "ComponentEditorSystemViewsItem.h"

#include "ComponentEditorSystemViewItem.h"

#include <editors/ComponentEditor/software/systemView/SystemViewsEditor.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/kactusExtensions/SystemView.h>

//-----------------------------------------------------------------------------
// Function: ComponentEditorSystemViewsItem::ComponentEditorSystemViewsItem()
//-----------------------------------------------------------------------------
ComponentEditorSystemViewsItem::ComponentEditorSystemViewsItem(
	ComponentEditorTreeModel* model, 
	LibraryInterface* libHandler,
	QSharedPointer<Component> component,
	ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent)
{
	foreach (QSharedPointer<SystemView> systemView, component->getSystemViews())
    {
		QSharedPointer<ComponentEditorSystemViewItem> systemViewItem(
			new ComponentEditorSystemViewItem(systemView, model, libHandler, component, this)); 
		childItems_.append(systemViewItem);
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSystemViewsItem::~ComponentEditorSystemViewsItem()
//-----------------------------------------------------------------------------
ComponentEditorSystemViewsItem::~ComponentEditorSystemViewsItem()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSystemViewsItem::getFont()
//-----------------------------------------------------------------------------
QFont ComponentEditorSystemViewsItem::getFont() const
{
    QFont font(ComponentEditorItem::getFont());        
    font.setBold(component_->hasSystemViews());    
    return font;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSystemViewsItem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorSystemViewsItem::getTooltip() const
{
	return tr("Contains the system views of the component");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSystemViewsItem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorSystemViewsItem::text() const
{
	return tr("System views");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSystemViewsItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorSystemViewsItem::editor()
{
	if (!editor_)
    {
		editor_ = new SystemViewsEditor(component_, libHandler_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childAdded(int)), this, SLOT(onAddChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childRemoved(int)), this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
			this, SIGNAL(helpUrlRequested(QString const&)));
	}

	return editor_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSystemViewsItem::createChild()
//-----------------------------------------------------------------------------
void ComponentEditorSystemViewsItem::createChild(int index)
{
	QSharedPointer<ComponentEditorSystemViewItem> systemViewItem(
		new ComponentEditorSystemViewItem(component_->getSystemViews().at(index), model_, libHandler_, component_, this));
	systemViewItem->setLocked(locked_);
	childItems_.insert(index, systemViewItem);
}
