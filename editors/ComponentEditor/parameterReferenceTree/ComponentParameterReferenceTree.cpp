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

#include <IPXACTmodels/common/ModuleParameter.h>
#include <IPXACTmodels/common/FileBuilder.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/BuildCommand.h>
#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/Segment.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/Component/DesignConfigurationInstantiation.h>
#include <IPXACTmodels/Component/DesignInstantiation.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/MemoryMapBase.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/MemoryRemap.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/RegisterBase.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/WriteValueConstraint.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/MirroredSlaveInterface.h>
#include <IPXACTmodels/Component/MasterInterface.h>
#include <IPXACTmodels/Component/RemapState.h>
#include <IPXACTmodels/Component/RemapPort.h>

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::ComponentParameterReferenceTree()
//-----------------------------------------------------------------------------
ComponentParameterReferenceTree::ComponentParameterReferenceTree(QSharedPointer<Component> component,
    QSharedPointer<ExpressionFormatter> expressionFormatter, QWidget *parent):
ParameterReferenceTree(expressionFormatter, parent),
component_(component)
{

}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::~ComponentParameterReferenceTree()
//-----------------------------------------------------------------------------
ComponentParameterReferenceTree::~ComponentParameterReferenceTree()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::setupTree()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::setupTree()
{
    if (!component_.isNull())
    {
        if (referenceExistsInFileSets())
        {
            createReferencesForFileSets();
        }

        if (referenceExistsInParameters(component_->getParameters()))
        {
            QTreeWidgetItem* topParametersItem = createTopItem("Parameters");
            createParameterReferences(component_->getParameters(), topParametersItem);
        }

        if (referenceExistsInMemoryMaps())
        {
            createReferencesForMemoryMaps();
        }

        if (referenceExistsInAddressSpaces())
        {
            createReferencesForAddressSpaces();
        }

        if (referenceExistsInInstantiations())
        {
            createReferencesForInstantiations();
        }

        if (referenceExistsInPorts())
        {
            createReferencesForPorts();
        }

        if (referenceExistsInBusInterfaces())
        {
            createReferencesForBusInterfaces();
        }

        if (referenceExistsInRemapStates())
        {
            createReferencesForRemapStates();
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
// Function: ComponentParameterReferenceTree::referenceExistsInFileSets()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::referenceExistsInFileSets() const
{
    foreach (QSharedPointer<FileSet> fileSet, *component_->getFileSets())
    {
        if (fileSetHasReference(fileSet))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::fileSetHasReference()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::fileSetHasReference(QSharedPointer<FileSet> fileSet) const
{
    return referenceExistsInFileBuilders(fileSet->getDefaultFileBuilders()) ||
        referenceExistsInFiles(fileSet->getFiles());
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::referenceExistsInFileBuilders()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::referenceExistsInFileBuilders(
    QSharedPointer<QList<QSharedPointer<FileBuilder> > > fileBuilders) const
{
    foreach (QSharedPointer<FileBuilder> builder, *fileBuilders)
    {
        if (fileBuilderHasReference(builder))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::fileBuilderHasReference()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::fileBuilderHasReference(QSharedPointer<FileBuilder> fileBuilder) const
{
    return fileBuilder->getReplaceDefaultFlags().contains(getTargetID());
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::referenceExistsInFiles()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::referenceExistsInFiles(
    QSharedPointer<QList<QSharedPointer<File> > > fileList) const
{
    foreach (QSharedPointer<File> currentFile, *fileList)
    {
        if (fileHasReference(currentFile))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::fileHasReference()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::fileHasReference(QSharedPointer<File> file) const
{
    if (file->getBuildCommand())
    {
        if (buildCommandHasReference(file->getBuildCommand()))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::buildCommandHasReference()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::buildCommandHasReference(QSharedPointer<BuildCommand> fileBuildCommand) const
{
    return fileBuildCommand->getReplaceDefaultFlags().contains(getTargetID());
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createReferencesForFileSets()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createReferencesForFileSets()
{
    QTreeWidgetItem* topFileSetsItem = createTopItem("File sets");

    foreach (QSharedPointer<FileSet> fileSet, *component_->getFileSets())
    {
        if (fileSetHasReference(fileSet))
        {
            QTreeWidgetItem* fileSetItem = createMiddleItem(fileSet->name(), topFileSetsItem);

            if (referenceExistsInFileBuilders(fileSet->getDefaultFileBuilders()))
            {
                createReferencesForFileBuilders(fileSet->getDefaultFileBuilders(), fileSetItem);
            }

            if (referenceExistsInFiles(fileSet->getFiles()))
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

    foreach (QSharedPointer<FileBuilder> builder, *fileBuilders)
    {
        if (fileBuilderHasReference(builder))
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
    QString fileType = fileBuilder->getFileType();
    if (fileType == QLatin1String("user"))
    {
        fileType = fileBuilder->getUserFileType();
    }

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

    foreach (QSharedPointer<File> currentFile, *fileList)
    {
        if (fileHasReference(currentFile))
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

    if (buildCommand && buildCommandHasReference(buildCommand))
    {
        QTreeWidgetItem* buildCommandItem = createMiddleItem("Build command", fileItem);

        createItem("Replace default flags", buildCommand->getReplaceDefaultFlags(), buildCommandItem);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::referenceExistsInModuleParameters()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::referenceExistsInModuleParameters(
    QSharedPointer<QList<QSharedPointer<ModuleParameter> > > parameters) const
{
    foreach(QSharedPointer<Parameter> moduleParameter, *parameters)
    {
        if (parameterHasReference(moduleParameter))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createReferencesForModuleParameters()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createReferencesForModuleParameters(
    QSharedPointer<QList<QSharedPointer<ModuleParameter> > > parameters, QTreeWidgetItem* parentItem)
{
    foreach (QSharedPointer<ModuleParameter> moduleParameter, *parameters)
    {
        if (parameterHasReference(moduleParameter))
        {
            QTreeWidgetItem* moduleParameterItem = createMiddleItem(moduleParameter->name(), parentItem);
            createItemsForParameter(moduleParameter, moduleParameterItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::referenceExistsInAddressSpaces()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::referenceExistsInAddressSpaces() const
{
    foreach(QSharedPointer<AddressSpace> addressSpace, *component_->getAddressSpaces())
    {
        if (referenceExistsInSingleAddressSpace(addressSpace))
        {
            return true;
        }

        if (referenceExistsInDefaultMemoryRemap(addressSpace->getLocalMemoryMap()))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::referenceExistsInAddressSpace()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::referenceExistsInSingleAddressSpace(
    QSharedPointer<AddressSpace> addressSpace) const
{
    return addressSpace->getWidth().contains(getTargetID()) ||
        addressSpace->getRange().contains(getTargetID()) ||
        referenceExistsInSegments(addressSpace) || 
        referenceExistsInDefaultMemoryRemap(addressSpace->getLocalMemoryMap());
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::referenceExistsInSegments()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::referenceExistsInSegments(QSharedPointer<AddressSpace> addressSpace) const
{
    foreach (QSharedPointer<Segment> segment, *addressSpace->getSegments())
    {
        if (referenceExistsInSingleSegment(segment))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::referenceExistsInSingleSegment()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::referenceExistsInSingleSegment(QSharedPointer<Segment> segment) const
{
    return segment->getAddressOffset().contains(getTargetID()) || segment->getRange().contains(getTargetID());
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::referenceExistsInInstantiations()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::referenceExistsInInstantiations() const
{
    return referenceExistsInComponentInstantiations() || referenceExistsInDesignConfigurationInstantiations() ||
        referenceExistsInDesignInstantiations();
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::referenceExistsInComponentInstantiations()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::referenceExistsInComponentInstantiations() const
{
    foreach (QSharedPointer<ComponentInstantiation> instantiation, *component_->getComponentInstantiations())
    {
        if (referenceExistsInSingleComponentInstantiation(instantiation))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::referenceExistsInSingleComponentInstantiation()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::referenceExistsInSingleComponentInstantiation(
    QSharedPointer<ComponentInstantiation> instantiation) const
{
    return referenceExistsInModuleParameters(instantiation->getModuleParameters()) ||
        referenceExistsInParameters(instantiation->getParameters()) ||
        referenceExistsInFileBuilders(instantiation->getDefaultFileBuilders());
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::referenceExistsInDesignConfigurationInstantiations()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::referenceExistsInDesignConfigurationInstantiations() const
{
    foreach (QSharedPointer<DesignConfigurationInstantiation> instantiation,
        *component_->getDesignConfigurationInstantiations())
    {
        if (referenceExistsInSingleDesignConfigurationInstantiation(instantiation))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::referenceExistsInSingleDesignConfigurationInstantiation()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::referenceExistsInSingleDesignConfigurationInstantiation(
    QSharedPointer<DesignConfigurationInstantiation> instantiation) const
{
    return referenceExistsInParameters(instantiation->getParameters()) ||
        referenceExistsInConfigurableElementValues(
        instantiation->getDesignConfigurationReference()->getConfigurableElementValues());
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::referenceExistsInDesignInstantiations()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::referenceExistsInDesignInstantiations() const
{
    foreach (QSharedPointer<DesignInstantiation> instantiation, *component_->getDesignInstantiations())
    {
        if (referenceExistsInSingleDesignInstantiation(instantiation))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::referenceExistsInSingleDesignInstantiation()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::referenceExistsInSingleDesignInstantiation(
    QSharedPointer<DesignInstantiation> instantiation) const
{
    if (instantiation->getDesignReference())
    {
        return referenceExistsInConfigurableElementValues(
            instantiation->getDesignReference()->getConfigurableElementValues());
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createReferencesForInstantiations()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createReferencesForInstantiations()
{
    QTreeWidgetItem* topInstantiationsItem = createTopItem("Instantiations");
    if (referenceExistsInComponentInstantiations())
    {
        createReferencesForComponentInstantiations(topInstantiationsItem);
    }
    if (referenceExistsInDesignConfigurationInstantiations())
    {
        createReferencesForDesignConfigurationInstantiations(topInstantiationsItem);
    }
    if (referenceExistsInDesignInstantiations())
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

    foreach (QSharedPointer<ComponentInstantiation> instantiation, *component_->getComponentInstantiations())
    {
        if (referenceExistsInSingleComponentInstantiation(instantiation))
        {
            QTreeWidgetItem* instantiationItem =
                createMiddleItem(instantiation->name(), componentInstantiationsItem);

            if (referenceExistsInParameters(instantiation->getParameters()))
            {
                QTreeWidgetItem* parametersItem = createMiddleItem(QLatin1String("Parameters"), instantiationItem);
                colourItemGrey(parametersItem);
                createParameterReferences(instantiation->getParameters(), parametersItem);
            }

            if (referenceExistsInModuleParameters(instantiation->getModuleParameters()))
            {
                QTreeWidgetItem* moduleParametersItem =
                    createMiddleItem(QLatin1String("Module Parameters"), instantiationItem);
                colourItemGrey(moduleParametersItem);
                createReferencesForModuleParameters(instantiation->getModuleParameters(), moduleParametersItem);
            }

            if (referenceExistsInFileBuilders(instantiation->getDefaultFileBuilders()))
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

    foreach (QSharedPointer<DesignConfigurationInstantiation> instantiation,
        *component_->getDesignConfigurationInstantiations())
    {
        if (referenceExistsInSingleDesignConfigurationInstantiation(instantiation))
        {
            QTreeWidgetItem* instantiationItem = createMiddleItem(instantiation->name(),
                designConfigurationInstantiationsItem);
            if (referenceExistsInParameters(instantiation->getParameters()))
            {
                QTreeWidgetItem* parametersItem = createMiddleItem(QLatin1String("Parameters"), instantiationItem);
                colourItemGrey(parametersItem);
                createParameterReferences(instantiation->getParameters(), parametersItem);
            }

            QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > elements =
                instantiation->getDesignConfigurationReference()->getConfigurableElementValues();
            if (referenceExistsInConfigurableElementValues(elements))
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
    foreach (QSharedPointer<DesignInstantiation> instantiation, *component_->getDesignInstantiations())
    {
        if (referenceExistsInSingleDesignInstantiation(instantiation))
        {
            QTreeWidgetItem* instantiationItem = createMiddleItem(instantiation->name(), designInstantiationsItem);

            QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > elements =
                instantiation->getDesignReference()->getConfigurableElementValues();
            if (referenceExistsInConfigurableElementValues(elements))
            {
                createReferencesForConfigurableElementValues(elements, instantiationItem);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::referenceExistsInPorts()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::referenceExistsInPorts()
{
    foreach(QSharedPointer<Port> port, *component_->getPorts())
    {
        if (portHasreference(port))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::portHasReference()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::portHasreference(QSharedPointer<Port> port)
{
    QString targetID = getTargetID();

    return port->getLeftBound().contains(targetID) || port->getRightBound().contains(targetID) ||
        port->getArrayLeft().contains(targetID) || port->getArrayRight().contains(targetID) ||
        port->getDefaultValue().contains(targetID);
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createReferencesForPorts()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createReferencesForPorts()
{
    QTreeWidgetItem* topPortsItem = createTopItem("Ports");

    foreach (QSharedPointer<Port> port, *component_->getPorts())
    {
        if (portHasreference(port))
        {
            QTreeWidgetItem* portItem = createMiddleItem(port->name(), topPortsItem);
            createItemsForPort(port, portItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::referenceExistsInMemoryMaps()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::referenceExistsInMemoryMaps()
{
    foreach (QSharedPointer<MemoryMap> memoryMap, *component_->getMemoryMaps())
    {
        if (referenceExistsInSingleMemoryMap(memoryMap))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::referenceExistsInSingleMemoryMap()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::referenceExistsInSingleMemoryMap(QSharedPointer<MemoryMap> memoryMap)
{
    if (referenceExistsInDefaultMemoryRemap(memoryMap))
    {
        return true;
    }

    foreach (QSharedPointer<MemoryRemap> memoryRemap, *memoryMap->getMemoryRemaps())
    {
        if (referenceExistsInSingleMemoryRemap(memoryRemap))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::referenceExistsInSingleMemoryMap()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::referenceExistsInDefaultMemoryRemap(QSharedPointer<MemoryMapBase> memoryMap) const
{
    if (memoryMap)
    {
        foreach (QSharedPointer<MemoryBlockBase> memoryBlock, *memoryMap->getMemoryBlocks())
        {
            QSharedPointer<AddressBlock> addressBlock = memoryBlock.dynamicCast<AddressBlock>();

            if (addressBlock)
            {
                if (referenceExistsInAddressBlock(addressBlock))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::referenceExistsInSingleMemoryRemap()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::referenceExistsInSingleMemoryRemap(QSharedPointer<MemoryRemap> memoryRemap)
{
    foreach(QSharedPointer<MemoryBlockBase> memoryBlock, *memoryRemap->getMemoryBlocks())
    {
        QSharedPointer<AddressBlock> addressBlock = memoryBlock.dynamicCast<AddressBlock>();

        if (addressBlock)
        {
            if (referenceExistsInAddressBlock(addressBlock))
            {
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::referenceExistsInAddressBlock()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::referenceExistsInAddressBlock(QSharedPointer<AddressBlock> addressBlock)
    const
{
    if (referenceExistsInAddressBlockValues(addressBlock))
    {
        return true;
    }

    return referenceExistsInRegisters(addressBlock->getRegisterData());
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::referenceExistsInAddressBlockValues()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::referenceExistsInAddressBlockValues(
    QSharedPointer<AddressBlock> addressBlock) const
{
    QString targetID = getTargetID();

    return addressBlock->getBaseAddress().contains(targetID) || addressBlock->getRange().contains(targetID) ||
        addressBlock->getWidth().contains(targetID);
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::referenceExistsInRegisters()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::referenceExistsInRegisters(
    QSharedPointer<QList<QSharedPointer<RegisterBase> > > registerList) const
{
    foreach (QSharedPointer<RegisterBase> registerItem, *registerList)
    {
        QSharedPointer<Register> targetRegister = registerItem.dynamicCast<Register>();

        if (targetRegister)
        {
            if (registerHasReference(targetRegister))
            {
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::registerHasReference()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::registerHasReference(QSharedPointer<Register> targetRegister) const
{
    QString targetID = getTargetID();

    if (targetRegister->getAddressOffset().contains(targetID) ||
        targetRegister->getDimension().contains(targetID) || targetRegister->getSize().contains(targetID) ||
        targetRegister->getIsPresent().contains(targetID))
    {
        return true;
    }

    if (referenceExistsInRegisterFields(targetRegister))
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::referenceExistsInRegisterFields()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::referenceExistsInRegisterFields(QSharedPointer<Register> targetRegister) const
{
    foreach (QSharedPointer<Field> registerField, *targetRegister->getFields())
    {
        if (registerFieldHasReference(registerField))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::registerFieldHasReference()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::registerFieldHasReference(QSharedPointer<Field> targetField) const
{
    QString targetID = getTargetID();

    return targetField->getBitOffset().contains(targetID) || targetField->getBitWidth().contains(targetID) ||
        targetField->getIsPresent().contains(targetID) || targetField->getResetValue().contains(targetID) ||
        targetField->getResetMask().contains(targetID) ||
        (targetField->getWriteConstraint() &&
        (targetField->getWriteConstraint()->getMinimum().contains(targetID) ||
        targetField->getWriteConstraint()->getMaximum().contains(targetID)));
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createReferencesForAddressSpaces()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createReferencesForAddressSpaces()
{
    QTreeWidgetItem* topAddressSpaceItem = createTopItem("Address Spaces");

    foreach (QSharedPointer<AddressSpace> addressSpace, *component_->getAddressSpaces())
    {
        if (referenceExistsInSingleAddressSpace(addressSpace))
        {
            QTreeWidgetItem* addressSpaceItem = createMiddleItem(addressSpace->name(), topAddressSpaceItem);

            if (referenceExistsInSingleAddressSpace(addressSpace))
            {
                createItemsForAddressSpace(addressSpace, addressSpaceItem);
            }

            if (referenceExistsInSegments(addressSpace))
            {
                QTreeWidgetItem* segmentsItem = createMiddleItem("Segments", addressSpaceItem);
                colourItemGrey(segmentsItem);

                createReferencesForSegments(addressSpace, segmentsItem);
            }

            if (addressSpace->hasLocalMemoryMap() &&
                referenceExistsInDefaultMemoryRemap(addressSpace->getLocalMemoryMap()))
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
    foreach (QSharedPointer<Segment> segment, *addressSpace->getSegments())
    {
        if (referenceExistsInSingleSegment(segment))
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

    foreach (QSharedPointer<MemoryMap> memoryMap, *component_->getMemoryMaps())
    {
        if (referenceExistsInSingleMemoryMap(memoryMap))
        {
            QTreeWidgetItem* memoryMapTreeItem = createMiddleItem(memoryMap->name(), topMemoryMapItem);

            QTreeWidgetItem* memoryRemapsTreeItem = createMiddleItem("Memory remaps", memoryMapTreeItem);
            colourItemGrey(memoryRemapsTreeItem);

            if (referenceExistsInDefaultMemoryRemap(memoryMap))
            {
                QTreeWidgetItem* defaultMemoryRemapItem = createMiddleItem("Default", memoryRemapsTreeItem);
                createReferencesForSingleMemoryMap(memoryMap, defaultMemoryRemapItem);
            }

            foreach (QSharedPointer<MemoryRemap> memoryRemap, *memoryMap->getMemoryRemaps())
            {
                if (referenceExistsInSingleMemoryRemap(memoryRemap))
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
    QTreeWidgetItem* middleAddressBlocksItem = createMiddleItem("Address blocks", memoryRemapItem);

    colourItemGrey(middleAddressBlocksItem);

    foreach (QSharedPointer<MemoryBlockBase> memoryBlock, *memoryMap->getMemoryBlocks())
    {
        QSharedPointer<AddressBlock> addressBlock = memoryBlock.dynamicCast<AddressBlock>();

        if (addressBlock)
        {
            if (referenceExistsInAddressBlock(addressBlock))
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

    if (referenceExistsInAddressBlockValues(addressBlock))
    {
        createItemsForAddressBlock(addressBlock, addressBlockItem);
    }

    if (referenceExistsInRegisters(addressBlock->getRegisterData()))
    {
        QTreeWidgetItem* registersItem = createMiddleItem("Registers", addressBlockItem);
        colourItemGrey(registersItem);

        foreach (QSharedPointer<RegisterBase> baseRegister, *addressBlock->getRegisterData())
        {
            QSharedPointer<Register> targetRegister = baseRegister.dynamicCast<Register>();

            if (targetRegister)
            {
                if (registerHasReference(targetRegister))
                {
                    createReferencesForSingleRegister(targetRegister, registersItem);
                }
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
    createItemsForRegister(targetRegister, registerItem);

    if (referenceExistsInRegisterFields(targetRegister))
    {
        QTreeWidgetItem* fieldsItem = createMiddleItem("Fields", registerItem);
        colourItemGrey(fieldsItem);

        foreach (QSharedPointer<Field> registerField, *targetRegister->getFields())
        {
            if (registerFieldHasReference(registerField))
            {
                QTreeWidgetItem* singleFieldItem = createMiddleItem(registerField->name(), fieldsItem);
                createItemsForField(registerField, singleFieldItem);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::referenceExistsInBusInterfaces()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::referenceExistsInBusInterfaces() const
{
    foreach (QSharedPointer<BusInterface> busInterface, *component_->getBusInterfaces())
    {
        if (referenceExistsInSingleBusInterface(busInterface))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::referenceExistsInSingleBusInterface()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::referenceExistsInSingleBusInterface(
    QSharedPointer<BusInterface> busInterface) const
{
    foreach (QSharedPointer<Parameter> parameter, *busInterface->getParameters())
    {
        if (parameterHasReference(parameter))
        {
            return true;
        }
    }

    if (busInterface->getMirroredSlave())
    {
        if (referenceExistsInMirroredSlave(busInterface->getMirroredSlave()))
        {
            return true;
        }
    }

    if (busInterface->getMaster())
    {
        if (referenceExistsInMasterInterface(busInterface->getMaster()))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::referenceExistsInMirroredSlave()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::referenceExistsInMirroredSlave(
    QSharedPointer<MirroredSlaveInterface> mirrorSlave) const
{
    return (mirroredSlaveRemapAddressHasReference(mirrorSlave) ||mirroredSlaveRangeHasReference(mirrorSlave));
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::mirroredSlaveRemapAddressHasReference()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::mirroredSlaveRemapAddressHasReference(
    QSharedPointer<MirroredSlaveInterface> mirrorSlave) const
{
    foreach (QSharedPointer<MirroredSlaveInterface::RemapAddress> remapAddress, *mirrorSlave->getRemapAddresses())
    {
        if (remapAddress->remapAddress_.contains(getTargetID()))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::mirroredSlaveRangeHasReference()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::mirroredSlaveRangeHasReference(
    QSharedPointer<MirroredSlaveInterface> mirrorSlave) const
{
    return mirrorSlave->getRange().contains(getTargetID());
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::referenceExistsInMasterInterface()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::referenceExistsInMasterInterface(
    QSharedPointer<MasterInterface> masterInterface) const
{
    return masterInterface->getBaseAddress().contains(getTargetID());
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createReferencesForBusInterfaces()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createReferencesForBusInterfaces()
{
    QTreeWidgetItem* topBusInterfaceItem = createTopItem("Bus Interfaces");

    foreach (QSharedPointer<BusInterface> busInterface, *component_->getBusInterfaces())
    {
        if (referenceExistsInSingleBusInterface(busInterface))
        {
            QTreeWidgetItem* busInterfaceItem = createMiddleItem(busInterface->name(), topBusInterfaceItem);

            if (busInterface->getMirroredSlave())
            {
                QSharedPointer<MirroredSlaveInterface> mirrorSlave(busInterface->getMirroredSlave());

                if (referenceExistsInMirroredSlave(mirrorSlave))
                {
                    QTreeWidgetItem* mirroredInterfaceItem =
                        createMiddleItem("Mirrored Slave Interface", busInterfaceItem);
                    colourItemGrey(mirroredInterfaceItem);

                    if (mirroredSlaveRemapAddressHasReference(mirrorSlave))
                    {
                        foreach (QSharedPointer<MirroredSlaveInterface::RemapAddress> remapAddress,
                            *mirrorSlave->getRemapAddresses())
                        {
                            createItem("Remap Address", remapAddress->remapAddress_, mirroredInterfaceItem);
                        }
                    }
                    if (mirroredSlaveRangeHasReference(mirrorSlave))
                    {
                        createItem("Range", mirrorSlave->getRange(), mirroredInterfaceItem);
                    }
                }
            }

            if (busInterface->getMaster())
            {
                QSharedPointer<MasterInterface> master(busInterface->getMaster());

                if (referenceExistsInMasterInterface(master))
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

            if (referenceExistsInParameters(busInterface->getParameters()))
            {
                QTreeWidgetItem* parametersItem = createMiddleItem(QString("Parameters"), busInterfaceItem);
                colourItemGrey(parametersItem);
                createParameterReferences(busInterface->getParameters(), parametersItem);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::referenceExistsInRemapStates()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::referenceExistsInRemapStates() const
{
    foreach(QSharedPointer<RemapState> remapState, *component_->getRemapStates())
    {
        if (referenceExistsInSingleRemapState(remapState))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::referenceExistsInSingleRemapState()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::referenceExistsInSingleRemapState(QSharedPointer<RemapState> remapState)
    const
{
    foreach (QSharedPointer<RemapPort> remapPort, *remapState->getRemapPorts())
    {
        if (referenceExistsInSingleRemapPort(remapPort))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::referenceExistsInSingleRemapPort()
//-----------------------------------------------------------------------------
bool ComponentParameterReferenceTree::referenceExistsInSingleRemapPort(QSharedPointer<RemapPort> remapPort) const
{
    if (remapPort->getValue().contains(getTargetID()))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterReferenceTree::createReferencesForRemapStates()
//-----------------------------------------------------------------------------
void ComponentParameterReferenceTree::createReferencesForRemapStates()
{
    QTreeWidgetItem* topRemapStatesItem = createTopItem("Remap States");

    foreach (QSharedPointer<RemapState> remapState, *component_->getRemapStates())
    {
        if (referenceExistsInSingleRemapState(remapState))
        {
            QTreeWidgetItem* remapStateItem = createMiddleItem(remapState->name(), topRemapStatesItem);

            QTreeWidgetItem* remapPortsItem = createMiddleItem("Remap Ports", remapStateItem);
            colourItemGrey(remapPortsItem);

            foreach (QSharedPointer<RemapPort> remapPort, *remapState->getRemapPorts())
            {
                if (referenceExistsInSingleRemapPort(remapPort))
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

    if (targetRegister->getAddressOffset().contains(targetID))
    {
        createItem("Offset", targetRegister->getAddressOffset(), parent);
    }
    if (targetRegister->getDimension().contains(targetID))
    {
        createItem("Dimension", targetRegister->getDimension(), parent);
    }
    if (targetRegister->getIsPresent().contains(targetID))
    {
        createItem("Is present", targetRegister->getIsPresent(), parent);
    }
    if (targetRegister->getSize().contains(targetID))
    {
        createItem("Size", targetRegister->getSize(), parent);
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
    if (targetField->getResetValue().contains(targetID))
    {
        createItem("Reset value", targetField->getResetValue(), parent);
    }
    if (targetField->getResetMask().contains(targetID))
    {
        createItem("Reset mask", targetField->getResetMask(), parent);
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
