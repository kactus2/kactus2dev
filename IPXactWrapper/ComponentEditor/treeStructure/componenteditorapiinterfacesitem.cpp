/* 
 *  	Created on: 24.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorapiinterfacesitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorapiinterfacesitem.h"
#include "componenteditorapiinterfaceitem.h"
#include <IPXactWrapper/ComponentEditor/software/apiInterface/apiinterfaceseditor.h>

ComponentEditorAPIInterfacesItem::ComponentEditorAPIInterfacesItem(
	ComponentEditorTreeModel* model,
	LibraryInterface* libHandler,
	QSharedPointer<Component> component,
	ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
apiInterfaces_(component->getApiInterfaces()) {

	foreach (QSharedPointer<ApiInterface> apiInterface, apiInterfaces_) {
		QSharedPointer<ComponentEditorAPIInterfaceItem> apiItem(
			new ComponentEditorAPIInterfaceItem(apiInterface, model, libHandler, component, this));
		childItems_.append(apiItem);
	}
}

ComponentEditorAPIInterfacesItem::~ComponentEditorAPIInterfacesItem() {
}

QFont ComponentEditorAPIInterfacesItem::getFont() const {
    QFont font(ComponentEditorItem::getFont());
    if ( childItems_.empty())
    {
        font.setBold(false);
    }
    return font;
}

QString ComponentEditorAPIInterfacesItem::getTooltip() const {
	return tr("Contains the API interfaces of the component");
}

QString ComponentEditorAPIInterfacesItem::text() const {
	return tr("API interfaces");
}

ItemEditor* ComponentEditorAPIInterfacesItem::editor() {
	if (!editor_) {
		editor_ = new ApiInterfacesEditor(component_, libHandler_);
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

void ComponentEditorAPIInterfacesItem::createChild( int index ) {
	QSharedPointer<ComponentEditorAPIInterfaceItem> apiItem(
		new ComponentEditorAPIInterfaceItem(apiInterfaces_.at(index), model_, libHandler_, component_, this));
    apiItem->setLocked(locked_);

	childItems_.insert(index, apiItem);
}
