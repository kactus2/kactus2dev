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
#include <editors/common/ComponentInstanceEditor/ConfigurableElementsFilter.h>
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
    auto filter = new ConfigurableElementsFilter(this);
    setModel(model_, filter);

    view_.setToolTip(QString());
    view_.setAlternatingRowColors(false);
    view_.setColumnHidden(ConfigurableElementsColumns::CHOICE, true);
    view_.setColumnHidden(ConfigurableElementsColumns::ARRAY_LEFT, true);
    view_.setColumnHidden(ConfigurableElementsColumns::ARRAY_RIGHT, true);

    connect(model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(delegate_, SIGNAL(addConfigurableElement(QString const&, QString const&, int)),
        model_, SLOT(onAddItem(QString const&, QString const&, int)), Qt::UniqueConnection);
    connect(delegate_, SIGNAL(removeConfigurableElement(QString const&, int)),
        model_, SLOT(onRemoveItem(QString const&, int)), Qt::UniqueConnection);
    connect(delegate_, SIGNAL(dataChangedInID(QString const&, QString const&)),
        model_, SLOT(emitDataChangeForID(QString const&, QString const&)), Qt::UniqueConnection);
    connect(model_, SIGNAL(invalidateFilter()), filter, SLOT(invalidate()), Qt::UniqueConnection);

    connect(filterSelection_, SIGNAL(clicked(bool)),
        filter, SLOT(setShowImmediateValues(bool)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsEditor::setComponent()
//-----------------------------------------------------------------------------
void ComponentInstanceConfigurableElementsEditor::setComponent(QSharedPointer<Component> component,
    QSharedPointer<ComponentInstance> instance, QSharedPointer<ViewConfiguration> viewConfiguration,
    QSharedPointer<IEditProvider> editProvider)
{
    model_->setParameters(component, instance, viewConfiguration);
    delegate_->setChoices(component->getChoices());
    delegate_->setEditProvider(editProvider);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsEditor::clear()
//-----------------------------------------------------------------------------
void ComponentInstanceConfigurableElementsEditor::clear() 
{
	model_->clear();
}
