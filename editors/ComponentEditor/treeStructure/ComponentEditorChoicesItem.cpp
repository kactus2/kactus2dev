//-----------------------------------------------------------------------------
// File: ComponentEditorChoicesItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: 
// Date: 
//
// Description:
// The choices item in the component editor navigation tree.
//-----------------------------------------------------------------------------

#include "ComponentEditorChoicesItem.h"

#include <editors/ComponentEditor/common/ExpressionParser.h>
#include <editors/ComponentEditor/choices/ChoicesEditor.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Choice.h>
#include <IPXACTmodels/Component/validators/ChoiceValidator.h>

//-----------------------------------------------------------------------------
// Function: ComponentEditorChoicesItem::ComponentEditorChoicesItem()
//-----------------------------------------------------------------------------
ComponentEditorChoicesItem::ComponentEditorChoicesItem(ComponentEditorTreeModel* model,
    LibraryInterface* libHandler, QSharedPointer<Component> component,
    QSharedPointer<ExpressionParser> expressionParser, ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
choiceValidator_(new ChoiceValidator(expressionParser))
{

}

//-----------------------------------------------------------------------------
// Function: ComponentEditorChoicesItem::~()
//-----------------------------------------------------------------------------
ComponentEditorChoicesItem::~ComponentEditorChoicesItem()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentEditorChoicesItem::getFont()
//-----------------------------------------------------------------------------
QFont ComponentEditorChoicesItem::getFont() const
{
    QFont font(ComponentEditorItem::getFont());
    font.setBold(!component_->getChoices()->isEmpty());
    return font;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorChoicesItem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorChoicesItem::getTooltip() const
{
    return tr("Contains the choices available in the component");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorChoicesItem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorChoicesItem::text() const
{
    return tr("Choices");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorChoicesItem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorChoicesItem::isValid() const
{
    QStringList choiceNames;
    foreach (QSharedPointer<Choice> choice, *component_->getChoices())
    {
        if (choiceNames.contains(choice->name()) || !choiceValidator_->validate(choice))
        {
            return false;
        }
        else
        {
            choiceNames.append(choice->name());
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorChoicesItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorChoicesItem::editor()
{
    if (!editor_)
    {
        editor_ = new ChoicesEditor(component_, choiceValidator_);
        editor_->setProtection(locked_);
        connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
            this, SIGNAL(helpUrlRequested(QString const&)), Qt::UniqueConnection);
    }

    return editor_;
}
