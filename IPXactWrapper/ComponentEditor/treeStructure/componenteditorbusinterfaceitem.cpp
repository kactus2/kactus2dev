/* 
 *  	Created on: 14.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorbusinterfaceitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorbusinterfaceitem.h"
#include <IPXactWrapper/ComponentEditor/busInterfaces/businterfaceeditor.h>

#include <QFont>
#include <QApplication>

ComponentEditorBusInterfaceItem::ComponentEditorBusInterfaceItem(QSharedPointer<BusInterface> busif,
																 ComponentEditorTreeModel* model,
																 LibraryInterface* libHandler,
																 QSharedPointer<Component> component,
																 ComponentEditorItem* parent,
                                                                 QWidget* parentWnd):
ComponentEditorItem(model, libHandler, component, parent),
busif_(busif),
parentWnd_(parentWnd) {

}

ComponentEditorBusInterfaceItem::~ComponentEditorBusInterfaceItem() {
}

QString ComponentEditorBusInterfaceItem::text() const {
	return busif_->getName();
}

bool ComponentEditorBusInterfaceItem::isValid() const {
	// check that the bus interface is valid
	if (!busif_->isValid(component_->getPortBounds())) {
		return false;
	}

	// check that the bus definition is found
	else if (!libHandler_->contains(busif_->getBusType())) {
		return false;
	}

	// if there is an abstraction definition then check that it is found
	else if (busif_->getAbstractionType().isValid()) {
		if (!libHandler_->contains(busif_->getAbstractionType())) {
			return false;
		}
	}

	return true;
}

ItemEditor* ComponentEditorBusInterfaceItem::editor() {
	if (!editor_) {
		editor_ = new BusInterfaceEditor(libHandler_, component_, busif_, 0, parentWnd_);
		editor_->setDisabled(locked_);
		connect(editor_, SIGNAL(contentChanged()),
			this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
			this, SIGNAL(helpUrlRequested(QString const&)));
	}
	return editor_;
}

QFont ComponentEditorBusInterfaceItem::getFont() const {
	return QApplication::font();
}

QString ComponentEditorBusInterfaceItem::getTooltip() const {
	return tr("Defines properties of a specific interface in a component");
}

bool ComponentEditorBusInterfaceItem::canBeOpened() const {
	// if the bus type is valid vlnv
	if (busif_->getBusType().isValid()) {

		// if bus definition exists then it can be opened
		return libHandler_->contains(busif_->getBusType());
	}
	else {
		return false;
	}
}

void ComponentEditorBusInterfaceItem::openItem( bool /*builtinEditor = false*/ ) {
	VLNV busdefVLNV = busif_->getBusType();
	VLNV absdefVLNV = busif_->getAbstractionType();
	emit openBus(busdefVLNV, absdefVLNV);
}
