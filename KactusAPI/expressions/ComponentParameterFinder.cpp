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

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Cpu.h>
#include <IPXACTmodels/Component/ComponentGenerator.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/MemoryBlockBase.h>
#include <IPXACTmodels/Component/RegisterBase.h>
#include <IPXACTmodels/Component/IndirectInterface.h>

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::ComponentParameterFinder()
//-----------------------------------------------------------------------------
ComponentParameterFinder::ComponentParameterFinder(QSharedPointer<Component const> component) noexcept :
component_(component)
{
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::ComponentParameterFinder()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> ComponentParameterFinder::getParameterWithID(QStringView parameterId) const
{
    return searchParameter(parameterId);
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::hasId()
//-----------------------------------------------------------------------------
bool ComponentParameterFinder::hasId(QStringView id) const
{
    return !component_.isNull() && searchParameter(id);
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::nameForId()
//-----------------------------------------------------------------------------
QString ComponentParameterFinder::nameForId(QStringView id) const
{
    if (QSharedPointer <Parameter> targetParameter = getParameterWithID(id); 
        targetParameter)
    {
        return targetParameter->name();
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::valueForId()
//-----------------------------------------------------------------------------
QString ComponentParameterFinder::valueForId(QStringView id) const
{
    if (QSharedPointer<Parameter> targetParameter = getParameterWithID(id); 
        targetParameter)
    {
        return targetParameter->getValue();
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::getAllParameterIds()
//-----------------------------------------------------------------------------
QStringList ComponentParameterFinder::getAllParameterIds() const
{
    QStringList allParameterIds;

    if (!component_.isNull())
    {
        for (QSharedPointer<Parameter> parameter : *component_->getParameters())
        {
            allParameterIds.append(parameter->getValueId());
        }

        for (QSharedPointer<Parameter> cpuParameter : allCpuParameters())
        {
            allParameterIds.append(cpuParameter->getValueId());
        }

        for (QSharedPointer<Parameter> generatorParameter : allGeneratorParameters())
        {
            allParameterIds.append(generatorParameter->getValueId());
        }

        for (QSharedPointer<Parameter> busInterfaceParameter : allBusInterfaceParameters())
        {
            allParameterIds.append(busInterfaceParameter->getValueId());
        }

        for (QSharedPointer<Parameter> addressSpaceParameter : allAddressSpaceParameters())
        {
            allParameterIds.append(addressSpaceParameter->getValueId());
        }

        for (QSharedPointer<Parameter> registerParameter : allRegisterParameters())
        {
            allParameterIds.append(registerParameter->getValueId());
        }

        for (auto indirectInterfaceParameter : allIndirectInterfacesParameters())
        {
            allParameterIds.append(indirectInterfaceParameter->getValueId());
        }
    }

    allParameterIds.removeAll(QString());
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
        parameterCount += allIndirectInterfacesParameters().count();
    }
    return parameterCount;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::registerParameterModel()
//-----------------------------------------------------------------------------
void ComponentParameterFinder::registerParameterModel(QAbstractItemModel const* /*model*/)
{
    // Nothing to do.
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::setComponent()
//-----------------------------------------------------------------------------
void ComponentParameterFinder::setComponent(QSharedPointer<Component const> component)
{
    component_ = component;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::searchParameter()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> ComponentParameterFinder::searchParameter(QStringView parameterId) const
{
    if (component_ == nullptr)
    {
        return nullptr;
    }

    for (QSharedPointer<Parameter> parameter : *component_->getParameters())
    {
        if (parameter->getValueId() == parameterId)
        {
            return parameter;
        }
    }

    for (QSharedPointer<Parameter> busInterfaceParameter : allBusInterfaceParameters())
    {
        if (busInterfaceParameter->getValueId() == parameterId)
        {
            return busInterfaceParameter;
        }
    }

    for (QSharedPointer<Parameter> cpuParameter : allCpuParameters())
    {
        if (cpuParameter->getValueId() == parameterId)
        {
            return cpuParameter;
        }
    }

    for (QSharedPointer<Parameter> generatorParameter : allGeneratorParameters())
    {
        if (generatorParameter->getValueId() == parameterId)
        {
            return generatorParameter;
        }
    }

    for (QSharedPointer<Parameter> addressSpaceParameter : allAddressSpaceParameters())
    {
        if (addressSpaceParameter->getValueId() == parameterId)
        {
            return addressSpaceParameter;
        }
    }

    for (QSharedPointer<Parameter> registerParameter : allRegisterParameters())
    {
        if (registerParameter->getValueId() == parameterId)
        {
            return registerParameter;
        }
    }

    for (auto indirectInterfaceParameter : allIndirectInterfacesParameters())
    {
        if (indirectInterfaceParameter->getValueId() == parameterId)
        {
            return indirectInterfaceParameter;
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
    for (QSharedPointer<BusInterface> busInterface : *component_->getBusInterfaces())
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
    for (QSharedPointer<Cpu> cpu : *component_->getCpus())
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
    for (QSharedPointer<ComponentGenerator> generator : *component_->getComponentGenerators())
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
    for (QSharedPointer<AddressSpace> addressSpace : *component_->getAddressSpaces())
    {
        parameters.append(*addressSpace->getParameters());

        if (addressSpace->getLocalMemoryMap())
        {
            for (QSharedPointer<MemoryBlockBase> memoryBlock : *addressSpace->getLocalMemoryMap()->getMemoryBlocks())
            {
                QSharedPointer<AddressBlock> addressBlock = memoryBlock.dynamicCast<AddressBlock>();
                if (addressBlock)
                {
                    for (QSharedPointer<RegisterBase> registerBase : *addressBlock->getRegisterData())
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
    for (QSharedPointer<MemoryMapBase> memoryMap : *component_->getMemoryMaps())
    {
        for (QSharedPointer<MemoryBlockBase> memoryBlock : *memoryMap->getMemoryBlocks())
        {
            QSharedPointer<AddressBlock> addressBlock = memoryBlock.dynamicCast<AddressBlock>();
            if (addressBlock)
            {
                for (QSharedPointer<RegisterBase> registerBase : *addressBlock->getRegisterData())
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
    for (QSharedPointer<BusInterface> busInterface : *component_->getBusInterfaces())
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

    for (QSharedPointer<MemoryMapBase> memoryMap : *component_->getMemoryMaps())
    {
        for (QSharedPointer<MemoryBlockBase> memoryBlock : *memoryMap->getMemoryBlocks())
        {
            QSharedPointer<AddressBlock> addressBlock = memoryBlock.dynamicCast<AddressBlock>();
            if (addressBlock)
            {
                for (QSharedPointer<RegisterBase> registerBase : *addressBlock->getRegisterData())
                {
                    parameterCount += registerBase->getParameters()->count();
                }
            }
        }
    }

    return parameterCount;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::allIndirectInterfacesParameters()
//-----------------------------------------------------------------------------
QList<QSharedPointer<Parameter> > ComponentParameterFinder::allIndirectInterfacesParameters() const
{
    QList<QSharedPointer<Parameter> > indirectInterfacesParameters;

    for (auto indirectInterface : *component_->getIndirectInterfaces())
    {
        indirectInterfacesParameters.append(*indirectInterface->getParameters());
    }

    return indirectInterfacesParameters;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::getComponent()
//-----------------------------------------------------------------------------
QSharedPointer<const Component> ComponentParameterFinder::getComponent() const noexcept
{
    return component_;
}
