//-----------------------------------------------------------------------------
// File: componenteditorapiinterfacesitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 24.5.2012
//
// Description:
// The API interfaces-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditorapiinterfacesitem.h"
#include "componenteditorapiinterfaceitem.h"

#include <editors/ComponentEditor/software/apiInterface/apiinterfaceseditor.h>

#include <IPXACTmodels/Component/Component.h>

//-----------------------------------------------------------------------------
// Function: ComponentEditorAPIInterfacesItem::ComponentEditorAPIInterfacesItem()
//-----------------------------------------------------------------------------
ComponentEditorAPIInterfacesItem::ComponentEditorAPIInterfacesItem(
	ComponentEditorTreeModel* model,
	LibraryInterface* libHandler,
	QSharedPointer<Component> component,
	ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
apiInterfaces_(component->getApiInterfaces())
{
	foreach (QSharedPointer<ApiInterface> apiInterface, apiInterfaces_)
    {
		QSharedPointer<ComponentEditorAPIInterfaceItem> apiItem(
			new ComponentEditorAPIInterfaceItem(apiInterface, model, libHandler, component, this));
		childItems_.append(apiItem);
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAPIInterfacesItem::~ComponentEditorAPIInterfacesItem()
//-----------------------------------------------------------------------------
ComponentEditorAPIInterfacesItem::~ComponentEditorAPIInterfacesItem()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAPIInterfacesItem::getFont()
//-----------------------------------------------------------------------------
QFont ComponentEditorAPIInterfacesItem::getFont() const
{
    QFont font(ComponentEditorItem::getFont());
    font.setBold(!apiInterfaces_.empty());
    return font;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAPIInterfacesItem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorAPIInterfacesItem::getTooltip() const
{
	return tr("Contains the API interfaces of the component");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAPIInterfacesItem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorAPIInterfacesItem::text() const
{
	return tr("API interfaces");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAPIInterfacesItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorAPIInterfacesItem::editor()
{
	if (!editor_)
    {
		editor_ = new ApiInterfacesEditor(component_, libHandler_);
		editor_->setProtection(locked_);

		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childAdded(int)), this, SLOT(onAddChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childRemoved(int)),	this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAPIInterfacesItem::createChild()
//-----------------------------------------------------------------------------
void ComponentEditorAPIInterfacesItem::createChild(int index)
{
	QSharedPointer<ComponentEditorAPIInterfaceItem> apiItem(
		new ComponentEditorAPIInterfaceItem(component_->getApiInterfaces().at(index),
        model_, libHandler_, component_, this));
    apiItem->setLocked(locked_);

	childItems_.insert(index, apiItem);
}
