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
														 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
channels_(component->getChannels()),
editor_(component) {

	editor_.hide();

	connect(&editor_, SIGNAL(contentChanged()), 
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
}

ComponentEditorChannelsItem::~ComponentEditorChannelsItem() {
}

QString ComponentEditorChannelsItem::text() const {
	return tr("Channels");
}

ItemEditor* ComponentEditorChannelsItem::editor() {
	return &editor_;
}

const ItemEditor* ComponentEditorChannelsItem::editor() const {
	return &editor_;
}

QString ComponentEditorChannelsItem::getTooltip() const {
	return tr("Contains the channels specified for the component");
}
