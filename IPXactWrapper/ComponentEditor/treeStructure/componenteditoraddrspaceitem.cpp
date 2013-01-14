/* 
 *  	Created on: 10.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditoraddrspaceitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditoraddrspaceitem.h"
#include <models/addressblock.h>
#include "componenteditoraddrblockitem.h"
#include <IPXactWrapper/ComponentEditor/addressSpaces/localMemoryMap/localmemorymapgraphitem.h>
#include <IPXactWrapper/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapsvisualizer.h>
#include <IPXactWrapper/ComponentEditor/addressSpaces/addressSpaceVisualizer/addressspacevisualizer.h>

#include <QFont>
#include <QApplication>

ComponentEditorAddrSpaceItem::ComponentEditorAddrSpaceItem(QSharedPointer<AddressSpace> addrSpace,
														   ComponentEditorTreeModel* model,
														   LibraryInterface* libHandler,
														   QSharedPointer<Component> component,
														   ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
addrSpace_(addrSpace),
localMemMap_(addrSpace->getLocalMemoryMap()),
items_(addrSpace->getLocalMemoryMap()->getItems()),
editor_(component, addrSpace),
graphItem_(NULL),
localMemMapVisualizer_(new MemoryMapsVisualizer(component)),
addrSpaceVisualizer_(new AddressSpaceVisualizer(addrSpace, component)) {

	setObjectName(tr("ComponentEditorAddrSpaceItem"));

	editor_.hide();

	graphItem_ = new LocalMemoryMapGraphItem(addrSpace_, localMemMap_);
	localMemMapVisualizer_->addMemoryMapItem(graphItem_);
	graphItem_->refresh();

	foreach (QSharedPointer<MemoryMapItem> memItem, items_) {

		// if the item is for address block then create child for it
		QSharedPointer<AddressBlock> addrBlock = memItem.dynamicCast<AddressBlock>();
		if (addrBlock) {
			QSharedPointer<ComponentEditorAddrBlockItem> addrBlockItem(
				new ComponentEditorAddrBlockItem(addrBlock, model, libHandler, component, this));

			addrBlockItem->setVisualizer(localMemMapVisualizer_);
			childItems_.append(addrBlockItem);
		}
	}

	connect(&editor_, SIGNAL(contentChanged()),
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(childAdded(int)),
		this, SLOT(onAddChild(int)), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(childRemoved(int)),
		this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(helpUrlRequested(QString const&)),
		this, SIGNAL(helpUrlRequested(QString const&)));
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

bool ComponentEditorAddrSpaceItem::isValid() const {
	return addrSpace_->isValid();
}

ItemEditor* ComponentEditorAddrSpaceItem::editor() {
	return &editor_;
}

const ItemEditor* ComponentEditorAddrSpaceItem::editor() const {
	return &editor_;
}

QFont ComponentEditorAddrSpaceItem::getFont() const {
	return QApplication::font();
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
			new ComponentEditorAddrBlockItem(addrBlock, model_, libHandler_, component_, this));

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
