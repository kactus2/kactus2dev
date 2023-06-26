//-----------------------------------------------------------------------------
// File: ComponentParameterReferenceCounter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 04.10.2017
//
// Description:
// Calculates the amount of references made to component parameters.
//-----------------------------------------------------------------------------

#include "ComponentParameterReferenceCounter.h"

#include <IPXACTmodels/common/ModuleParameter.h>
#include <IPXACTmodels/common/Parameter.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Cpu.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/BuildCommand.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/MemoryRemap.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/Region.h>
#include <IPXACTmodels/Component/RegisterBase.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/RegisterFile.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/WriteValueConstraint.h>
#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/MirroredSlaveInterface.h>
#include <IPXACTmodels/Component/MasterInterface.h>
#include <IPXACTmodels/Component/RemapState.h>
#include <IPXACTmodels/Component/RemapPort.h>
#include <IPXACTmodels/Component/IndirectInterface.h>

#include <KactusAPI/include/AbstractParameterInterface.h>

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::ComponentParameterReferenceCounter()
//-----------------------------------------------------------------------------
ComponentParameterReferenceCounter::ComponentParameterReferenceCounter(
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<Component> component):
ParameterReferenceCounter(parameterFinder),
component_(component)
{

}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::setComponent()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceCounter::setComponent(QSharedPointer<Component> newComponent)
{
    component_ = newComponent;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::recalculateReferencesToParameters()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceCounter::recalculateReferencesToParameters(QVector<QString> const& parameterList,
    AbstractParameterInterface* parameterInterface)
{
    for (auto parameterName : parameterList)
    {
        int referenceCount = 0;
        QString parameterID = QString::fromStdString(parameterInterface->getID(parameterName.toStdString()));
        if (!parameterID.isEmpty())
        {
            referenceCount += countReferencesInFileSets(parameterID);
            referenceCount += countReferencesInParameters(parameterID, component_->getParameters());
            referenceCount += countReferencesInMemoryMaps(parameterID);
            referenceCount += countReferencesInAddressSpaces(parameterID);
            referenceCount += countReferencesInInstantiations(parameterID);
            referenceCount += countReferencesInPorts(parameterID);
            referenceCount += countReferencesInBusInterfaces(parameterID);
            referenceCount += countReferencesInRemapStates(parameterID);
            referenceCount += countReferencesInIndirectInterfaces(parameterID);
            referenceCount += countReferencesInCpus(parameterID);

            parameterInterface->setUsageCount(parameterName.toStdString(), referenceCount);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInFileSets()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInFileSets(QString const& parameterID) const
{
    int referenceCount = 0;

    for (QSharedPointer<FileSet> fileSet : *component_->getFileSets())
    {
        referenceCount += countReferencesInSingleFileSet(parameterID, fileSet);
    }

    return referenceCount;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInSingleFileSet()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInSingleFileSet(QString const& parameterID,
    QSharedPointer<FileSet> fileSet) const
{
    int referenceCount = 0;

    referenceCount += countReferencesInFileBuilders(parameterID, fileSet->getDefaultFileBuilders());
    referenceCount += countReferencesInFiles(parameterID, fileSet->getFiles());

    return referenceCount;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInFileBuilders()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInFileBuilders(QString const& parameterID,
    QSharedPointer<QList<QSharedPointer<FileBuilder> > > builders) const
{
    int referenceCount = 0;

    for (QSharedPointer<FileBuilder> fileBuilder : *builders)
    {
        referenceCount += countReferencesInSingleFileBuilder(parameterID, fileBuilder);
    }

    return referenceCount;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInSingleFileBuilder()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInSingleFileBuilder(QString const& parameterID,
    QSharedPointer<FileBuilder> builder) const
{
    return countReferencesInExpression(parameterID, builder->getReplaceDefaultFlags());
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInFiles()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInFiles(QString const& parameterID,
    QSharedPointer<QList<QSharedPointer<File> > > files) const
{
    int referenceCount = 0;

    for (QSharedPointer<File> singleFile : *files)
    {
        referenceCount += countReferencesInSingleFile(parameterID, singleFile);
    }

    return referenceCount;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInSingleFile()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInSingleFile(QString const& parameterID,
    QSharedPointer<File> file) const
{
    if (file->getBuildCommand())
    {
        return countReferencesInBuildCommand(parameterID, file->getBuildCommand());
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInBuildCommand()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInBuildCommand(QString const& parameterID,
    QSharedPointer<BuildCommand> buildCommand) const
{
    return countReferencesInExpression(parameterID, buildCommand->getReplaceDefaultFlags());
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInMemoryMaps()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInMemoryMaps(QString const& parameterID) const
{
    int referenceCount = 0;

    for (QSharedPointer<MemoryMap> memoryMap : *component_->getMemoryMaps())
    {
        referenceCount += countReferencesInSingleMemoryMap(parameterID, memoryMap);
    }

    return referenceCount;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInSingleMemoryMap()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInSingleMemoryMap(QString const& parameterID,
    QSharedPointer<MemoryMap> memoryMap) const
{
    int referenceCount = 0;
    
    referenceCount += countReferencesInBaseMemoryMap(parameterID, memoryMap);

    for (QSharedPointer<MemoryRemap> remap : *memoryMap->getMemoryRemaps())
    {
        referenceCount += countReferencesInBaseMemoryMap(parameterID, remap);
    }

    return referenceCount;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInBaseMemoryMap()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInBaseMemoryMap(QString const& parameterID,
    QSharedPointer<MemoryMapBase> memoryMap) const
{
    int referenceCount = 0;
    referenceCount += countReferencesInMemoryMapValues(parameterID, memoryMap);

    for (QSharedPointer<MemoryBlockBase> memoryBlock : *memoryMap->getMemoryBlocks())
    {
        QSharedPointer<AddressBlock> addressBlock = memoryBlock.dynamicCast<AddressBlock>();
        if (addressBlock)
        {
            referenceCount += countReferenceInAddressBlock(parameterID, addressBlock);
        }
    }

    return referenceCount;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInMemoryMapValues()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInMemoryMapValues(QString const& parameterID,
    QSharedPointer<MemoryMapBase> memoryMap) const
{
    int referenceCount = 0;

    referenceCount += countReferencesInExpression(parameterID, memoryMap->getIsPresent());

    return referenceCount;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferenceInAddressBlock()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferenceInAddressBlock(QString const& parameterID,
    QSharedPointer<AddressBlock> addressBlock) const
{
    int referenceCount = 0;

    referenceCount += countReferencesInAddressBlockValues(parameterID, addressBlock);
    referenceCount += countReferencesInRegisters(parameterID, addressBlock->getRegisterData());
    referenceCount += countReferencesInRegisterFiles(parameterID, addressBlock->getRegisterData());

    return referenceCount;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInAddressBlockValues()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInAddressBlockValues(QString const& parameterID,
    QSharedPointer<AddressBlock> addressBlock) const
{
    int referenceCount = 0;

    referenceCount += countReferencesInExpression(parameterID, addressBlock->getBaseAddress());
    referenceCount += countReferencesInExpression(parameterID, addressBlock->getRange());
    referenceCount += countReferencesInExpression(parameterID, addressBlock->getWidth());
    referenceCount += countReferencesInExpression(parameterID, addressBlock->getIsPresent());

    return referenceCount;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInRegisters()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInRegisters(QString const& parameterID,
    QSharedPointer<QList<QSharedPointer<RegisterBase>>> registers) const
{
    int referenceCounter = 0;

    for (QSharedPointer<RegisterBase> registerItem : *registers)
    {
        QSharedPointer<Register> targetRegister = registerItem.dynamicCast<Register>();
        if (targetRegister)
        {
            referenceCounter += countReferencesInSingleRegister(parameterID, targetRegister);
        }
    }

    return referenceCounter;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInRegisterFiles()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInRegisterFiles(QString const& parameterID,
    QSharedPointer<QList<QSharedPointer<RegisterBase>>> const& files) const
{
    int referenceCounter = 0;
    for (auto registerData : *files)
    {
        QSharedPointer<RegisterFile> targetFile = registerData.dynamicCast<RegisterFile>();
        if (targetFile)
        {
            referenceCounter += countReferencesInSingleRegisterFile(parameterID, targetFile);
        }
    }

    return referenceCounter;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInSingleRegister()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInSingleRegister(QString const& parameterID,
    QSharedPointer<Register> targetRegister) const
{
    int referenceCount = 0;

    referenceCount += countReferencesInSingleBaseRegister(parameterID, targetRegister);
    referenceCount += countReferencesInExpression(parameterID, targetRegister->getSize());
    referenceCount += countReferencesInFields(parameterID, targetRegister->getFields());

    return referenceCount;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInSingleBaseRegister()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInSingleBaseRegister(QString const& parameterID,
    QSharedPointer<RegisterBase> baseRegister) const
{
    int referenceCount = 0;

    referenceCount += countReferencesInExpression(parameterID, baseRegister->getAddressOffset());
    referenceCount += countReferencesInExpression(parameterID, baseRegister->getDimension());
    referenceCount += countReferencesInExpression(parameterID, baseRegister->getIsPresent());

    return referenceCount;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInSingleRegisterFile()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInSingleRegisterFile(QString const& parameterID,
    QSharedPointer<RegisterFile> targetFile) const
{
    int referenceCount = 0;

    referenceCount += countReferencesInSingleBaseRegister(parameterID, targetFile);
    referenceCount += countReferencesInExpression(parameterID, targetFile->getRange());
    referenceCount += countReferencesInRegisters(parameterID, targetFile->getRegisterData());
    referenceCount += countReferencesInRegisterFiles(parameterID, targetFile->getRegisterData());

    return referenceCount;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInFields()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInFields(QString const& parameterID,
    QSharedPointer<QList<QSharedPointer<Field> > > fields) const
{
    int referenceCount = 0;

    for (QSharedPointer<Field> registerField : *fields)
    {
        referenceCount += countReferencesInSingleField(parameterID, registerField);
    }

    return referenceCount;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInField()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInSingleField(QString const& parameterID,
    QSharedPointer<Field> registerField) const
{
    int referenceCount = 0;

    referenceCount += countReferencesInExpression(parameterID, registerField->getBitOffset());
    referenceCount += countReferencesInExpression(parameterID, registerField->getBitWidth());
    referenceCount += countReferencesInExpression(parameterID, registerField->getIsPresent());
    referenceCount += countReferencesInFieldResets(parameterID, registerField->getResets());
    referenceCount += countReferencesInWriteConstraint(parameterID, registerField->getWriteConstraint());

    return referenceCount;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInFieldResets()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInFieldResets(QString const& parameterID,
    QSharedPointer<QList<QSharedPointer<FieldReset> > > resets) const
{
    int referenceCount = 0;

    for (auto fieldReset : *resets)
    {
        referenceCount += countReferencesInSingleFieldReset(parameterID, fieldReset);
    }

    return referenceCount;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInSingleFieldReset()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInSingleFieldReset(QString const& parameterID,
    QSharedPointer<FieldReset> fieldReset) const
{
    int referenceCount = 0;

    referenceCount += countReferencesInExpression(parameterID, fieldReset->getResetValue());
    referenceCount += countReferencesInExpression(parameterID, fieldReset->getResetMask());

    return referenceCount;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInWriteConstraint()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInWriteConstraint(QString const& parameterID,
    QSharedPointer<WriteValueConstraint> writeConstraint) const
{
    int referenceCount = 0;

    if (writeConstraint)
    {
        referenceCount += countReferencesInExpression(parameterID, writeConstraint->getMinimum());
        referenceCount += countReferencesInExpression(parameterID, writeConstraint->getMaximum());
    }

    return referenceCount;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInAddressSpaces()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInAddressSpaces(QString const& parameterID) const
{
    int referenceCount = 0;

    for (QSharedPointer<AddressSpace> space : *component_->getAddressSpaces())
    {
        referenceCount += countReferencesInSingleAddressSpace(parameterID, space);
    }

    return referenceCount;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInSingleAddressSpace()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInSingleAddressSpace(QString const& parameterID,
    QSharedPointer<AddressSpace> space) const
{
    int referenceCount = 0;

    referenceCount += countReferencesInSingleAddressSpaceItems(parameterID, space);
    if (space->getLocalMemoryMap())
    {
        referenceCount += countReferencesInBaseMemoryMap(parameterID, space->getLocalMemoryMap());
    }

    referenceCount += countReferencesInSegments(parameterID, space->getSegments());

    return referenceCount;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInSingleAddressSpaceItems()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInSingleAddressSpaceItems(QString const& parameterID,
    QSharedPointer<AddressSpace> space) const
{
    int referenceCount = 0;

    referenceCount += countReferencesInExpression(parameterID, space->getWidth());
    referenceCount += countReferencesInExpression(parameterID, space->getRange());

    return referenceCount;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInSegments()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInSegments(QString const& parameterID,
    QSharedPointer<QList<QSharedPointer<Segment> > > segments) const
{
    int referenceCount = 0;

    for (QSharedPointer<Segment> segment : *segments)
    {
        referenceCount += countReferencesInSingleSegment(parameterID, segment);
    }

    return referenceCount;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInSegment()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInSingleSegment(QString const& parameterID,
    QSharedPointer<Segment> segment) const
{
    int referenceCounter = 0;

    referenceCounter += countReferencesInExpression(parameterID, segment->getAddressOffset());
    referenceCounter += countReferencesInExpression(parameterID, segment->getRange());

    return referenceCounter;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInInstantiations()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInInstantiations(QString const& parameterID) const
{
    int referenceCounter = 0;

    referenceCounter +=
        countReferencesInComponentInstantiations(parameterID, component_->getComponentInstantiations());
    referenceCounter += countReferencesInDesignConfigurationInstantiations(
        parameterID, component_->getDesignConfigurationInstantiations());
    referenceCounter += countReferencesInDesignInstantiations(parameterID);

    return referenceCounter;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInComponentInstantiations()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInComponentInstantiations(QString const& parameterID,
    QSharedPointer<QList<QSharedPointer<ComponentInstantiation> > > instantiations) const
{
    int referenceCounter = 0;

    for (QSharedPointer<ComponentInstantiation> singleInstantiation : *instantiations)
    {
        referenceCounter += countReferencesInSingleComponentInstantiation(parameterID, singleInstantiation);
    }

    return referenceCounter;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInSingleComponentInstantiation()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInSingleComponentInstantiation(QString const& parameterID,
    QSharedPointer<ComponentInstantiation> instantiation) const
{
    int referenceCounter = 0;

    referenceCounter += countReferencesInModuleParameters(parameterID, instantiation->getModuleParameters());
    referenceCounter += countReferencesInParameters(parameterID, instantiation->getParameters());
    referenceCounter += countReferencesInFileBuilders(parameterID, instantiation->getDefaultFileBuilders());

    return referenceCounter;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInModuleParameters()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInModuleParameters(QString const& parameterID,
    QSharedPointer<QList<QSharedPointer<ModuleParameter> > > moduleParameters) const
{
    int referenceCounter = 0;

    for (QSharedPointer<ModuleParameter> parameter : *moduleParameters)
    {
        referenceCounter += countReferencesInSingleParameter(parameterID, parameter);
    }

    return referenceCounter;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInDesignConfigurationInstantiations()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInDesignConfigurationInstantiations(
    QString const& parameterID,
    QSharedPointer<QList<QSharedPointer<DesignConfigurationInstantiation> > > instantiations) const
{
    int referenceCounter = 0;

    for (QSharedPointer<DesignConfigurationInstantiation> configurationInstantiation : *instantiations)
    {
        referenceCounter +=
            countReferencesInSingleDesignConfigurationInstantiation(parameterID, configurationInstantiation);
    }

    return referenceCounter;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInDesignConfigurationInstantiation()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInSingleDesignConfigurationInstantiation(
    QString const& parameterID, QSharedPointer<DesignConfigurationInstantiation> instantiation) const
{
    int refererenceCounter = 0;

    refererenceCounter += countReferencesInParameters(parameterID, instantiation->getParameters());
    
    QSharedPointer<ConfigurableVLNVReference> configurationReference =
        instantiation->getDesignConfigurationReference();
    if (configurationReference && configurationReference->isValid())
    {
        refererenceCounter += countReferencesInConfigurableElementValues(
            parameterID, configurationReference->getConfigurableElementValues());
    }

    return refererenceCounter;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInDesignInstantiations()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInDesignInstantiations(QString const& parameterID) const
{
    int referenceCounter = 0;

    for (QSharedPointer<DesignInstantiation> instantiation : *component_->getDesignInstantiations())
    {
        referenceCounter += countReferencesInSingleDesigninstantiation(parameterID, instantiation);
    }

    return referenceCounter;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInDesigninstantiation()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInSingleDesigninstantiation(QString const& parameterID,
    QSharedPointer<DesignInstantiation> instantiation) const
{
    int referenceCounter = 0;

    QSharedPointer<ConfigurableVLNVReference> designReference = instantiation->getDesignReference();
    if (designReference && designReference->isValid())
    {
        referenceCounter += countReferencesInConfigurableElementValues(
            parameterID, designReference->getConfigurableElementValues());
    }

    return referenceCounter;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInPorts()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInPorts(QString const& parameterID) const
{
    int referenceCounter = 0;

    for (QSharedPointer<Port> port : *component_->getPorts())
    {
        referenceCounter += countReferencesInSinglePort(parameterID, port);
    }

    return referenceCounter;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInSinglePort()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInSinglePort(QString const& parameterID,
    QSharedPointer<Port> port) const
{
    int referenceCounter = 0;

    referenceCounter += countReferencesInExpression(parameterID, port->getLeftBound());
    referenceCounter += countReferencesInExpression(parameterID, port->getRightBound());
    referenceCounter += countReferencesInExpression(parameterID, port->getArrayLeft());
    referenceCounter += countReferencesInExpression(parameterID, port->getArrayRight());
    referenceCounter += countReferencesInExpression(parameterID, port->getDefaultValue());

    return referenceCounter;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInBusInterfaces()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInBusInterfaces(QString const& parameterID) const
{
    int referenceCounter = 0;

    for (QSharedPointer<BusInterface> busInterface : *component_->getBusInterfaces())
    {
        referenceCounter += countReferencesInSingleBusInterface(parameterID, busInterface);
    }

    return referenceCounter;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInSingleBusInterface()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInSingleBusInterface(QString const& parameterID,
    QSharedPointer<BusInterface> busInterface) const
{
    int referenceCounter = 0;

    referenceCounter += countReferencesInParameters(parameterID, busInterface->getParameters());

    if (busInterface->getMirroredSlave())
    {
        referenceCounter +=
            countReferencesInMirroredSlaveInterface(parameterID, busInterface->getMirroredSlave());
    }
    if (busInterface->getMaster())
    {
        referenceCounter += countReferencesInMasterInterface(parameterID, busInterface->getMaster());
    }

    return referenceCounter;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInMirroredSlaveInterface()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInMirroredSlaveInterface(QString const& parameterID,
    QSharedPointer<MirroredSlaveInterface> mirroredSlave) const
{
    int referenceCounter = 0;

    referenceCounter += countReferencesInExpression(parameterID, mirroredSlave->getRange());
    
    for (QSharedPointer<MirroredSlaveInterface::RemapAddress> remapAddress :
        *mirroredSlave->getRemapAddresses())
    {
        referenceCounter += countReferencesInRemapAddress(parameterID, remapAddress);
    }

    return referenceCounter;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInRemapAddress()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInRemapAddress(QString const& parameterID,
    QSharedPointer<MirroredSlaveInterface::RemapAddress> remapAddress) const
{
    return countReferencesInExpression(parameterID, remapAddress->remapAddress_);
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInMasterInterface()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInMasterInterface(QString const& parameterID,
    QSharedPointer<MasterInterface> master) const
{
    return countReferencesInExpression(parameterID, master->getBaseAddress());
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInRemapStates()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInRemapStates(QString const& parameterID) const
{
    int referenceCount = 0;

    for (QSharedPointer<RemapState> remap : *component_->getRemapStates())
    {
        referenceCount += countReferencesInSingleRemapState(parameterID, remap);
    }

    return referenceCount;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInSingleRemapState()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInSingleRemapState(QString const& parameterID,
    QSharedPointer<RemapState> remapState) const
{
    int referenceCounter = 0;

    for (QSharedPointer<RemapPort> port : *remapState->getRemapPorts())
    {
        referenceCounter += countReferencesInSingleRemapPort(parameterID, port);
    }

    return referenceCounter;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInSingleRemapPort()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInSingleRemapPort(QString const& parameterID,
    QSharedPointer<RemapPort> port) const
{
    return countReferencesInExpression(parameterID, port->getValue());
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInIndirectInterfaces()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInIndirectInterfaces(QString const& parameterID) const
{
    int referenceCounter = 0;

    for (auto singleInterface : *component_->getIndirectInterfaces())
    {
        referenceCounter += countRefrencesInSingleIndirectInterface(parameterID, singleInterface);
    }

    return referenceCounter;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countRefrencesInSingleIndirectInterface()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countRefrencesInSingleIndirectInterface(QString const& parameterID,
    QSharedPointer<IndirectInterface> indirectInterface) const
{
    return countReferencesInParameters(parameterID, indirectInterface->getParameters());
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInCpus()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInCpus(QString const& parameterID) const
{
    int referenceCounter = 0;

    for (auto singleCpu : *component_->getCpus())
    {
        referenceCounter += countReferencesInSingleCpu(parameterID, singleCpu);
    }

    return referenceCounter;
}


//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInSingleCpu()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInSingleCpu(QString const& parameterID,
    QSharedPointer<Cpu> cpu) const
{
    int referenceCounter = 0;

    referenceCounter += countReferencesInExpression(parameterID, cpu->getRange());
    referenceCounter += countReferencesInExpression(parameterID, cpu->getWidth());
    referenceCounter += countReferencesInExpression(parameterID, cpu->getAddressUnitBits());

    referenceCounter += countReferencesInRegions(parameterID, cpu->getRegions());

    return referenceCounter;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInRegions()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInRegions(QString const& parameterID,
    QSharedPointer<QList<QSharedPointer<Region> > > regions) const
{
    int referenceCounter = 0;

    for (auto region : *regions)
    {
        referenceCounter += countReferencesInSingleRegion(parameterID, region);
    }

    return referenceCounter;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceCounter::countReferencesInSingleRegion()
//-----------------------------------------------------------------------------
int ComponentParameterReferenceCounter::countReferencesInSingleRegion(QString const& parameterID,
    QSharedPointer<Region> region) const
{
    int referenceCounter = 0;

    referenceCounter += countReferencesInExpression(parameterID, region->getAddressOffset());
    referenceCounter += countReferencesInExpression(parameterID, region->getRange());

    return referenceCounter;
}
