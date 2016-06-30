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
#include <IPXACTmodels/common/ConfigurableElementValue.h>

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::ComponentParameterFinder()
//-----------------------------------------------------------------------------
ComponentParameterFinder::ComponentParameterFinder(QSharedPointer<Component> component,
	QString activeViewName):component_(component), activeViewName_(activeViewName)
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
	// First, search for the parameter corresponding the id.
	QSharedPointer<Parameter> parameter = searchParameter(parameterId);

    if (parameter)
	{
		// If parameter was found, provide a copy of it.
		QSharedPointer<Parameter> returnParameter = QSharedPointer<Parameter>(parameter);

		if ( cevs_ )
		{
			// If any configurable element values are defined, try to value referring to the parameter.
			foreach ( QSharedPointer<ConfigurableElementValue> cev, *cevs_ )
			{
				if (cev->getReferenceId() == parameterId)
				{
					// If a matching value was found, its value will become the value of the copy.
					returnParameter->setValue(cev->getConfigurableValue());
					break;
				}
			}
		}

		return returnParameter;
	}

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
// Function: ComponentParameterFinder::setActiveView()
//-----------------------------------------------------------------------------
void ComponentParameterFinder::setActiveView(QString const& view)
{
	activeViewName_ = view;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::setCEVs()
//-----------------------------------------------------------------------------
void ComponentParameterFinder::setCEVs(QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > values)
{
	cevs_ = values;
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
	QList<QSharedPointer<Parameter> > viewParameters;

	QSharedPointer<View> view = component_->getModel()->findView(activeViewName_);

	if (view)
	{
		QSharedPointer<ComponentInstantiation> instantiation = component_->getModel()->
			findComponentInstantiation(view->getComponentInstantiationRef());

		if (instantiation)
		{
			foreach (QSharedPointer<ModuleParameter> parameter, *instantiation->getModuleParameters())
			{
				viewParameters.append(parameter);
			}

			foreach (QSharedPointer<Parameter> parameter, *instantiation->getParameters())
			{
				viewParameters.append(parameter);
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
