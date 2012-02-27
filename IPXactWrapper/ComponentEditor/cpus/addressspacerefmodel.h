/* 
 *  	Created on: 27.2.2012
 *      Author: Antti Kamppi
 * 		filename: addressspacerefmodel.h
 *		Project: Kactus 2
 */

#ifndef ADDRESSSPACEREFMODEL_H
#define ADDRESSSPACEREFMODEL_H

#include <common/widgets/listManager/listmanagermodel.h>
#include <models/component.h>

#include <QSharedPointer>

/*! \brief Model class to manage a list of address space references.
 *
 */
class AddressSpaceRefModel : public ListManagerModel {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this model.
	 * \param component Pointer to the component being edited.
	 * \param items QStringList that contains items to add.
	 *
	*/
	AddressSpaceRefModel(QObject *parent,
		QSharedPointer<Component> component,
		const QStringList& items = QStringList());
	
	//! \brief The destructor
	virtual ~AddressSpaceRefModel();

	/*! \brief Get the data stored for the specified item.
	*
	* \param index ModelIndex of the wanted item.
	* \param role Specifies what kind of data is requested.
	*/
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

private:
	//! \brief No copying
	AddressSpaceRefModel(const AddressSpaceRefModel& other);

	//! \brief No assignment
	AddressSpaceRefModel& operator=(const AddressSpaceRefModel& other);

	//! \brief Pointer to the component being edited.
	QSharedPointer<Component> component_;
};

#endif // ADDRESSSPACEREFMODEL_H
