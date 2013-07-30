/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorcominterfacesitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorcominterfacesitem.h"
#include "componenteditorcominterfaceitem.h"
#include <IPXactWrapper/ComponentEditor/software/comInterface/cominterfaceseditor.h>

ComponentEditorComInterfacesItem::ComponentEditorComInterfacesItem(ComponentEditorTreeModel* model, 
																   LibraryInterface* libHandler,
																   QSharedPointer<Component> component,
																   ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
interfaces_(component->getComInterfaces()) {

	foreach (QSharedPointer<ComInterface> iface, interfaces_) {
		QSharedPointer<ComponentEditorComInterfaceItem> interfaceItem(new ComponentEditorComInterfaceItem(
			iface, model, libHandler, component, this));
		childItems_.append(interfaceItem);
	}
}

ComponentEditorComInterfacesItem::~ComponentEditorComInterfacesItem() {
}

QFont ComponentEditorComInterfacesItem::getFont() const {
    QFont font(ComponentEditorItem::getFont());
    if ( childItems_.isEmpty() )
    {
        font.setBold(false);
    }
    return font;
}

QString ComponentEditorComInterfacesItem::text() const {
	return tr("COM interfaces");
}

ItemEditor* ComponentEditorComInterfacesItem::editor() {
	if (!editor_) {
		editor_ = new ComInterfacesEditor(libHandler_, component_);
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

QString ComponentEditorComInterfacesItem::getTooltip() const {
	return tr("Contains the COM interfaces of a component");
}

void ComponentEditorComInterfacesItem::createChild( int index ) {
	QSharedPointer<ComponentEditorComInterfaceItem> interfaceItem(new ComponentEditorComInterfaceItem(
		interfaces_.at(index), model_, libHandler_, component_, this));
	interfaceItem->setLocked(locked_);
	childItems_.insert(index, interfaceItem);
}
