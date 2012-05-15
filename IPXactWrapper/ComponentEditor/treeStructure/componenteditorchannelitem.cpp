/* 
 *  	Created on: 15.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorchannelitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorchannelitem.h"

ComponentEditorChannelItem::ComponentEditorChannelItem(QSharedPointer<Channel> channel, 
													   ComponentEditorTreeModel* model,
													   LibraryInterface* libHandler,
													   QSharedPointer<Component> component,
													   QWidget* widget,
													   ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
channel_(channel),
editor_(component_, channel, widget) {

}

ComponentEditorChannelItem::~ComponentEditorChannelItem() {
}

QString ComponentEditorChannelItem::text() const {
	return channel_->getName();
}

bool ComponentEditorChannelItem::isValid() const {
	if (!channel_->isValid(component_->getBusInterfaceNames())) {
		return false;
	}
	return editor_.isValid();
}

ItemEditor* ComponentEditorChannelItem::editor() {
	return &editor_;
}
