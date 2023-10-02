//-----------------------------------------------------------------------------
// File: ModesItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 08.08.2023
//
// Description:
// The Modes-item in the component editor navigation tree.
//-----------------------------------------------------------------------------

#include "ModesItem.h"
#include "SingleModeItem.h"

#include <editors/ComponentEditor/modes/ModesEditor.h>
#include <editors/common/ExpressionSet.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Mode.h>
#include <IPXACTmodels/Component/validators/ModeValidator.h>
#include <IPXACTmodels/Component/validators/PortSliceValidator.h>
 
//-----------------------------------------------------------------------------
// Function: ModesItem::ModesItem()
//-----------------------------------------------------------------------------
ModesItem::ModesItem(ComponentEditorTreeModel* model, LibraryInterface* libHandler,
    QSharedPointer<Component> component, 
    QSharedPointer<ReferenceCounter> referenceCounter,
    ExpressionSet expressions,
    ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
modes_(component->getModes()),
expressions_(expressions),
validator_(new ModeValidator(component, expressions.parser))
{
    setParameterFinder(expressions.finder);
    setExpressionFormatter(expressions.formatter);
    setReferenceCounter(referenceCounter);

    for (QSharedPointer<Mode> mode : *modes_)
    {
        QSharedPointer<SingleModeItem> singleRemapItem(new SingleModeItem(mode, model,
            libHandler, component, referenceCounter, expressions, validator_, this));

        childItems_.append(singleRemapItem);
    }
}

//-----------------------------------------------------------------------------
// Function: ModesItem::getFont()
//-----------------------------------------------------------------------------
QFont ModesItem::getFont() const
{
    QFont font(ComponentEditorItem::getFont());
    font.setBold(!childItems_.isEmpty());
    return font;
}

//-----------------------------------------------------------------------------
// Function: ModesItem::text()
//-----------------------------------------------------------------------------
QString ModesItem::text() const
{
    return tr("Modes");
}

//-----------------------------------------------------------------------------
// Function: ModesItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ModesItem::editor()
{
    if (!editor_)
    {
        editor_ = new ModesEditor(component_, libHandler_, expressions_);
        editor_->setProtection(locked_);
        
        connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childAdded(int)), this, SLOT(onAddChild(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childRemoved(int)), this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
            this, SIGNAL(helpUrlRequested(QString const&)), Qt::UniqueConnection);

        connectItemEditorToReferenceCounter();
    }

    return editor_;
}

//-----------------------------------------------------------------------------
// Function: ModesItem::getTooltip()
//-----------------------------------------------------------------------------
QString ModesItem::getTooltip() const
{
    return tr("Contains the operation modes of the component");
}

//-----------------------------------------------------------------------------
// Function: ModesItem::createChild()
//-----------------------------------------------------------------------------
void ModesItem::createChild(int index)
{
    QSharedPointer<SingleModeItem> modeItem(new SingleModeItem(modes_->at(index), model_,
        libHandler_, component_, referenceCounter_, expressions_, validator_, this));

    modeItem->setLocked(locked_);

    childItems_.insert(index, modeItem);
}
