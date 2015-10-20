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

#include <IPXACTmodels/Component/Component.h>

//-----------------------------------------------------------------------------
// Function: componenteditorparametersitem::ComponentEditorParametersItem()
//-----------------------------------------------------------------------------
ComponentEditorParametersItem::ComponentEditorParametersItem(ComponentEditorTreeModel* model,
    LibraryInterface* libHandler, QSharedPointer<Component> component, QSharedPointer<ReferenceCounter> refCounter,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent),
parameters_(component->getParameters())
{
    setReferenceCounter(refCounter);
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);
}

//-----------------------------------------------------------------------------
// Function: componenteditorparametersitem::~ComponentEditorParametersItem()
//-----------------------------------------------------------------------------
ComponentEditorParametersItem::~ComponentEditorParametersItem() 
{

}

//-----------------------------------------------------------------------------
// Function: componenteditorparametersitem::getFont()
//-----------------------------------------------------------------------------
QFont ComponentEditorParametersItem::getFont() const
{
    QFont font(ComponentEditorItem::getFont());    
    font.setBold(!parameters_->isEmpty());    
    return font;
}

//-----------------------------------------------------------------------------
// Function: componenteditorparametersitem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorParametersItem::text() const
{
	return tr("Parameters");
}

//-----------------------------------------------------------------------------
// Function: componenteditorparametersitem::isValid()
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
    }
    return true;
}

//-----------------------------------------------------------------------------
// Function: componenteditorparametersitem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorParametersItem::editor()
{
	if (!editor_)
    {
		editor_ = new ParametersEditor(component_, libHandler_, parameterFinder_, expressionFormatter_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));

        connectItemEditorToReferenceCounter();
        connectReferenceTree();
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: componenteditorparametersitem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorParametersItem::getTooltip() const
{
	return tr("Contains the configurable elements of the component");
}
