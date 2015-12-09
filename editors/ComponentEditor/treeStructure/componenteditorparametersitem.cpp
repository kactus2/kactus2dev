//-----------------------------------------------------------------------------
// File: componenteditorparametersitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 09.05.2012
//
// Description:
// The parameters-item in the component editor navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditorparametersitem.h"

#include <editors/ComponentEditor/parameters/parameterseditor.h>

#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>

#include <IPXACTmodels/common/validators/ParameterValidator2014.h>

#include <IPXACTmodels/Component/Component.h>

//-----------------------------------------------------------------------------
// Function: ComponentEditorParametersItem::ComponentEditorParametersItem()
//-----------------------------------------------------------------------------
ComponentEditorParametersItem::ComponentEditorParametersItem(ComponentEditorTreeModel* model,
    LibraryInterface* libHandler, QSharedPointer<Component> component, QSharedPointer<ReferenceCounter> refCounter,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ExpressionFormatter> expressionFormatter,
    ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
    expressionParser_(expressionParser),
    validator_(new ParameterValidator2014(expressionParser, component->getChoices()))
{
    setReferenceCounter(refCounter);
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorParametersItem::~ComponentEditorParametersItem()
//-----------------------------------------------------------------------------
ComponentEditorParametersItem::~ComponentEditorParametersItem() 
{

}

//-----------------------------------------------------------------------------
// Function: ComponentEditorParametersItem::getFont()
//-----------------------------------------------------------------------------
QFont ComponentEditorParametersItem::getFont() const
{
    QFont font(ComponentEditorItem::getFont());    
    font.setBold(component_->hasParameters());    
    return font;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorParametersItem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorParametersItem::text() const
{
	return tr("Parameters");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorParametersItem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorParametersItem::isValid() const
{
    QStringList parameterNames;
    foreach (QSharedPointer<Parameter> parameter, *component_->getParameters())
    {
        if (parameterNames.contains(parameter->name()))
        {
            return false;
        }
        else
        {
            parameterNames.append(parameter->name());
        }

        if (!validator_->validate(parameter))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorParametersItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorParametersItem::editor()
{
	if (!editor_)
    {
		editor_ = new ParametersEditor(component_, libHandler_, validator_, expressionParser_, 
            parameterFinder_, expressionFormatter_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));

        connectItemEditorToReferenceCounter();
        connectReferenceTree();
	}

	return editor_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorParametersItem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorParametersItem::getTooltip() const
{
	return tr("Contains the configurable elements of the component");
}
