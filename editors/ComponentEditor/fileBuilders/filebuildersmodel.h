//-----------------------------------------------------------------------------
// File: filebuildersmodel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 31.05.2012
//
// Description:
// Model that contains the items to edit file builders.
//-----------------------------------------------------------------------------

#ifndef FILEBUILDERSMODEL_H
#define FILEBUILDERSMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QSharedPointer>
#include <QObject>

class FileBuilder;

//-----------------------------------------------------------------------------
//! Model that contains the items to edit file builders.
//-----------------------------------------------------------------------------
class FileBuildersModel : public QAbstractTableModel
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] fileBuilders    Contains the file builders to edit.
	 *      @param [in] parent          Pointer to the owner of this model.
	 */
	FileBuildersModel(QSharedPointer<QList<QSharedPointer<FileBuilder> > > fileBuilders, QObject* parent);

	//! The destructor.
	virtual ~FileBuildersModel();

	/*!
     *  Get the number of rows in the model.
	 *
	 *      @param [in] parent  Model index of the parent of the item.
     *                          Must be invalid because this is not hierarchical model.
	 *
	 *      @return Number of rows currently in the model.
	 */
	virtual int rowCount(const QModelIndex& parent = QModelIndex() ) const;

	/*!
     *  Get the number of columns in the model.
	 *
	 *      @param [in] parent  Model index of the parent of the item.
     *                          Must be invalid because this is not hierarchical model.
	 *
	 *      @return Always returns 4
	 */
	virtual int columnCount(const QModelIndex& parent = QModelIndex() ) const;

	/*!
     *  Get the data for the specified item for specified role.
	 *
	 *      @param [in] index   Identifies the item that's data is wanted.
	 *      @param [in] role    Specifies what kind of data is wanted.
	 *
	 *      @return QVariant containing the data
	 */
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole ) const;

	/*!
     *  Get the data for the headers.
	 *
	 *      @param [in] section         The column that's header is wanted.
	 *      @param [in] orientation     Only Qt::Horizontal is supported.
	 *      @param [in] role            Specified the type of data that is wanted.
	 *
	 *      @return QVariant containing the data to be displayed
	 */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

	/*!
     *  Set the data for specified item.
	 *
	 *      @param [in] index   Specifies the item that's data is modified
	 *      @param [in] value   The value to be set.
	 *      @param [in] role    The role that is trying to modify the data. Only Qt::EditRole is supported.
	 *
	 *      @return True if data was successfully set.
	 */
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole );

	/*!
     *  Get information on how specified item can be handled.
	 *
	 *      @param [in] index   Specifies the item that's flags are wanted.
	 *
	 *      @return Qt::ItemFlags that define how object can be handled.
	 */
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;

	/*!
     *  Check if the model is in valid state or not.
	 *
	 *      @return True if all items in model are valid.
	 */
	bool isValid() const;

public slots:

	/*!
     *  A new item should be added to given index.
	 *
	 *      @param [in] index   The position where new item should be added at.
	 */
	void onAddItem(const QModelIndex& index);

	/*!
     *  An item should be removed from the model.
	 * 
	 *      @param [in] index   Identifies the item that should be removed.
	 */
	void onRemoveItem(const QModelIndex& index);

signals:

	//! Emitted when contents of the model change.
	void contentChanged();

	//! Prints an error message to the user.
	void errorMessage(const QString& msg) const;
	
	//! Prints a notification to user.
	void noticeMessage(const QString& msg) const;

private:

	//! No copying. No assignment.
	FileBuildersModel(const FileBuildersModel& other);
	FileBuildersModel& operator=(const FileBuildersModel& other);
	
	//! Contains the file builders to edit.
    QSharedPointer<QList<QSharedPointer<FileBuilder> > > fileBuilders_;
};

#endif // FILEBUILDERSMODEL_H
