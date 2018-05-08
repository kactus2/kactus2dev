//-----------------------------------------------------------------------------
// File: ComponentInstanceConfigurableElementsEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 01.09.2017
//
// Description:
// Editor for configurable elements of a component instance.
//-----------------------------------------------------------------------------

#include "ComponentInstanceConfigurableElementsEditor.h"

#include <editors/common/ComponentInstanceEditor/ConfigurableElementsColumns.h>
#include <editors/common/ComponentInstanceEditor/ComponentInstanceConfigurableElementsFilter.h>
#include <editors/common/ComponentInstanceEditor/ComponentInstanceConfigurableElementsModel.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/ComponentInstance.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsEditor::ComponentInstanceConfigurableElementsEditor()
//-----------------------------------------------------------------------------
ComponentInstanceConfigurableElementsEditor::ComponentInstanceConfigurableElementsEditor(
    QSharedPointer<ConfigurableElementFinder> elementFinder,
    QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> configurableElementFormatter,
    QSharedPointer<ExpressionFormatter> instanceExpressionFormatter,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ExpressionParser> instanceParser,
    QAbstractItemModel* completionModel, QWidget *parent):
ConfigurableElementEditor(parameterFinder, configurableElementFormatter, completionModel, parent),
model_(new ComponentInstanceConfigurableElementsModel(parameterFinder, elementFinder, configurableElementFormatter,
    instanceExpressionFormatter, expressionParser, instanceParser, this))
{
    ComponentInstanceConfigurableElementsFilter* filter(new ComponentInstanceConfigurableElementsFilter(this));
    setModel(model_, filter);

    hideUnnecessaryColumns();

    connect(model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(getDelegate(), SIGNAL(addConfigurableElement(QString const&, QString const&, QString const&, int)),
        model_, SLOT(onAddElement(QString const&, QString const&, QString const&, int)), Qt::UniqueConnection);
    connect(getDelegate(), SIGNAL(removeConfigurableElement(QString const&, QString const&, int)),
        model_, SLOT(onRemoveElement(QString const&, QString const&, int)), Qt::UniqueConnection);
    connect(getDelegate(), SIGNAL(dataChangedInID(QString const&, QString const&)),
        model_, SLOT(onDataChangedInID(QString const&, QString const&)), Qt::UniqueConnection);
    connect(model_, SIGNAL(invalidateFilter()), filter, SLOT(onInvalidateFilter()), Qt::UniqueConnection);

    connect(getFilterSelectionBox(), SIGNAL(clicked(bool)),
        model_, SIGNAL(showImmediateValuesInModels(bool)), Qt::UniqueConnection);
    connect(getFilterSelectionBox(), SIGNAL(clicked(bool)),
        filter, SLOT(setShowImmediateValues(bool)), Qt::UniqueConnection);
    connect(getFilterSelectionBox(), SIGNAL(clicked(bool)),
        this, SLOT(setFirstParentColumnsSpannable()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsEditor::~ComponentInstanceConfigurableElementsEditor()
//-----------------------------------------------------------------------------
ComponentInstanceConfigurableElementsEditor::~ComponentInstanceConfigurableElementsEditor() 
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsEditor::setComponent()
//-----------------------------------------------------------------------------
void ComponentInstanceConfigurableElementsEditor::setComponent(QSharedPointer<Component> component,
    QSharedPointer<ComponentInstance> instance, QSharedPointer<ViewConfiguration> viewConfiguration,
    QSharedPointer<IEditProvider> editProvider)
{
    model_->setParameters(component, instance, viewConfiguration);
    getDelegate()->setChoices(component->getChoices());
    getDelegate()->setEditProvider(editProvider);

    expandView();
    setFirstParentColumnsSpannable();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsEditor::clear()
//-----------------------------------------------------------------------------
void ComponentInstanceConfigurableElementsEditor::clear() 
{
	model_->clear();
}
