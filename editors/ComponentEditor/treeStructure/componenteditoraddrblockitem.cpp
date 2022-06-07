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
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapsvisualizer.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/addressblockgraphitem.h>
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
            this, SLOT(onChildAddressingChanged(int)), Qt::UniqueConnection);
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
	QSharedPointer<Register> reg = regmodel.dynamicCast<Register>();
	if (reg)
    {
		QSharedPointer<ComponentEditorRegisterItem> regItem(new ComponentEditorRegisterItem(reg,
            addrBlock_->getRegisterData(), model_, libHandler_, component_, parameterFinder_, expressionFormatter_,
            referenceCounter_, expressionParser_, addressBlockValidator_->getRegisterValidator(),
            blockInterface_->getSubInterface(), this));

        connect(this, SIGNAL(registerNameChanged(QString const&, QString const&)),
            regItem.data(), SIGNAL(registerNameChanged(QString const&, QString const&)), Qt::UniqueConnection);

		regItem->setLocked(locked_);
		
		if (visualizer_)
        {
			regItem->setVisualizer(visualizer_);
		}

        if (reg->getFields()->isEmpty())
        {
            QSharedPointer<Field> newField (new Field());
            reg->getFields()->append(newField);

            regItem->createChild(0);
        }

        connect(regItem.data(), SIGNAL(addressingChanged()),
            this, SLOT(onAddressingChanged()), Qt::UniqueConnection);
		childItems_.insert(index, regItem);
	}

    QSharedPointer<RegisterFile> regFile = regmodel.dynamicCast<RegisterFile>();
    if (regFile)
    {
        QSharedPointer<ComponentEditorRegisterFileItem> regFileItem(new ComponentEditorRegisterFileItem(regFile,
            model_, libHandler_, component_, parameterFinder_, expressionFormatter_, referenceCounter_,
            expressionParser_, addressBlockValidator_->getRegisterFileValidator(),
            blockInterface_->getSubInterface(), addrBlock_, this));
        regFileItem->setLocked(locked_);

        if (visualizer_)
        {
            regFileItem->setVisualizer(visualizer_);
        }

        connect(regFileItem.data(), SIGNAL(addressingChanged()),
            this, SLOT(onAddressingChanged()), Qt::UniqueConnection);
        childItems_.insert(index, regFileItem);
    }

    if (visualizer_)
    {
        auto childItem = static_cast<MemoryVisualizationItem*>(childItems_.at(index)->getGraphicsItem());
        Q_ASSERT(childItem);

        graphItem_->addChild(childItem);
        onAddressingChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::removeChild()
//-----------------------------------------------------------------------------
void ComponentEditorAddrBlockItem::removeChild(int index)
{
    if (visualizer_)
    {
        auto childItem = static_cast<MemoryVisualizationItem*>(childItems_.at(index)->getGraphicsItem());
        Q_ASSERT(childItem);

        graphItem_->removeChild(childItem);

        onAddressingChanged();
    }

    ComponentEditorItem::removeChild(index);
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
// Function: ComponentEditorAddrBlockItem::onChildAddressingChanged()
//-----------------------------------------------------------------------------
void ComponentEditorAddrBlockItem::onChildAddressingChanged(int index)
{
    if (graphItem_ != nullptr)
    {
        auto childRegister = childItems_.at(index).dynamicCast<ComponentEditorRegisterItem>();

        if (childRegister)
        {
            childRegister->updateGraphics();
            childRegister->onAddressingChanged();
            return;
        }

        auto childRegisterFile = childItems_.at(index).dynamicCast<ComponentEditorRegisterFileItem>();

        if (childRegisterFile)
        {
            childRegisterFile->updateGraphics();
            childRegisterFile->onAddressingChanged();
        }
    }
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
        auto regItem = child.dynamicCast<ComponentEditorRegisterItem>();
        auto regFileItem = child.dynamicCast<ComponentEditorRegisterFileItem>();

        if (regItem)
        {
          regItem->setVisualizer(visualizer_);
        }
        else if (regFileItem)
        {
          regFileItem->setVisualizer(visualizer_);
        }
   
        auto childGraphicItem = static_cast<MemoryVisualizationItem*>(child->getGraphicsItem());
        graphItem_->addChild(childGraphicItem);
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

    if (graphItem_)
    {
        graphItem_->setAddressableUnitBits(newAddressUnitBits);
        graphItem_->updateDisplay();
        graphItem_->redoChildLayout();
    }

    emit changeInAddressUnitBits(newAddressUnitBits);
}
