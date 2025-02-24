//-----------------------------------------------------------------------------
// File: filesetrefmodel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 07.02.2012
//
// Description:
// Model class to manage a list of file set references.
//-----------------------------------------------------------------------------

#ifndef FILESETREFMODEL_H
#define FILESETREFMODEL_H

#include <common/widgets/listManager/listmanagermodel.h>

#include <QSharedPointer>

class FileSetInterface;
class Component;

//-----------------------------------------------------------------------------
//! Model class to manage a list of file set references.
//-----------------------------------------------------------------------------
class FileSetRefModel : public ListManagerModel
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *    @param [in] parent              Pointer to the owner of this model.
	 *    @param [in] fileSetInterface    Interface for accessing file sets.
	 *    @param [in] items               QStringList that contains the items to add.
	 */
    FileSetRefModel(QObject *parent, FileSetInterface* fileSetInterface, const QStringList& items = QStringList());

	/*!
     *  The destructor.
     */
	virtual ~FileSetRefModel();

	/*!
     *  Get the data stored for the specified item.
	 *
	 *    @param [in] index   ModelIndex of the wanted item.
	 *    @param [in] role    Specifies what kind of data is requested.
	 */
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

private:

    //! No copying. //! No assignment.
	FileSetRefModel(const FileSetRefModel& other);
	FileSetRefModel& operator=(const FileSetRefModel& other);

    //! Interface for accessing file sets.
    FileSetInterface* fileSetInterface_;
};

#endif // FILESETREFMODEL_H
