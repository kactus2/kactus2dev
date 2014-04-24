/* 
 *	Created on:	26.4.2013
 *	Author:		Antti Kamppi
 *	File name:	dirlistmanagermodel.h
 *	Project:		Kactus 2
*/

#ifndef DIRLISTMANAGERMODEL_H
#define DIRLISTMANAGERMODEL_H

#include <common/widgets/listManager/listmanagermodel.h>
#include <IPXACTmodels/component.h>

#include <QSharedPointer>

class LibraryInterface;

/*! \brief The model which reimplements the validity check of dir paths.
 *
 */
class DirListManagerModel : public ListManagerModel {
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * Method: 		DirListManagerModel
	 * Full name:	DirListManagerModel::DirListManagerModel
	 * Access:		private 
	 * 
	 * \param handler Pointer to the instance that manages the library.
	 * \param Pointer to the component which is being edited.
	 * \param parent Pointer to the owner of the model.
	 * \param items QStringList which contains the items to add.
	 *
	*/
	DirListManagerModel(LibraryInterface* handler, 
		QSharedPointer<Component> component,
		QObject *parent = 0, 
		const QStringList& items = QStringList());
	
	//! \brief The destructor.
	virtual ~DirListManagerModel();

	/*! \brief Get the data stored for the specified item.
	*
	* \param index ModelIndex of the wanted item.
	* \param role Specifies what kind of data is requested.
	*/
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

private:
	
	//! \brief No copying
	DirListManagerModel(const DirListManagerModel& other);

	//! \brief No assignment
	DirListManagerModel& operator=(const DirListManagerModel& other);

	//! \brief Pointer to the instance which manages the library.
	LibraryInterface* handler_;

	//! \brief Pointer to the component being edited.
	QSharedPointer<Component> component_;
};

#endif // DIRLISTMANAGERMODEL_H
