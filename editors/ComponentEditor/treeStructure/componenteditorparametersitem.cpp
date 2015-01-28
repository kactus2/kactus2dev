/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorparametersitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorparametersitem.h"

#include <editors/ComponentEditor/parameters/parameterseditor.h>

#include <IPXACTmodels/choice.h>

ComponentEditorParametersItem::ComponentEditorParametersItem(
    ComponentEditorTreeModel* model, LibraryInterface* libHandler, QSharedPointer<Component> component,
    QSharedPointer<ReferenceCounter> refCounter, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter, ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent),
parameters_(component->getParameters())
{
    setReferenceCounter(refCounter);
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);
}

ComponentEditorParametersItem::~ComponentEditorParametersItem() 
{
}

QFont ComponentEditorParametersItem::getFont() const
{
    QFont font(ComponentEditorItem::getFont());    
    font.setBold(!parameters_.isEmpty());    
    return font;
}

QString ComponentEditorParametersItem::text() const
{
	return tr("Parameters");
}

bool ComponentEditorParametersItem::isValid() const
{
	return component_->validateParameters(parameters_);
}

ItemEditor* ComponentEditorParametersItem::editor()
{
	if (!editor_)
    {
		editor_ = new ParametersEditor(component_, libHandler_, parameterFinder_, expressionFormatter_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()),
			this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
			this, SIGNAL(helpUrlRequested(QString const&)));

        connectItemEditorToReferenceCounter();

        connectReferenceTree();
	}
	return editor_;
}

QString ComponentEditorParametersItem::getTooltip() const
{
	return tr("Contains the configurable elements of the component");
}
