/* 
 *  	Created on: 7.2.2012
 *      Author: Antti Kamppi
 * 		filename: channelinterfacemodel.h
 *		Project: Kactus 2
 */

#ifndef CHANNELINTERFACEMODEL_H
#define CHANNELINTERFACEMODEL_H

#include <models/component.h>

#include <common/widgets/listManager/listmanagermodel.h>

#include <QSharedPointer>

/*! \brief ChannelInterfaceModel is a model class to manage a list of interfaces in a channel.
 *
 * This class can be used as a model class for a list view to implement a 
 * widget to manage a list of interface references.
 * 
 * The model uses same functions as it's base class ListManagerModel except it
 * reimplements the data() function to provide better error reporting of invalid
 * interface references.
 */
class ChannelInterfaceModel : public ListManagerModel {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this model.
	 * \param component Pointer to the component being edited.
	 * \param items Contains the items to  add.
	 *
	*/
	ChannelInterfaceModel(QObject *parent,
		QSharedPointer<Component> component,
		const QStringList& items = QStringList());
	
	//! \brief The destructor.
	virtual ~ChannelInterfaceModel();

	/*! \brief Get the data stored for the specified item.
	*
	* \param index ModelIndex of the wanted item.
	* \param role Specifies what kind of data is requested.
	*/
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

private:
	//! \brief No copying
	ChannelInterfaceModel(const ChannelInterfaceModel& other);

	//! \brief No assignment
	ChannelInterfaceModel& operator=(const ChannelInterfaceModel& other);

	//! \brief Pointer to the component being edited.
	QSharedPointer<Component> component_;
};

#endif // CHANNELINTERFACEMODEL_H
