/* 
 *  	Created on: 10.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditoraddrspaceitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditoraddrspaceitem.h"
#include <IPXACTmodels/addressblock.h>
#include "componenteditoraddrblockitem.h"
#include <editors/ComponentEditor/addressSpaces/localMemoryMap/localmemorymapgraphitem.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapsvisualizer.h>
#include <editors/ComponentEditor/addressSpaces/addressSpaceVisualizer/addressspacevisualizer.h>

ComponentEditorAddrSpaceItem::ComponentEditorAddrSpaceItem(QSharedPointer<AddressSpace> addrSpace,
														   ComponentEditorTreeModel* model,
														   LibraryInterface* libHandler,
														   QSharedPointer<Component> component,
                                                           QSharedPointer<ReferenceCounter> referenceCounter,
                                                           QSharedPointer<ParameterFinder> parameterFinder,
                                                           QSharedPointer<ExpressionFormatter> expressionFormatter,
                                                           QSharedPointer<ExpressionParser> expressionParser,
														   ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
addrSpace_(addrSpace),
localMemMap_(addrSpace->getLocalMemoryMap()),
items_(addrSpace->getLocalMemoryMap()->getItems()),
graphItem_(NULL),
localMemMapVisualizer_(new MemoryMapsVisualizer()),
addrSpaceVisualizer_(new AddressSpaceVisualizer(addrSpace)),
expressionParser_(expressionParser)
{
    setReferenceCounter(referenceCounter);
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);

	setObjectName(tr("ComponentEditorAddrSpaceItem"));

	graphItem_ = new LocalMemoryMapGraphItem(addrSpace_, localMemMap_);
	localMemMapVisualizer_->addMemoryMapItem(graphItem_);
	graphItem_->refresh();

	foreach (QSharedPointer<MemoryMapItem> memItem, items_) {

		// if the item is for address block then create child for it
		QSharedPointer<AddressBlock> addrBlock = memItem.dynamicCast<AddressBlock>();
		if (addrBlock) {
			QSharedPointer<ComponentEditorAddrBlockItem> addrBlockItem(
				new ComponentEditorAddrBlockItem(addrBlock, model, libHandler, component, referenceCounter_,
                parameterFinder_, expressionFormatter_,expressionParser_, this));

			addrBlockItem->setVisualizer(localMemMapVisualizer_);
			childItems_.append(addrBlockItem);
		}
	}
}

ComponentEditorAddrSpaceItem::~ComponentEditorAddrSpaceItem() {
	if (localMemMapVisualizer_) {
		delete localMemMapVisualizer_;
		localMemMapVisualizer_ = NULL;
	}
	if (addrSpaceVisualizer_) {
		delete addrSpaceVisualizer_;
		addrSpaceVisualizer_ = NULL;
	}
}

QString ComponentEditorAddrSpaceItem::text() const {
	return addrSpace_->getName();
}

bool ComponentEditorAddrSpaceItem::isValid() const
{
	return addrSpace_->isValid(component_->getChoices(), component_->getRemapStateNames());
}

ItemEditor* ComponentEditorAddrSpaceItem::editor() {
	if (!editor_) {
		editor_ = new AddressSpaceEditor(component_, libHandler_, addrSpace_, parameterFinder_,
            expressionFormatter_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()),
			this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childAdded(int)),
			this, SLOT(onAddChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childRemoved(int)),
			this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(errorMessage(QString const&)),
            this, SIGNAL(errorMessage(QString const&)));
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
			this, SIGNAL(helpUrlRequested(QString const&)));

        connectItemEditorToReferenceCounter();
	}
	return editor_;
}

QString ComponentEditorAddrSpaceItem::getTooltip() const {
	return tr("Address space defines a logical address space seen by a master"
		" bus interface");
}

void ComponentEditorAddrSpaceItem::createChild( int index ) {
	QSharedPointer<MemoryMapItem> memItem = items_[index];
	QSharedPointer<AddressBlock> addrBlock = memItem.dynamicCast<AddressBlock>();
	if (addrBlock) {
		QSharedPointer<ComponentEditorAddrBlockItem> addrBlockItem(
			new ComponentEditorAddrBlockItem(addrBlock, model_, libHandler_, component_, referenceCounter_,
            parameterFinder_, expressionFormatter_, expressionParser_, this));
		addrBlockItem->setLocked(locked_);

		if (localMemMapVisualizer_) {
			addrBlockItem->setVisualizer(localMemMapVisualizer_);
		}
		childItems_.insert(index, addrBlockItem);
	}
}

QGraphicsItem* ComponentEditorAddrSpaceItem::getGraphicsItem() {
	return graphItem_;
}

void ComponentEditorAddrSpaceItem::updateGraphics() {
	graphItem_->refresh();
}

void ComponentEditorAddrSpaceItem::removeGraphicsItem() {
	Q_ASSERT(graphItem_);

	// remove the graph item from the scene
	localMemMapVisualizer_->removeMemoryMapItem(graphItem_);

	disconnect(graphItem_, SIGNAL(selectEditor()),
		this, SLOT(onSelectRequest()));

	// delete the graph item
	delete graphItem_;
	graphItem_ = NULL;
}

ItemVisualizer* ComponentEditorAddrSpaceItem::visualizer() {
	addrSpaceVisualizer_->refresh();
	return addrSpaceVisualizer_;
}

void ComponentEditorAddrSpaceItem::onEditorChanged() {
	ComponentEditorItem::onEditorChanged();
	addrSpaceVisualizer_->refresh();
}
