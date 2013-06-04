/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorportsitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorportsitem.h"
#include <IPXactWrapper/ComponentEditor/ports/portseditor.h>

ComponentEditorPortsItem::ComponentEditorPortsItem(ComponentEditorTreeModel* model,
												   LibraryInterface* libHandler,
												   QSharedPointer<Component> component,
												   ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent),
ports_(component->getPorts()) {

}

ComponentEditorPortsItem::~ComponentEditorPortsItem() {
}

QString ComponentEditorPortsItem::text() const {
	return tr("Ports");
}

bool ComponentEditorPortsItem::isValid() const {
	bool hasViews = component_->hasViews();
	foreach (QSharedPointer<Port> port, ports_) {
		if (!port->isValid(hasViews)) {
			return false;
		}
	}
	return true;
}

ItemEditor* ComponentEditorPortsItem::editor() {
	if (!editor_) {
		editor_ = new PortsEditor(component_, libHandler_);
		editor_->setDisabled(locked_);
		connect(editor_, SIGNAL(contentChanged()),
			this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
			this, SIGNAL(helpUrlRequested(QString const&)), Qt::UniqueConnection);
	}
	return editor_;
}

QString ComponentEditorPortsItem::getTooltip() const {
	return tr("Contains the external ports of the component");
}
