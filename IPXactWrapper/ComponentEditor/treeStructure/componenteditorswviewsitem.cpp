/* 
 *  	Created on: 24.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorswviewsitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorswviewsitem.h"
#include "componenteditorswviewitem.h"
#include <IPXactWrapper/ComponentEditor/software/swView/swviewseditor.h>

ComponentEditorSWViewsItem::ComponentEditorSWViewsItem(
	ComponentEditorTreeModel* model, 
	LibraryInterface* libHandler,
	QSharedPointer<Component> component,
	ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
swViews_(component->getSWViews()) {

	foreach (QSharedPointer<SWView> swView, swViews_) {

		QSharedPointer<ComponentEditorSWViewItem> swViewItem(
			new ComponentEditorSWViewItem(swView, model, libHandler, component, this)); 
		childItems_.append(swViewItem);
	}
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
	if (!editor_) {
		editor_ = new SWViewsEditor(component_, libHandler_);
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

void ComponentEditorSWViewsItem::createChild( int index ) {
	QSharedPointer<ComponentEditorSWViewItem> swViewItem(
		new ComponentEditorSWViewItem(swViews_.at(index), model_, libHandler_, component_, this));
	swViewItem->setLocked(locked_);
	childItems_.insert(index, swViewItem);
}
