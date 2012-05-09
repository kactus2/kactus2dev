/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditormodelparamsitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditormodelparamsitem.h"

ComponentEditorModelParamsItem::ComponentEditorModelParamsItem( ComponentEditorTreeModel* model,
															   LibraryInterface* libHandler,
															   QSharedPointer<Component> component, 
															   QWidget* widget, 
															   ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent),
editor_(component, 0, widget){

}

ComponentEditorModelParamsItem::~ComponentEditorModelParamsItem() {
}

QString ComponentEditorModelParamsItem::text() const {
	return tr("Model parameters");
}

bool ComponentEditorModelParamsItem::isValid() const {
	return true;
}

ItemEditor* ComponentEditorModelParamsItem::editor() {
	return NULL;
}
