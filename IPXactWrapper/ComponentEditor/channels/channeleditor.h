/* 
 *
 *  Created on: 4.4.2011
 *      Author: Antti Kamppi
 * 		filename: channeleditor.h
 */

#ifndef CHANNELEDITOR_H
#define CHANNELEDITOR_H

#include <IPXactWrapper/ComponentEditor/itemeditor.h>

#include "channelinterfacemanager.h"
#include <common/widgets/nameGroupBox/namegroupbox.h>

#include <QLineEdit>
#include <QPlainTextEdit>
#include <QSharedPointer>

class Component;
class Channel;

/*! \brief ChannelEditor is widget for editing channel-element within component.
 *
 */
class ChannelEditor : public ItemEditor {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 * \param dataPointer Pointer to the channel being edited.
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	ChannelEditor(QSharedPointer<Component> component, 
		void* dataPointer, 
		QWidget *parent);
	
	//! \brief The destructor
	virtual ~ChannelEditor();

	/*! \brief Check for the validity of the edited model parameters.
	*
	* \return True if all model parameters are in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Remove the edited channel from the model.
	 *
	 * This function should be used when user wants to remove a channel from 
	 * the component.
	 *
	*/
	virtual void removeModel();

	/*! \brief Make the changes from the widgets editors to the IPXact model.
	 *
	*/
	virtual void makeChanges();

private:

	//! \brief No copying
	ChannelEditor(const ChannelEditor& other);

	//! No assignment
	ChannelEditor& operator=(const ChannelEditor& other);

	//! \brief Pointer to the component being edited.
	QSharedPointer<Component> component_;

	//! \brief Pointer to the channel being edited.
	Channel* channel_;

	//! \brief Contains the name, displayName and description.
	NameGroupBox nameGroup_;

	//! \brief Editor to set the interfaces belonging to this channel.
	ChannelInterfaceManager interfaceEdit_;
};

#endif // CHANNELEDITOR_H
