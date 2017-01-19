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
	 *      @param [in] basePath    Path to start the relative paths from.
	 *      @param [in] items       QStringList which contains the items to add.
     *      @param [in] parent      The owner of the model.
	 */
	DirListManagerModel(QString const& basePath, QStringList const& items = QStringList(), QObject* parent = 0);
	
	//! The destructor.
	virtual ~DirListManagerModel();

	/*!
     *  Get the data stored for the specified item.
	 *
	 *      @param [in] index   ModelIndex of the wanted item.
	 *      @param [in] role    Specifies what kind of data is requested.
	 */
	virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

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

	//! The path to start relative paths from.
	QString basePath_;
};

#endif // DIRLISTMANAGERMODEL_H
