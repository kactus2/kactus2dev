/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorgeneralitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorgeneralitem.h"

ComponentEditorGeneralItem::ComponentEditorGeneralItem( ComponentEditorTreeModel* model,
													   LibraryInterface* libHandler,
													   QSharedPointer<Component> component,
													   QWidget* widget,
													   ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent),
editor_(widget, libHandler, component, widget) {
}

ComponentEditorGeneralItem::~ComponentEditorGeneralItem() {
}

QString ComponentEditorGeneralItem::text() const {
	return tr("General");
}

bool ComponentEditorGeneralItem::isValid() const {
	return editor_.isValid();
}

ItemEditor* ComponentEditorGeneralItem::editor() {
	return &editor_;
}
