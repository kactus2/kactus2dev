/* 
 *  	Created on: 28.6.2012
 *      Author: Antti Kamppi
 * 		filename: cominterfaceseditor.cpp
 *		Project: Kactus 2
 */

#include "cominterfaceseditor.h"
#include "cominterfacesdelegate.h"

ComInterfacesEditor::ComInterfacesEditor(QSharedPointer<Component> component,
										 QWidget* parent /*= 0*/):
ItemEditor(component, parent),
view_(this),
proxy_(this),
model_(component, this) {

}

ComInterfacesEditor::~ComInterfacesEditor() {
}

bool ComInterfacesEditor::isValid() const {
	return model_.isValid();
}

void ComInterfacesEditor::makeChanges() {
	// TODO remove this in final
}

void ComInterfacesEditor::refresh() {
	view_.update();
}
