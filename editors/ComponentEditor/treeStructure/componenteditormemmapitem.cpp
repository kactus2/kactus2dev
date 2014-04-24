/* 
 *  	Created on: 16.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditormemmapitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditormemmapitem.h"
#include <editors/ComponentEditor/memoryMaps/memorymapeditor.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapsvisualizer.h>
#include "componenteditoraddrblockitem.h"
#include <IPXACTmodels/memorymapitem.h>
#include <IPXACTmodels/addressblock.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapgraphitem.h>

#include <QFont>
#include <QApplication>
#include <QGraphicsScene>

ComponentEditorMemMapItem::ComponentEditorMemMapItem(QSharedPointer<MemoryMap> memoryMap, 
													 ComponentEditorTreeModel* model,
													 LibraryInterface* libHandler,
													 QSharedPointer<Component> component,
													 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
memoryMap_(memoryMap),
items_(memoryMap->getItems()),
visualizer_(NULL),
graphItem_(NULL) {

	setObjectName(tr("ComponentEditorMemMapItem"));

	// create the child objects in the tree
	foreach (QSharedPointer<MemoryMapItem> memItem, items_) {
		
		// if the item is for address block then create child for it
		QSharedPointer<AddressBlock> addrBlock = memItem.dynamicCast<AddressBlock>();
		if (addrBlock) {
			QSharedPointer<ComponentEditorAddrBlockItem> addrBlockItem(
				new ComponentEditorAddrBlockItem(addrBlock, model, libHandler, component, this));
			childItems_.append(addrBlockItem);
		}
	}

	Q_ASSERT(memoryMap_);
}

ComponentEditorMemMapItem::~ComponentEditorMemMapItem() {
}

QString ComponentEditorMemMapItem::text() const {
	return memoryMap_->getName();
}

bool ComponentEditorMemMapItem::isValid() const {
	return memoryMap_->isValid();
}

ItemEditor* ComponentEditorMemMapItem::editor() {
	if (!editor_) {
		editor_ = new MemoryMapEditor(component_, libHandler_, memoryMap_);
		editor_->setDisabled(locked_);
		connect(editor_, SIGNAL(contentChanged()), 
			this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childAdded(int)),
			this, SLOT(onAddChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childRemoved(int)),
			this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
			this, SIGNAL(helpUrlRequested(QString const&)));
	}
	return editor_;
}

QFont ComponentEditorMemMapItem::getFont() const {
	return QApplication::font();
}

QString ComponentEditorMemMapItem::getTooltip() const {
	return tr("Contains the details of a single memory map that can be referenced"
		" by containing component's slave interfaces");
}

void ComponentEditorMemMapItem::createChild( int index ) {

	QSharedPointer<MemoryMapItem> memItem = items_[index];
	QSharedPointer<AddressBlock> addrBlock = memItem.dynamicCast<AddressBlock>();
	if (addrBlock) {
		QSharedPointer<ComponentEditorAddrBlockItem> addrBlockItem(
			new ComponentEditorAddrBlockItem(addrBlock, model_, libHandler_, component_, this));
		addrBlockItem->setLocked(locked_);
		
		if (visualizer_) {
			addrBlockItem->setVisualizer(visualizer_);
		}

		childItems_.insert(index, addrBlockItem);
	}
}

ItemVisualizer* ComponentEditorMemMapItem::visualizer() {
	return visualizer_;
}	

void ComponentEditorMemMapItem::setVisualizer( MemoryMapsVisualizer* visualizer ) {
	visualizer_ = visualizer;

	graphItem_ = new MemoryMapGraphItem(memoryMap_);
	visualizer_->addMemoryMapItem(graphItem_);
	graphItem_->refresh();

	// update the visualizers of address block items
	foreach (QSharedPointer<ComponentEditorItem> item, childItems_) {
		QSharedPointer<ComponentEditorAddrBlockItem> addrItem = item.staticCast<ComponentEditorAddrBlockItem>();
		addrItem->setVisualizer(visualizer_);
	}

	connect(graphItem_, SIGNAL(selectEditor()),
		this, SLOT(onSelectRequest()), Qt::UniqueConnection);
}

QGraphicsItem* ComponentEditorMemMapItem::getGraphicsItem() {
	return graphItem_;
}

void ComponentEditorMemMapItem::updateGraphics() {
	graphItem_->refresh();
}

void ComponentEditorMemMapItem::removeGraphicsItem() {
	Q_ASSERT(graphItem_);

	// remove the graph item from the scene
	visualizer_->removeMemoryMapItem(graphItem_);

	disconnect(graphItem_, SIGNAL(selectEditor()),
		this, SLOT(onSelectRequest()));

	// delete the graph item
	delete graphItem_;
	graphItem_ = NULL;
}
