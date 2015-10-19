//-----------------------------------------------------------------------------
// File: componenteditorchannelsitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 09.05.2012
//
// Description:
// The channels-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditorchannelsitem.h"

#include <editors/ComponentEditor/channels/channelseditor.h>

#include <IPXACTmodels/Component/Component.h>

//-----------------------------------------------------------------------------
// Function: ComponentEditorChannelsItem::ComponentEditorChannelsItem()
//-----------------------------------------------------------------------------
ComponentEditorChannelsItem::ComponentEditorChannelsItem(ComponentEditorTreeModel* model,
    LibraryInterface* libHandler,
    QSharedPointer<Component> component,
    ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
    channels_(component->getChannels())
{

}

//-----------------------------------------------------------------------------
// Function: ComponentEditorChannelsItem::~ComponentEditorChannelsItem()
//-----------------------------------------------------------------------------
ComponentEditorChannelsItem::~ComponentEditorChannelsItem()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorChannelsItem::getFont()
//-----------------------------------------------------------------------------
QFont ComponentEditorChannelsItem::getFont() const
{
    QFont font(ComponentEditorItem::getFont());
    font.setBold(!channels_->isEmpty());
    return font;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorChannelsItem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorChannelsItem::text() const
{
	return tr("Channels");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorChannelsItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorChannelsItem::editor()
{
	if (!editor_)
    {
		editor_ = new ChannelsEditor(component_, libHandler_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));
	}

	return editor_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorChannelsItem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorChannelsItem::getTooltip() const
{
	return tr("Contains the channels specified for the component");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorChannelsItem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorChannelsItem::isValid() const 
{
	/*QStringList interfaceNames = component_->getBusInterfaceNames();

	// check each channel
	foreach (QSharedPointer<Channel> channel, channels_) {
		if (!channel->isValid(interfaceNames)) {
			return false;
		}
	}*/

	// all channels were valid
	return true;
}
