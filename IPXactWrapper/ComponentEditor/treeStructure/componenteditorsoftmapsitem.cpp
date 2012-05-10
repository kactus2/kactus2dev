/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorsoftmapsitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorsoftmapsitem.h"

ComponentEditorSoftMapsItem::ComponentEditorSoftMapsItem(ComponentEditorTreeModel* model,
														 LibraryInterface* libHandler, 
														 QSharedPointer<Component> component,
														 QWidget* widget,
														 ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent),
editor_(libHandler, component, widget, widget) {

}

ComponentEditorSoftMapsItem::~ComponentEditorSoftMapsItem() {
}

QString ComponentEditorSoftMapsItem::text() const {
	return tr("Software mappings");
}

bool ComponentEditorSoftMapsItem::isValid() const {
	return editor_.isValid();
}

ItemEditor* ComponentEditorSoftMapsItem::editor() {
	return &editor_;
}
