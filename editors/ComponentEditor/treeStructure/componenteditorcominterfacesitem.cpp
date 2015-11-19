//-----------------------------------------------------------------------------
// File: componenteditorcominterfacesitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 09.05.2012
//
// Description:
// The COM interfaces-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditorcominterfacesitem.h"

#include "componenteditorcominterfaceitem.h"

#include <editors/ComponentEditor/software/comInterface/cominterfaceseditor.h>

#include <IPXACTmodels/Component/Component.h>

//-----------------------------------------------------------------------------
// Function: ComponentEditorComInterfacesItem::ComponentEditorComInterfacesItem()
//-----------------------------------------------------------------------------
ComponentEditorComInterfacesItem::ComponentEditorComInterfacesItem(ComponentEditorTreeModel* model, 
    LibraryInterface* libHandler,
    QSharedPointer<Component> component,
    ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent)
{
    foreach (QSharedPointer<ComInterface> comInterface, component->getComInterfaces())
    {
        QSharedPointer<ComponentEditorComInterfaceItem> interfaceItem(new ComponentEditorComInterfaceItem(
            comInterface, model, libHandler, component, this));
        childItems_.append(interfaceItem);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorComInterfacesItem::~ComponentEditorComInterfacesItem()
//-----------------------------------------------------------------------------
ComponentEditorComInterfacesItem::~ComponentEditorComInterfacesItem()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorComInterfacesItem::getFont()
//-----------------------------------------------------------------------------
QFont ComponentEditorComInterfacesItem::getFont() const
{
    QFont font(ComponentEditorItem::getFont());
    font.setBold(!component_->getComInterfaces().isEmpty());
    return font;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorComInterfacesItem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorComInterfacesItem::text() const
{
	return tr("COM interfaces");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorComInterfacesItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorComInterfacesItem::editor()
{
	if (!editor_)
    {
		editor_ = new ComInterfacesEditor(libHandler_, component_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childAdded(int)), this, SLOT(onAddChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childRemoved(int)),	this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorComInterfacesItem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorComInterfacesItem::getTooltip() const
{
	return tr("Contains the COM interfaces of a component");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorComInterfacesItem::createChild()
//-----------------------------------------------------------------------------
void ComponentEditorComInterfacesItem::createChild(int index)
{
	QSharedPointer<ComponentEditorComInterfaceItem> interfaceItem(new ComponentEditorComInterfaceItem(
		component_->getComInterfaces().at(index), model_, libHandler_, component_, this));
	interfaceItem->setLocked(locked_);
	childItems_.insert(index, interfaceItem);
}
