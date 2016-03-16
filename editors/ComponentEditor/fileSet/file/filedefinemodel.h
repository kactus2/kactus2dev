//-----------------------------------------------------------------------------
// File: filedefinemodel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 17.11.2015
//
// Description:
// Model to display the define-elements for a file.
//-----------------------------------------------------------------------------

#ifndef FILEDEFINEMODEL_H
#define FILEDEFINEMODEL_H

#include <QAbstractTableModel>
#include <QString>
#include <QList>
#include <QSharedPointer>

class File;
class NameValuePair;
//-----------------------------------------------------------------------------
//! Model to display the define-elements for a file.
//-----------------------------------------------------------------------------
class FileDefineModel : public QAbstractTableModel
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] file    The file being modified.
     *      @param [in] parent  The owner of this widget.
	*/
	FileDefineModel(QSharedPointer<File> file, QObject* parent);

	//! The destructor
	virtual ~FileDefineModel();

	/*! Get the number of rows in the model.
	 *
	 *      @param [in] parent Model index of the parent of the item. 
	 *
	 *      @return Number of rows currently in the model.
	*/
	virtual int rowCount(QModelIndex const& parent = QModelIndex()) const;

	/*! Get the number of columns in the model
	 *
	 *      @param [in] parent Model index of the parent of the item.
	 *
	 *      @return Number of columns currently in the model.
	*/
	virtual int columnCount(QModelIndex const& parent = QModelIndex()) const;

	/*! Get the data for the specified item for specified role.
	 *
	 *      @param [in] index   Identifies the item that's data is wanted.
	 *      @param [in] role    Specifies what kind of data is wanted
	 *
	 *      @return QVariant containing the data
	*/
	virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

	/*! Get the data for the headers
	 *
	 *      @param [in] section         The column that's header is wanted
	 *      @param [in] orientation     Only Qt::Horizontal is supported
	 *      @param [in] role            Specified the type of data that is wanted.
	 *
	 *      @return The string to display as header.
	*/
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	/*! Get information on how specified item can be handled.
	 *
	 *      @param [in] index Specifies the item that's flags are wanted.
	 *
	 *      @return Qt::ItemFlags that define how object can be handled.
	*/
	virtual Qt::ItemFlags flags(QModelIndex const& index) const;

	/*! Set the data for specified item.
	 *
	 *      @param [in] index   Specifies the item that's data is modified
	 *      @param [in] value   The value to be set.
	 *      @param [in] role    The role that is trying to modify the data. 
	 *
	 *      @return True if data was successfully set.
	*/
	virtual bool setData(QModelIndex const& index, QVariant const& value, int role = Qt::EditRole);

	/*! Insert rows to the model
	 *
	 *      @param [in] row     The row to start inserting rows to.
	 *      @param [in] count   The number of rows to insert.
	 *      @param [in] parent  Model index of the parent to which the rows are inserted.
	 *
	 *      @return True if rows were successfully inserted.
	*/
	bool insertRows(int row, int count, QModelIndex const& parent = QModelIndex());

	/*! Remove rows from the model
	 *
	 *      @param [in] row     The row to start removing rows from.
	 *      @param [in] count   The number of rows to remove
	 *      @param [in] parent  Model index of the parent from which the rows are removed.
	 *
	 *      @return True if rows were successfully removed.
	*/
	bool removeRows(int row, int count, QModelIndex const& parent = QModelIndex());

	/*! Is the model in valid condition.
	 *
	 *      @return True if the items in the model are valid.
	*/
	bool isValid() const;

signals:

	//! Emitted when the contents of the editor change
	void contentChanged();

private:

	//! No copying
	FileDefineModel(const FileDefineModel& other);

	//! No assignment
	FileDefineModel& operator=(const FileDefineModel& other);
	
	//! The defines displayed in the model.
	QSharedPointer<QList<QSharedPointer<NameValuePair> > > defines_;

};

#endif // FILEDEFINEMODEL_H
