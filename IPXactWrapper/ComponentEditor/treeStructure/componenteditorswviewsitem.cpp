/* 
 *  	Created on: 24.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorswviewsitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorswviewsitem.h"
#include "componenteditorswviewitem.h"

ComponentEditorSWViewsItem::ComponentEditorSWViewsItem(
	ComponentEditorTreeModel* model, 
	LibraryInterface* libHandler,
	QSharedPointer<Component> component,
	ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
swViews_(component->getSWViews()),
editor_(component) {

	foreach (QSharedPointer<SWView> swView, swViews_) {

		QSharedPointer<ComponentEditorSWViewItem> swViewItem(
			new ComponentEditorSWViewItem(swView, model, libHandler, component, this)); 
		childItems_.append(swViewItem);
	}

	editor_.hide();

	connect(&editor_, SIGNAL(contentChanged()), 
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(childAdded(int)),
		this, SLOT(onAddChild(int)), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(childRemoved(int)),
		this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
}

ComponentEditorSWViewsItem::~ComponentEditorSWViewsItem() {
}

QString ComponentEditorSWViewsItem::getTooltip() const {
	return tr("Contains the software views of the component");
}

QString ComponentEditorSWViewsItem::text() const {
	return tr("Software views");
}

ItemEditor* ComponentEditorSWViewsItem::editor() {
	return &editor_;
}

const ItemEditor* ComponentEditorSWViewsItem::editor() const {
	return &editor_;
}

void ComponentEditorSWViewsItem::createChild( int index ) {
	QSharedPointer<ComponentEditorSWViewItem> swViewItem(
		new ComponentEditorSWViewItem(swViews_.at(index), model_, libHandler_, component_, this)); 
	childItems_.insert(index, swViewItem);
}
