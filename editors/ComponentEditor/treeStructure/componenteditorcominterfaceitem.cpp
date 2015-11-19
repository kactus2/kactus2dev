//-----------------------------------------------------------------------------
// File: componenteditorcominterfaceitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 16.5.2012
//
// Description:
// The item for a single COM interface in component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditorcominterfaceitem.h"

#include <library/LibraryManager/libraryinterface.h>

#include <editors/ComponentEditor/software/comInterface/ComInterfaceEditor.h>

#include <IPXACTmodels/kactusExtensions/ComInterface.h>

//-----------------------------------------------------------------------------
// Function: ComponentEditorComInterfaceItem::ComponentEditorComInterfaceItem()
//-----------------------------------------------------------------------------
ComponentEditorComInterfaceItem::ComponentEditorComInterfaceItem(QSharedPointer<ComInterface> comInterface,
    ComponentEditorTreeModel* model, LibraryInterface* libHandler,
    QSharedPointer<Component> component, ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
    interface_(comInterface),
    editAction_(new QAction(tr("Edit"), this))
{
    connect(editAction_, SIGNAL(triggered(bool)), this, SLOT(openItem()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorComInterfaceItem::~ComponentEditorComInterfaceItem()
//-----------------------------------------------------------------------------
ComponentEditorComInterfaceItem::~ComponentEditorComInterfaceItem()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorComInterfaceItem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorComInterfaceItem::text() const
{
	return interface_->name();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorComInterfaceItem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorComInterfaceItem::isValid() const
{
	// check that the COM interface is valid
	if (!interface_->isValid()) 
    {
		return false;
	}

	// check that if there is a COM definition it is found
	else if (interface_->getComType().isValid())
    {
		if (!libHandler_->contains(interface_->getComType()))
        {
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorComInterfaceItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorComInterfaceItem::editor()
{
	if (!editor_)
    {
		editor_ = new ComInterfaceEditor(libHandler_, component_, interface_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorComInterfaceItem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorComInterfaceItem::getTooltip() const
{
	return tr("Specifies a virtual communication interface for the component");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorComInterfaceItem::canBeOpened()
//-----------------------------------------------------------------------------
bool ComponentEditorComInterfaceItem::canBeOpened() const
{
    return interface_->getComType().isValid() && libHandler_->contains(interface_->getComType());
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorComInterfaceItem::actions()
//-----------------------------------------------------------------------------
QList<QAction*> ComponentEditorComInterfaceItem::actions() const
{
    QList<QAction*> actionList;
    actionList.append(editAction_);

    return actionList;   
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorComInterfaceItem::openItem()
//-----------------------------------------------------------------------------
void ComponentEditorComInterfaceItem::openItem() 
{
	VLNV comdefVLNV = interface_->getComType();
	emit openComDefinition(comdefVLNV);
}
