/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorchannelsitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORCHANNELSITEM_H
#define COMPONENTEDITORCHANNELSITEM_H

#include <IPXactWrapper/ComponentEditor/channels/channelseditor.h>

#include "componenteditoritem.h"

/*! \brief The channels-item in the component editor's navigation tree.
 *
 */
class ComponentEditorChannelsItem : public ComponentEditorItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param model Pointer to the model that owns the items.
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the parent item.
	 *
	*/
	ComponentEditorChannelsItem(ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorChannelsItem();

	/*! \brief Get the tool tip for the item.
	 * 
	 * \return The text for the tool tip to print to user.
	*/
	virtual QString getTooltip() const;

	/*! \brief Get the text to be displayed to user in the tree for this item.
	 *
	 * \return QString Contains the text to display.
	*/
	virtual QString text() const;

	/*! \brief Get pointer to the editor of this item.
	 *
	 * \return Pointer to the editor to use for this item.
	*/
	virtual ItemEditor* editor();

	/*! \brief Get pointer to the editor of this item.
	 *
	 * \return Pointer to the editor to use for this item.
	*/
	virtual const ItemEditor* editor() const;

private:
	//! \brief No copying
	ComponentEditorChannelsItem(const ComponentEditorChannelsItem& other);

	//! \brief No assignment
	ComponentEditorChannelsItem& operator=(const ComponentEditorChannelsItem& other);

	//! \brief The channels being edited.
	QList<QSharedPointer<Channel> >& channels_;

	//! \brief The editor to edit the channels
	ChannelsEditor editor_;
};

#endif // COMPONENTEDITORCHANNELSITEM_H
