//-----------------------------------------------------------------------------
// File: channelseditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 14.06.2012
//
// Description:
// The editor to add/remove/edit the channels of a component.
//-----------------------------------------------------------------------------

#ifndef CHANNELSEDITOR_H
#define CHANNELSEDITOR_H

#include <editors/ComponentEditor/itemeditor.h>

#include "channelsmodel.h"

#include <common/views/EditableTableView/editabletableview.h>

#include <QSharedPointer>
#include <QSortFilterProxyModel>

class ChannelValidator;
class Component;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! The editor to add/remove/edit the channels of a component.
//-----------------------------------------------------------------------------
class ChannelsEditor : public ItemEditor
{
	Q_OBJECT

public:
	
	/*! The constructor
	 *
	 *      @param [in] component   The component being edited.
	 *      @param [in] handler     The instance managing the library.
     *      @param [in] validator   The validator for channel elements.
	 *      @param [in] parent      The parent of this editor.
	*/
	ChannelsEditor(QSharedPointer<Component> component,	LibraryInterface* handler, 
        QSharedPointer<ChannelValidator> validator, QWidget* parent = 0);
	
	//! The destructor
	virtual ~ChannelsEditor();

	/*! Reload the information from the model to the editor.
	*/
	virtual void refresh();

protected:

	//! Handler for widget's show event
	virtual void showEvent(QShowEvent* event);

private:

	//! No copying
	ChannelsEditor(const ChannelsEditor& other);
	ChannelsEditor& operator=(const ChannelsEditor& other);

	//! The view that displays the channels.
	EditableTableView view_;

	//! The proxy that does the editing.
	QSortFilterProxyModel proxy_;

	//! The model that manages the channels being edited.
	ChannelsModel model_;
};

#endif // CHANNELSEDITOR_H
