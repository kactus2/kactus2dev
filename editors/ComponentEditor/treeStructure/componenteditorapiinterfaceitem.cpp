//-----------------------------------------------------------------------------
// File: componenteditorapiinterfaceitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 24.05.2012
//
// Description:
// The item for single API-interface in component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditorapiinterfaceitem.h"

#include <library/LibraryManager/libraryinterface.h>

#include <editors/ComponentEditor/software/apiInterface/ApiInterfaceEditor.h>

#include <IPXACTmodels/kactusExtensions/ApiInterface.h>

//-----------------------------------------------------------------------------
// Function: ComponentEditorAPIInterfaceItem::ComponentEditorAPIInterfaceItem()
//-----------------------------------------------------------------------------
ComponentEditorAPIInterfaceItem::ComponentEditorAPIInterfaceItem(
    QSharedPointer<ApiInterface> APIInterface,
    ComponentEditorTreeModel* model,
    LibraryInterface* libHandler,
    QSharedPointer<Component> component,
    ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
    apiIf_(APIInterface)
{
    Q_ASSERT(apiIf_);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAPIInterfaceItem::~ComponentEditorAPIInterfaceItem()
//-----------------------------------------------------------------------------
ComponentEditorAPIInterfaceItem::~ComponentEditorAPIInterfaceItem()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAPIInterfaceItem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorAPIInterfaceItem::getTooltip() const
{
	return tr("Contains the details of an API interface in the component");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAPIInterfaceItem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorAPIInterfaceItem::text() const
{
	return apiIf_->name();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAPIInterfaceItem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorAPIInterfaceItem::isValid() const
{
	// check that the API interface is valid
	if (!apiIf_->isValid())
    {
		return false;
	}

	// check that if API definition is specified it must exist
	if (apiIf_->getApiType().isValid() && !libHandler_->contains(apiIf_->getApiType()))
    {
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAPIInterfaceItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorAPIInterfaceItem::editor()
{
	if (!editor_)
    {
		editor_ = new ApiInterfaceEditor(libHandler_, component_, apiIf_, NULL);
		editor_->setProtection(locked_);

		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));
	}

	return editor_;
}
