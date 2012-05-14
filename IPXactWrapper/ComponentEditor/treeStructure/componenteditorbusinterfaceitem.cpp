/* 
 *  	Created on: 14.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorbusinterfaceitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorbusinterfaceitem.h"

ComponentEditorBusInterfaceItem::ComponentEditorBusInterfaceItem(QSharedPointer<BusInterface> busif,
																 ComponentEditorTreeModel* model,
																 LibraryInterface* libHandler,
																 QSharedPointer<Component> component,
																 QWidget* widget,
																 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
busif_(busif),
editor_(libHandler, component, busif, widget) {

}

ComponentEditorBusInterfaceItem::~ComponentEditorBusInterfaceItem() {
}

QString ComponentEditorBusInterfaceItem::text() const {
	return busif_->getName();
}

bool ComponentEditorBusInterfaceItem::isValid() const {
	if (!busif_->isValid(component_->getPortBounds())) {
		return false;
	}
	return editor_.isValid();
}

ItemEditor* ComponentEditorBusInterfaceItem::editor() {
	return &editor_;
}
