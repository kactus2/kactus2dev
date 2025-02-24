//-----------------------------------------------------------------------------
// File: ResetTypesItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 03.01.2019
//
// Description:
// The Reset types-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "ResetTypesItem.h"

#include <IPXACTmodels/Component/Component.h>

#include <editors/ComponentEditor/resetTypes/ResetTypesEditor.h>

//-----------------------------------------------------------------------------
// Function: ResetTypesItem::ResetTypesItem()
//-----------------------------------------------------------------------------
ResetTypesItem::ResetTypesItem(ComponentEditorTreeModel* model, LibraryInterface* libHandler,
    QSharedPointer<Component> component, QSharedPointer<ComponentValidator> validator, ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
componentValidator_(validator)
{

}

//-----------------------------------------------------------------------------
// Function: ResetTypesItem::~ResetTypesItem()
//-----------------------------------------------------------------------------
ResetTypesItem::~ResetTypesItem()
{

}

//-----------------------------------------------------------------------------
// Function: ResetTypesItem::getFont()
//-----------------------------------------------------------------------------
QFont ResetTypesItem::getFont() const
{
    QFont font(ComponentEditorItem::getFont());
    font.setBold(!component_->getResetTypes()->isEmpty());
    return font;
}

//-----------------------------------------------------------------------------
// Function: ResetTypesItem::text()
//-----------------------------------------------------------------------------
QString ResetTypesItem::text() const
{
	return tr("Reset types");
}

//-----------------------------------------------------------------------------
// Function: ResetTypesItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ResetTypesItem::editor()
{
	if (!editor_)
    {
        editor_ = new ResetTypesEditor(component_, libHandler_, componentValidator_);
        editor_->setProtection(locked_);
        connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));
	}

	return editor_;
}

//-----------------------------------------------------------------------------
// Function: ResetTypesItem::getTooltip()
//-----------------------------------------------------------------------------
QString ResetTypesItem::getTooltip() const
{
	return tr("Contains the reset type elements of the component");
}

//-----------------------------------------------------------------------------
// Function: ResetTypesItem::isValid()
//-----------------------------------------------------------------------------
bool ResetTypesItem::isValid() const
{
    return componentValidator_->hasValidResetTypes(component_);
}
