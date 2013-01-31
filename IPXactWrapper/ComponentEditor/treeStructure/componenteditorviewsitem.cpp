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
editor_(component, libHandler) {

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
	connect(&editor_, SIGNAL(helpUrlRequested(QString const&)),
		this, SIGNAL(helpUrlRequested(QString const&)));
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
