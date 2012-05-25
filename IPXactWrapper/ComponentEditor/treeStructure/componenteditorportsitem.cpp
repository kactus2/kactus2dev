/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorportsitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorportsitem.h"

ComponentEditorPortsItem::ComponentEditorPortsItem(ComponentEditorTreeModel* model,
												   LibraryInterface* libHandler,
												   QSharedPointer<Component> component,
												   ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent),
ports_(component->getPorts()),
editor_(component, libHandler) {
	
	editor_.hide();
	connect(&editor_, SIGNAL(contentChanged()),
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
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
	return editor_.isValid();
}

ItemEditor* ComponentEditorPortsItem::editor() {
	return &editor_;
}

QString ComponentEditorPortsItem::getTooltip() const {
	return tr("Contains the external ports of the component");
}
