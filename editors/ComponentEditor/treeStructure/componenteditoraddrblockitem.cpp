/* 
 *  	Created on: 23.8.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditoraddrblockitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditoraddrblockitem.h"
#include "componenteditorregisteritem.h"

#include <editors/ComponentEditor/memoryMaps/SingleAddressBlockEditor.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapsvisualizer.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/addressblockgraphitem.h>
#include <editors/ComponentEditor/visualization/memoryvisualizationitem.h>

#include <IPXACTmodels/register.h>

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::ComponentEditorAddrBlockItem()
//-----------------------------------------------------------------------------
ComponentEditorAddrBlockItem::ComponentEditorAddrBlockItem(QSharedPointer<AddressBlock> addrBlock,
														   ComponentEditorTreeModel* model,
														   LibraryInterface* libHandler,
														   QSharedPointer<Component> component,
                                                           QSharedPointer<ReferenceCounter> referenceCounter,
                                                           QSharedPointer<ParameterFinder> parameterFinder,
                                                           QSharedPointer<ExpressionFormatter> expressionFormatter,
                                                           QSharedPointer<ExpressionParser> expressionParser,
														   ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
addrBlock_(addrBlock),
visualizer_(NULL),
graphItem_(NULL),
expressionParser_(expressionParser),
addressUnitBits_(0)
{
    setReferenceCounter(referenceCounter);
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);

	setObjectName(tr("ComponentEditorAddrBlockItem"));

	foreach (QSharedPointer<RegisterModel> regModel, addrBlock->getRegisterData())
    {
		QSharedPointer<Register> reg = regModel.dynamicCast<Register>();
		
		// if the item was a register 
		if (reg)
        {
			QSharedPointer<ComponentEditorRegisterItem> regItem(new ComponentEditorRegisterItem(reg, model,
                libHandler, component, parameterFinder_, expressionFormatter_, referenceCounter_,
                expressionParser_, this));
			childItems_.append(regItem);
		}
	}

	Q_ASSERT(addrBlock_);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::~ComponentEditorAddrBlockItem()
//-----------------------------------------------------------------------------
ComponentEditorAddrBlockItem::~ComponentEditorAddrBlockItem()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorAddrBlockItem::getTooltip() const
{
	return tr("Contains details of a single address block within a memory map.");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorAddrBlockItem::text() const
{
	return addrBlock_->getName();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorAddrBlockItem::isValid() const 
{
	return addrBlock_->isValid(component_->getChoices());
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorAddrBlockItem::editor()
{
	if (!editor_)
    {
        editor_ = new SingleAddressBlockEditor(addrBlock_, component_, libHandler_, parameterFinder_,
            expressionFormatter_, expressionParser_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(graphicsChanged()), this, SLOT(onGraphicsChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childAdded(int)), this, SLOT(onAddChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childRemoved(int)), this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(errorMessage(const QString&)),
            this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));
        
        connect(this, SIGNAL(changeInAddressUnitBits(int)),
            editor_, SIGNAL(addressUnitBitsChanged(int)), Qt::UniqueConnection);

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
	QSharedPointer<RegisterModel> regmodel = addrBlock_->getRegisterData()[index];
	QSharedPointer<Register> reg = regmodel.dynamicCast<Register>();
	if (reg)
    {
		QSharedPointer<ComponentEditorRegisterItem> regItem(new ComponentEditorRegisterItem(reg, model_,
            libHandler_, component_, parameterFinder_, expressionFormatter_, referenceCounter_, 
            expressionParser_, this));
		regItem->setLocked(locked_);
		
		if (visualizer_)
        {
			regItem->setVisualizer(visualizer_);
		}

        updateGraphics();
        parent()->updateGraphics();

		childItems_.insert(index, regItem);
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::onEditorChanged()
//-----------------------------------------------------------------------------
void ComponentEditorAddrBlockItem::onEditorChanged() 
{
	// on address block also the grand parent must be updated
	if (parent() && parent()->parent())
    {
		emit contentChanged(parent()->parent());
	}

	// call the base class to update this and parent
	ComponentEditorItem::onEditorChanged();
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
	MemoryVisualizationItem* parentItem = static_cast<MemoryVisualizationItem*>(parent()->getGraphicsItem());
	Q_ASSERT(parentItem);

	// create the graph item for the address block
	graphItem_ = new AddressBlockGraphItem(addrBlock_, parentItem);
    graphItem_->setAddressableUnitBits(addressUnitBits_);

	// register the addr block graph item for the parent
	parentItem->addChild(graphItem_);
	
	// update the visualizers for register items
	foreach (QSharedPointer<ComponentEditorItem> item, childItems_)
    {
		QSharedPointer<ComponentEditorRegisterItem> regItem = item.staticCast<ComponentEditorRegisterItem>();
		regItem->setVisualizer(visualizer_);
	}

    updateGraphics();

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
		graphItem_->refresh();
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
		MemoryVisualizationItem* parentItem = static_cast<MemoryVisualizationItem*>(parent()->getGraphicsItem());
		Q_ASSERT(parentItem);

		// unregister addr block graph item from the memory map graph item
		parentItem->removeChild(graphItem_);

		// take the child from the parent
		graphItem_->setParent(NULL);

		disconnect(graphItem_, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()));

		// delete the graph item
		delete graphItem_;
		graphItem_ = NULL;

		// tell the parent to refresh itself
		parentItem->refresh();
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
        graphItem_->refresh();
    }

    emit changeInAddressUnitBits(newAddressUnitBits);
}