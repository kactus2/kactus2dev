/* 
 *  	Created on: 15.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorchannelitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORCHANNELITEM_H
#define COMPONENTEDITORCHANNELITEM_H

#include "componenteditoritem.h"
#include <IPXactWrapper/ComponentEditor/channels/channeleditor.h>
#include <models/channel.h>

#include <QSharedPointer>

/*! \brief The item for a single channel in the component's navigation tree.
 *
 */
class ComponentEditorChannelItem : public ComponentEditorItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param channel Pointer to the channel being edited.
	 * \param model Pointer to the model that owns the items.
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param widget Pointer to the QWidget that displays the editor.
	 * \param parent Pointer to the parent item.
	 *
	*/
	ComponentEditorChannelItem(QSharedPointer<Channel> channel,
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		QWidget* widget,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorChannelItem();

	/*! \brief Get the text to be displayed to user in the tree for this item.
	 *
	 * \return QString Contains the text to display.
	*/
	virtual QString text() const;

	/*! \brief Check the validity of this item and sub items.
	 *
	 * \return bool True if item is in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Get pointer to the editor of this item.
	 *
	 * \return Pointer to the editor to use for this item.
	*/
	virtual ItemEditor* editor();

private:
	//! \brief No copying
	ComponentEditorChannelItem(const ComponentEditorChannelItem& other);

	//! \brief No assignment
	ComponentEditorChannelItem& operator=(const ComponentEditorChannelItem& other);

	//! \brief Pointer to the channel being edited.
	QSharedPointer<Channel> channel_;

	//! \brief The editor to edit the channel.
	ChannelEditor editor_;
};

#endif // COMPONENTEDITORCHANNELITEM_H
