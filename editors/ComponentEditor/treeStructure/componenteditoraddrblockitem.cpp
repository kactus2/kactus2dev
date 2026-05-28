//-----------------------------------------------------------------------------
// File: componenteditoraddrblockitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 23.08.2012
//
// Description:
// The item for a single address block in component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditoraddrblockitem.h"
#include "componenteditorregisteritem.h"
#include "componenteditorregisterfileitem.h"

#include <editors/ComponentEditor/memoryMaps/SingleAddressBlockEditor.h>
#include <KactusAPI/include/RegisterInterface.h>
#include <KactusAPI/include/AddressBlockInterface.h>
#include <KactusAPI/include/FieldInterface.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapsvisualizer.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/addressblockgraphitem.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/registergraphitem.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/registerfilegraphitem.h>
#include <editors/ComponentEditor/visualization/memoryvisualizationitem.h>

#include <IPXACTmodels/Component/RegisterBase.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/RegisterFile.h>
#include <IPXACTmodels/Component/Field.h>

#include <IPXACTmodels/Component/validators/AddressBlockValidator.h>

#include <QApplication>
//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::ComponentEditorAddrBlockItem()
//-----------------------------------------------------------------------------
ComponentEditorAddrBlockItem::ComponentEditorAddrBlockItem(QSharedPointer<MemoryMapBase> containingMap,
    QSharedPointer<AddressBlock> addrBlock, ComponentEditorTreeModel* model, LibraryInterface* libHandler,
    QSharedPointer<Component> component, QSharedPointer<ReferenceCounter> referenceCounter,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<AddressBlockValidator> addressBlockValidator,
    AddressBlockInterface* blockInterface, ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
containingMap_(containingMap),
addrBlock_(addrBlock),
visualizer_(NULL),
expressionParser_(expressionParser),
addressUnitBits_(0),
addressBlockValidator_(addressBlockValidator),
blockInterface_(blockInterface)
{
    setReferenceCounter(referenceCounter);
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);

	setObjectName(tr("ComponentEditorAddrBlockItem"));

    const int registerCount = addrBlock_->getRegisterData()->count();
    for (int i = 0; i < registerCount; ++i)
    {
        createChild(i);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorAddrBlockItem::getTooltip() const
{
	return tr("Contains details of a single address block.");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorAddrBlockItem::text() const
{
	return addrBlock_->name();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorAddrBlockItem::isValid() const 
{
    return addressBlockValidator_->validate(addrBlock_, QString::number(addressUnitBits_));
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorAddrBlockItem::editor()
{
	if (!editor_)
    {
        editor_ = new SingleAddressBlockEditor(blockInterface_, addrBlock_, containingMap_, component_,
            libHandler_, parameterFinder_, expressionFormatter_, expressionParser_, addressBlockValidator_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(graphicsChanged()), this, SLOT(onGraphicsChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childGraphicsChanged(int)), this, SLOT(onChildGraphicsChanged(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(addressingChanged(bool)), this, SLOT(onAddressingChanged(bool)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childAddressingChanged(int)), 
            this, SLOT(onChildAddressingChangedLocally(int)), Qt::UniqueConnection); // Signal from registers/regfile table
		connect(editor_, SIGNAL(childAdded(int)), this, SLOT(onAddChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childRemoved(int)), this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(errorMessage(const QString&)),
            this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));
        
        connect(this, SIGNAL(changeInAddressUnitBits(int)),
            editor_, SIGNAL(addressUnitBitsChanged(int)), Qt::UniqueConnection);

        connect(editor_, SIGNAL(registerNameChanged(QString const&, QString const&)),
            this, SIGNAL(registerNameChanged(QString const&, QString const&)), Qt::UniqueConnection);

        connect(this, SIGNAL(addressBlockNameChanged(QString const&, QString const&)),
            editor_, SLOT(onAddressBlockNameChanged(QString const&, QString const&)), Qt::UniqueConnection);

        connectItemEditorToReferenceCounter();

        emit changeInAddressUnitBits(addressUnitBits_);
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::createChild()
//-----------------------------------------------------------------------------
void ComponentEditorAddrBlockItem::createChild( int index )
{
    QSharedPointer<RegisterBase> regmodel = addrBlock_->getRegisterData()->at(index);
	
	if (auto reg = regmodel.dynamicCast<Register>())
    {
        RegisterInterface* regInterface = blockInterface_->getSubInterface();

        QSharedPointer<ComponentEditorRegisterItem> regItem(new ComponentEditorRegisterItem(reg,
            addrBlock_->getRegisterData(), model_, libHandler_, component_, parameterFinder_, expressionFormatter_,
            referenceCounter_, expressionParser_, addressBlockValidator_->getRegisterValidator(),
            regInterface, this));

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
            FieldInterface* fieldInterface = regInterface->getSubInterface();
            fieldInterface->setFields(reg->getFields());
            fieldInterface->addField(0);

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
            expressionParser_, addressBlockValidator_->getRegisterFileValidator(),
            blockInterface_->getSubInterface(), addrBlock_, addressUnitBits_, this));
        regFileItem->setLocked(locked_);

        if (visualizer_)
        {
            regFileItem->setVisualizer(visualizer_);
            createGraphicsItemsForChild(regFileItem.data());
            onLayoutRefreshRequested();
        }

        connect(regFileItem.data(), SIGNAL(refreshLayout()), this, SLOT(onLayoutRefreshRequested()), Qt::UniqueConnection);
        connect(regFileItem.data(), SIGNAL(addressingChanged()),
            this, SLOT(onChildAddressingChanged()), Qt::UniqueConnection);
        childItems_.insert(index, regFileItem);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::removeChild()
//-----------------------------------------------------------------------------
void ComponentEditorAddrBlockItem::removeChild(int index)
{
    if (!visualizer_)
    {
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
        registerFileEditorItem->removeGraphicsItems();
        childItems_.removeAt(index);
        onLayoutRefreshRequested();
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::onGraphicsChanged()
//-----------------------------------------------------------------------------
void ComponentEditorAddrBlockItem::onGraphicsChanged()
{
    updateGraphics();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::onGraphicsChanged()
//-----------------------------------------------------------------------------
void ComponentEditorAddrBlockItem::onChildGraphicsChanged(int index)
{
    childItems_.at(index)->updateGraphics();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::createGraphicsItemsForChild()
//-----------------------------------------------------------------------------
void ComponentEditorAddrBlockItem::createGraphicsItemsForChild(ComponentEditorItem* childEditor)
{
    if (auto regItem = dynamic_cast<ComponentEditorRegisterItem*>(childEditor))
    {
        for (auto const& graphItem : graphItems_)
        {
            auto addrBlockGraphItem = static_cast<AddressBlockGraphItem*>(graphItem);
            
            regItem->createGraphicsItems(addrBlockGraphItem);

            for (auto const& regGraphItem : regItem->getGraphicsItems().values(addrBlockGraphItem))
            {
                auto asRegGraphItem = static_cast<RegisterGraphItem*>(regGraphItem);
                addrBlockGraphItem->addChild(asRegGraphItem);
                regGraphItem->setParentItem(addrBlockGraphItem);
            }

            addrBlockGraphItem->redoChildLayout();
        }

        regItem->createGraphicsItemsForChildren();
    }
    else if (auto regFileItem = dynamic_cast<ComponentEditorRegisterFileItem*>(childEditor))
    {
        for (auto const& graphItem : graphItems_)
        {
            auto addrBlockGraphItem = static_cast<AddressBlockGraphItem*>(graphItem);

            regFileItem->createGraphicsItems(addrBlockGraphItem);

            for (auto const& regFileGraphItem : regFileItem->getGraphicsItems().values(addrBlockGraphItem))
            {
                auto asRegFileGraphItem = static_cast<RegisterGraphItem*>(regFileGraphItem);
                addrBlockGraphItem->addChild(asRegFileGraphItem);
                regFileGraphItem->setParentItem(addrBlockGraphItem);
            }

            addrBlockGraphItem->redoChildLayout();
        }

        regFileItem->createGraphicsItemsForChildren();
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::onAddressingChanged()
//-----------------------------------------------------------------------------
void ComponentEditorAddrBlockItem::onAddressingChanged(bool needRedraw)
{
    for (auto const& graphItem : graphItems_)
    {
        static_cast<AddressBlockGraphItem*>(graphItem)->redoChildLayout();
    }

    emit addressingChanged(needRedraw);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::onChildAddressingChangedLocally()
//-----------------------------------------------------------------------------
void ComponentEditorAddrBlockItem::onChildAddressingChangedLocally(int index)
{
    // Index corresponds to register data index in address block register data 
    // (also: register data index == order in component editor tree)

    if (auto childRegister = childItems_.at(index).dynamicCast<ComponentEditorRegisterItem>())
    {
        // Need to delete register graph items as well as field graph items
        childRegister->removeGraphicsItems();
        
        // Create new graph items with updated addressing
        createGraphicsItemsForChild(childRegister.data());
    }
    else if (auto childRegisterFile = childItems_.at(index).dynamicCast<ComponentEditorRegisterFileItem>())
    {
        // Need to delete register graph items as well as field graph items
        childRegisterFile->removeGraphicsItems();

        // Create new graph items with updated addressing
        createGraphicsItemsForChild(childRegisterFile.data());
    }

    // Redo layout of child registers, inform parent item of addressing changes
    onLayoutRefreshRequested();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::onChildAddressingChanged()
//-----------------------------------------------------------------------------
void ComponentEditorAddrBlockItem::onChildAddressingChanged()
{
    if (auto registerEditor = dynamic_cast<ComponentEditorRegisterItem*>(sender()))
    {
        registerEditor->removeGraphicsItems();

        // Create new graph items with updated addressing
        createGraphicsItemsForChild(registerEditor);
    }
    else if (auto registerFileEditor = dynamic_cast<ComponentEditorRegisterFileItem*>(sender()))
    {
        // Need to delete register graph items as well as field graph items
        registerFileEditor->removeGraphicsItems();

        // Create new graph items with updated addressing
        createGraphicsItemsForChild(registerFileEditor);
    }

    // Redo layout of child registers, inform parent item of addressing changes
    onLayoutRefreshRequested();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::onLayoutRefreshRequested()
//-----------------------------------------------------------------------------
void ComponentEditorAddrBlockItem::onLayoutRefreshRequested()
{
    for (auto const& graphItem : graphItems_)
    {
        static_cast<AddressBlockGraphItem*>(graphItem)->redoChildLayout();
    }

    // Ask only for parent to redo layout, otherwise duplicate child items may be created
    emit refreshLayout();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::visualizer()
//-----------------------------------------------------------------------------
ItemVisualizer* ComponentEditorAddrBlockItem::visualizer()
{
	return visualizer_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::setVisualizer()
//-----------------------------------------------------------------------------
void ComponentEditorAddrBlockItem::setVisualizer( MemoryMapsVisualizer* visualizer )
{
	visualizer_ = visualizer;

	// update the visualizers for register items
	for (auto child : childItems_)
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
// Function: ComponentEditorAddrBlockItem::createGraphicsItems()
//-----------------------------------------------------------------------------
void ComponentEditorAddrBlockItem::createGraphicsItems(QGraphicsItem* parentItem)
{
    auto memoryParent = static_cast<MemoryVisualizationItem*>(parentItem);
    Q_ASSERT(memoryParent);

    auto addrBlockBaseAddress = expressionParser_->parseExpression(addrBlock_->getBaseAddress()).toULongLong();

    bool hasDim = addrBlock_->getDimension().isEmpty() == false;
    auto blockDim = expressionParser_->parseExpression(addrBlock_->getDimension()).toULongLong();

    bool hasStride = addrBlock_->getStride().isEmpty() == false;
    auto blockStride = expressionParser_->parseExpression(addrBlock_->getStride()).toULongLong();

    auto blockRange = expressionParser_->parseExpression(addrBlock_->getRange()).toULongLong();
    auto minimumStride = blockRange;

    if (!hasDim || blockDim == 0)
    {
        blockDim = 1;
    }

    // Create address block replicas
    for (quint64 i = 0; i < blockDim; ++i)
    {
        quint64 realOffset = addrBlockBaseAddress;

        // Separate replicas by stride
        if (hasStride)
        {
            realOffset += i * blockStride;
        }
        else
        {
            // Separate by minimum stride
            realOffset += i * minimumStride;
        }

        auto newItem = new AddressBlockGraphItem(addrBlock_, expressionParser_, parentItem);
        newItem->setOffset(realOffset);
        newItem->setAddressableUnitBits(addressUnitBits_);

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
// Function: ComponentEditorAddrBlockItem::createGraphicsItemsForChildren()
//-----------------------------------------------------------------------------
void ComponentEditorAddrBlockItem::createGraphicsItemsForChildren()
{
    for (auto const& child : childItems_)
    {
        createGraphicsItemsForChild(child.data());
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::updateGraphics()
//-----------------------------------------------------------------------------
void ComponentEditorAddrBlockItem::updateGraphics()
{
	for (auto const& graphItem : graphItems_)
    {
        static_cast<AddressBlockGraphItem*>(graphItem)->updateDisplay();
	}

    for (auto const& child : childItems_)
    {
        child->updateGraphics();
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::removeGraphicsItems()
//-----------------------------------------------------------------------------
void ComponentEditorAddrBlockItem::removeGraphicsItems()
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
        auto abGraphItem = static_cast<AddressBlockGraphItem*>(graphItem);

        Q_ASSERT(abGraphItem->parentItem());
        auto parentItem = static_cast<MemoryVisualizationItem*>(abGraphItem->parentItem());
        parentItem->removeChild(abGraphItem);

        abGraphItem->setParent(nullptr);
        disconnect(abGraphItem, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()));

        // delete the graph item
        delete abGraphItem;
        abGraphItem = nullptr;
    }

    graphItems_.clear();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::addressUnitBitsChanged()
//-----------------------------------------------------------------------------
void ComponentEditorAddrBlockItem::addressUnitBitsChanged(int newAddressUnitBits)
{
    addressUnitBits_ = newAddressUnitBits;

    for (auto const& child : childItems_)
    {
        if (auto regFileItem = child.dynamicCast<ComponentEditorRegisterFileItem>())
        {
            regFileItem->addressUnitBitsChanged(newAddressUnitBits);
        }
    }

    for (auto const& graphItem : graphItems_)
    {
        auto abGraphItem = static_cast<AddressBlockGraphItem*>(graphItem);
        abGraphItem->setAddressableUnitBits(newAddressUnitBits);
        abGraphItem->updateDisplay();
        abGraphItem->redoChildLayout();

        // Update child registers, which depend on new AUB value
        abGraphItem->updateChildrenDisplay();
    }

    emit changeInAddressUnitBits(newAddressUnitBits);
}
