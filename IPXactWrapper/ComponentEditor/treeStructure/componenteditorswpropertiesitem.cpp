/* 
 *  	Created on: 24.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorswpropertiesitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorswpropertiesitem.h"

ComponentEditorSWPropertiesItem::ComponentEditorSWPropertiesItem(
	ComponentEditorTreeModel* model,
	LibraryInterface* libHandler,
	QSharedPointer<Component> component,
	ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
swProperties_(component->getSWProperties()),
editor_(component, NULL) {

	connect(&editor_, SIGNAL(contentChanged()),
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
}

ComponentEditorSWPropertiesItem::~ComponentEditorSWPropertiesItem() {
}

QString ComponentEditorSWPropertiesItem::getTooltip() const {
	return tr("Contains the software properties of the component");
}

QString ComponentEditorSWPropertiesItem::text() const {
	return tr("Software properties");
}

bool ComponentEditorSWPropertiesItem::isValid() const {
	foreach (QSharedPointer<ComProperty> property, swProperties_) {
		if (!property->isValid()) {
			return false;
		}
	}
	return true;
}

ItemEditor* ComponentEditorSWPropertiesItem::editor() {
	return &editor_;
}
