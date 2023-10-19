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
    ExpressionSet parameterExpressions,
    ExpressionSet moduleParameterExpressions,
    ExpressionSet defaultExpressions,
    QAbstractItemModel* completionModel, QWidget *parent):
    QWidget(parent),
    parameterEditor_(parameterExpressions, defaultExpressions, completionModel, this),
    moduleParameterEditor_(moduleParameterExpressions, defaultExpressions, nullptr, this)
{
    parameterEditor_.setTitle(tr("Parameters"));
    connect(&parameterEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&parameterEditor_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
    connect(&parameterEditor_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);

    moduleParameterEditor_.setTitle(tr("Module parameters"));
    connect(&moduleParameterEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&moduleParameterEditor_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
    connect(&moduleParameterEditor_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(&parameterEditor_);
    layout->addWidget(&moduleParameterEditor_);
    layout->setContentsMargins(0, 0, 0, 0);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsEditor::setComponent()
//-----------------------------------------------------------------------------
void ComponentInstanceConfigurableElementsEditor::setComponent(QSharedPointer<Component> component,
    QSharedPointer<ComponentInstance> instance, QSharedPointer<ViewConfiguration> viewConfiguration,
    QSharedPointer<IEditProvider> editProvider)
{
    parameterEditor_.setEditProvider(editProvider);
    moduleParameterEditor_.setEditProvider(editProvider);
    moduleParameterEditor_.clear();

    auto identifier = tr("component %1").arg(component->getVlnv().toString());
    parameterEditor_.setParameters(identifier, component->getParameters(), component->getChoices(),
        component->getRevision(), instance->getConfigurableElementValues());

    QSharedPointer<QList<QSharedPointer<Parameter> > > moduleParameters(new QList<QSharedPointer<Parameter> >());

    if (viewConfiguration && !viewConfiguration->getViewReference().isEmpty())
    {
        QString referencedView = viewConfiguration->getViewReference();
        for (QSharedPointer<View> view : *component->getViews())
        {
            if (view->name() == referencedView)
            {
                QString referencedComponentInstantiation = view->getComponentInstantiationRef();
                if (!referencedComponentInstantiation.isEmpty())
                {
                    for (QSharedPointer<ComponentInstantiation> instantiation : *component->getComponentInstantiations())
                    {
                        if (instantiation->name() == referencedComponentInstantiation)
                        {
                            for (QSharedPointer<ModuleParameter> moduleParameter : *instantiation->getModuleParameters())
                            {
                                moduleParameters->append(moduleParameter);
                            }
                        }
                    }
                }
            }
        }

        moduleParameterEditor_.setParameters(identifier, moduleParameters, component->getChoices(), 
            component->getRevision(), viewConfiguration->getViewConfigurableElements());
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsEditor::clear()
//-----------------------------------------------------------------------------
void ComponentInstanceConfigurableElementsEditor::clear() 
{
    parameterEditor_.clear();
    moduleParameterEditor_.clear();
}
