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
														 ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent),
editor_(libHandler, component) {
	editor_.hide();
	connect(&editor_, SIGNAL(contentChanged()),
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
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

QString ComponentEditorSoftMapsItem::getTooltip() const {
	return tr("Contains the software mappings of the component");
}
