/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorchannelsitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorchannelsitem.h"

ComponentEditorChannelsItem::ComponentEditorChannelsItem(ComponentEditorTreeModel* model,
														 LibraryInterface* libHandler,
														 QSharedPointer<Component> component,
														 QWidget* widget,
														 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent) {

}

ComponentEditorChannelsItem::~ComponentEditorChannelsItem() {
}

QString ComponentEditorChannelsItem::text() const {
	return tr("Channels");
}

bool ComponentEditorChannelsItem::isValid() const {
	return true;
}

ItemEditor* ComponentEditorChannelsItem::editor() {
	return NULL;
}
