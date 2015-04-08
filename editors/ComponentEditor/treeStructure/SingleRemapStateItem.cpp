//-----------------------------------------------------------------------------
// File: SingleRemapStateItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 08.04.2015
//
// Description:
// The single remap state item in the component editor navigation tree.
//-----------------------------------------------------------------------------

#include "SingleRemapStateItem.h"

#include <editors/ComponentEditor/remapStates/SingleRemapStateEditor.h>

//-----------------------------------------------------------------------------
// Function: SingleRemapStateItem::SingleRemapStateItem()
//-----------------------------------------------------------------------------
SingleRemapStateItem::SingleRemapStateItem(QSharedPointer<RemapState> remapState, ComponentEditorTreeModel* model,
    LibraryInterface* libHandler, QSharedPointer<Component> component, ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
remapState_(remapState)
{

}

//-----------------------------------------------------------------------------
// Function: SingleRemapStateItem::~SingleRemapStateItem()
//-----------------------------------------------------------------------------
SingleRemapStateItem::~SingleRemapStateItem()
{

}

//-----------------------------------------------------------------------------
// Function: SingleRemapStateItem::getTooltip()
//-----------------------------------------------------------------------------
QString SingleRemapStateItem::getTooltip() const
{
    return tr("Specifies a remap state of the component");
}

//-----------------------------------------------------------------------------
// Function: SingleRemapStateItem::text()
//-----------------------------------------------------------------------------
QString SingleRemapStateItem::text() const
{
    return remapState_->getName();
}

//-----------------------------------------------------------------------------
// Function: SingleRemapStateItem::isValid()
//-----------------------------------------------------------------------------
bool SingleRemapStateItem::isValid() const
{
    return remapState_->isValid(component_->getPortNames());
}

//-----------------------------------------------------------------------------
// Function: SingleRemapStateItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* SingleRemapStateItem::editor()
{
    if (!editor_)
    {
        editor_ = new SingleRemapStateEditor(component_, remapState_, libHandler_);
        editor_->setProtection(locked_);

        connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
            this, SIGNAL(helpUrlRequested(QString const&)), Qt::UniqueConnection);
    }

    return editor_;
}
