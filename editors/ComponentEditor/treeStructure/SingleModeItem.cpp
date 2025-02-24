//-----------------------------------------------------------------------------
// File: SingleModeItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 08.08.2023
//
// Description:
// The single Mode item used in the component editor navigation tree.
//-----------------------------------------------------------------------------

#include "SingleModeItem.h"

#include <editors/ComponentEditor/modes/SingleModeEditor.h>
#include <editors/common/ExpressionSet.h>

#include <IPXACTmodels/Component/Mode.h>
#include <IPXACTmodels/Component/validators/ModeValidator.h>

//-----------------------------------------------------------------------------
// Function: SingleModeItem::SingleModeItem()
//-----------------------------------------------------------------------------
SingleModeItem::SingleModeItem(QSharedPointer<Mode> mode, ComponentEditorTreeModel* model,
    LibraryInterface* libHandler, QSharedPointer<Component> component,
    QSharedPointer<ReferenceCounter> referenceCounter, ExpressionSet expressions,
    QSharedPointer<ModeValidator> validator, QSharedPointer<ExpressionParser> modeConditionParser,
    ComponentEditorItem* parent) :
ComponentEditorItem(model, libHandler, component, parent),
mode_(mode),
validator_(validator),
expressions_(expressions),
modeConditionParser_(modeConditionParser)
{
    setParameterFinder(expressions_.finder);
    setExpressionFormatter(expressions_.formatter);
    setReferenceCounter(referenceCounter);
}

//-----------------------------------------------------------------------------
// Function: SingleModeItem::getTooltip()
//-----------------------------------------------------------------------------
QString SingleModeItem::getTooltip() const
{
    return tr("Specifies a mode of the component");
}

//-----------------------------------------------------------------------------
// Function: SingleModeItem::text()
//-----------------------------------------------------------------------------
QString SingleModeItem::text() const
{
    return mode_->name();
}

//-----------------------------------------------------------------------------
// Function: SingleModeItem::isValid()
//-----------------------------------------------------------------------------
bool SingleModeItem::isValid() const
{
    validator_->setConditionParser(modeConditionParser_); // Each mode item has its own condition parser.
    return validator_->validate(mode_);
}

//-----------------------------------------------------------------------------
// Function: SingleModeItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* SingleModeItem::editor()
{
    if (!editor_)
    {
        editor_ = new SingleModeEditor(component_, mode_, validator_, libHandler_, expressions_);
        editor_->setProtection(locked_);

        connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(errorMessage(const QString&)),
            this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
            this, SIGNAL(helpUrlRequested(QString const&)), Qt::UniqueConnection);

        connectItemEditorToReferenceCounter();
    }

    validator_->setConditionParser(modeConditionParser_); // Update validator parser, every mode has own parser
    return editor_;
}
