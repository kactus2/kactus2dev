//-----------------------------------------------------------------------------
// File: ComponentParameterFinder.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 15.01.2015
//
// Description:
// The implementation for finding parameters with the correct ID.
//-----------------------------------------------------------------------------

#include "ComponentParameterFinder.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Cpu.h>
#include <IPXACTmodels/Component/ComponentGenerator.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/MemoryBlockBase.h>
#include <IPXACTmodels/Component/RegisterBase.h>
#include <IPXACTmodels/common/Parameter.h>

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::ComponentParameterFinder()
//-----------------------------------------------------------------------------
ComponentParameterFinder::ComponentParameterFinder(QSharedPointer<Component const> component) : component_(component)
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

        foreach (QSharedPointer<Parameter> cpuParameter, allCpuParameters())
        {
            allParameterIds.append(cpuParameter->getValueId());
        }

        foreach (QSharedPointer<Parameter> generatorParameter, allGeneratorParameters())
        {
            allParameterIds.append(generatorParameter->getValueId());
        }

        foreach (QSharedPointer<Parameter> busInterfaceParameter, allBusInterfaceParameters())
        {
            allParameterIds.append(busInterfaceParameter->getValueId());
        }

        foreach (QSharedPointer<Parameter> addressSpaceParameter, allAddressSpaceParameters())
        {
            allParameterIds.append(addressSpaceParameter->getValueId());
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
        parameterCount += allGeneratorParameters().count();
        parameterCount += allCpuParameters().count();
        parameterCount += allAddressSpaceParameters().count();
        parameterCount += busInterfaceParameterCount();
        parameterCount += registerParameterCount();
    }
    return parameterCount;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::setComponent()
//-----------------------------------------------------------------------------
void ComponentParameterFinder::setComponent(QSharedPointer<Component const> component)
{
    component_ = component;
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
    if (component_)
    {
        foreach (QSharedPointer<Parameter> parameter, *component_->getParameters())
        {
            if (parameter->getValueId() == parameterId)
            {
                return parameter;
            }
        }

        foreach (QSharedPointer<Parameter> busInterfaceParameter, allBusInterfaceParameters())
        {
            if (busInterfaceParameter->getValueId() == parameterId)
            {
                return busInterfaceParameter;
            }
        }

        foreach (QSharedPointer<Parameter> cpuParameter, allCpuParameters())
        {
            if (cpuParameter->getValueId() == parameterId)
            {
                return cpuParameter;
            }
        }

        foreach (QSharedPointer<Parameter> generatorParameter, allGeneratorParameters())
        {
            if (generatorParameter->getValueId() == parameterId)
            {
                return generatorParameter;
            }
        }

        foreach (QSharedPointer<Parameter> addressSpaceParameter, allAddressSpaceParameters())
        {
            if (addressSpaceParameter->getValueId() == parameterId)
            {
                return addressSpaceParameter;
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

    return QSharedPointer<Parameter>();
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
// Function: ComponentParameterFinder::allCpuParameters()
//-----------------------------------------------------------------------------
QList<QSharedPointer<Parameter> > ComponentParameterFinder::allCpuParameters() const
{
    QList<QSharedPointer<Parameter> > cpuParameters;
    foreach (QSharedPointer<Cpu> cpu, *component_->getCpus())
    {
        cpuParameters.append(*cpu->getParameters());
    }

    return cpuParameters;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::allGeneratorParameters()
//-----------------------------------------------------------------------------
QList<QSharedPointer<Parameter> > ComponentParameterFinder::allGeneratorParameters() const
{
    QList<QSharedPointer<Parameter> > generatorParameters;
    foreach (QSharedPointer<ComponentGenerator> generator, *component_->getComponentGenerators())
    {
        generatorParameters.append(*generator->getParameters());
    }

    return generatorParameters;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::allAddressSpaceParameters()
//-----------------------------------------------------------------------------
QList<QSharedPointer<Parameter> > ComponentParameterFinder::allAddressSpaceParameters() const
{
    QList<QSharedPointer<Parameter> > parameters;
    foreach (QSharedPointer<AddressSpace> addressSpace, *component_->getAddressSpaces())
    {
        parameters.append(*addressSpace->getParameters());

        if (addressSpace->getLocalMemoryMap())
        {
            foreach (QSharedPointer<MemoryBlockBase> memoryBlock, *addressSpace->getLocalMemoryMap()->getMemoryBlocks())
            {
                QSharedPointer<AddressBlock> addressBlock = memoryBlock.dynamicCast<AddressBlock>();
                if (addressBlock)
                {
                    foreach (QSharedPointer<RegisterBase> registerBase, *addressBlock->getRegisterData())
                    {
                        parameters.append(*registerBase->getParameters());
                    }
                }
            }
        }
    }

    return parameters;
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
