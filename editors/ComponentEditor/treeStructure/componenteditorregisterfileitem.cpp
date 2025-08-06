//-----------------------------------------------------------------------------
// File: componenteditorregisterfileitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Dan Chianucci
// Date: 19.06.2018
//
// Description:
// Item representing a register file in the component editor browser tree.
//-----------------------------------------------------------------------------

#include "componenteditorregisterfileitem.h"

#include "componenteditorregisteritem.h"

#include <KactusAPI/include/ExpressionParser.h>

#include <editors/ComponentEditor/memoryMaps/SingleRegisterFileEditor.h>
#include <KactusAPI/include/RegisterInterface.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapsvisualizer.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/registerfilegraphitem.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/registergraphitem.h>

#include <editors/ComponentEditor/visualization/memoryvisualizationitem.h>

#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/RegisterFile.h>
#include <IPXACTmodels/Component/validators/RegisterFileValidator.h>

#include <QApplication>

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::ComponentEditorRegisterFileItem()
//-----------------------------------------------------------------------------
ComponentEditorRegisterFileItem::ComponentEditorRegisterFileItem(
    QSharedPointer<RegisterFile> regFile,
    ComponentEditorTreeModel* model,
    LibraryInterface* libHandler,
    QSharedPointer<Component> component,
    QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ReferenceCounter> referenceCounter,
    QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<RegisterFileValidator> registerFileValidator,
    RegisterInterface* registerInterface,
    QSharedPointer<AddressBlock> containingBlock,
    unsigned int addressUnitBits,
    ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
registerFile_(regFile),
visualizer_(nullptr),
expressionParser_(expressionParser),
registerFileValidator_(registerFileValidator),
registerInterface_(registerInterface),
containingBlock_(containingBlock),
addressUnitBits_(addressUnitBits)
{
    setReferenceCounter(referenceCounter);
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);
    setObjectName(tr("ComponentEditorRegFileItem"));

    const int registerCount = registerFile_->getRegisterData()->count();
    for (int i = 0; i < registerCount; ++i)
    {
        createChild(i);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorRegisterFileItem::getTooltip() const
{
    return tr("Contains details of a single register file.");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorRegisterFileItem::text() const
{
    return registerFile_->name();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorRegisterFileItem::isValid() const
{
    return registerFileValidator_->validate(registerFile_, QString::number(addressUnitBits_), containingBlock_->getWidth());
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::createChild()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterFileItem::createChild( int index )
{
    QSharedPointer<RegisterBase> regmodel = registerFile_->getRegisterData()->at(index);

    if (auto reg = regmodel.dynamicCast<Register>())
    {
        QSharedPointer<ComponentEditorRegisterItem> regItem(new ComponentEditorRegisterItem(reg,
            registerFile_->getRegisterData(), model_, libHandler_, component_, parameterFinder_,
            expressionFormatter_, referenceCounter_, expressionParser_,
            registerFileValidator_->getRegisterValidator(), registerInterface_, this));

        connect(this, SIGNAL(registerNameChanged(QString const&, QString const&)),
            regItem.data(), SIGNAL(registerNameChanged(QString const&, QString const&)), Qt::UniqueConnection);

        regItem->setLocked(locked_);

        if (visualizer_)
        {
            regItem->setVisualizer(visualizer_);
            createGraphicsItemsForChild(regItem.data());
            onLayoutRefreshRequested();
        }

        if (reg->getFields()->isEmpty())
        {
            QSharedPointer<Field> newField (new Field());
            reg->getFields()->append(newField);

            regItem->createChild(0);
        }
        
        connect(regItem.data(), SIGNAL(addressingChanged()),
            this, SLOT(onChildAddressingChanged()), Qt::UniqueConnection);
        childItems_.insert(index, regItem);
    }
    else if (auto regFile = regmodel.dynamicCast<RegisterFile>())
    {
        QSharedPointer<ComponentEditorRegisterFileItem> regFileItem(new ComponentEditorRegisterFileItem(regFile,
            model_, libHandler_, component_, parameterFinder_, expressionFormatter_, referenceCounter_,
            expressionParser_, registerFileValidator_, registerInterface_, containingBlock_, addressUnitBits_, this));
        regFileItem->setLocked(locked_);
     
        if (visualizer_)
        {
            regFileItem->setVisualizer(visualizer_);
            createGraphicsItemsForChild(regFileItem.data());
            onLayoutRefreshRequested();
        }

        connect(regFileItem.data(), SIGNAL(addressingChanged()),
            this, SLOT(onChildAddressingChanged()), Qt::UniqueConnection);
        connect(regFileItem.data(), SIGNAL(refreshLayout()), this, SLOT(onLayoutRefreshRequested()), Qt::UniqueConnection);
        childItems_.insert(index, regFileItem);
    }
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::removeChild()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterFileItem::removeChild(int index)
{
    if (!visualizer_)
    {
        ComponentEditorItem::removeChild(index);
        return;
    }

    auto childEditorItem = childItems_.at(index);
    Q_ASSERT(childEditorItem);

    if (auto registerEditorItem = childEditorItem.dynamicCast<ComponentEditorRegisterItem>())
    {
        // Remove the graph items of the child
        registerEditorItem->removeGraphicsItems();

        // Remove the child itself
        childItems_.removeAt(index);
        onLayoutRefreshRequested();
    }
    else if (auto registerFileEditorItem = childEditorItem.dynamicCast<ComponentEditorRegisterFileItem>())
    {
        // Remove the graph items of the child
        registerFileEditorItem->removeGraphicsItems();

        // Remove the child itself
        childItems_.removeAt(index);
        onLayoutRefreshRequested();
    }
}

//-----------------------------------------------------------------------------

// Function: ComponentEditorRegisterFileItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorRegisterFileItem::editor()
{
    if (!editor_)
    {
        editor_ = new SingleRegisterFileEditor(registerInterface_, registerFile_, component_, libHandler_,
            parameterFinder_, expressionFormatter_, expressionParser_, registerFileValidator_);
        editor_->setProtection(locked_);
        connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(graphicsChanged()), this, SLOT(onGraphicsChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childGraphicsChanged(int)), this, SLOT(onChildGraphicsChanged(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(addressingChanged()), this, SLOT(onAddressingChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childAddressingChanged(int)),
            this, SLOT(onChildAddressingChangedLocally(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childAdded(int)), this, SLOT(onAddChild(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childRemoved(int)), this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));

        connect(editor_, SIGNAL(registerNameChanged(QString const&, QString const&)),
            this, SIGNAL(registerNameChanged(QString const&, QString const&)), Qt::UniqueConnection);

        connectItemEditorToReferenceCounter();
    }

    return editor_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::visualizer()
//-----------------------------------------------------------------------------
ItemVisualizer* ComponentEditorRegisterFileItem::visualizer()
{
    return visualizer_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::setVisualizer()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterFileItem::setVisualizer( MemoryMapsVisualizer* visualizer )
{
    visualizer_ = visualizer;

    // update the visualizers for child items
    for (QSharedPointer<ComponentEditorItem> child : childItems_)
    {
        if (auto regItem = child.dynamicCast<ComponentEditorRegisterItem>())
        {
            regItem->setVisualizer(visualizer_);
        }
        else if (auto regFileItem = child.dynamicCast<ComponentEditorRegisterFileItem>())
        {
            regFileItem->setVisualizer(visualizer_);
        }

        createGraphicsItemsForChild(child.data());
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::addressUnitBitsChanged()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterFileItem::addressUnitBitsChanged(int newAddressUnitBits)
{
    addressUnitBits_ = newAddressUnitBits;

    for (auto const& child : childItems_)
    {
        if (auto regFileItem = child.dynamicCast<ComponentEditorRegisterFileItem>())
        {
            regFileItem->addressUnitBitsChanged(newAddressUnitBits);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::updateGraphics()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterFileItem::updateGraphics()
{
    onGraphicsChanged();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::onGraphicsChanged()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterFileItem::onGraphicsChanged()
{
    for (auto const& graphItem : graphItems_)
    {
        static_cast<RegisterFileGraphItem*>(graphItem)->updateDisplay();
    }

    for (auto const& child : childItems_)
    {
        child->updateGraphics();
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::onChildGraphicsChanged()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterFileItem::onChildGraphicsChanged(int index)
{
    childItems_.at(index)->updateGraphics();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::createGraphicsItems()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterFileItem::createGraphicsItems(QGraphicsItem* parentItem)
{
    auto memoryParent = static_cast<MemoryVisualizationItem*>(parentItem);
    Q_ASSERT(memoryParent);

    auto parentOffset = memoryParent->getOffset();

    auto registerFileOffset = expressionParser_->parseExpression(registerFile_->getAddressOffset()).toULongLong();

    bool hasDim = registerFile_->getDimension().isEmpty() == false;
    auto regDim = expressionParser_->parseExpression(registerFile_->getDimension()).toULongLong();

    bool hasStride = registerFile_->getStride().isEmpty() == false;
    auto registerFileStride = expressionParser_->parseExpression(registerFile_->getStride()).toULongLong();

    auto registerFileRange = expressionParser_->parseExpression(registerFile_->getRange()).toULongLong();
    auto minimumStride = registerFileRange;

    if (!hasDim || regDim == 0)
    {
        regDim = 1;
    }

    // Create register replicas
    for (quint64 i = 0; i < regDim; ++i)
    {
        quint64 realOffset = registerFileOffset;

        // Separate replicas by stride
        if (hasStride)
        {
            realOffset += i * registerFileStride;
        }
        else
        {
            // Separate by minimum stride
            realOffset += i * minimumStride;
        }

        // Total offset is register offset + parent offset (address block/register file)
        realOffset += parentOffset;

        auto newItem = new RegisterFileGraphItem(registerFile_, expressionParser_, parentItem);
        newItem->setOffset(realOffset);

        // Mark fields with index per replica for identification
        if (hasDim)
        {
            newItem->setReplicaIndex(i);
        }

        graphItems_.insert(parentItem, newItem);
        newItem->updateDisplay();
        connect(newItem, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()), Qt::UniqueConnection);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::createGraphicsItemsForChildren()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterFileItem::createGraphicsItemsForChildren()
{
    for (auto const& childEditor : childItems_)
    {
        createGraphicsItemsForChild(childEditor.data());
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::createGraphicsItemsForChild()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterFileItem::createGraphicsItemsForChild(ComponentEditorItem* childEditor)
{
    Q_ASSERT(childEditor);

    // childEditor == child item to create graph items for
    if (auto registerEditor = dynamic_cast<ComponentEditorRegisterItem*>(childEditor))
    {
        // For each graph item of current editor item (= parent of created child graph items)
        for (auto const& graphItem : graphItems_)
        {
            auto regFileGraphItem = static_cast<RegisterFileGraphItem*>(graphItem);
            
            // Create graph items for children 
            registerEditor->createGraphicsItems(graphItem);

            // Add each created child graph item to current graph item
            for (auto const& item : registerEditor->getGraphicsItems().values(graphItem))
            {
                auto asRegGraphItem = static_cast<RegisterGraphItem*>(item);
                regFileGraphItem->addChild(asRegGraphItem);
                asRegGraphItem->setParentItem(graphItem);
            }

            registerEditor->updateGraphics();
            regFileGraphItem->redoChildLayout();
        }

        // Create children to child graph item once
        registerEditor->createGraphicsItemsForChildren();
    }
    // TODO consider merging these
    else if (auto registerFileEditor = dynamic_cast<ComponentEditorRegisterFileItem*>(childEditor))
    {
        // For each graph item of current editor item (= parent of created child graph items)
        for (auto const& graphItem : graphItems_)
        {
            auto regFileGraphItem = static_cast<RegisterFileGraphItem*>(graphItem);

            // Create graph items for children 
            registerFileEditor->createGraphicsItems(graphItem);

            // Add each created child graph item to current graph item
            for (auto const& item : registerFileEditor->getGraphicsItems().values(graphItem))
            {
                auto asRegFileGraphItem = static_cast<RegisterFileGraphItem*>(item);
                regFileGraphItem->addChild(asRegFileGraphItem);
                asRegFileGraphItem->setParentItem(graphItem);
            }

            registerFileEditor->updateGraphics();
            regFileGraphItem->redoChildLayout();
        }

        // Create children to child graph item once
        registerFileEditor->createGraphicsItemsForChildren();
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::onAddressingChanged()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterFileItem::onAddressingChanged()
{
    if (graphItems_.isEmpty())
    {
        return;
    }

    for (auto const& graphItem : graphItems_)
    {
        static_cast<RegisterFileGraphItem*>(graphItem)->redoChildLayout();
    }

    emit addressingChanged();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::onChildAddressingChangedLocally()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterFileItem::onChildAddressingChangedLocally(int index)
{
    childItems_.at(index)->updateGraphics();

    if (auto childRegister = childItems_.at(index).dynamicCast<ComponentEditorRegisterItem>())
    {
        // Delete graph items as well as its child graph items
        childRegister->removeGraphicsItems();
        createGraphicsItemsForChild(childRegister.data());
    }
    else if (auto childRegisterFile = childItems_.at(index).dynamicCast<ComponentEditorRegisterFileItem>())
    {
        // Delete graph items as well as its child graph items
        childRegisterFile->removeGraphicsItems();
        createGraphicsItemsForChild(childRegisterFile.data());
    }

    // Inform parent of layout changes
    onLayoutRefreshRequested();
}


//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::onLayoutRefreshRequested()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterFileItem::onLayoutRefreshRequested()
{
    for (auto const& graphItem : graphItems_)
    {
        static_cast<RegisterFileGraphItem*>(graphItem)->redoChildLayout();
    }

    emit refreshLayout();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::onChildAddressingChanged()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterFileItem::onChildAddressingChanged()
{
    if (auto registerEditor = dynamic_cast<ComponentEditorRegisterItem*>(sender()))
    {
        // Need to delete register graph items as well as field graph items
        registerEditor->removeGraphicsItems();
        createGraphicsItemsForChild(registerEditor);
    }
    else if (auto registerFileEditor = dynamic_cast<ComponentEditorRegisterFileItem*>(sender()))
    {
        // Need to delete register graph items as well as field graph items
        registerFileEditor->removeGraphicsItems();
        createGraphicsItemsForChild(registerFileEditor);
    }

    // Redo layout of child registers, inform parent item of addressing changes
    onLayoutRefreshRequested();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::removeGraphicsItems()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterFileItem::removeGraphicsItems()
{
    // First remove child graph items
    for (auto const& child : childItems_)
    {
        if (auto registerFileItem = child.dynamicCast<ComponentEditorRegisterFileItem>())
        {
            registerFileItem->removeGraphicsItems();
        }
        else if (auto registerItem = child.dynamicCast<ComponentEditorRegisterItem>())
        {
            registerItem->removeGraphicsItems();
        }
    }

    // Remove this item's graph items
    for (auto& graphItem : graphItems_)
    {
        auto regFileGraphItem = static_cast<RegisterFileGraphItem*>(graphItem);

        Q_ASSERT(regFileGraphItem->parentItem());
        auto parentItem = static_cast<MemoryVisualizationItem*>(regFileGraphItem->parentItem());
        parentItem->removeChild(regFileGraphItem);

        regFileGraphItem->setParent(nullptr);
        disconnect(regFileGraphItem, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()));

        // delete the graph item
        delete regFileGraphItem;
        regFileGraphItem = nullptr;
    }

    graphItems_.clear();
}
