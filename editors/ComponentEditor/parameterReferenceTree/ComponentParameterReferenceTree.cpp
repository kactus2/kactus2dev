//-----------------------------------------------------------------------------
// File: ComponentParameterReferenceTree.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 28.09.2018
//
// Description:
// Tree widget for displaying references to a parameter within a component.
//-----------------------------------------------------------------------------

#include "ComponentParameterReferenceTree.h"

#include <editors/ComponentEditor/referenceCounter/ComponentParameterReferenceCounter.h>

#include <IPXACTmodels/common/ModuleParameter.h>
#include <IPXACTmodels/common/FileBuilder.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Cpu.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/BuildCommand.h>
#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/Segment.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/Component/DesignConfigurationInstantiation.h>
#include <IPXACTmodels/Component/DesignInstantiation.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/PowerDomain.h>
#include <IPXACTmodels/Component/MemoryMapBase.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/MemoryRemap.h>
#include <IPXACTmodels/Component/Mode.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/Region.h>
#include <IPXACTmodels/Component/RegisterBase.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/RegisterFile.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/WriteValueConstraint.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/MirroredTargetInterface.h>
#include <IPXACTmodels/Component/InitiatorInterface.h>
#include <IPXACTmodels/Component/RemapState.h>
#include <IPXACTmodels/Component/RemapPort.h>
#include <IPXACTmodels/Component/IndirectInterface.h>

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::ComponentParameterReferenceTree()
//-----------------------------------------------------------------------------
ComponentParameterReferenceTree::ComponentParameterReferenceTree(QSharedPointer<Component> component,
    QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ComponentParameterReferenceCounter> referenceCounter, QWidget *parent):
ParameterReferenceTree(expressionFormatter, parent),
component_(component),
referenceCounter_(referenceCounter)
{

}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::setComponent()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::setComponent(QSharedPointer<Component> newComponent)
{
    component_ = newComponent;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::setupTree()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::setupTree()
{
    if (!component_.isNull())
    {
        if (referenceCounter_->countReferencesInFileSets(getTargetID()) > 0)
        {
            createReferencesForFileSets();
        }

        if (referenceCounter_->countReferencesInParameters(getTargetID(), component_->getParameters()) > 0)
        {
            QTreeWidgetItem* topParametersItem = createTopItem("Parameters");
            createParameterReferences(component_->getParameters(), topParametersItem);
        }

        if (referenceCounter_->countReferencesInMemoryMaps(getTargetID()) > 0)
        {
            createReferencesForMemoryMaps();
        }

        if (referenceCounter_->countReferencesInAddressSpaces(getTargetID()) > 0)
        {
            createReferencesForAddressSpaces();
        }

        if (referenceCounter_->countReferencesInInstantiations(getTargetID()) > 0)
        {
            createReferencesForInstantiations();
        }

        if (referenceCounter_->countReferencesInPorts(getTargetID()) > 0)
        {
            createReferencesForPorts();
        }

        if (referenceCounter_->countReferencesInBusInterfaces(getTargetID()) > 0)
        {
            createReferencesForBusInterfaces();
        }

        if (referenceCounter_->countReferencesInRemapStates(getTargetID()) > 0)
        {
            createReferencesForRemapStates();
        }

        if (referenceCounter_->countReferencesInModes(getTargetID()) > 0)
        {
            createReferencesForModes();
        }

        if (referenceCounter_->countReferencesInIndirectInterfaces(getTargetID()) > 0)
        {
            createReferencesForIndirectInterfaces();
        }

        if (referenceCounter_->countReferencesInCpus(getTargetID()) > 0)
        {
            createReferencesForCpus();
        }

        if (referenceCounter_->countReferencesInPowerDomains(getTargetID()) > 0)
        {
            createReferencesForPowerDomains();
        }

        if (topLevelItemCount() == 0)
        {
            createTopItem("No references found.");
        }
    }
    else
    {
        createTopItem("Component does not exist.");
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::getReferenceCounter()
//-----------------------------------------------------------------------------
QSharedPointer<ParameterReferenceCounter> ComponentParameterReferenceTree::getReferenceCounter() const
{
    return referenceCounter_;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createReferencesForFileSets()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createReferencesForFileSets()
{
    QTreeWidgetItem* topFileSetsItem = createTopItem("File sets");

    QString targetID = getTargetID();

    for (QSharedPointer<FileSet> fileSet : *component_->getFileSets())
    {
        if (referenceCounter_->countReferencesInSingleFileSet(targetID, fileSet) > 0)
        {
            QTreeWidgetItem* fileSetItem = createMiddleItem(fileSet->name(), topFileSetsItem);

            if (referenceCounter_->countReferencesInFileBuilders(targetID, fileSet->getDefaultFileBuilders()) > 0)
            {
                createReferencesForFileBuilders(fileSet->getDefaultFileBuilders(), fileSetItem);
            }

            if (referenceCounter_->countReferencesInFiles(targetID, fileSet->getFiles()) > 0)
            {
                createReferencesForFiles(fileSet->getFiles(), fileSetItem);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createReferencesForFileBuilders()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createReferencesForFileBuilders(
    QSharedPointer<QList<QSharedPointer<FileBuilder> > > fileBuilders, QTreeWidgetItem* parentItem)
{
    QTreeWidgetItem* buildCommandsItem = createMiddleItem("Default file build commands", parentItem);
    colourItemGrey(buildCommandsItem);

    for (QSharedPointer<FileBuilder> builder : *fileBuilders)
    {
        if (referenceCounter_->countReferencesInSingleFileBuilder(getTargetID(), builder) > 0)
        {
            createReferencesForSingleFileBuilder(builder, buildCommandsItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createReferencesForSingleFileBuilder()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createReferencesForSingleFileBuilder(QSharedPointer<FileBuilder> fileBuilder,
    QTreeWidgetItem* parentItem)
{
    QString fileType = fileBuilder->getFileType().type_;

    QString fileTypeIdentifier = QObject::tr("File type: %1").arg(fileType);

    QTreeWidgetItem* builderItem = createMiddleItem(fileTypeIdentifier, parentItem);

    createItem(QLatin1String("Replace default flags"), fileBuilder->getReplaceDefaultFlags(), builderItem);
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createReferencesForFiles()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createReferencesForFiles(QSharedPointer<QList<QSharedPointer<File> > > fileList,
    QTreeWidgetItem* parentItem)
{
    QTreeWidgetItem* filesItem = createMiddleItem("Files", parentItem);
    colourItemGrey(filesItem);

    for (QSharedPointer<File> currentFile : *fileList)
    {
        if (referenceCounter_->countReferencesInSingleFile(getTargetID(), currentFile) > 0)
        {
            createReferencesForSingleFile(currentFile, filesItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createReferencesForSingleFile()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createReferencesForSingleFile(QSharedPointer<File> selectedFile,
    QTreeWidgetItem* parentItem)
{
    QTreeWidgetItem* fileItem = createMiddleItem(selectedFile->name(), parentItem);

    QSharedPointer<BuildCommand> buildCommand = selectedFile->getBuildCommand();

    if (buildCommand && referenceCounter_->countReferencesInBuildCommand(getTargetID(), buildCommand) > 0)
    {
        QTreeWidgetItem* buildCommandItem = createMiddleItem("Build command", fileItem);

        createItem("Replace default flags", buildCommand->getReplaceDefaultFlags(), buildCommandItem);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createReferencesForModuleParameters()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createReferencesForModuleParameters(
    QSharedPointer<QList<QSharedPointer<ModuleParameter> > > parameters, QTreeWidgetItem* parentItem)
{
    for (QSharedPointer<ModuleParameter> moduleParameter : *parameters)
    {
        if (referenceCounter_->countReferencesInSingleParameter(getTargetID(), moduleParameter) > 0)
        {
            QTreeWidgetItem* moduleParameterItem = createMiddleItem(moduleParameter->name(), parentItem);
            createItemsForParameter(moduleParameter, moduleParameterItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createReferencesForInstantiations()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createReferencesForInstantiations()
{
    QTreeWidgetItem* topInstantiationsItem = createTopItem("Instantiations");
    QString targetID = getTargetID();

    if (referenceCounter_->countReferencesInComponentInstantiations(
        targetID, component_->getComponentInstantiations()) > 0)
    {
        createReferencesForComponentInstantiations(topInstantiationsItem);
    }
    if (referenceCounter_->countReferencesInDesignConfigurationInstantiations(
        targetID, component_->getDesignConfigurationInstantiations()) > 0)
    {
        createReferencesForDesignConfigurationInstantiations(topInstantiationsItem);
    }
    if (referenceCounter_->countReferencesInDesignInstantiations(targetID) > 0)
    {
        createReferencesForDesignInstantiations(topInstantiationsItem);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createReferencesForComponentInstantiations()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createReferencesForComponentInstantiations(QTreeWidgetItem* topInstantiationsItem)
{
    QTreeWidgetItem* componentInstantiationsItem =
        createMiddleItem("Component instantiations", topInstantiationsItem);

    QString targetID = getTargetID();

    for (QSharedPointer<ComponentInstantiation> instantiation : *component_->getComponentInstantiations())
    {
        if (referenceCounter_->countReferencesInSingleComponentInstantiation(targetID, instantiation) > 0)
        {
            QTreeWidgetItem* instantiationItem =
                createMiddleItem(instantiation->name(), componentInstantiationsItem);

            if (referenceCounter_->countReferencesInParameters(targetID, instantiation->getParameters()) > 0)
            {
                QTreeWidgetItem* parametersItem = createMiddleItem(QLatin1String("Parameters"), instantiationItem);
                colourItemGrey(parametersItem);
                createParameterReferences(instantiation->getParameters(), parametersItem);
            }

            if (referenceCounter_->countReferencesInModuleParameters(
                targetID, instantiation->getModuleParameters()) > 0)
            {
                QTreeWidgetItem* moduleParametersItem =
                    createMiddleItem(QLatin1String("Module Parameters"), instantiationItem);
                colourItemGrey(moduleParametersItem);
                createReferencesForModuleParameters(instantiation->getModuleParameters(), moduleParametersItem);
            }

            if (referenceCounter_->countReferencesInFileBuilders(
                targetID, instantiation->getDefaultFileBuilders()) > 0)
            {
                createReferencesForFileBuilders(instantiation->getDefaultFileBuilders(), instantiationItem);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createReferencesForDesignConfigurationInstantiations()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createReferencesForDesignConfigurationInstantiations(
    QTreeWidgetItem* topInstantiationsItem)
{
    QTreeWidgetItem* designConfigurationInstantiationsItem =
        createMiddleItem("Design Configuration Instantiations", topInstantiationsItem);

    QString targetID = getTargetID();

    for (QSharedPointer<DesignConfigurationInstantiation> instantiation :
        *component_->getDesignConfigurationInstantiations())
    {
        if (referenceCounter_->countReferencesInSingleDesignConfigurationInstantiation(
            targetID, instantiation) > 0)
        {
            QTreeWidgetItem* instantiationItem = createMiddleItem(instantiation->name(),
                designConfigurationInstantiationsItem);
            if (referenceCounter_->countReferencesInParameters(targetID, instantiation->getParameters()) > 0)
            {
                QTreeWidgetItem* parametersItem = createMiddleItem(QLatin1String("Parameters"), instantiationItem);
                colourItemGrey(parametersItem);
                createParameterReferences(instantiation->getParameters(), parametersItem);
            }

            QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > elements =
                instantiation->getDesignConfigurationReference()->getConfigurableElementValues();
            if (referenceCounter_->countReferencesInConfigurableElementValues(targetID, elements) > 0)
            {
                createReferencesForConfigurableElementValues(elements, instantiationItem);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createReferencesForDesignInstantiations()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createReferencesForDesignInstantiations(QTreeWidgetItem* topInstantiationsItem)
{
    QTreeWidgetItem* designInstantiationsItem = createMiddleItem("Design instantiations", topInstantiationsItem);
    QString targetID = getTargetID();

    for (QSharedPointer<DesignInstantiation> instantiation : *component_->getDesignInstantiations())
    {
        if (referenceCounter_->countReferencesInSingleDesigninstantiation(targetID, instantiation) > 0)
        {
            QTreeWidgetItem* instantiationItem = createMiddleItem(instantiation->name(), designInstantiationsItem);

            QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > elements =
                instantiation->getDesignReference()->getConfigurableElementValues();
            if (referenceCounter_->countReferencesInConfigurableElementValues(targetID, elements) > 0)
            {
                createReferencesForConfigurableElementValues(elements, instantiationItem);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createReferencesForPorts()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createReferencesForPorts()
{
    QTreeWidgetItem* topPortsItem = createTopItem("Ports");

    for (QSharedPointer<Port> port : *component_->getPorts())
    {
        if (referenceCounter_->countReferencesInSinglePort(getTargetID(), port) > 0)
        {
            QTreeWidgetItem* portItem = createMiddleItem(port->name(), topPortsItem);
            createItemsForPort(port, portItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createReferencesForAddressSpaces()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createReferencesForAddressSpaces()
{
    QTreeWidgetItem* topAddressSpaceItem = createTopItem("Address Spaces");
    QString targetID = getTargetID();

    for (QSharedPointer<AddressSpace> addressSpace : *component_->getAddressSpaces())
    {
        if (referenceCounter_->countReferencesInSingleAddressSpace(targetID, addressSpace) > 0)
        {
            QTreeWidgetItem* addressSpaceItem = createMiddleItem(addressSpace->name(), topAddressSpaceItem);

            if (referenceCounter_->countReferencesInSingleAddressSpaceItems(targetID, addressSpace) > 0)
            {
                createItemsForAddressSpace(addressSpace, addressSpaceItem);
            }

            if (referenceCounter_->countReferencesInSegments(targetID, addressSpace->getSegments()) > 0)
            {
                QTreeWidgetItem* segmentsItem = createMiddleItem("Segments", addressSpaceItem);
                colourItemGrey(segmentsItem);

                createReferencesForSegments(addressSpace, segmentsItem);
            }

            if (addressSpace->hasLocalMemoryMap() &&
                referenceCounter_->countReferencesInBaseMemoryMap(targetID, addressSpace->getLocalMemoryMap()) > 0)
            {
                QSharedPointer <MemoryMapBase> localMemoryMap = addressSpace->getLocalMemoryMap();
                QTreeWidgetItem* memoryMapItem = createMiddleItem(localMemoryMap->name(), addressSpaceItem);
                createReferencesForSingleMemoryMap(localMemoryMap, memoryMapItem);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createItemsForAddressSpace()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createItemsForAddressSpace(QSharedPointer<AddressSpace> addressSpace,
    QTreeWidgetItem* parent)
{
    if (addressSpace->getWidth().contains(getTargetID()))
    {
        createItem(QLatin1String("Width"), addressSpace->getWidth(), parent);
    }

    if (addressSpace->getRange().contains(getTargetID()))
    {
        createItem(QLatin1String("Range"), addressSpace->getRange(), parent);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createReferencesForSegments()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createReferencesForSegments(QSharedPointer<AddressSpace> addressSpace,
    QTreeWidgetItem* parent)
{
    for (QSharedPointer<Segment> segment : *addressSpace->getSegments())
    {
        if (referenceCounter_->countReferencesInSingleSegment(getTargetID(), segment) > 0)
        {
            createItemsForSegment(segment, parent);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createItemsForSegment()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createItemsForSegment(QSharedPointer<Segment> segment,
    QTreeWidgetItem* parent)
{
    QTreeWidgetItem* segmentItem = createMiddleItem(segment->name(), parent);

    if (segment->getAddressOffset().contains(getTargetID()))
    {
        createItem("Offset", segment->getAddressOffset(), segmentItem);
    }

    if (segment->getRange().contains(getTargetID()))
    {
        createItem("Range", segment->getRange(), segmentItem);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createReferencesForMemoryMaps()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createReferencesForMemoryMaps()
{
    QTreeWidgetItem* topMemoryMapItem = createTopItem("Memory maps");
    QString targetID = getTargetID();

    for (QSharedPointer<MemoryMap> memoryMap : *component_->getMemoryMaps())
    {
        if (referenceCounter_->countReferencesInSingleMemoryMap(targetID, memoryMap) > 0)
        {
            QTreeWidgetItem* memoryMapTreeItem = createMiddleItem(memoryMap->name(), topMemoryMapItem);

            QTreeWidgetItem* memoryRemapsTreeItem = createMiddleItem("Memory remaps", memoryMapTreeItem);
            colourItemGrey(memoryRemapsTreeItem);

            if (referenceCounter_->countReferencesInBaseMemoryMap(targetID, memoryMap) > 0)
            {
                QTreeWidgetItem* defaultMemoryRemapItem = createMiddleItem("Default", memoryRemapsTreeItem);
                createReferencesForSingleMemoryMap(memoryMap, defaultMemoryRemapItem);
            }

            for (QSharedPointer<MemoryRemap> memoryRemap : *memoryMap->getMemoryRemaps())
            {
                if (referenceCounter_->countReferencesInBaseMemoryMap(targetID, memoryRemap) > 0)
                {
                    QTreeWidgetItem* memoryRemapItem = createMiddleItem(memoryRemap->name(), memoryRemapsTreeItem);
                    createReferencesForSingleMemoryMap(memoryRemap, memoryRemapItem);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createReferencesForSingleMemoryMap()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createReferencesForSingleMemoryMap(QSharedPointer<MemoryMapBase> memoryMap,
    QTreeWidgetItem* memoryRemapItem)
{
    QString targetID = getTargetID();
    if (referenceCounter_->countReferencesInMemoryMapValues(targetID, memoryMap) > 0)
    {
        createItemsForMemoryMap(memoryMap, memoryRemapItem);
    }

    QTreeWidgetItem* middleAddressBlocksItem = createMiddleItem("Address blocks", memoryRemapItem);

    colourItemGrey(middleAddressBlocksItem);

    for (QSharedPointer<MemoryBlockBase> memoryBlock : *memoryMap->getMemoryBlocks())
    {
        QSharedPointer<AddressBlock> addressBlock = memoryBlock.dynamicCast<AddressBlock>();

        if (addressBlock)
        {
            if (referenceCounter_->countReferenceInAddressBlock(getTargetID(), addressBlock) > 0)
            {
                createReferencesForSingleAddressBlock(addressBlock, middleAddressBlocksItem);

            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createReferencesForSingleAddressBlock()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createReferencesForSingleAddressBlock(
    QSharedPointer<AddressBlock> addressBlock, QTreeWidgetItem* middleAddressBlocksItem)
{
    QTreeWidgetItem* addressBlockItem = createMiddleItem(addressBlock->name(),
        middleAddressBlocksItem);
    QString targetID = getTargetID();

    if (referenceCounter_->countReferencesInAddressBlockValues(targetID, addressBlock) > 0)
    {
        createItemsForAddressBlock(addressBlock, addressBlockItem);
    }

    if (referenceCounter_->countReferencesInRegisters(targetID, addressBlock->getRegisterData()) > 0)
    {
        createReferencesForRegisters(targetID, addressBlock->getRegisterData(), addressBlockItem);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createReferencesForRegisters()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createReferencesForRegisters(QString const& targetID,
    QSharedPointer<QList<QSharedPointer<RegisterBase>>> registerData, QTreeWidgetItem* parentItem)
{
    if (referenceCounter_->countReferencesInRegisters(targetID, registerData) > 0)
    {
        QTreeWidgetItem* registersItem = createMiddleItem("Registers", parentItem);
        colourItemGrey(registersItem);

        for (auto baseRegister : *registerData)
        {
            QSharedPointer<Register> targetRegister = baseRegister.dynamicCast<Register>();
            if (targetRegister && referenceCounter_->countReferencesInSingleRegister(targetID, targetRegister) > 0)
            {
                createReferencesForSingleRegister(targetRegister, registersItem);
            }
        }
    }
    if (referenceCounter_->countReferencesInRegisterFiles(targetID, registerData) > 0)
    {
        QTreeWidgetItem* registerFileItem = createMiddleItem("Register Files", parentItem);
        colourItemGrey(registerFileItem);
        for (auto baseRegister : *registerData)
        {
            QSharedPointer<RegisterFile> targetFile = baseRegister.dynamicCast<RegisterFile>();
            if (targetFile && referenceCounter_->countReferencesInSingleRegisterFile(targetID, targetFile) > 0)
            {
                createReferencesForSingleRegisterFile(targetFile, registerFileItem);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createReferencesForSingleRegister()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createReferencesForSingleRegister(QSharedPointer<Register> targetRegister,
    QTreeWidgetItem* parentItem)
{
    QTreeWidgetItem* registerItem = createMiddleItem(targetRegister->name(), parentItem);
    QString targetID = getTargetID();

    createItemsForRegister(targetRegister, registerItem);

    if (referenceCounter_->countReferencesInFields(targetID, targetRegister->getFields()) > 0)
    {
        QTreeWidgetItem* fieldsItem = createMiddleItem("Fields", registerItem);
        colourItemGrey(fieldsItem);

        for (QSharedPointer<Field> registerField : *targetRegister->getFields())
        {
            if (referenceCounter_->countReferencesInSingleField(targetID, registerField) > 0)
            {
                QTreeWidgetItem* singleFieldItem = createMiddleItem(registerField->name(), fieldsItem);
                createItemsForField(registerField, singleFieldItem);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createReferencesForSingleRegisterFile()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createReferencesForSingleRegisterFile(
    QSharedPointer<RegisterFile> targetFile, QTreeWidgetItem* parentItem)
{
    QTreeWidgetItem* fileItem = createMiddleItem(targetFile->name(), parentItem);
    QString targetID = getTargetID();

    createItemsForRegisterFile(targetFile, fileItem);

    createReferencesForRegisters(targetID, targetFile->getRegisterData(), fileItem);
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createReferencesForBusInterfaces()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createReferencesForBusInterfaces()
{
    QTreeWidgetItem* topBusInterfaceItem = createTopItem("Bus Interfaces");
    QString targetID = getTargetID();

    for (QSharedPointer<BusInterface> busInterface : *component_->getBusInterfaces())
    {
        if (referenceCounter_->countReferencesInSingleBusInterface(targetID, busInterface) > 0)
        {
            QTreeWidgetItem* busInterfaceItem = createMiddleItem(busInterface->name(), topBusInterfaceItem);

            if (busInterface->getMirroredSlave())
            {
                QSharedPointer<MirroredTargetInterface> mirrorSlave(busInterface->getMirroredSlave());

                if (referenceCounter_->countReferencesInMirroredSlaveInterface(targetID, mirrorSlave) > 0)
                {
                    QTreeWidgetItem* mirroredInterfaceItem =
                        createMiddleItem("Mirrored Slave Interface", busInterfaceItem);
                    colourItemGrey(mirroredInterfaceItem);

                    for (QSharedPointer<MirroredTargetInterface::RemapAddress> remapAddress :
                        *mirrorSlave->getRemapAddresses())
                    {
                        if (referenceCounter_->countReferencesInRemapAddress(targetID, remapAddress) > 0)
                        {
                            createItem("Remap Address", remapAddress->remapAddress_, mirroredInterfaceItem);
                        }
                    }

                    if (mirrorSlave->getRange().contains(targetID))
                    {
                        createItem("Range", mirrorSlave->getRange(), mirroredInterfaceItem);
                    }
                }
            }

            if (busInterface->getMaster())
            {
                QSharedPointer<InitiatorInterface> master(busInterface->getMaster());

                if (referenceCounter_->countReferencesInMasterInterface(targetID, master) > 0)
                {
                    if (busInterface->getInterfaceMode() == General::MASTER)
                    {
                        QTreeWidgetItem* masterInterfaceItem = createMiddleItem(
                            "Master Interface", busInterfaceItem);
                        colourItemGrey(masterInterfaceItem);

                        createItem("Base Address", master->getBaseAddress(), masterInterfaceItem);
                    }
                    else
                    {
                        QTreeWidgetItem* masterInterfaceItem = createMiddleItem(
                            "Mirrored Master Interface", busInterfaceItem);
                        colourItemGrey(masterInterfaceItem);

                        createItem("Base Address", master->getBaseAddress(), masterInterfaceItem);
                    }
                }
            }

            if (referenceCounter_->countReferencesInParameters(targetID, busInterface->getParameters()) > 0)
            {
                QTreeWidgetItem* parametersItem = createMiddleItem(QString("Parameters"), busInterfaceItem);
                colourItemGrey(parametersItem);
                createParameterReferences(busInterface->getParameters(), parametersItem);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createReferencesForRemapStates()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createReferencesForRemapStates()
{
    QTreeWidgetItem* topRemapStatesItem = createTopItem("Remap States");
    QString targetID = getTargetID();

    for (QSharedPointer<RemapState> remapState : *component_->getRemapStates())
    {
        if (referenceCounter_->countReferencesInSingleRemapState(targetID, remapState) > 0)
        {
            QTreeWidgetItem* remapStateItem = createMiddleItem(remapState->name(), topRemapStatesItem);

            QTreeWidgetItem* remapPortsItem = createMiddleItem("Remap Ports", remapStateItem);
            colourItemGrey(remapPortsItem);

            for (QSharedPointer<RemapPort> remapPort : *remapState->getRemapPorts())
            {
                if (referenceCounter_->countReferencesInSingleRemapPort(targetID, remapPort) > 0)
                {
                    QString itemName = remapPort->getPortNameRef();

                    if (!remapPort->getPortIndex().isEmpty())
                    {
                        itemName.append("[" + remapPort->getPortIndex() + "]");
                    }
                    createItem(itemName, remapPort->getValue(), remapPortsItem);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createReferencesForModes()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createReferencesForModes()
{
    QTreeWidgetItem* topModesItem = createTopItem("Modes");
    QString targetID = getTargetID();

    for (auto const& mode : *component_->getModes())
    {
        if (referenceCounter_->countReferencesInSingleMode(targetID, mode) > 0)
        {
            QTreeWidgetItem* modeItem = createMiddleItem(mode->name(), topModesItem);

            if (referenceCounter_->countReferencesInModeCondition(targetID, mode->getCondition()) > 0)
            {
                createItem(QStringLiteral("Condition"), mode->getCondition(), modeItem);
            }

            createPortSliceItems(targetID, mode, modeItem);

            createFieldSliceItems(targetID, mode, modeItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createPortSliceItems()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createPortSliceItems(QString const& targetID,
    QSharedPointer<Mode> mode, QTreeWidgetItem* parentItem)
{
    if (referenceCounter_->countReferencesInPortSlices(targetID, mode) == 0)
    {
        return;
    }

    QTreeWidgetItem* portsItem = createMiddleItem(QStringLiteral("Condition Ports"), parentItem);
    colourItemGrey(portsItem);

    for (auto const& portSlice : *mode->getPortSlices())
    {
        if (referenceCounter_->countReferencesInSinglePortSlice(targetID, portSlice) > 0)
        {
            QTreeWidgetItem* sliceItem = createMiddleItem(portSlice->name(), parentItem);

            if (portSlice->getLeftRange().contains(targetID))
            {
                createItem(QStringLiteral("Left bound"), portSlice->getLeftRange(), sliceItem);
            }

            if (portSlice->getRightRange().contains(targetID))
            {
                createItem(QStringLiteral("Right bound"), portSlice->getRightRange(), sliceItem);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createFieldSliceItems()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createFieldSliceItems(QString const& targetID,
    QSharedPointer<Mode> mode, QTreeWidgetItem* parentItem)
{
    if (referenceCounter_->countReferencesInFieldSlices(targetID, mode) == 0)
    {
        return;
    }

    QTreeWidgetItem* fieldsItem = createMiddleItem(QStringLiteral("Condition Fields"), parentItem);
    colourItemGrey(fieldsItem);

    for (auto const& fieldSlice : *mode->getFieldSlices())
    {
        if (referenceCounter_->countReferencesInSingleFieldSlice(targetID, fieldSlice) > 0)
        {
            QTreeWidgetItem* sliceItem = createMiddleItem(fieldSlice->name(), parentItem);

            if (fieldSlice->getLeft().contains(targetID))
            {
                createItem(QStringLiteral("Leftmost bit"), fieldSlice->getLeft(), sliceItem);
            }

            if (fieldSlice->getRight().contains(targetID))
            {
                createItem(QStringLiteral("Rightmost bit"), fieldSlice->getRight(), sliceItem);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createReferencesForIndirectInterfaces()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createReferencesForIndirectInterfaces()
{
    QTreeWidgetItem* topIndirectInterfacesItem = createTopItem("Indirect Interfaces");
    QString targetID = getTargetID();

    for (auto indirectInterface : *component_->getIndirectInterfaces())
    {
        if (referenceCounter_->countRefrencesInSingleIndirectInterface(targetID, indirectInterface) > 0)
        {
            QTreeWidgetItem* interfaceItem = createMiddleItem(indirectInterface->name(), topIndirectInterfacesItem);

            QTreeWidgetItem* parametersItem = createMiddleItem(QLatin1String("Parameters"), interfaceItem);
            colourItemGrey(parametersItem);
            createParameterReferences(indirectInterface->getParameters(), parametersItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createReferencesForCpus()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createReferencesForCpus()
{
    QTreeWidgetItem* topCpusItem = createTopItem("CPUs");
    QString targetID = getTargetID();

    for (auto cpu : *component_->getCpus())
    {
        if (referenceCounter_->countReferencesInSingleCpu(targetID, cpu) > 0)
        {
            QTreeWidgetItem* cpuItem = createMiddleItem(cpu->name(), topCpusItem);

            if (cpu->getWidth().contains(targetID))
            {
                createItem("Width", cpu->getWidth(), cpuItem);
            }
            if (cpu->getRange().contains(targetID))
            {
                createItem("Range", cpu->getRange(), cpuItem);
            }
            if (cpu->getAddressUnitBits().contains(targetID))
            {
                createItem("Address unit bits", cpu->getAddressUnitBits(), cpuItem);
            }

            if (referenceCounter_->countReferencesInRegions(targetID, cpu->getRegions()) > 0)
            {
                QTreeWidgetItem* regionsItem = createMiddleItem(QStringLiteral("Regions"), cpuItem);
                colourItemGrey(regionsItem);

                for (auto const& region : *cpu->getRegions())
                {
                    if (referenceCounter_->countReferencesInSingleRegion(targetID, region) > 0)
                    {
                        createItemsForRegion(region, regionsItem);
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createItemsForRegion()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createItemsForRegion(QSharedPointer<Region> region, QTreeWidgetItem* parent)
{
    QString targetID = getTargetID();

    QTreeWidgetItem* regionItem = createMiddleItem(region->name(), parent);

    if (region->getAddressOffset().contains(targetID))
    {
        createItem("Offset", region->getAddressOffset(), regionItem);
    }
    if (region->getRange().contains(targetID))
    {
        createItem("Range", region->getRange(), regionItem);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createReferencesForPowerDomains()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createReferencesForPowerDomains()
{
    auto topPowerDomainsItem = createTopItem("Power domains");
    auto targetID = getTargetID();

    for (auto domain : *component_->getPowerDomains())
    {
        if (referenceCounter_->countReferencesInSinglePowerDomain(targetID, domain) > 0)
        {
            QTreeWidgetItem* domainItem = createMiddleItem(domain->name(), topPowerDomainsItem);

            createItem("Always on", domain->getAlwaysOn(), domainItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createItemsForPort()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createItemsForPort(QSharedPointer<Port> port, QTreeWidgetItem* parent)
{
    QString targetID = getTargetID();

    if (port->getLeftBound().contains(targetID))
    {
        createItem("Left Bound", port->getLeftBound(), parent);
    }
    if (port->getRightBound().contains(targetID))
    {
        createItem("Right Bound", port->getRightBound(), parent);
    }
    if (port->getDefaultValue().contains(targetID))
    {
        createItem("Default Value", port->getDefaultValue(), parent);
    }
    if (port->getArrayLeft().contains(targetID))
    {
        createItem("Array Left", port->getArrayLeft(), parent);
    }
    if (port->getArrayRight().contains(targetID))
    {
        createItem("Array Right", port->getArrayRight(), parent);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createItemsForRegister()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createItemsForRegister(QSharedPointer<Register> targetRegister,
    QTreeWidgetItem* parent)
{
    QString targetID = getTargetID();

    createItemsForBaseRegister(targetRegister, parent);
    if (targetRegister->getSize().contains(targetID))
    {
        createItem("Size", targetRegister->getSize(), parent);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createItemsForBaseRegister()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createItemsForBaseRegister(QSharedPointer<RegisterBase> baseRegister,
    QTreeWidgetItem* parent)
{
    QString targetID = getTargetID();

    if (baseRegister->getAddressOffset().contains(targetID))
    {
        createItem("Offset", baseRegister->getAddressOffset(), parent);
    }
    if (baseRegister->getDimension().contains(targetID))
    {
        createItem("Dimension", baseRegister->getDimension(), parent);
    }
    if (baseRegister->getIsPresent().contains(targetID))
    {
        createItem("Is present", baseRegister->getIsPresent(), parent);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createItemsForRegisterFile()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createItemsForRegisterFile(QSharedPointer<RegisterFile> targetFile,
    QTreeWidgetItem* parent)
{
    QString targetID = getTargetID();

    createItemsForBaseRegister(targetFile, parent);
    if (targetFile->getRange().contains(targetID))
    {
        createItem("Range", targetFile->getRange(), parent);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createItemsForMemoryMap()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createItemsForMemoryMap(QSharedPointer<MemoryMapBase> targetMap,
    QTreeWidgetItem* parent)
{
    QString targetID = getTargetID();

    if (targetMap->getIsPresent().contains(targetID))
    {
        createItem("Is Present", targetMap->getIsPresent(), parent);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createItemsForAddressBlock()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createItemsForAddressBlock(QSharedPointer<AddressBlock> targetAddressBlock,
    QTreeWidgetItem* parent)
{
    QString targetID = getTargetID();

    if (targetAddressBlock->getBaseAddress().contains(targetID))
    {
        createItem("Base Address", targetAddressBlock->getBaseAddress(), parent);
    }
    if (targetAddressBlock->getRange().contains(targetID))
    {
        createItem("Range", targetAddressBlock->getRange(), parent);
    }
    if (targetAddressBlock->getWidth().contains(targetID))
    {
        createItem("Width", targetAddressBlock->getWidth(), parent);
    }
    if (targetAddressBlock->getIsPresent().contains(targetID))
    {
        createItem("Is Present", targetAddressBlock->getIsPresent(), parent);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createItemsForField()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createItemsForField(QSharedPointer<Field> targetField, QTreeWidgetItem* parent)
{
    QString targetID = getTargetID();

    if (targetField->getBitOffset().contains(targetID))
    {
        createItem("Offset", targetField->getBitOffset(), parent);
    }
    if (targetField->getBitWidth().contains(targetID))
    {
        createItem("Width", targetField->getBitWidth(), parent);
    }
    if (targetField->getIsPresent().contains(targetID))
    {
        createItem("Is Present", targetField->getIsPresent(), parent);
    }
    if (referenceCounter_->countReferencesInFieldResets(targetID, targetField->getResets()) > 0)
    {
        createItemsForFieldResets(targetID, targetField->getResets(), parent);
    }

    if (targetField->getWriteConstraint())
    {
        if (targetField->getWriteConstraint()->getMinimum().contains(targetID))
        {
            createItem("Write constraint minimum", targetField->getWriteConstraint()->getMinimum(), parent);
        }
        if (targetField->getWriteConstraint()->getMaximum().contains(targetID))
        {
            createItem("Write constraint maximum", targetField->getWriteConstraint()->getMaximum(), parent);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createItemsForFieldResets()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createItemsForFieldResets(QString const& targetID,
    QSharedPointer<QList<QSharedPointer<FieldReset>>> fieldResets, QTreeWidgetItem* fieldItem)
{
    QTreeWidgetItem* resetsItem = createMiddleItem("Resets", fieldItem);
    colourItemGrey(resetsItem);

    for (auto singleReset : *fieldResets)
    {
        if (referenceCounter_->countReferencesInSingleFieldReset(targetID, singleReset) > 0)
        {
            QString resetType = singleReset->getResetTypeReference();
            if (resetType.isEmpty())
            {
                resetType = QLatin1String("HARD");
            }

            QTreeWidgetItem* singleResetItem = createMiddleItem(resetType, resetsItem);

            if (singleReset->getResetValue().contains(targetID))
            {
                createItem("Reset value", singleReset->getResetValue(), singleResetItem);
            }
            if (singleReset->getResetMask().contains(targetID))
            {
                createItem("Reset mask", singleReset->getResetMask(), singleResetItem);
            }
        }
    }
}
