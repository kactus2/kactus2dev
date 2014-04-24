/* 
 *  	Created on: 7.2.2012
 *      Author: Antti Kamppi
 * 		filename: filesetrefmodel.h
 *		Project: Kactus 2
 */

#ifndef FILESETREFMODEL_H
#define FILESETREFMODEL_H

#include <common/widgets/listManager/listmanagermodel.h>
#include <models/component.h>

#include <QSharedPointer>

/*! \brief FileSetRefModel is a model class to manage a list of file set references.
 *
 * This class can be used as a model class for a list view to implement a 
 * widget to manage a list of file set references.
 * 
 * The model uses same functions as it's base class ListManagerModel except it
 * reimplements the data() function to provide better error reporting of invalid
 * file set references.
 */
class FileSetRefModel : public ListManagerModel {
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * \param parent Pointer to the owner of this model.
	 * \param component Pointer to the component being edited.
	 * \param items QStringList that contains the items to add.
	 *
	*/
	FileSetRefModel(QObject *parent,
		QSharedPointer<Component> component,
		const QStringList& items = QStringList());
	
	//! \brief The destructor
	virtual ~FileSetRefModel();

	/*! \brief Get the data stored for the specified item.
	*
	* \param index ModelIndex of the wanted item.
	* \param role Specifies what kind of data is requested.
	*/
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

private:
	//! \brief No copying
	FileSetRefModel(const FileSetRefModel& other);

	//! \brief No assignment
	FileSetRefModel& operator=(const FileSetRefModel& other);

	//! \brief Pointer to the component being edited.
	QSharedPointer<Component> component_;
};

#endif // FILESETREFMODEL_H
