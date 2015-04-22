/* 
 *  	Created on: 28.8.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorfielditem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorfielditem.h"
#include <editors/ComponentEditor/memoryMaps/fieldeditor.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapsvisualizer.h>
#include <editors/ComponentEditor/visualization/memoryvisualizationitem.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/fieldgraphitem.h>

ComponentEditorFieldItem::ComponentEditorFieldItem(QSharedPointer<Register> reg,
												   QSharedPointer<Field> field,
												   ComponentEditorTreeModel* model,
												   LibraryInterface* libHandler,
												   QSharedPointer<Component> component,
												   ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
reg_(reg),
field_(field),
visualizer_(NULL),
graphItem_(NULL) {

	Q_ASSERT(field_);

	setObjectName(tr("ComponentEditorFieldItem"));
}

ComponentEditorFieldItem::~ComponentEditorFieldItem() {
}

QString ComponentEditorFieldItem::getTooltip() const {
	return tr("Contains details of a single field within a register.");
}

QString ComponentEditorFieldItem::text() const {
	return field_->getName();
}

bool ComponentEditorFieldItem::isValid() const 
{
	return field_->isValid(reg_->getSize(), component_->getChoices());
}

ItemEditor* ComponentEditorFieldItem::editor() {
	if (!editor_) {
		editor_ = new FieldEditor(field_, component_, libHandler_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), 
			this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
			this, SIGNAL(helpUrlRequested(QString const&)));
	}
	return editor_;
}

void ComponentEditorFieldItem::onEditorChanged() {
	// on field also the grand parent must be updated
	if (parent() && parent()->parent()) {
		emit contentChanged(parent()->parent());

		// on field also the grand grand parent must be updated
		if (parent()->parent()->parent()) {
			emit contentChanged(parent()->parent()->parent());

			// on field also the grand grand grand parent must be updated
			if (parent()->parent()->parent()->parent()) {
				emit contentChanged(parent()->parent()->parent()->parent());
			}
		}
	}

	// call the base class to update this and parent
	ComponentEditorItem::onEditorChanged();
}

ItemVisualizer* ComponentEditorFieldItem::visualizer() {
	return visualizer_;
}

void ComponentEditorFieldItem::setVisualizer( MemoryMapsVisualizer* visualizer ) {
	visualizer_ = visualizer;

	// get the graphics item for the memory map
	MemoryVisualizationItem* parentItem = static_cast<MemoryVisualizationItem*>(parent()->getGraphicsItem());
	Q_ASSERT(parentItem);

	// create the graph item for the address block
	graphItem_ = new FieldGraphItem(field_, parentItem);

	// register the addr block graph item for the parent
	parentItem->addChild(graphItem_);

	// tell child to refresh itself
	graphItem_->refresh();

	connect(graphItem_, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()), Qt::UniqueConnection);
}

QGraphicsItem* ComponentEditorFieldItem::getGraphicsItem() {
	return graphItem_;
}

void ComponentEditorFieldItem::updateGraphics() {
	if (graphItem_) {
		graphItem_->refresh();
	}
}

void ComponentEditorFieldItem::removeGraphicsItem() {
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
