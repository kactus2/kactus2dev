//-----------------------------------------------------------------------------
// File: channelseditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 14.06.2012
//
// Description:
// The editor to add/remove/edit the channels of a component.
//-----------------------------------------------------------------------------

#include "channelseditor.h"
#include "channelsdelegate.h"

#include <common/widgets/summaryLabel/summarylabel.h>

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/Component/Component.h>

#include <QVBoxLayout>
#include <QHeaderView>
//-----------------------------------------------------------------------------
// Function: ChannelsEditor::ChannelsEditor()
//-----------------------------------------------------------------------------
ChannelsEditor::ChannelsEditor(QSharedPointer<Component> component, LibraryInterface* handler, 
    QSharedPointer<ChannelValidator> validator, QWidget* parent):
ItemEditor(component, handler, parent),
    view_(this),
    proxy_(this),
    model_(component->getChannels(), validator, this)
{
    // display a label on top the table
    SummaryLabel* summaryLabel = new SummaryLabel(tr("Channels"), this);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
    layout->addWidget(&view_);
    layout->setContentsMargins(0, 0, 0, 0);

    proxy_.setSourceModel(&model_);
    view_.setModel(&proxy_);

    const QString componentPath = handler->getDirectoryPath(component->getVlnv());
	QString defaultPath = QString("%1/channelListing.csv").arg(componentPath);
	view_.setDefaultImportExportPath(defaultPath);
	view_.setAllowImportExport(true);
	
	view_.setItemsDraggable(false);

	view_.setItemDelegate(new ChannelsDelegate(component, this));	

	connect(&model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(channelAdded(int)), this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(channelRemoved(int)), this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(QModelIndex const&)),
		&model_, SLOT(onAddItem(QModelIndex const&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(QModelIndex const&)),
		&model_, SLOT(onRemoveItem(QModelIndex const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ChannelsEditor::~ChannelsEditor()
//-----------------------------------------------------------------------------
ChannelsEditor::~ChannelsEditor()
{
}

//-----------------------------------------------------------------------------
// Function: ChannelsEditor::refresh()
//-----------------------------------------------------------------------------
void ChannelsEditor::refresh()
{
	view_.update();
}

//-----------------------------------------------------------------------------
// Function: ChannelsEditor::showEvent()
//-----------------------------------------------------------------------------
void ChannelsEditor::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/channels.html");
}
