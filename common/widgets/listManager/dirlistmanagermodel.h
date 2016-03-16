//-----------------------------------------------------------------------------
// File: dirlistmanagermodel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 26.04.2013
//
// Description:
// The model which re-implements the validity check of dir paths.
//-----------------------------------------------------------------------------

#ifndef DIRLISTMANAGERMODEL_H
#define DIRLISTMANAGERMODEL_H

#include <common/widgets/listManager/listmanagermodel.h>

#include <QSharedPointer>

class LibraryInterface;
class Component;

//-----------------------------------------------------------------------------
//! The model which re-implements the validity check of dir paths.
//-----------------------------------------------------------------------------
class DirListManagerModel : public ListManagerModel
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] handler     Pointer to the instance that manages the library.
	 *      @param [in] Component   Pointer to the component which is being edited.
	 *      @param [in] parent      Pointer to the owner of the model.
	 *      @param [in] items       QStringList which contains the items to add.
	 */
	DirListManagerModel(LibraryInterface* handler, QSharedPointer<Component> component, QObject *parent = 0,
        const QStringList& items = QStringList());
	
	//! The destructor.
	virtual ~DirListManagerModel();

	/*!
     *  Get the data stored for the specified item.
	 *
	 *      @param [in] index   ModelIndex of the wanted item.
	 *      @param [in] role    Specifies what kind of data is requested.
	 */
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

private:
	
	//! No copying. No assignment.
	DirListManagerModel(const DirListManagerModel& other);
	DirListManagerModel& operator=(const DirListManagerModel& other);

    /*!
     *  Check if the file exists for the given path.
     *
     *      @param [in] relativePath    The path to the file.
     *
     *      @return True, if the path exists, otherwise false.
     */
    bool directoryExistsForPath(QString const& relativePath) const;

	//! Pointer to the instance which manages the library.
	LibraryInterface* handler_;

	//! Pointer to the component being edited.
	QSharedPointer<Component> component_;
};

#endif // DIRLISTMANAGERMODEL_H
