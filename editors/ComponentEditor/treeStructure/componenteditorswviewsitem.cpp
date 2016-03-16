//-----------------------------------------------------------------------------
// File: componenteditorswviewsitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 24.05.2012
//
// Description:
// The Software Views-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditorswviewsitem.h"

#include "componenteditorswviewitem.h"

#include <editors/ComponentEditor/software/swView/swviewseditor.h>

#include <IPXACTmodels/Component/Component.h>

//-----------------------------------------------------------------------------
// Function: ComponentEditorSWViewsItem::ComponentEditorSWViewsItem()
//-----------------------------------------------------------------------------
ComponentEditorSWViewsItem::ComponentEditorSWViewsItem(
	ComponentEditorTreeModel* model, 
	LibraryInterface* libHandler,
	QSharedPointer<Component> component,
	ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent)
{
	foreach (QSharedPointer<SWView> swView, component->getSWViews())
    {
		QSharedPointer<ComponentEditorSWViewItem> swViewItem(
			new ComponentEditorSWViewItem(swView, model, libHandler, component, this)); 
		childItems_.append(swViewItem);
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSWViewsItem::~ComponentEditorSWViewsItem()
//-----------------------------------------------------------------------------
ComponentEditorSWViewsItem::~ComponentEditorSWViewsItem()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSWViewsItem::getFont()
//-----------------------------------------------------------------------------
QFont ComponentEditorSWViewsItem::getFont() const
{
    QFont font(ComponentEditorItem::getFont());
    font.setBold(component_->hasSWViews());    
    return font;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSWViewsItem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorSWViewsItem::getTooltip() const
{
	return tr("Contains the software views of the component");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSWViewsItem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorSWViewsItem::text() const
{
	return tr("Software views");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSWViewsItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorSWViewsItem::editor()
{
	if (!editor_)
    {
		editor_ = new SWViewsEditor(component_, libHandler_);
		editor_->setProtection(locked_);

		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childAdded(int)), this, SLOT(onAddChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childRemoved(int)),	this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));
	}

	return editor_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSWViewsItem::createChild()
//-----------------------------------------------------------------------------
void ComponentEditorSWViewsItem::createChild(int index)
{
	QSharedPointer<ComponentEditorSWViewItem> swViewItem(
		new ComponentEditorSWViewItem(component_->getSWViews().at(index), model_, libHandler_, component_, this));
	swViewItem->setLocked(locked_);
	childItems_.insert(index, swViewItem);
}
