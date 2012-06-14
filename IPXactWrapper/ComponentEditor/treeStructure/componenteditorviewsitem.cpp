/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorviewsitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorviewsitem.h"
#include "componenteditorviewitem.h"

ComponentEditorViewsItem::ComponentEditorViewsItem(ComponentEditorTreeModel* model,
												   LibraryInterface* libHandler,
												   QSharedPointer<Component> component,
												   ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
views_(component->getViews()),
editor_(component) {

	setObjectName(tr("ComponentEditorViewsItem"));

	foreach (QSharedPointer<View> view, views_) {

		QSharedPointer<ComponentEditorViewItem> viewItem(new ComponentEditorViewItem(
			view, model, libHandler, component, this));
		childItems_.append(viewItem);
	}

	editor_.hide();

	connect(&editor_, SIGNAL(contentChanged()), 
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(childAdded(int)),
		this, SLOT(onAddChild(int)), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(childRemoved(int)),
		this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
}

ComponentEditorViewsItem::~ComponentEditorViewsItem() {
}

QString ComponentEditorViewsItem::text() const {
	return tr("Views");
}

ItemEditor* ComponentEditorViewsItem::editor() {
	return &editor_;
}

const ItemEditor* ComponentEditorViewsItem::editor() const {
	return &editor_;
}

QString ComponentEditorViewsItem::getTooltip() const {
	return tr("Contains the views of the component");
}

void ComponentEditorViewsItem::createChild( int index ) {
	QSharedPointer<ComponentEditorViewItem> viewItem(
		new ComponentEditorViewItem(views_.at(index), model_, libHandler_, component_, this));	
	childItems_.insert(index, viewItem);
}

void ComponentEditorViewsItem::onEditorChanged() {
	// call the base class implementation
	ComponentEditorItem::onEditorChanged();

	// also inform of child changes
	foreach (QSharedPointer<ComponentEditorItem> childItem, childItems_) {
		// tell the model that data has changed for the child
		emit contentChanged(childItem.data());

		// tell the child to update it's editor contents
		childItem->refreshEditor();
	}
}
