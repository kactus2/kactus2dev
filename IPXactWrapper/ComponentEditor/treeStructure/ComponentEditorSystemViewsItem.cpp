//-----------------------------------------------------------------------------
// File: ComponentEditorSystemViewsItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 13.7.2012
//
// Description:
// The System Views-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "ComponentEditorSystemViewsItem.h"
#include "ComponentEditorSystemViewItem.h"

ComponentEditorSystemViewsItem::ComponentEditorSystemViewsItem(
	ComponentEditorTreeModel* model, 
	LibraryInterface* libHandler,
	QSharedPointer<Component> component,
	ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
systemViews_(component->getSystemViews()),
editor_(component, libHandler) {

	foreach (QSharedPointer<SystemView> systemView, systemViews_) {

		QSharedPointer<ComponentEditorSystemViewItem> systemViewItem(
			new ComponentEditorSystemViewItem(systemView, model, libHandler, component, this)); 
		childItems_.append(systemViewItem);
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

ComponentEditorSystemViewsItem::~ComponentEditorSystemViewsItem() {
}

QString ComponentEditorSystemViewsItem::getTooltip() const {
	return tr("Contains the system views of the component");
}

QString ComponentEditorSystemViewsItem::text() const {
	return tr("System views");
}

ItemEditor* ComponentEditorSystemViewsItem::editor() {
	return &editor_;
}

const ItemEditor* ComponentEditorSystemViewsItem::editor() const {
	return &editor_;
}

void ComponentEditorSystemViewsItem::createChild( int index ) {
	QSharedPointer<ComponentEditorSystemViewItem> systemViewItem(
		new ComponentEditorSystemViewItem(systemViews_.at(index), model_, libHandler_, component_, this)); 
	childItems_.insert(index, systemViewItem);
}
