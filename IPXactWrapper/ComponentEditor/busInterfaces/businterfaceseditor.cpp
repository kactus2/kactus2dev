/* 
 *  	Created on: 15.6.2012
 *      Author: Antti Kamppi
 * 		filename: businterfaceseditor.cpp
 *		Project: Kactus 2
 */

#include "businterfaceseditor.h"

BusInterfacesEditor::BusInterfacesEditor(LibraryInterface* handler,
										 QSharedPointer<Component> component, 
										 QWidget* parent /*= 0*/ ):
ItemEditor(component, parent),
view_(this),
proxy_(this),
model_(handler, component, this) {

}

BusInterfacesEditor::~BusInterfacesEditor() {
}

bool BusInterfacesEditor::isValid() const {
	return model_.isValid();
}

void BusInterfacesEditor::makeChanges() {
	// TODO remove this in final
}

void BusInterfacesEditor::refresh() {
	view_.update();
}
