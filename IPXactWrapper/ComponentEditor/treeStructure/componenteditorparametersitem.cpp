/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorparametersitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorparametersitem.h"

ComponentEditorParametersItem::ComponentEditorParametersItem(ComponentEditorTreeModel* model,
															 LibraryInterface* libHandler,
															 QSharedPointer<Component> component,
															 QWidget* widget,
															 ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent) {

}

ComponentEditorParametersItem::~ComponentEditorParametersItem() {
}

QString ComponentEditorParametersItem::text() const {
	return tr("Parameters");
}

bool ComponentEditorParametersItem::isValid() const {
	return true;
}

ItemEditor* ComponentEditorParametersItem::editor() {
	return NULL;
}
