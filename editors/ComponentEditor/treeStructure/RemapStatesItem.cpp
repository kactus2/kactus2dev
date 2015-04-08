//-----------------------------------------------------------------------------
// File: RemapStatesItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 07.04.2015
//
// Description:
// The Remap states-item in the component editor navigation tree.
//-----------------------------------------------------------------------------

#include "RemapStatesItem.h"
#include "SingleRemapStateItem.h"

#include <editors/ComponentEditor/remapStates/RemapStatesEditor.h>

//-----------------------------------------------------------------------------
// Function: RemapStatesItem::RemapStatesItem()
//-----------------------------------------------------------------------------
RemapStatesItem::RemapStatesItem(ComponentEditorTreeModel* model,
    LibraryInterface* libHandler, QSharedPointer<Component> component, ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
remapStates_(component->getRemapStates())
{
    foreach(QSharedPointer<RemapState> remapState, *remapStates_)
    {
        QSharedPointer<SingleRemapStateItem> singleRemapItem(new SingleRemapStateItem(remapState, model,
            libHandler, component, this));

        childItems_.append(singleRemapItem);
    }
}

//-----------------------------------------------------------------------------
// Function: RemapStatesItem::~RemapStatesItem()
//-----------------------------------------------------------------------------
RemapStatesItem::~RemapStatesItem()
{

}

//-----------------------------------------------------------------------------
// Function: RemapStatesItem::getFont()
//-----------------------------------------------------------------------------
QFont RemapStatesItem::getFont() const
{
    QFont font(ComponentEditorItem::getFont());
    font.setBold(!childItems_.isEmpty());
    return font;
}

//-----------------------------------------------------------------------------
// Function: RemapStatesItem::text()
//-----------------------------------------------------------------------------
QString RemapStatesItem::text() const
{
    return tr("Remap states");
}

//-----------------------------------------------------------------------------
// Function: RemapStatesItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* RemapStatesItem::editor()
{
    if (!editor_)
    {
        editor_ = new RemapStatesEditor(component_, libHandler_);
        editor_->setProtection(locked_);
        
        connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childAdded(int)), this, SLOT(onAddChild(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childRemoved(int)), this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
            this, SIGNAL(helpUrlRequested(QString const&)), Qt::UniqueConnection);
    }

    return editor_;
}

//-----------------------------------------------------------------------------
// Function: RemapStatesItem::getTooltip()
//-----------------------------------------------------------------------------
QString RemapStatesItem::getTooltip() const
{
    return tr("Contains the remap states of the component");
}

//-----------------------------------------------------------------------------
// Function: RemapStatesItem::createChild()
//-----------------------------------------------------------------------------
void RemapStatesItem::createChild(int index)
{
    QSharedPointer<SingleRemapStateItem> remapItem(new SingleRemapStateItem(remapStates_->at(index), model_,
        libHandler_, component_, this));

    remapItem->setLocked(locked_);

    childItems_.insert(index, remapItem);
}
