/* 
 *  	Created on: 23.8.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditoraddrblockitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditoraddrblockitem.h"
#include <editors/ComponentEditor/memoryMaps/addressblockeditor.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapsvisualizer.h>
#include <IPXACTmodels/register.h>
#include "componenteditorregisteritem.h"
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/addressblockgraphitem.h>
#include <editors/ComponentEditor/visualization/memoryvisualizationitem.h>

#include <QApplication>

#include <QDebug>

ComponentEditorAddrBlockItem::ComponentEditorAddrBlockItem(QSharedPointer<AddressBlock> addrBlock,
														   ComponentEditorTreeModel* model,
														   LibraryInterface* libHandler,
														   QSharedPointer<Component> component,
														   ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
addrBlock_(addrBlock),
regItems_(addrBlock->getRegisterData()),
visualizer_(NULL),
graphItem_(NULL) {

	setObjectName(tr("ComponentEditorAddrBlockItem"));

	foreach (QSharedPointer<RegisterModel> regModel, regItems_) {
		QSharedPointer<Register> reg = regModel.dynamicCast<Register>();
		
		// if the item was a register 
		if (reg) {
			QSharedPointer<ComponentEditorRegisterItem> regItem(new ComponentEditorRegisterItem(
				reg, model, libHandler, component, this));
			childItems_.append(regItem);
		}
	}

	Q_ASSERT(addrBlock_);
}

ComponentEditorAddrBlockItem::~ComponentEditorAddrBlockItem() {
}

QString ComponentEditorAddrBlockItem::getTooltip() const {
	return tr("Contains details of a single address block within a memory map.");
}

QString ComponentEditorAddrBlockItem::text() const {
	return addrBlock_->getName();
}

bool ComponentEditorAddrBlockItem::isValid() const {
	return addrBlock_->isValid();
}

ItemEditor* ComponentEditorAddrBlockItem::editor() {
	if (!editor_) {
		editor_ = new AddressBlockEditor(addrBlock_, component_, libHandler_);
		editor_->setDisabled(locked_);
		connect(editor_, SIGNAL(contentChanged()), 
			this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childAdded(int)),
			this, SLOT(onAddChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childRemoved(int)),
			this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(errorMessage(const QString&)),
            this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
			this, SIGNAL(helpUrlRequested(QString const&)));
	}
	return editor_;
}

QFont ComponentEditorAddrBlockItem::getFont() const {
	return QApplication::font();
}

void ComponentEditorAddrBlockItem::createChild( int index ) {
	QSharedPointer<RegisterModel> regmodel = regItems_[index];
	QSharedPointer<Register> reg = regmodel.dynamicCast<Register>();
	if (reg) {
		QSharedPointer<ComponentEditorRegisterItem> regItem(
			new ComponentEditorRegisterItem(reg, model_, libHandler_, component_, this));
		regItem->setLocked(locked_);
		
		if (visualizer_) {
			regItem->setVisualizer(visualizer_);
		}

		childItems_.insert(index, regItem);
	}
}

void ComponentEditorAddrBlockItem::onEditorChanged() {
	
	// on address block also the grand parent must be updated
	if (parent() && parent()->parent()) {
		emit contentChanged(parent()->parent());
	}

	// call the base class to update this and parent
	ComponentEditorItem::onEditorChanged();
}

ItemVisualizer* ComponentEditorAddrBlockItem::visualizer() {
	return visualizer_;
}

void ComponentEditorAddrBlockItem::setVisualizer( MemoryMapsVisualizer* visualizer ) {
	visualizer_ = visualizer;

	// get the graphics item for the memory map
	MemoryVisualizationItem* parentItem = static_cast<MemoryVisualizationItem*>(parent()->getGraphicsItem());
	Q_ASSERT(parentItem);

	// create the graph item for the address block
	graphItem_ = new AddressBlockGraphItem(addrBlock_, parentItem);

	// register the addr block graph item for the parent
	parentItem->addChild(graphItem_);

	// tell child to refresh itself
	graphItem_->refresh();

	// update the visualizers for register items
	foreach (QSharedPointer<ComponentEditorItem> item, childItems_) {
		QSharedPointer<ComponentEditorRegisterItem> regItem = item.staticCast<ComponentEditorRegisterItem>();
		regItem->setVisualizer(visualizer_);
	}

	connect(graphItem_, SIGNAL(selectEditor()),
		this, SLOT(onSelectRequest()), Qt::UniqueConnection);
}

QGraphicsItem* ComponentEditorAddrBlockItem::getGraphicsItem() {
	return graphItem_;
}

void ComponentEditorAddrBlockItem::updateGraphics() {
	if (graphItem_) {
		graphItem_->refresh();
	}
}

void ComponentEditorAddrBlockItem::removeGraphicsItem() {

	if (graphItem_) {
		// get the graphics item for the memory map
		MemoryVisualizationItem* parentItem = static_cast<MemoryVisualizationItem*>(parent()->getGraphicsItem());
		Q_ASSERT(parentItem);

		// unregister addr block graph item from the memory map graph item
		parentItem->removeChild(graphItem_);

		// take the child from the parent
		graphItem_->setParent(NULL);

		disconnect(graphItem_, SIGNAL(selectEditor()),
			this, SLOT(onSelectRequest()));

		// delete the graph item
		delete graphItem_;
		graphItem_ = NULL;

		// tell the parent to refresh itself
		parentItem->refresh();
	}
}
