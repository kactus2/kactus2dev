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

#include <IPXACTmodels/addressblock.h>
#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/memorymap.h>
#include <IPXACTmodels/parameter.h>
#include <IPXACTmodels/registermodel.h>

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

        foreach (QSharedPointer<ModelParameter> modelParameter, *component_->getModelParameters())
        {
            if (modelParameter->getValueId() == parameterId)
            {
                return modelParameter;
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

        foreach (QSharedPointer<ModelParameter> componentModelParameter, *component_->getModelParameters())
        {
            if (componentModelParameter->getValueId() == id)
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

    return targetParameter->getName();
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

        foreach (QSharedPointer<ModelParameter> modelParameter, *component_->getModelParameters())
        {
            allParameterIds.append(modelParameter->getValueId());
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
    foreach (QSharedPointer<View> view, component_->getViews())
    {
        viewParameters.append(*view->getParameters());
        foreach(QSharedPointer<ModelParameter> moduleParameter, *view->getModuleParameters())
        {
            viewParameters.append(moduleParameter);
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
    foreach (QSharedPointer<BusInterface> busInterface, component_->getBusInterfaces())
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
    foreach (QSharedPointer<MemoryMap> memoryMap, component_->getMemoryMaps())
    {
        foreach (QSharedPointer<MemoryMapItem> blockItem, memoryMap->getItems())
        {
            QSharedPointer<AddressBlock> addressBlock = blockItem.dynamicCast<AddressBlock>();
            if (addressBlock)
            {
                foreach (QSharedPointer<RegisterModel> registerModel, addressBlock->getRegisterData())
                {
                    registerParameters.append(registerModel->getParameters());
                }
            }
        }
    }

    return registerParameters;
}
