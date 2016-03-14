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
ComponentParameterFinder::ComponentParameterFinder(QSharedPointer<Component> component):
component_(component)
{

}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::~ComponentParameterFinder()
//-----------------------------------------------------------------------------
ComponentParameterFinder::~ComponentParameterFinder()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::changeComponent()
//-----------------------------------------------------------------------------
void ComponentParameterFinder::changeComponent(QSharedPointer<Component> newComponent)
{
    component_ = newComponent;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::ComponentParameterFinder()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> ComponentParameterFinder::getParameterWithID(QString const& parameterId) const
{
    if (!component_.isNull())
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
    }

    return QSharedPointer<Parameter>(new Parameter);
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::hasId()
//-----------------------------------------------------------------------------
bool ComponentParameterFinder::hasId(QString const& id) const
{
    if (!component_.isNull())
    {
        foreach (QSharedPointer<Parameter> parameter, *component_->getParameters())
        {
            if (parameter->getValueId() == id)
            {
                return true;
            }
        }

        foreach (QSharedPointer<Parameter> viewParameter, allViewParameters())
        {
            if (viewParameter->getValueId() == id)
            {
                return true;
            }
        }

        foreach (QSharedPointer<Parameter> busInterfaceParameter, allBusInterfaceParameters())
        {
            if (busInterfaceParameter->getValueId() == id)
            {
                return true;
            }
        }

        foreach (QSharedPointer<Parameter> registerParameter, allRegisterParameters())
        {
            if (registerParameter->getValueId() == id)
            {
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::nameForId()
//-----------------------------------------------------------------------------
QString ComponentParameterFinder::nameForId(QString const& id) const
{
    QSharedPointer <Parameter> targetParameter = getParameterWithID(id);

    return targetParameter->name();
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::valueForId()
//-----------------------------------------------------------------------------
QString ComponentParameterFinder::valueForId(QString const& id) const
{
    QSharedPointer <Parameter> targetParameter = getParameterWithID(id);

    return targetParameter->getValue();
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
    QStringList allParameterIds = getAllParameterIds();

    return allParameterIds.size();
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::setComponent()
//-----------------------------------------------------------------------------
void ComponentParameterFinder::setComponent(QSharedPointer<Component> component)
{
    component_ = component;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::allViewParameters()
//-----------------------------------------------------------------------------
QList<QSharedPointer<Parameter> > ComponentParameterFinder::allViewParameters() const
{
    QList<QSharedPointer<Parameter> > viewParameters;
    foreach (QSharedPointer<View> view, *component_->getViews())
    {
        if (!view->getComponentInstantiationRef().isEmpty())
        {
            foreach (QSharedPointer<ComponentInstantiation> instantiation,
                *component_->getComponentInstantiations())
            {
                if (instantiation->name() == view->getComponentInstantiationRef())
                {
                    foreach (QSharedPointer<ModuleParameter> parameter, *instantiation->getModuleParameters())
                    {
                        viewParameters.append(parameter);
                    }
                    foreach (QSharedPointer<Parameter> parameter, *instantiation->getParameters())
                    {
                        viewParameters.append(parameter);
                    }

                    break;
                }
            }
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
                    foreach (QSharedPointer<Parameter> parameter, *registerBase->getParameters())
                    {
                        registerParameters.append(parameter);
                    }
                }
            }
        }
    }

    return registerParameters;
}
