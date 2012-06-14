/* 
 *  	Created on: 14.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorbusinterfaceitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorbusinterfaceitem.h"

#include <QFont>
#include <QApplication>

ComponentEditorBusInterfaceItem::ComponentEditorBusInterfaceItem(QSharedPointer<BusInterface> busif,
																 ComponentEditorTreeModel* model,
																 LibraryInterface* libHandler,
																 QSharedPointer<Component> component,
																 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
busif_(busif),
editor_(libHandler, component, busif) {
	editor_.hide();

	connect(&editor_, SIGNAL(contentChanged()),
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
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

const ItemEditor* ComponentEditorBusInterfaceItem::editor() const {
	return &editor_;
}

QFont ComponentEditorBusInterfaceItem::getFont() const {
	return QApplication::font();
}

QString ComponentEditorBusInterfaceItem::getTooltip() const {
	return tr("Defines properties of a specific interface in a component");
}
