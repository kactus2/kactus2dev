//-----------------------------------------------------------------------------
// File: componenteditorregisteritem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 24.08.2012
//
// Description:
// The item for single register in component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditorregisteritem.h"
#include "componenteditorfielditem.h"

#include <KactusAPI/include/ExpressionParser.h>

#include <editors/ComponentEditor/memoryMaps/SingleRegisterEditor.h>
#include <KactusAPI/include/FieldInterface.h>
#include <KactusAPI/include/RegisterInterface.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapsvisualizer.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/registergraphitem.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/addressblockgraphitem.h>

#include <editors/ComponentEditor/visualization/memoryvisualizationitem.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/fieldgraphitem.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/Field.h>

#include <IPXACTmodels/Component/validators/RegisterValidator.h>

#include <QApplication>

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::ComponentEditorRegisterItem()
//-----------------------------------------------------------------------------
ComponentEditorRegisterItem::ComponentEditorRegisterItem(QSharedPointer<Register> reg,
    QSharedPointer<QList<QSharedPointer<RegisterBase>>> containingRegisterData, ComponentEditorTreeModel* model,
    LibraryInterface* libHandler, QSharedPointer<Component> component,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ReferenceCounter> referenceCounter, QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<RegisterValidator> registerValidator, RegisterInterface* registerInterface,
    ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
reg_(reg),
expressionParser_(expressionParser),
registerValidator_(registerValidator),
registerInterface_(registerInterface),
containingRegisterData_(containingRegisterData)
{
    setReferenceCounter(referenceCounter);
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);

	setObjectName(tr("ComponentEditorRegisterItem"));

    const int fieldCount = reg_->getFields()->count();

	for (int i = 0; i < fieldCount; ++i)
    {
        createChild(i);
    }

}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::getToolTip()
//-----------------------------------------------------------------------------
QString ComponentEditorRegisterItem::getTooltip() const
{
	return tr("Contains details of a single register.");
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorRegisterItem::text() const
{
	return reg_->name();
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorRegisterItem::isValid() const
{
    return registerValidator_->validate(reg_);
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorRegisterItem::editor()
{
	if (!editor_)
    {
        editor_ = new SingleRegisterEditor(reg_, component_, libHandler_, parameterFinder_, expressionParser_,
            registerInterface_, containingRegisterData_);
        editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(graphicsChanged()), this, SLOT(onGraphicsChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(addressingChanged()), this, SLOT(onAddressingChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childGraphicsChanged(int)), this, SLOT(onChildGraphicsChanged(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childAddressingChanged(int)), this, SLOT(onChildAddressingChangedLocally(int)), Qt::UniqueConnection);
        
		connect(editor_, SIGNAL(childAdded(int)), this, SLOT(onAddChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childRemoved(int)), this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));

        connect(editor_, SIGNAL(fieldNameChanged(QString const&, QString const&)),
            this, SIGNAL(fieldNameChanged(QString const&, QString const&)), Qt::UniqueConnection);

        connect(this, SIGNAL(registerNameChanged(QString const&, QString const&)),
            editor_, SLOT(onRegisterNameChanged(QString const&, QString const&)), Qt::UniqueConnection);

        connectItemEditorToReferenceCounter();
	}

	return editor_;
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::createChild()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::createChild( int index )
{
	QSharedPointer<ComponentEditorFieldItem> fieldItem(new ComponentEditorFieldItem(
		reg_, reg_->getFields()->at(index), model_, libHandler_, component_, parameterFinder_, 
        referenceCounter_, expressionParser_, expressionFormatter_, registerValidator_->getFieldValidator(),
        registerInterface_->getSubInterface(), this));
	fieldItem->setLocked(locked_);
	
	if (visualizer_)
    {
		fieldItem->setVisualizer(visualizer_);
        createGraphicsItemsForChild(fieldItem.data());

        connect(this, SIGNAL(fieldNameChanged(QString const&, QString const&)),
            fieldItem.data(), SIGNAL(fieldNameChanged(QString const&, QString const&)), Qt::UniqueConnection);
        connect(fieldItem.data(), SIGNAL(addressingChanged()), this, SLOT(onChildAddressingChanged()), Qt::UniqueConnection);
	}

	childItems_.insert(index, fieldItem);
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::removeChild()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::removeChild(int index)
{
    // Clear field visualizers for removed field
    auto fieldItem = childItems_.at(index).dynamicCast<ComponentEditorFieldItem>();

    fieldItem->removeGraphicsItems();

    // Then remove field
    childItems_.removeAt(index);
    
    // Recreate the layout of all register graph item replicas
    for (auto const& item : registerItems_)
    {
        auto registerItem = static_cast<RegisterGraphItem*>(item);
        registerItem->redoChildLayout();
    }
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::visualizer()
//-----------------------------------------------------------------------------
ItemVisualizer* ComponentEditorRegisterItem::visualizer()
{
	return visualizer_;
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::setVisualizer()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::setVisualizer(MemoryMapsVisualizer* visualizer)
{
    visualizer_ = visualizer;

    // Set visualizer for each field item and create its graph items
	for (auto const& item : childItems_)
    {
		auto fieldItem = item.staticCast<ComponentEditorFieldItem>();
		fieldItem->setVisualizer(visualizer_);

        createGraphicsItemsForChild(fieldItem.data());

        connect(item.data(), SIGNAL(addressingChanged()), this, SLOT(onChildAddressingChanged()));
	}
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::getGraphicsItem()
//-----------------------------------------------------------------------------
QGraphicsItem* ComponentEditorRegisterItem::getGraphicsItem()
{
    return registerItem_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterItem::getGraphicsItems()
//-----------------------------------------------------------------------------
QList<QGraphicsItem*> const& ComponentEditorRegisterItem::getGraphicsItems() const
{
    return registerItems_;
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::updateGraphics()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::updateGraphics()
{
    onGraphicsChanged();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterItem::onGraphicsChanged()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::onGraphicsChanged()
{
    for (auto const& item : registerItems_)
    {
        auto registerItem = static_cast<RegisterGraphItem*>(item);
        registerItem->updateDisplay();
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterItem::onChildGraphicsChanged()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::onChildGraphicsChanged(int index)
{
    childItems_.at(index)->updateGraphics();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterItem::createGraphicItemsForChild()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::createGraphicsItemsForChild(ComponentEditorItem* childEditor)
{
    Q_ASSERT(childEditor);

    auto child = static_cast<ComponentEditorFieldItem*>(childEditor);

    // add field items as children for every register item replica
    for (auto const& graphItem : registerItems_)
    {
        auto regItem = static_cast<RegisterGraphItem*>(graphItem);
        child->createGraphicsItems(regItem);
        for (auto const& childGraphItem : child->getGraphicsItems().values(regItem))
        {
            regItem->addChild(childGraphItem);
        }

        regItem->redoChildLayout();
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterItem::createGraphicsItems()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::createGraphicsItems(QGraphicsItem* parentItem)
{
    auto memoryParent = static_cast<MemoryVisualizationItem*>(parentItem);
    Q_ASSERT(memoryParent);

    auto addressUnitSize = memoryParent->getAddressUnitSize();
    auto parentOffset = memoryParent->getOffset();

    auto registerOffset = expressionParser_->parseExpression(reg_->getAddressOffset()).toULongLong();

    bool hasDim = reg_->getDimension().isEmpty() == false;
    auto regDim = expressionParser_->parseExpression(reg_->getDimension()).toULongLong();

    bool hasStride = reg_->getStride().isEmpty() == false;
    auto regStride = expressionParser_->parseExpression(reg_->getStride()).toULongLong();

    // Address unit size needed for calculating minimum stride
    // Minimum stride is the size of the register in AUB (reg size is given in bits)
    auto regSize = expressionParser_->parseExpression(reg_->getSize()).toULongLong();
    auto minimumStride = regSize % addressUnitSize ? regSize / addressUnitSize + 1 : regSize / addressUnitSize;

    if (!hasDim || regDim == 0)
    {
        regDim = 1;
    }

    // Create register replicas
    for (quint64 i = 0; i < regDim; ++i)
    {
        quint64 realOffset = registerOffset;

        // Separate replicas by stride
        if (hasStride)
        {
            realOffset += i * regStride;
        }
        else
        {
            // Separate by minimum stride
            realOffset += i * minimumStride;
        }

        // Total offset is register offset + parent offset (address block/register file)
        realOffset += parentOffset;

        auto newItem = new RegisterGraphItem(reg_, expressionParser_, parentItem);
        newItem->setOffset(realOffset);

        // Mark fields with index per replica for identification
        if (hasDim)
        {
            newItem->setReplicaIndex(i);
        }

        registerItems_.append(newItem);
        newItem->updateDisplay();
        connect(newItem, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()), Qt::UniqueConnection);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterItem::createGraphicItemsForChildren()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::createGraphicsItemsForChildren()
{
    for (auto const& childEditor : childItems_)
    {
        createGraphicsItemsForChild(childEditor.data());
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterItem::removeChildGraphItems()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::removeChildGraphicsItems()
{
    for (auto const& child : childItems_)
    {
        child.staticCast<ComponentEditorFieldItem>()->removeGraphicsItems();
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterItem::onAddressingChanged()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::onAddressingChanged()
{
    for (auto const& item : registerItems_)
    {
        auto registerItem = static_cast<RegisterGraphItem*>(item);
        registerItem->redoChildLayout();
    }
 
    emit addressingChanged();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterItem::onChildAddressingChanged()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::onChildAddressingChanged()
{
    if (auto editorFieldItem = dynamic_cast<ComponentEditorFieldItem*>(sender()))
    {
        // Remove and recreate child graphic items
        editorFieldItem->removeGraphicsItems();
        createGraphicsItemsForChild(editorFieldItem);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterItem::onChildAddressingChangedLocally()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::onChildAddressingChangedLocally(int childIndex)
{
    // Signal from fields table editor (RegisterEditor) is handled here.

    Q_ASSERT(childIndex < childItems_.size());
    auto targetItem = childItems_.at(childIndex).dynamicCast<ComponentEditorFieldItem>();

    if (!targetItem)
    {
        return;
    }

    // Remove and recreate child graphic items of changed field
    targetItem->removeGraphicsItems();
    createGraphicsItemsForChild(targetItem.data());
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::removeGraphicsItem()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::removeGraphicsItem()
{
    if (registerItem_)
    {
        // get the graphics item for the address block.
        auto parentItem = static_cast<MemoryVisualizationItem*>(parent()->getGraphicsItem());
        Q_ASSERT(parentItem);

        registerItem_->setParent(nullptr);

        disconnect(registerItem_, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()));

        delete registerItem_;
        registerItem_ = nullptr;
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterItem::removeGraphicsItems()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::removeGraphicsItems()
{
    // First remove child graph items
    for (auto const& child : childItems_)
    {
        child.staticCast<ComponentEditorFieldItem>()->removeGraphicsItems();
    }

    // Remove this item's graph items
    for (auto const& item : registerItems_)
    {
        auto registerItem = static_cast<RegisterGraphItem*>(item);

        Q_ASSERT(registerItem->parentItem());
        auto parentItem = static_cast<AddressBlockGraphItem*>(registerItem->parentItem());
        parentItem->removeChild(registerItem);

        qDebug() << "REG CHILD ITEMS:" << registerItem->childItems().size();

        registerItem->setParent(nullptr);
        disconnect(registerItem, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()));

        // delete the graph item
        delete registerItem;
        registerItem = nullptr;
    }

    registerItems_.clear();
}
