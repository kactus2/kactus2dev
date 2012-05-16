/* 
 *  	Created on: 16.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditor.cpp
 *		Project: Kactus 2
 */

#include "componenteditor.h"

ComponentEditor::ComponentEditor(LibraryInterface* libHandler,
								 QSharedPointer<Component> component,
								 QWidget *parent):
TabDocument(parent, DOC_PROTECTION_SUPPORT),
libHandler_(libHandler),
component_(component),
navigationView_(libHandler, *component->getVlnv(), this),
navigationModel_(libHandler, this, this) {

	Q_ASSERT(component_);
	Q_ASSERT(libHandler_);
}

ComponentEditor::~ComponentEditor() {
}
