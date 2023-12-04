//-----------------------------------------------------------------------------
// File: componenteditorparametersitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 09.05.2012
//
// Description:
// The parameters-item in the component editor navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditorparametersitem.h"

#include <KactusAPI/include/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/parameters/parameterseditor.h>
#include <KactusAPI/include/ParametersInterface.h>

#include <IPXACTmodels/common/validators/ParameterValidator.h>
#include <IPXACTmodels/Component/Component.h>

//-----------------------------------------------------------------------------
// Function: ComponentEditorParametersItem::ComponentEditorParametersItem()
//-----------------------------------------------------------------------------
ComponentEditorParametersItem::ComponentEditorParametersItem(ComponentEditorTreeModel* model,
    LibraryInterface* libHandler, QSharedPointer<Component> component, QSharedPointer<ReferenceCounter> refCounter,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ExpressionFormatter> expressionFormatter, ComponentEditorItem* parent):
ParameterizableItem(model, libHandler, component, parent),
expressionParser_(expressionParser),
validator_(new ParameterValidator(expressionParser, component->getChoices(), component->getRevision())),
parameterInterface_(new ParametersInterface(validator_, expressionParser, expressionFormatter))
{
    parameterInterface_->setParameters(component->getParameters());
    parameterInterface_->setChoices(component->getChoices());

    setReferenceCounter(refCounter);
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);
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
    return parameterInterface_->validateItems();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorParametersItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorParametersItem::editor()
{
	if (!editor_)
    {
		editor_ = new ParametersEditor(component_, libHandler_, parameterFinder_, expressionParser_,
            expressionFormatter_, parameterInterface_);
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
