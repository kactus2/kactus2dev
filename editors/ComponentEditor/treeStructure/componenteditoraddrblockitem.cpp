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
graphItem_(NULL),
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
        connect(editor_, SIGNAL(addressingChanged()), this, SLOT(onAddressingChanged()), Qt::UniqueConnection);
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
            regItem->createGraphicsItems(graphItem_);
			regItem->setVisualizer(visualizer_);

            for (auto const& regGraphItem : regItem->getGraphicsItems())
            {
                auto asRegGraphItem = static_cast<RegisterGraphItem*>(regGraphItem);
                graphItem_->addChild(asRegGraphItem);
                regGraphItem->setParentItem(graphItem_);
            }

            onAddressingChanged();
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
        }

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
    if (visualizer_)
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
        onAddressingChanged();
    }
    else if (auto registerFileEditorItem = childEditorItem.dynamicCast<ComponentEditorRegisterFileItem>())
    {
        auto childItem = static_cast<MemoryVisualizationItem*>(childItems_.at(index)->getGraphicsItem());
        Q_ASSERT(childItem);

        // TODO update this

        graphItem_->removeChild(childItem);
        onAddressingChanged();
        ComponentEditorItem::removeChild(index);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::onGraphicsChanged()
//-----------------------------------------------------------------------------
void ComponentEditorAddrBlockItem::onGraphicsChanged()
{
    if (graphItem_)
    {
        graphItem_->updateDisplay();
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::onGraphicsChanged()
//-----------------------------------------------------------------------------
void ComponentEditorAddrBlockItem::onChildGraphicsChanged(int index)
{
    childItems_.at(index)->updateGraphics();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::onAddressingChanged()
//-----------------------------------------------------------------------------
void ComponentEditorAddrBlockItem::onAddressingChanged()
{
    if (graphItem_ != nullptr)
    {
        graphItem_->redoChildLayout();

        for (auto child : childItems_)
        {
            child->updateGraphics();
        }

        emit addressingChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::onChildAddressingChangedLocally()
//-----------------------------------------------------------------------------
void ComponentEditorAddrBlockItem::onChildAddressingChangedLocally(int index)
{
    if (auto childRegister = childItems_.at(index).dynamicCast<ComponentEditorRegisterItem>())
    {
        // Need to delete register graph items as well as field graph items
        //childRegister->removeChildGraphItems();
        childRegister->removeGraphicsItems();
        
        // Create new graph items with updated addressing
        childRegister->createGraphicsItems(graphItem_);
        childRegister->createGraphicsItemsForChildren();

        for (auto const& regGraphItem : childRegister->getGraphicsItems())
        {
            auto asRegGraphItem = static_cast<RegisterGraphItem*>(regGraphItem);
            graphItem_->addChild(asRegGraphItem);
            regGraphItem->setParentItem(graphItem_);
        }

        childRegister->updateGraphics();
        childRegister->onAddressingChanged();

        // Redo layout of child registers, inform parent item of addressing changes
        onAddressingChanged();
    }
    else if (auto childRegisterFile = childItems_.at(index).dynamicCast<ComponentEditorRegisterFileItem>())
    {
        // TODO update this
        childRegisterFile->updateGraphics();
        childRegisterFile->onAddressingChanged();
    }
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
        registerEditor->createGraphicsItems(graphItem_);
        registerEditor->createGraphicsItemsForChildren();

        for (auto const& regGraphItem : registerEditor->getGraphicsItems())
        {
            auto asRegGraphItem = static_cast<RegisterGraphItem*>(regGraphItem);
            graphItem_->addChild(asRegGraphItem);
            regGraphItem->setParentItem(graphItem_);
        }

        registerEditor->updateGraphics();

        // Redo layout of child registers, inform parent item of addressing changes
        onAddressingChanged();
    }
    // TODO handle register file addressing changes
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

	// get the graphics item for the memory map
	auto parentItem = static_cast<MemoryVisualizationItem*>(parent()->getGraphicsItem());
	Q_ASSERT(parentItem);

	// create the graph item for the address block
	graphItem_ = new AddressBlockGraphItem(addrBlock_, expressionParser_, parentItem);
    graphItem_->setAddressableUnitBits(addressUnitBits_);

	// update the visualizers for register items
	for (auto child : childItems_)
    {        
        if (auto regItem = child.dynamicCast<ComponentEditorRegisterItem>())
        {
            regItem->createGraphicsItems(graphItem_);
            regItem->setVisualizer(visualizer_); 

            // Add created graph items as children
            for (auto const& regGraphItem : regItem->getGraphicsItems())
            {
                auto asRegGraphItem = static_cast<RegisterGraphItem*>(regGraphItem);
                graphItem_->addChild(asRegGraphItem);
                regGraphItem->setParentItem(graphItem_);
            }
        }
        else if (auto regFileItem = child.dynamicCast<ComponentEditorRegisterFileItem>())
        {
            regFileItem->setVisualizer(visualizer_);
            auto childGraphicItem = static_cast<MemoryVisualizationItem*>(child->getGraphicsItem());
            graphItem_->addChild(childGraphicItem);
        }
   	}

    graphItem_->redoChildLayout();

	connect(graphItem_, SIGNAL(selectEditor()),	this, SLOT(onSelectRequest()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::getGraphicsItem()
//-----------------------------------------------------------------------------
QGraphicsItem* ComponentEditorAddrBlockItem::getGraphicsItem()
{
	return graphItem_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::updateGraphics()
//-----------------------------------------------------------------------------
void ComponentEditorAddrBlockItem::updateGraphics()
{
	if (graphItem_)
    {
		graphItem_->updateDisplay();
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::removeGraphicsItem()
//-----------------------------------------------------------------------------
void ComponentEditorAddrBlockItem::removeGraphicsItem()
{
	if (graphItem_)
    {
		// get the graphics item for the memory map
		auto parentItem = static_cast<MemoryVisualizationItem*>(parent()->getGraphicsItem());
		Q_ASSERT(parentItem);

		// unregister addr block graph item from the memory map graph item
		parentItem->removeChild(graphItem_);

		// take the child from the parent
		graphItem_->setParent(nullptr);

		disconnect(graphItem_, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()));

		// delete the graph item
		delete graphItem_;
		graphItem_ = nullptr;
	}
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

    if (graphItem_)
    {
        graphItem_->setAddressableUnitBits(newAddressUnitBits);
        graphItem_->updateDisplay();
        graphItem_->redoChildLayout();
    }

    emit changeInAddressUnitBits(newAddressUnitBits);
}
