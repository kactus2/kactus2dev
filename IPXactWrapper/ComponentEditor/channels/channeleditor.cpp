/* 
 *
 *  Created on: 4.4.2011
 *      Author: Antti Kamppi
 * 		filename: channeleditor.cpp
 */

#include "channeleditor.h"

#include <models/component.h>
#include <models/channel.h>

#include <QVBoxLayout>

ChannelEditor::ChannelEditor(QSharedPointer<Component> component, 
							 void* dataPointer, 
							 QWidget *parent): 
ItemEditor(component, parent),
component_(component),
channel_(static_cast<Channel*>(dataPointer)),
nameGroup_(this, tr("Names and description")),
interfaceEdit_(component, dataPointer, this) {

	Q_ASSERT(dataPointer);
	initialize();
}

ChannelEditor::ChannelEditor( QSharedPointer<Component> component,
							 QSharedPointer<Channel> channel,
							 QWidget *parent ):
ItemEditor(component, parent),
component_(component),
channel_(channel.data()),
nameGroup_(this, tr("Names and description")),
interfaceEdit_(component, channel_, this) {

	Q_ASSERT(component);
	Q_ASSERT(channel);
	initialize();
}

ChannelEditor::~ChannelEditor() {
}

void ChannelEditor::initialize() {

	connect(&nameGroup_, SIGNAL(nameChanged(const QString&)),
		this, SIGNAL(nameChanged(const QString&)), Qt::UniqueConnection);
	connect(&nameGroup_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&interfaceEdit_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&nameGroup_);
	layout->addWidget(&interfaceEdit_);

	refresh();
}

bool ChannelEditor::isValid() const {
	if (!nameGroup_.isValid())
		return false;
	else if (!interfaceEdit_.isValid())
		return false;
	
	return true;
}

void ChannelEditor::removeModel() {
	component_->removeChannel(channel_->getName());
	channel_ = 0;
}

void ChannelEditor::makeChanges() {
	channel_->setName(nameGroup_.getName());
	channel_->setDisplayName(nameGroup_.getDisplayName());
	channel_->setDescription(nameGroup_.getDescription());

	channel_->setInterfaces(interfaceEdit_.items());
}

void ChannelEditor::refresh() {
	nameGroup_.setName(channel_->getName());
	nameGroup_.setDisplayName(channel_->getDisplayName());
	nameGroup_.setDescription(channel_->getDescription());
	interfaceEdit_.initialize(channel_->getInterfaces());
}
