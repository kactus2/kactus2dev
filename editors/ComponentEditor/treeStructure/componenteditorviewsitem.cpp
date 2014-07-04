/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorviewsitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorviewsitem.h"
#include "componenteditorviewitem.h"
#include <editors/ComponentEditor/views/viewseditor.h>

ComponentEditorViewsItem::ComponentEditorViewsItem(ComponentEditorTreeModel* model,
												   LibraryInterface* libHandler,
												   QSharedPointer<Component> component,
												   ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
views_(component->getViews()) {

	setObjectName(tr("ComponentEditorViewsItem"));

	foreach (QSharedPointer<View> view, views_) {

		QSharedPointer<ComponentEditorViewItem> viewItem(new ComponentEditorViewItem(
			view, model, libHandler, component, this));
		childItems_.append(viewItem);
	}
}

ComponentEditorViewsItem::~ComponentEditorViewsItem() {
}

QString ComponentEditorViewsItem::getTooltip() const {
    return tr("Contains the views of the component");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorViewsItem::getFont()
//-----------------------------------------------------------------------------
QFont ComponentEditorViewsItem::getFont() const
{
    QFont font(ComponentEditorItem::getFont());
    font.setBold(!views_.isEmpty());
    return font;
}

QString ComponentEditorViewsItem::text() const {
	return tr("Views");
}

ItemEditor* ComponentEditorViewsItem::editor() {
	if (!editor_) {
		editor_ = new ViewsEditor(component_, libHandler_);
		editor_->setProtection(locked_);
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

void ComponentEditorViewsItem::createChild( int index ) {
	QSharedPointer<ComponentEditorViewItem> viewItem(
		new ComponentEditorViewItem(views_.at(index), model_, libHandler_, component_, this));
	viewItem->setLocked(locked_);
	childItems_.insert(index, viewItem);
}
