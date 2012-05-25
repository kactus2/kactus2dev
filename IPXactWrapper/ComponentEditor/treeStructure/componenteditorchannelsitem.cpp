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
														 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
channels_(component->getChannels()) {

	foreach (QSharedPointer<Channel> channel, channels_) {
		QSharedPointer<ComponentEditorChannelItem> channelItem(new ComponentEditorChannelItem(
			channel, model, libHandler, component, this));
		childItems_.append(channelItem);
	}
}

ComponentEditorChannelsItem::~ComponentEditorChannelsItem() {
}

QString ComponentEditorChannelsItem::text() const {
	return tr("Channels");
}

ItemEditor* ComponentEditorChannelsItem::editor() {
	return NULL;
}

QString ComponentEditorChannelsItem::getTooltip() const {
	return tr("Contains the channels specified for the component");
}
