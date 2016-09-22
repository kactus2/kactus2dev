//-----------------------------------------------------------------------------
// File: ComponentParameterFinder.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 15.01.2015
//
// Description:
// The implementation for finding parameters with the correct ID.
//-----------------------------------------------------------------------------

#include "ComponentParameterFinder.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/MemoryBlockBase.h>
#include <IPXACTmodels/Component/RegisterBase.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/common/Parameter.h>

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::ComponentParameterFinder()
//-----------------------------------------------------------------------------
ComponentParameterFinder::ComponentParameterFinder(QSharedPointer<Component> component) : component_(component)
{
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::~ComponentParameterFinder()
//-----------------------------------------------------------------------------
ComponentParameterFinder::~ComponentParameterFinder()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::ComponentParameterFinder()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> ComponentParameterFinder::getParameterWithID(QString const& parameterId) const
{
    // First, search for the parameter corresponding the id.
    QSharedPointer<Parameter> parameter = searchParameter(parameterId);

    return parameter;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::hasId()
//-----------------------------------------------------------------------------
bool ComponentParameterFinder::hasId(QString const& id) const
{
    if (!component_.isNull() && searchParameter(id))
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::nameForId()
//-----------------------------------------------------------------------------
QString ComponentParameterFinder::nameForId(QString const& id) const
{
    QSharedPointer <Parameter> targetParameter = getParameterWithID(id);

    if (targetParameter)
    {
        return targetParameter->name();
    }

    return "";
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::valueForId()
//-----------------------------------------------------------------------------
QString ComponentParameterFinder::valueForId(QString const& id) const
{
    QSharedPointer<Parameter> targetParameter = getParameterWithID(id);

    if (targetParameter)
    {
        return targetParameter->getValue();
    }

    return "";
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::getAllParameterIds()
//-----------------------------------------------------------------------------
QStringList ComponentParameterFinder::getAllParameterIds() const
{
    QStringList allParameterIds;

    if (!component_.isNull())
    {
        foreach (QSharedPointer<Parameter> parameter, *component_->getParameters())
        {
            allParameterIds.append(parameter->getValueId());
        }

        foreach (QSharedPointer<Parameter> viewParameter, allViewParameters())
        {
            allParameterIds.append(viewParameter->getValueId());
        }

        foreach (QSharedPointer<Parameter> busInterfaceParameter, allBusInterfaceParameters())
        {
            allParameterIds.append(busInterfaceParameter->getValueId());
        }

        foreach (QSharedPointer<Parameter> registerParameter, allRegisterParameters())
        {
            allParameterIds.append(registerParameter->getValueId());
        }
    }

    allParameterIds.removeAll("");
    return allParameterIds;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::getNumberOfParameters()
//-----------------------------------------------------------------------------
int ComponentParameterFinder::getNumberOfParameters() const
{
    int parameterCount = 0;
    if (!component_.isNull())
    {
        parameterCount += component_->getParameters()->count();
        parameterCount += viewParameterCount();
        parameterCount += busInterfaceParameterCount();
        parameterCount += registerParameterCount();
    }
    return parameterCount;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::setComponent()
//-----------------------------------------------------------------------------
void ComponentParameterFinder::setComponent(QSharedPointer<Component> component)
{
    component_ = component;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::setActiveView()
//-----------------------------------------------------------------------------
void ComponentParameterFinder::setActiveView(QSharedPointer<View> view)
{
    activeView_ = view;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::setActiveView()
//-----------------------------------------------------------------------------
void ComponentParameterFinder::setActiveView(QString const& viewName)
{
    if (component_)
    {
        activeView_ = component_->getModel()->findView(viewName);
    }
    else
    {
        activeView_ = QSharedPointer<View>();
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::registerParameterModel()
//-----------------------------------------------------------------------------
void ComponentParameterFinder::registerParameterModel(QAbstractItemModel const* /*model*/)
{
    // Nothing to do.
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::searchParameter()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> ComponentParameterFinder::searchParameter(QString const& parameterId) const
{
    foreach (QSharedPointer<Parameter> parameter, *component_->getParameters())
    {
        if (parameter->getValueId() == parameterId)
        {
            return parameter;
        }
    }

    foreach (QSharedPointer<Parameter> viewParameter, allViewParameters())
    {
        if (viewParameter->getValueId() == parameterId)
        {
            return viewParameter;
        }
    }

    foreach (QSharedPointer<Parameter> busInterfaceParameter, allBusInterfaceParameters())
    {
        if (busInterfaceParameter->getValueId() == parameterId)
        {
            return busInterfaceParameter;
        }
    }

    foreach (QSharedPointer<Parameter> registerParameter, allRegisterParameters())
    {
        if (registerParameter->getValueId() == parameterId)
        {
            return registerParameter;
        }
    }

    return QSharedPointer<Parameter>();
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::allViewParameters()
//-----------------------------------------------------------------------------
QList<QSharedPointer<Parameter> > ComponentParameterFinder::allViewParameters() const
{
    // The parameters found in views.
    QList<QSharedPointer<Parameter> > viewParameters;

    // The list of views to go through.
    QSharedPointer<QList<QSharedPointer<View> > > views(new QList<QSharedPointer<View> >);

    // If an active views is set, use it only. Otherwise use all views in the component.
    if (activeView_)
    {
        views->append(activeView_);
    }
    else
    {
        views = component_->getViews();
    }

    // Go through the specified views.
    foreach (QSharedPointer<View> view, *views)
    {
        QSharedPointer<ComponentInstantiation> instantiation = component_->getModel()->
            findComponentInstantiation(view->getComponentInstantiationRef());

        // If an instantiation exists, append its module parameters as well as parameters.
        if (instantiation)
        {
            foreach (QSharedPointer<ModuleParameter> parameter, *instantiation->getModuleParameters())
            {
                viewParameters.append(parameter);
            }

            viewParameters.append(*instantiation->getParameters());
        }
    }

    return viewParameters;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::allBusInterfaceParameters()
//-----------------------------------------------------------------------------
QList<QSharedPointer<Parameter> > ComponentParameterFinder::allBusInterfaceParameters() const
{
    QList<QSharedPointer<Parameter> > busInterfaceParameters;
    foreach (QSharedPointer<BusInterface> busInterface, *component_->getBusInterfaces())
    {
        busInterfaceParameters.append(*busInterface->getParameters());
    }

    return busInterfaceParameters;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::allRegisterParameters()
//-----------------------------------------------------------------------------
QList<QSharedPointer<Parameter> > ComponentParameterFinder::allRegisterParameters() const
{
    QList<QSharedPointer<Parameter> > registerParameters;
    foreach (QSharedPointer<MemoryMapBase> memoryMap, *component_->getMemoryMaps())
    {
        foreach (QSharedPointer<MemoryBlockBase> memoryBlock, *memoryMap->getMemoryBlocks())
        {
            QSharedPointer<AddressBlock> addressBlock = memoryBlock.dynamicCast<AddressBlock>();
            if (addressBlock)
            {
                foreach (QSharedPointer<RegisterBase> registerBase, *addressBlock->getRegisterData())
                {
                    registerParameters.append(*registerBase->getParameters());
                }
            }
        }
    }

    return registerParameters;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::viewParameterCount()
//-----------------------------------------------------------------------------
int ComponentParameterFinder::viewParameterCount() const
{
    int parameterCount = 0;

    foreach (QSharedPointer<View> view, *component_->getViews())
    {
        QSharedPointer<ComponentInstantiation> instantiation = component_->getModel()->
            findComponentInstantiation(view->getComponentInstantiationRef());

        if (instantiation)
        {
            parameterCount += instantiation->getModuleParameters()->count();

            parameterCount += instantiation->getParameters()->count();
        }
    }

    return parameterCount;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::busInterfaceParameterCount()
//-----------------------------------------------------------------------------
int ComponentParameterFinder::busInterfaceParameterCount() const
{
    int parameterCount = 0;
    foreach (QSharedPointer<BusInterface> busInterface, *component_->getBusInterfaces())
    {
        parameterCount += busInterface->getParameters()->count();
    }

    return parameterCount;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::registerParameterCount()
//-----------------------------------------------------------------------------
int ComponentParameterFinder::registerParameterCount() const
{
    int parameterCount = 0;

    foreach (QSharedPointer<MemoryMapBase> memoryMap, *component_->getMemoryMaps())
    {
        foreach (QSharedPointer<MemoryBlockBase> memoryBlock, *memoryMap->getMemoryBlocks())
        {
            QSharedPointer<AddressBlock> addressBlock = memoryBlock.dynamicCast<AddressBlock>();
            if (addressBlock)
            {
                foreach (QSharedPointer<RegisterBase> registerBase, *addressBlock->getRegisterData())
                {
                    parameterCount += registerBase->getParameters()->count();
                }
            }
        }
    }

    return parameterCount;
}
