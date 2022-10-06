//-----------------------------------------------------------------------------
// File: ComponentInstanceConfigurableElementsModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 17.08.2017
//
// Description:
// Model class to manage the configurable element values of component instances.
//-----------------------------------------------------------------------------

#include "ComponentInstanceConfigurableElementsModel.h"

#include <common/KactusColors.h>

#include <editors/common/ComponentInstanceEditor/ConfigurableElementsColumns.h>
#include <editors/common/ComponentInstanceEditor/ConfigurableElementsModel.h>
#include <editors/common/ComponentInstanceEditor/ParameterConfigurableElementsFilter.h>
#include <editors/common/ComponentInstanceEditor/MissingConfigurableElementsFilter.h>
#include <editors/common/ComponentInstanceEditor/EditorConfigurableElement.h>

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/ModuleParameter.h>

#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/Design/ComponentInstance.h>

#include <IPXACTmodels/designConfiguration/ViewConfiguration.h>

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsModel::ComponentInstanceConfigurableElementsModel()
//-----------------------------------------------------------------------------
ComponentInstanceConfigurableElementsModel::ComponentInstanceConfigurableElementsModel(
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ConfigurableElementFinder> elementFinder,
    QSharedPointer<ExpressionFormatter> elementExpressionFormatter,
    QSharedPointer<ExpressionFormatter> defaultValueFormatter,
    QSharedPointer<ExpressionParser> elementExpressionParser, QSharedPointer<ExpressionParser> defaultValueParser,
    QObject* parent):
    ConfigurableElementsModel(parameterFinder, elementExpressionFormatter, defaultValueFormatter, 
        elementExpressionParser, defaultValueParser, parent),
elementFinder_(elementFinder)
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsModel::setParameters()
//-----------------------------------------------------------------------------
void ComponentInstanceConfigurableElementsModel::setParameters(QSharedPointer<Component> component,
    QSharedPointer<ComponentInstance> instance, QSharedPointer<ViewConfiguration> viewConfiguration)
{
    
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > instanceConfigurableElements(
        new QList<QSharedPointer<ConfigurableElementValue> > ());

    if (instance)
    {
        instanceConfigurableElements->append(*instance->getConfigurableElementValues());
    }
    if (viewConfiguration)
    {
        instanceConfigurableElements->append(*viewConfiguration->getViewConfigurableElements());
    }

    containingItemName_ = instance->getInstanceName();
    choices_ = component->getChoices();
    itemConfigurableElementValues_ = instanceConfigurableElements;

    beginResetModel();

    configurableElements_.clear();

    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters = component->getParameters();
    if (parameters)
    {
        for (QSharedPointer<Parameter> parameterPointer : *parameters)
        {
            QString configuratedValue = parameterPointer->getValue();
            QString evaluatedDefaultValue = evaluateValue(ConfigurableElementsColumns::DEFAULT_VALUE,
                parameterPointer->getChoiceRef(), parameterPointer->getValue());

            QSharedPointer<EditorConfigurableElement> newElement(new EditorConfigurableElement(
                parameterPointer, evaluatedDefaultValue, configuratedValue));
            configurableElements_.append(newElement);
        }
    }

    QSharedPointer<QList<QSharedPointer<Parameter> > > moduleParameters =
        getModuleParameters(component, viewConfiguration);
    if (moduleParameters)
    {
        for (QSharedPointer<Parameter> moduleParameterPointer : *moduleParameters)
        {
            QString configuratedValue = moduleParameterPointer->getValue();
            QString evaluatedDefaultValue = evaluateValue(ConfigurableElementsColumns::DEFAULT_VALUE,
                moduleParameterPointer->getChoiceRef(), moduleParameterPointer->getValue());

            QSharedPointer<EditorConfigurableElement> newElement(new EditorConfigurableElement(
                moduleParameterPointer, evaluatedDefaultValue, configuratedValue));
            newElement->setType(QString("Module Parameter"));
            configurableElements_.append(newElement);
        }
    }

    validator_->componentChange(component->getChoices());

    restoreStoredConfigurableElements();

    elementFinder_->setConfigurableElementList(configurableElements_);

    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsModel::getModuleParameters()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Parameter> > > ComponentInstanceConfigurableElementsModel::
    getModuleParameters(QSharedPointer<Component> component, QSharedPointer<ViewConfiguration> viewConfiguration)
{
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters (new QList<QSharedPointer<Parameter> > ());

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
                                parameters->append(moduleParameter);
                            }
                            break;
                        }
                    }
                }

                break;
            }
        }
    }

    return parameters;
}
