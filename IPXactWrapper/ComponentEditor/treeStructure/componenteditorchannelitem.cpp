/* 
 *  	Created on: 15.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorchannelitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorchannelitem.h"

#include <QFont>
#include <QApplication>

ComponentEditorChannelItem::ComponentEditorChannelItem(QSharedPointer<Channel> channel, 
													   ComponentEditorTreeModel* model,
													   LibraryInterface* libHandler,
													   QSharedPointer<Component> component,
													   ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
channel_(channel),
editor_(component_, channel) {
	editor_.hide();

	connect(&editor_, SIGNAL(contentChanged()),
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
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

QFont ComponentEditorChannelItem::getFont() const {
	return QApplication::font();
}

QString ComponentEditorChannelItem::getTooltip() const {
	return tr("Contains a list of all mirrored bus interfaces that belong to the"
		" same channel");
}
