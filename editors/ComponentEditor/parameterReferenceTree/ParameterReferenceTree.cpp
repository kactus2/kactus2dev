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

#include <IPXACTmodels/memorymapitem.h>
#include <IPXACTmodels/businterface.h>

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
        if (referenceExistsInModelParameters(component_->getModelParameters()))
        {
            QTreeWidgetItem* topModelParameterItem = createTopItem("Model Parameters");
            createReferencesForModelParameters(component_->getModelParameters(), topModelParameterItem);
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

        if (referenceExistsInViews())
        {
            createReferencesForViews();
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
        parameter->getBitWidthLeft().contains(targetID_) || parameter->getBitWidthRight().contains(targetID_) ||
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
            QTreeWidgetItem* parameterItem = createMiddleItem(parameter->getName(), parentItem);
            createItemsForParameter(parameter, parameterItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInModelParameters()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInModelParameters(
    QSharedPointer<QList<QSharedPointer<ModelParameter> > > modelParameters) const
{
    foreach(QSharedPointer<Parameter> modelParameter, *modelParameters)
    {
        if(parameterHasReference(modelParameter))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createReferencesForModelParameters()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createReferencesForModelParameters(
    QSharedPointer<QList<QSharedPointer<ModelParameter> > > modelParametes, QTreeWidgetItem* parentItem)
{
    foreach (QSharedPointer<ModelParameter> modelParameter, *modelParametes)
    {
        if (parameterHasReference(modelParameter))
        {
            QTreeWidgetItem* modelParameterItem = createMiddleItem(modelParameter->getName(), parentItem);
            createItemsForParameter(modelParameter, modelParameterItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInAddressSpaces()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInAddressSpaces() const
{
    foreach(QSharedPointer<AddressSpace> addressSpace, component_->getAddressSpaces())
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
    return addressSpace->getWidthExpression().contains(targetID_) || 
        addressSpace->getRange().contains(targetID_) ||
        referenceExistsInSegments(addressSpace) || 
        referenceExistsInDefaultMemoryRemap(addressSpace->getLocalMemoryMap());
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInSegments()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInSegments(QSharedPointer<AddressSpace> addressSpace) const
{
    foreach (QSharedPointer<Segment> segment, addressSpace->getSegments())
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
// Function: ParameterReferenceTree::referenceExistsInViews()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInViews() const
{
    foreach(QSharedPointer<View> view, component_->getViews())
    {
        if (referenceExistsInView(view))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInView()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInView(QSharedPointer<View> view) const
{
    foreach (QSharedPointer<Parameter> parameter, *view->getParameters())
    {
        if (parameterHasReference(parameter))
        {
            return true;
        }
    }

    foreach(QSharedPointer<ModelParameter> moduleParameter, *view->getModuleParameters())
    {
        if (parameterHasReference(moduleParameter))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createReferencesForViews()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createReferencesForViews()
{
    QTreeWidgetItem* topViewsItem = createTopItem("Views");

    foreach (QSharedPointer<View> view, component_->getViews())
    {
        if (referenceExistsInView(view))
        {
            QTreeWidgetItem* viewItem = createMiddleItem(view->getName(), topViewsItem);
            if (referenceExistsInParameters(view->getParameters()))
            {
                QTreeWidgetItem* viewParametersItem = createMiddleItem("Parameters", viewItem);
                colourItemGrey(viewParametersItem);

                createParameterReferences(view->getParameters(), viewParametersItem);
            }
            if (referenceExistsInModelParameters(view->getModuleParameters()))
            {
                QTreeWidgetItem* viewModuleParametersItem = createMiddleItem("Module Parameters", viewItem);
                colourItemGrey(viewModuleParametersItem);

                createReferencesForModelParameters(view->getModuleParameters(), viewModuleParametersItem);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInPorts()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInPorts()
{
    foreach(QSharedPointer<Port> port, component_->getPorts())
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
    return port->getLeftBoundExpression().contains(targetID_) ||
        port->getRightBoundExpression().contains(targetID_) || port->getDefaultValue().contains(targetID_) ||
        port->getArrayLeft().contains(targetID_) || port->getArrayRight().contains(targetID_);
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createReferencesForPorts()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createReferencesForPorts()
{
    QTreeWidgetItem* topPortsItem = createTopItem("Ports");

    foreach (QSharedPointer<Port> port, component_->getPorts())
    {
        if (portHasreference(port))
        {
            QTreeWidgetItem* portItem = createMiddleItem(port->getName(), topPortsItem);
            createItemsForPort(port, portItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInMemoryMaps()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInMemoryMaps()
{
    foreach (QSharedPointer<MemoryMap> memoryMap, component_->getMemoryMaps())
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
bool ParameterReferenceTree::referenceExistsInDefaultMemoryRemap(QSharedPointer<MemoryMap> memoryMap) const
{
    foreach(QSharedPointer<MemoryMapItem> addressBlockItem, memoryMap->getItems())
    {
        QSharedPointer<AddressBlock> addressBlock = addressBlockItem.dynamicCast<AddressBlock>();

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
// Function: ParameterReferenceTree::()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInSingleMemoryRemap(QSharedPointer<MemoryRemap> memoryRemap)
{
    foreach(QSharedPointer<MemoryMapItem> addressBlockItem, memoryRemap->getItems())
    {
        QSharedPointer<AddressBlock> addressBlock = addressBlockItem.dynamicCast<AddressBlock>();

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

    foreach (QSharedPointer<RegisterModel> registerItem, addressBlock->getRegisterData())
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
// Function: ParameterReferenceTree::referenceExistsInAddressBlockValues()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInAddressBlockValues(QSharedPointer<AddressBlock> addressBlock) const
{
    return addressBlock->getBaseAddress().contains(targetID_) || addressBlock->getRange().contains(targetID_) ||
        addressBlock->getWidthExpression().contains(targetID_);
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::registerHasReference()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::registerHasReference(QSharedPointer<Register> targetRegister) const
{
    if (targetRegister->getAddressOffset().contains(targetID_) ||
        targetRegister->getDimensionExpression().contains(targetID_) ||
        targetRegister->getSizeExpression().contains(targetID_) ||
        targetRegister->getIsPresentExpression().contains(targetID_))
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
    foreach (QSharedPointer<Field> registerField, targetRegister->getFields())
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
    return targetField->getBitOffsetExpression().contains(targetID_) ||
        targetField->getBitWidthExpression().contains(targetID_) ||
        targetField->getIsPresentExpression().contains(targetID_);
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createReferencesForAddressSpaces()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createReferencesForAddressSpaces()
{
    QTreeWidgetItem* topAddressSpaceItem = createTopItem("Address Spaces");

    foreach (QSharedPointer<AddressSpace> addressSpace, component_->getAddressSpaces())
    {
        if (referenceExistsInSingleAddressSpace(addressSpace))
        {
            QTreeWidgetItem* addressSpaceItem = createMiddleItem(addressSpace->getName(), topAddressSpaceItem);

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
                QSharedPointer <MemoryMap> localMemoryMap = addressSpace->getLocalMemoryMap();
                QTreeWidgetItem* memoryMapItem = createMiddleItem(localMemoryMap->getName(), addressSpaceItem);
                createReferencesForSingleMemoryMap(localMemoryMap, memoryMapItem);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createItemsForAddressSpace()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createItemsForAddressSpace(QSharedPointer<AddressSpace> addressSpace, QTreeWidgetItem* parent)
{
    if (addressSpace->getWidthExpression().contains(targetID_))
    {
        createItem("Width", addressSpace->getWidthExpression(), parent);
    }

    if (addressSpace->getRange().contains(targetID_))
    {
        createItem("Range", addressSpace->getRange(), parent);
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createReferencesForSegments()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createReferencesForSegments(QSharedPointer<AddressSpace> addressSpace, QTreeWidgetItem* parent)
{
    foreach (QSharedPointer<Segment> segment, addressSpace->getSegments())
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
    QTreeWidgetItem* segmentItem = createMiddleItem(segment->getName(), parent);

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

    foreach (QSharedPointer<MemoryMap> memoryMap, component_->getMemoryMaps())
    {
        if (referenceExistsInSingleMemoryMap(memoryMap))
        {
            QTreeWidgetItem* memoryMapTreeItem = createMiddleItem(memoryMap->getName(), topMemoryMapItem);

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
                    QTreeWidgetItem* memoryRemapItem = createMiddleItem(memoryRemap->getName(), memoryRemapsTreeItem);
                    createReferencesForSingleMemoryMap(memoryRemap, memoryRemapItem);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createReferencesForSingleMemoryMap()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createReferencesForSingleMemoryMap(QSharedPointer<AbstractMemoryMap> memoryMap,
    QTreeWidgetItem* memoryRemapItem)
{
    QTreeWidgetItem* middleAddressBlocksItem = createMiddleItem("Address blocks", memoryRemapItem);

    colourItemGrey(middleAddressBlocksItem);

    foreach (QSharedPointer<MemoryMapItem> memorymapItem, memoryMap->getItems())
    {
        QSharedPointer<AddressBlock> addressBlock = memorymapItem.dynamicCast<AddressBlock>();

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
    QTreeWidgetItem* addressBlockItem = createMiddleItem(addressBlock->getName(),
        middleAddressBlocksItem);

    if (referenceExistsInAddressBlockValues(addressBlock))
    {
        createItemsForAddressBlock(addressBlock, addressBlockItem);
    }

    foreach (QSharedPointer<RegisterModel> registerModel,
        addressBlock->getRegisterData())
    {
        QSharedPointer<Register> targetRegister = registerModel.dynamicCast<Register>();

        if (targetRegister)
        {
            if (registerHasReference(targetRegister))
            {
                createReferencesForSingleRegister(targetRegister, addressBlockItem);
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
    QTreeWidgetItem* registerItem = createMiddleItem(targetRegister->getName(), parentItem);
    createItemsForRegister(targetRegister, registerItem);

    if (referenceExistsInRegisterFields(targetRegister))
    {
        QTreeWidgetItem* fieldsItem = createMiddleItem("Fields", registerItem);
        colourItemGrey(fieldsItem);

        foreach (QSharedPointer<Field> registerField, targetRegister->getFields())
        {
            if (registerFieldHasReference(registerField))
            {
                QTreeWidgetItem* singleFieldItem = createMiddleItem(registerField->getName(), fieldsItem);
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
    foreach (QSharedPointer<BusInterface> busInterface, component_->getBusInterfaces())
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
    return mirrorSlave->getRemapAddress().contains(targetID_);
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

    foreach (QSharedPointer<BusInterface> busInterface, component_->getBusInterfaces())
    {
        if (referenceExistsInSingleBusInterface(busInterface))
        {
            QTreeWidgetItem* busInterfaceItem = createMiddleItem(busInterface->getName(), topBusInterfaceItem);

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
                        createItem("Remap Address", mirrorSlave->getRemapAddress(), mirroredInterfaceItem);
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
            QTreeWidgetItem* remapStateItem = createMiddleItem(remapState->getName(), topRemapStatesItem);

            QTreeWidgetItem* remapPortsItem = createMiddleItem("Remap Ports", remapStateItem);
            colourItemGrey(remapPortsItem);

            foreach (QSharedPointer<RemapPort> remapPort, *remapState->getRemapPorts())
            {
                if (referenceExistsInSingleRemapPort(remapPort))
                {
                    QString itemName = remapPort->getPortNameRef();

                    if (remapPort->getPortIndex() >= 0)
                    {
                        itemName.append("[" + QString::number(remapPort->getPortIndex()) + "]");
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
    if (parameter->getBitWidthLeft().contains(targetID_))
    {
        createItem("Bit Width Left", parameter->getBitWidthLeft(), parent);
    }
    if (parameter->getBitWidthRight().contains(targetID_))
    {
        createItem("Bit Width Right", parameter->getBitWidthRight(), parent);
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
    if (port->getLeftBoundExpression().contains(targetID_))
    {
        createItem("Left (Higher) Bound", port->getLeftBoundExpression(), parent);
    }
    if (port->getRightBoundExpression().contains(targetID_))
    {
        createItem("Right (Lower) Bound", port->getRightBoundExpression(), parent);
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
    if (targetRegister->getDimensionExpression().contains(targetID_))
    {
        createItem("Dimension", targetRegister->getDimensionExpression(), parent);
    }
    if (targetRegister->getIsPresentExpression().contains(targetID_))
    {
        createItem("Is present", targetRegister->getIsPresentExpression(), parent);
    }
    if (targetRegister->getSizeExpression().contains(targetID_))
    {
        createItem("Size", targetRegister->getSizeExpression(), parent);
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
    if (targetAddressBlock->getWidthExpression().contains(targetID_))
    {
        createItem("Width", targetAddressBlock->getWidthExpression(), parent);
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createItemsForField()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createItemsForField(QSharedPointer<Field> targetField, QTreeWidgetItem* parent)
{
    if (targetField->getBitOffsetExpression().contains(targetID_))
    {
        createItem("Offset", targetField->getBitOffsetExpression(), parent);
    }
    if (targetField->getBitWidthExpression().contains(targetID_))
    {
        createItem("Width", targetField->getBitWidthExpression(), parent);
    }
    if (targetField->getIsPresentExpression().contains(targetID_))
    {
        createItem("Is Present", targetField->getIsPresentExpression(), parent);
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
