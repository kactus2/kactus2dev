//-----------------------------------------------------------------------------
// File: ParameterReferenceTree.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 20.01.2015
//
// Description:
// Tree widget for displaying references to a parameter within a component.
//-----------------------------------------------------------------------------

#include "ParameterReferenceTree.h"

#include <IPXACTmodels/Component/MemoryBlockBase.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/WriteValueConstraint.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/BuildCommand.h>

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/ModuleParameter.h>
#include <IPXACTmodels/common/FileBuilder.h>

namespace
{
    const QString VALUENAME = QString("Value");
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::ParameterReferenceTree()
//-----------------------------------------------------------------------------
ParameterReferenceTree::ParameterReferenceTree(QSharedPointer<Component> component,
    QSharedPointer<ExpressionFormatter> expressionFormatter, QString const& targetID, QWidget *parent /* = 0 */):
QTreeWidget(parent),
component_(component),
targetID_(targetID),
expressionFormatter_(expressionFormatter)
{
    QStringList labels;
    labels << "Referencing elements" << "Referencing expression";
    setHeaderLabels(labels);

    setColumnCount(COLUMN_COUNT);
    uniformRowHeights();

    setupTree();

    setColumnWidth(ITEM_NAME, 240);
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::~ParameterReferenceTree()
//-----------------------------------------------------------------------------
ParameterReferenceTree::~ParameterReferenceTree()
{

}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::setupTree()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::setupTree()
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

        if (referenceExistsInComponentInstantiations())
        {
            createReferencesForComponentInstantiations();
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
// Function: ParameterReferenceTree::referenceExistsInFileSets()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInFileSets() const
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
// Function: ParameterReferenceTree::fileSetHasReference()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::fileSetHasReference(QSharedPointer<FileSet> fileSet) const
{
    return referenceExistsInFileBuilders(fileSet->getDefaultFileBuilders()) ||
        referenceExistsInFiles(fileSet->getFiles());
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInFileBuilders()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInFileBuilders(
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
// Function: ParameterReferenceTree::fileBuilderHasReference()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::fileBuilderHasReference(QSharedPointer<FileBuilder> fileBuilder) const
{
    return fileBuilder->getReplaceDefaultFlags().contains(targetID_);
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInFiles()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInFiles(QSharedPointer<QList<QSharedPointer<File> > > fileList) const
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
// Function: ParameterReferenceTree::fileHasReference()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::fileHasReference(QSharedPointer<File> file) const
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
// Function: ParameterReferenceTree::buildCommandHasReference()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::buildCommandHasReference(QSharedPointer<BuildCommand> fileBuildCommand) const
{
    return fileBuildCommand->getReplaceDefaultFlags().contains(targetID_);
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createReferencesForFileSets()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createReferencesForFileSets()
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
// Function: ParameterReferenceTree::createReferencesForFileBuilders()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createReferencesForFileBuilders(
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
// Function: ParameterReferenceTree::createReferencesForSingleFileBuilder()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createReferencesForSingleFileBuilder(QSharedPointer<FileBuilder> fileBuilder,
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
// Function: ParameterReferenceTree::createReferencesForFiles()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createReferencesForFiles(QSharedPointer<QList<QSharedPointer<File> > > fileList,
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
// Function: ParameterReferenceTree::createReferencesForSingleFile()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createReferencesForSingleFile(QSharedPointer<File> selectedFile,
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
// Function: ParameterReferenceTree::referenceExistsInParameters()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInParameters(
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameterList) const
{
    foreach (QSharedPointer<Parameter> parameter, *parameterList)
    {
        if (parameterHasReference(parameter))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::parameterHasReference()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::parameterHasReference(QSharedPointer<Parameter> parameter) const
{
    if (parameter->getValue().contains(targetID_) ||
        parameter->getVectorLeft().contains(targetID_) || parameter->getVectorRight().contains(targetID_) ||
        parameter->getAttribute("kactus2:arrayLeft").contains(targetID_) ||
        parameter->getAttribute("kactus2:arrayRight").contains(targetID_))
    {
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createParameterReferences()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createParameterReferences(
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters, QTreeWidgetItem* parentItem)
{
    foreach (QSharedPointer<Parameter> parameter, *parameters)
    {
        if (parameterHasReference(parameter))
        {
            QTreeWidgetItem* parameterItem = createMiddleItem(parameter->name(), parentItem);
            createItemsForParameter(parameter, parameterItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInModuleParameters()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInModuleParameters(
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
// Function: ParameterReferenceTree::createReferencesForModuleParameters()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createReferencesForModuleParameters(
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
// Function: ParameterReferenceTree::referenceExistsInAddressSpaces()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInAddressSpaces() const
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
// Function: ParameterReferenceTree::referenceExistsInAddressSpace()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInSingleAddressSpace(QSharedPointer<AddressSpace> addressSpace) const
{
    return addressSpace->getWidth().contains(targetID_) ||
        addressSpace->getRange().contains(targetID_) ||
        referenceExistsInSegments(addressSpace) || 
        referenceExistsInDefaultMemoryRemap(addressSpace->getLocalMemoryMap());
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInSegments()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInSegments(QSharedPointer<AddressSpace> addressSpace) const
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
// Function: ParameterReferenceTree::referenceExistsInSingleSegment()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInSingleSegment(QSharedPointer<Segment> segment) const
{
    return segment->getAddressOffset().contains(targetID_) || segment->getRange().contains(targetID_);
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInComponentInstantiations()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInComponentInstantiations() const
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
// Function: ParameterReferenceTree::referenceExistsInSingleComponentInstantiation()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInSingleComponentInstantiation(
    QSharedPointer<ComponentInstantiation> instantiation) const
{
    return referenceExistsInModuleParameters(instantiation->getModuleParameters()) ||
        referenceExistsInParameters(instantiation->getParameters()) ||
        referenceExistsInFileBuilders(instantiation->getDefaultFileBuilders());
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createReferencesForComponentInstantiations()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createReferencesForComponentInstantiations()
{
    QTreeWidgetItem* topComponentInstantiationsItem = createTopItem("Component instantiations");

    foreach (QSharedPointer<ComponentInstantiation> instantiation, *component_->getComponentInstantiations())
    {
        if (referenceExistsInSingleComponentInstantiation(instantiation))
        {
            QTreeWidgetItem* instantiationItem =
                createMiddleItem(instantiation->name(), topComponentInstantiationsItem);

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
// Function: ParameterReferenceTree::referenceExistsInPorts()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInPorts()
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
// Function: ParameterReferenceTree::portHasReference()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::portHasreference(QSharedPointer<Port> port)
{
    return port->getLeftBound().contains(targetID_) || port->getRightBound().contains(targetID_) ||
        port->getArrayLeft().contains(targetID_) || port->getArrayRight().contains(targetID_) ||
        port->getDefaultValue().contains(targetID_);
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createReferencesForPorts()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createReferencesForPorts()
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
// Function: ParameterReferenceTree::referenceExistsInMemoryMaps()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInMemoryMaps()
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
// Function: ParameterReferenceTree::referenceExistsInSingleMemoryMap()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInSingleMemoryMap(QSharedPointer<MemoryMap> memoryMap)
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
// Function: ParameterReferenceTree::referenceExistsInSingleMemoryMap()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInDefaultMemoryRemap(QSharedPointer<MemoryMapBase> memoryMap) const
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
// Function: ParameterReferenceTree::()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInSingleMemoryRemap(QSharedPointer<MemoryRemap> memoryRemap)
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
// Function: ParameterReferenceTree::referenceExistsInAddressBlock()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInAddressBlock(QSharedPointer<AddressBlock> addressBlock) const
{
    if (referenceExistsInAddressBlockValues(addressBlock))
    {
        return true;
    }

    return referenceExistsInRegisters(addressBlock->getRegisterData());
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInAddressBlockValues()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInAddressBlockValues(QSharedPointer<AddressBlock> addressBlock) const
{
    return addressBlock->getBaseAddress().contains(targetID_) || addressBlock->getRange().contains(targetID_) ||
        addressBlock->getWidth().contains(targetID_);
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInRegisters()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInRegisters(
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
// Function: ParameterReferenceTree::registerHasReference()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::registerHasReference(QSharedPointer<Register> targetRegister) const
{
    if (targetRegister->getAddressOffset().contains(targetID_) ||
        targetRegister->getDimension().contains(targetID_) || targetRegister->getSize().contains(targetID_) ||
        targetRegister->getIsPresent().contains(targetID_))
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
// Function: ParameterReferenceTree::referenceExistsInRegisterFields()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInRegisterFields(QSharedPointer<Register> targetRegister) const
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
// Function: ParameterReferenceTree::registerFieldHasReference()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::registerFieldHasReference(QSharedPointer<Field> targetField) const
{
    return targetField->getBitOffset().contains(targetID_) || targetField->getBitWidth().contains(targetID_) ||
        targetField->getIsPresent().contains(targetID_) || targetField->getResetValue().contains(targetID_) ||
        targetField->getResetMask().contains(targetID_) ||
        (targetField->getWriteConstraint() &&
        (targetField->getWriteConstraint()->getMinimum().contains(targetID_) ||
        targetField->getWriteConstraint()->getMaximum().contains(targetID_)));
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createReferencesForAddressSpaces()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createReferencesForAddressSpaces()
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

            if (addressSpace->hasLocalMemoryMap() && referenceExistsInDefaultMemoryRemap(addressSpace->getLocalMemoryMap()))
            {
                QSharedPointer <MemoryMapBase> localMemoryMap = addressSpace->getLocalMemoryMap();
                QTreeWidgetItem* memoryMapItem = createMiddleItem(localMemoryMap->name(), addressSpaceItem);
                createReferencesForSingleMemoryMap(localMemoryMap, memoryMapItem);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createItemsForAddressSpace()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createItemsForAddressSpace(QSharedPointer<AddressSpace> addressSpace,
    QTreeWidgetItem* parent)
{
    if (addressSpace->getWidth().contains(targetID_))
    {
        createItem(QLatin1String("Width"), addressSpace->getWidth(), parent);
    }

    if (addressSpace->getRange().contains(targetID_))
    {
        createItem(QLatin1String("Range"), addressSpace->getRange(), parent);
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createReferencesForSegments()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createReferencesForSegments(QSharedPointer<AddressSpace> addressSpace,
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
// Function: ParameterReferenceTree::createItemsForSegment()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createItemsForSegment(QSharedPointer<Segment> segment, QTreeWidgetItem* parent)
{
    QTreeWidgetItem* segmentItem = createMiddleItem(segment->name(), parent);

    if (segment->getAddressOffset().contains(targetID_))
    {
        createItem("Offset", segment->getAddressOffset(), segmentItem);
    }

    if (segment->getRange().contains(targetID_))
    {
        createItem("Range", segment->getRange(), segmentItem);
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createReferencesForMemoryMaps()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createReferencesForMemoryMaps()
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
// Function: ParameterReferenceTree::createReferencesForSingleMemoryMap()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createReferencesForSingleMemoryMap(QSharedPointer<MemoryMapBase> memoryMap,
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
// Function: ParameterReferenceTree::createReferencesForSingleAddressBlock()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createReferencesForSingleAddressBlock(QSharedPointer<AddressBlock> addressBlock,
    QTreeWidgetItem* middleAddressBlocksItem)
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
// Function: ParameterReferenceTree::createReferencesForSingleRegister()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createReferencesForSingleRegister(QSharedPointer<Register> targetRegister,
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
// Function: ParameterReferenceTree::referenceExistsInBusInterfaces()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInBusInterfaces() const
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
// Function: ParameterReferenceTree::referenceExistsInSingleBusInterface()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInSingleBusInterface(QSharedPointer<BusInterface> busInterface) const
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
// Function: ParameterReferenceTree::referenceExistsInMirroredSlave()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInMirroredSlave(QSharedPointer<MirroredSlaveInterface> mirrorSlave) const
{
    return (mirroredSlaveRemapAddressHasReference(mirrorSlave) ||mirroredSlaveRangeHasReference(mirrorSlave));
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::mirroredSlaveRemapAddressHasReference()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::mirroredSlaveRemapAddressHasReference(
    QSharedPointer<MirroredSlaveInterface> mirrorSlave) const
{
    foreach (QSharedPointer<MirroredSlaveInterface::RemapAddress> remapAddress, *mirrorSlave->getRemapAddresses())
    {
        if (remapAddress->remapAddress_.contains(targetID_))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::mirroredSlaveRangeHasReference()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::mirroredSlaveRangeHasReference(QSharedPointer<MirroredSlaveInterface> mirrorSlave) const
{
    return mirrorSlave->getRange().contains(targetID_);
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInMasterInterface()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInMasterInterface(QSharedPointer<MasterInterface> masterInterface) const
{
    return masterInterface->getBaseAddress().contains(targetID_);
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createReferencesForBusInterfaces()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createReferencesForBusInterfaces()
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
                createParameterReferences(busInterface->getParameters(), busInterfaceItem);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInRemapStates()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInRemapStates() const
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
// Function: ParameterReferenceTree::referenceExistsInSingleRemapState()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInSingleRemapState(QSharedPointer<RemapState> remapState) const
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
// Function: ParameterReferenceTree::referenceExistsInSingleRemapPort()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInSingleRemapPort(QSharedPointer<RemapPort> remapPort) const
{
    if (remapPort->getValue().contains(targetID_))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createReferencesForRemapStates()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createReferencesForRemapStates()
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
// Function: ParameterReferenceTree::createTopItem()
//-----------------------------------------------------------------------------
QTreeWidgetItem* ParameterReferenceTree::createTopItem(QString const& itemName)
{
    QTreeWidgetItem* newItem = new QTreeWidgetItem();
    newItem->setText(ITEM_NAME, itemName);

    addTopLevelItem(newItem);

    colourItemGrey(newItem);

    return newItem;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createMiddleItem()
//-----------------------------------------------------------------------------
QTreeWidgetItem* ParameterReferenceTree::createMiddleItem(QString const& itemnName, QTreeWidgetItem* parent)
{
    QTreeWidgetItem* newItem = new QTreeWidgetItem(parent);
    newItem->setText(ITEM_NAME, itemnName);

    return newItem;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createItemsForParameter()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createItemsForParameter(QSharedPointer<Parameter> parameter, QTreeWidgetItem* parent)
{
    if (parameter->getValue().contains(targetID_))
    {
        createItem("Value", parameter->getValue(), parent);
    }
    if (parameter->getVectorLeft().contains(targetID_))
    {
        createItem("Bit Width Left", parameter->getVectorLeft(), parent);
    }
    if (parameter->getVectorRight().contains(targetID_))
    {
        createItem("Bit Width Right", parameter->getVectorRight(), parent);
    }
    if (parameter->getAttribute("kactus2:arrayLeft").contains(targetID_))
    {
        createItem("Array Left", parameter->getAttribute("kactus2:arrayLeft"), parent);
    }
    if (parameter->getAttribute("kactus2:arrayRight").contains(targetID_))
    {
        createItem("Array Right", parameter->getAttribute("kactus2:arrayRight"), parent);
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createItemsForPort()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createItemsForPort(QSharedPointer<Port> port, QTreeWidgetItem* parent)
{
    if (port->getLeftBound().contains(targetID_))
    {
        createItem("Left Bound", port->getLeftBound(), parent);
    }
    if (port->getRightBound().contains(targetID_))
    {
        createItem("Right Bound", port->getRightBound(), parent);
    }
    if (port->getDefaultValue().contains(targetID_))
    {
        createItem("Default Value", port->getDefaultValue(), parent);
    }
    if (port->getArrayLeft().contains(targetID_))
    {
        createItem("Array Left", port->getArrayLeft(), parent);
    }
    if (port->getArrayRight().contains(targetID_))
    {
        createItem("Array Right", port->getArrayRight(), parent);
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createItemsForRegister()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createItemsForRegister(QSharedPointer<Register> targetRegister,
    QTreeWidgetItem* parent)
{
    if (targetRegister->getAddressOffset().contains(targetID_))
    {
        createItem("Offset", targetRegister->getAddressOffset(), parent);
    }
    if (targetRegister->getDimension().contains(targetID_))
    {
        createItem("Dimension", targetRegister->getDimension(), parent);
    }
    if (targetRegister->getIsPresent().contains(targetID_))
    {
        createItem("Is present", targetRegister->getIsPresent(), parent);
    }
    if (targetRegister->getSize().contains(targetID_))
    {
        createItem("Size", targetRegister->getSize(), parent);
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createItemsForAddressBlock()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createItemsForAddressBlock(QSharedPointer<AddressBlock> targetAddressBlock,
    QTreeWidgetItem* parent)
{
    if (targetAddressBlock->getBaseAddress().contains(targetID_))
    {
        createItem("Base Address", targetAddressBlock->getBaseAddress(), parent);
    }
    if (targetAddressBlock->getRange().contains(targetID_))
    {
        createItem("Range", targetAddressBlock->getRange(), parent);
    }
    if (targetAddressBlock->getWidth().contains(targetID_))
    {
        createItem("Width", targetAddressBlock->getWidth(), parent);
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createItemsForField()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createItemsForField(QSharedPointer<Field> targetField, QTreeWidgetItem* parent)
{
    if (targetField->getBitOffset().contains(targetID_))
    {
        createItem("Offset", targetField->getBitOffset(), parent);
    }
    if (targetField->getBitWidth().contains(targetID_))
    {
        createItem("Width", targetField->getBitWidth(), parent);
    }
    if (targetField->getIsPresent().contains(targetID_))
    {
        createItem("Is Present", targetField->getIsPresent(), parent);
    }
    if (targetField->getResetValue().contains(targetID_))
    {
        createItem("Reset value", targetField->getResetValue(), parent);
    }
    if (targetField->getResetMask().contains(targetID_))
    {
        createItem("Reset mask", targetField->getResetMask(), parent);
    }
    if (targetField->getWriteConstraint())
    {
        if (targetField->getWriteConstraint()->getMinimum().contains(targetID_))
        {
            createItem("Write constraint minimum", targetField->getWriteConstraint()->getMinimum(), parent);
        }
        if (targetField->getWriteConstraint()->getMaximum().contains(targetID_))
        {
            createItem("Write constraint maximum", targetField->getWriteConstraint()->getMaximum(), parent);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createItem()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createItem
    (QString const& itemName, QString const& expression, QTreeWidgetItem* parent)
{
    QTreeWidgetItem* newItem = new QTreeWidgetItem(parent);
    newItem->setText(ITEM_NAME, itemName);
    newItem->setText(ITEM_EXPRESSION, expressionFormatter_->formatReferringExpression(expression));
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::colourItemGrey()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::colourItemGrey(QTreeWidgetItem* item)
{
    for (int columnIndex = 0; columnIndex < COLUMN_COUNT; ++columnIndex)
    {
        item->setBackgroundColor(columnIndex, QColor(230, 230, 230));
    }
}
