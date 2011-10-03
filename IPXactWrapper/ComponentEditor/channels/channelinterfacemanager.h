/* 
 *
 *  Created on: 4.4.2011
 *      Author: Antti Kamppi
 * 		filename: channelinterfacemanager.h
 */

#ifndef CHANNELINTERFACEMANAGER_H
#define CHANNELINTERFACEMANAGER_H

#include <common/widgets/listManager/listmanager.h>

#include <QSharedPointer>

class Component;
class Channel;

/*! \brief Widget to select bus interfaces from a component and add them to list.
 *
 */
class ChannelInterfaceManager : public ListManager {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited
	 * \param dataPointer Pointer to the channel being edited.
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	ChannelInterfaceManager(QSharedPointer<Component> component, 
		void* dataPointer, 
		QWidget *parent);
	
	//! \brief The destructor
	virtual ~ChannelInterfaceManager();

	/*! \brief Check if the editor is in valid state or not.
	 *
	 * \return True if the editor is in valid state.
	*/
	bool isValid() const;

public slots:

	//! \brief Called when user clicks "Add" button.
	virtual void onAdd();

	//! \brief Called when user clicks "Edit" button.
	virtual void onEdit();

private:

	//! \brief No copying
	ChannelInterfaceManager(const ChannelInterfaceManager& other);

	//! No assignment
	ChannelInterfaceManager& operator=(const ChannelInterfaceManager& other);
	
	//! \brief Pointer to the channel being edited.
	Channel* channel_;

	//! \brief Pointer to the component being edited.
	QSharedPointer<Component> component_;
};

#endif // CHANNELINTERFACEMANAGER_H
