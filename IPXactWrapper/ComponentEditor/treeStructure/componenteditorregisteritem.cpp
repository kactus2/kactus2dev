/* 
 *  	Created on: 24.8.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorregisteritem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorregisteritem.h"
#include <IPXactWrapper/ComponentEditor/memoryMaps/registereditor.h>
#include <IPXactWrapper/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapsvisualizer.h>
#include "componenteditorfielditem.h"
#include <IPXactWrapper/ComponentEditor/visualization/memoryvisualizationitem.h>
#include <IPXactWrapper/ComponentEditor/memoryMaps/memoryMapsVisualizer/registergraphitem.h>

#include <QFont>
#include <QApplication>

ComponentEditorRegisterItem::ComponentEditorRegisterItem(QSharedPointer<Register> reg,
														 ComponentEditorTreeModel* model,
														 LibraryInterface* libHandler, 
														 QSharedPointer<Component> component,
														 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
reg_(reg),
fields_(reg->getFields()),
editor_(new RegisterEditor(reg, component)),
visualizer_(NULL),
graphItem_(NULL) {

	setObjectName(tr("ComponentEditorRegisterItem"));

	foreach(QSharedPointer<Field> field, fields_) {
		if (field) {
			QSharedPointer<ComponentEditorFieldItem> fieldItem(new ComponentEditorFieldItem(
				reg, field, model, libHandler, component, this));
			childItems_.append(fieldItem);
		}
	}

	editor_->hide();

	connect(editor_, SIGNAL(contentChanged()), 
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
	connect(editor_, SIGNAL(childAdded(int)),
		this, SLOT(onAddChild(int)), Qt::UniqueConnection);
	connect(editor_, SIGNAL(childRemoved(int)),
		this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
	connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
		this, SIGNAL(helpUrlRequested(QString const&)));

	Q_ASSERT(reg_);
}

ComponentEditorRegisterItem::~ComponentEditorRegisterItem() {
	if (editor_) {
		delete editor_;
		editor_ = NULL;
	}
}

QString ComponentEditorRegisterItem::getTooltip() const {
	return tr("Contains details of a single register within an address block.");
}

QString ComponentEditorRegisterItem::text() const {
	return reg_->getName();
}

bool ComponentEditorRegisterItem::isValid() const {
	return reg_->isValid();
}

ItemEditor* ComponentEditorRegisterItem::editor() {
	return editor_;
}

const ItemEditor* ComponentEditorRegisterItem::editor() const {
	return editor_;
}

QFont ComponentEditorRegisterItem::getFont() const {
	return QApplication::font();
}

void ComponentEditorRegisterItem::createChild( int index ) {
	QSharedPointer<ComponentEditorFieldItem> fieldItem(new ComponentEditorFieldItem(
		reg_, fields_.at(index), model_, libHandler_, component_, this));
	fieldItem->setVisualizer(visualizer_);
	childItems_.insert(index, fieldItem);
}

void ComponentEditorRegisterItem::onEditorChanged() {

	// on register also the grand parent must be updated
	if (parent() && parent()->parent()) {
		emit contentChanged(parent()->parent());

		// on register also the grand grand parent must be updated
		if (parent()->parent()->parent()) {
			emit contentChanged(parent()->parent()->parent());
		}
	}

	// call the base class to update this and parent
	ComponentEditorItem::onEditorChanged();
}

ItemVisualizer* ComponentEditorRegisterItem::visualizer() {
	return visualizer_;
}

void ComponentEditorRegisterItem::setVisualizer( MemoryMapsVisualizer* visualizer ) {
	visualizer_ = visualizer;

	// get the graphics item for the memory map
	MemoryVisualizationItem* parentItem = static_cast<MemoryVisualizationItem*>(parent()->getGraphicsItem());
	Q_ASSERT(parentItem);

	// create the graph item for the address block
	graphItem_ = new RegisterGraphItem(reg_, parentItem);

	// register the addr block graph item for the parent
	parentItem->addChild(graphItem_);

	// tell child to refresh itself
	graphItem_->refresh();

	// update the visualizers for field items
	foreach (QSharedPointer<ComponentEditorItem> item, childItems_) {
		QSharedPointer<ComponentEditorFieldItem> fieldItem = item.staticCast<ComponentEditorFieldItem>();
		fieldItem->setVisualizer(visualizer_);
	}
}

QGraphicsItem* ComponentEditorRegisterItem::getGraphicsItem() {
	return graphItem_;
}

void ComponentEditorRegisterItem::updateGraphics() {
	graphItem_->refresh();
}

void ComponentEditorRegisterItem::removeGraphicsItem() {
	Q_ASSERT(graphItem_);

	// get the graphics item for the memory map
	MemoryVisualizationItem* parentItem = static_cast<MemoryVisualizationItem*>(parent()->getGraphicsItem());
	Q_ASSERT(parentItem);

	// unregister addr block graph item from the memory map graph item
	parentItem->removeChild(graphItem_);

	// take the child from the parent
	graphItem_->setParent(NULL);

	// delete the graph item
	delete graphItem_;
	graphItem_ = NULL;

	// tell the parent to refresh itself
	parentItem->refresh();
}
