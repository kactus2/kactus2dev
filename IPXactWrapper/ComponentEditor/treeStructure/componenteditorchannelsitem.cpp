/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorchannelsitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorchannelsitem.h"
#include "componenteditorchannelitem.h"

ComponentEditorChannelsItem::ComponentEditorChannelsItem(ComponentEditorTreeModel* model,
														 LibraryInterface* libHandler,
														 QSharedPointer<Component> component,
														 QWidget* widget,
														 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
channels_(component->getChannels()) {

	foreach (QSharedPointer<Channel> channel, channels_) {
		ComponentEditorChannelItem* channelItem = new ComponentEditorChannelItem(
			channel, model, libHandler, component, widget, this);
		childItems_.append(channelItem);
	}
}

ComponentEditorChannelsItem::~ComponentEditorChannelsItem() {
}

QString ComponentEditorChannelsItem::text() const {
	return tr("Channels");
}

bool ComponentEditorChannelsItem::isValid() const {
	foreach (QSharedPointer<Channel> channel, channels_) {
		if (!channel->isValid(component_->getBusInterfaceNames())) {
			return false;
		}
	}
	return true;
}

ItemEditor* ComponentEditorChannelsItem::editor() {
	return NULL;
}
