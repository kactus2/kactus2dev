/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorchannelsitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorchannelsitem.h"
#include <editors/ComponentEditor/channels/channelseditor.h>

ComponentEditorChannelsItem::ComponentEditorChannelsItem(ComponentEditorTreeModel* model,
														 LibraryInterface* libHandler,
														 QSharedPointer<Component> component,
														 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
channels_(component->getChannels()) {
	
}

ComponentEditorChannelsItem::~ComponentEditorChannelsItem() {
}

QFont ComponentEditorChannelsItem::getFont() const {
    QFont font(ComponentEditorItem::getFont());
    if ( channels_.isEmpty() )
    {
        font.setBold(false);
    }
    return font;
}

QString ComponentEditorChannelsItem::text() const {
	return tr("Channels");
}

ItemEditor* ComponentEditorChannelsItem::editor() {
	if (!editor_) {
		editor_ = new ChannelsEditor(component_, libHandler_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), 
			this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
			this, SIGNAL(helpUrlRequested(QString const&)));
	}
	return editor_;
}

QString ComponentEditorChannelsItem::getTooltip() const {
	return tr("Contains the channels specified for the component");
}

bool ComponentEditorChannelsItem::isValid() const {
	QStringList interfaceNames = component_->getBusInterfaceNames();

	// check each channel
	foreach (QSharedPointer<Channel> channel, channels_) {
		if (!channel->isValid(interfaceNames)) {
			return false;
		}
	}

	// all channels were valid
	return true;
}
