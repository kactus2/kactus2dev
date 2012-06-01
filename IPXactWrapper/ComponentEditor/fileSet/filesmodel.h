/* 
 *  	Created on: 1.6.2012
 *      Author: Antti Kamppi
 * 		filename: filesmodel.h
 *		Project: Kactus 2
 */

#ifndef FILESMODEL_H
#define FILESMODEL_H

#include <models/file.h>

#include <QAbstractTableModel>
#include <QList>
#include <QSharedPointer>

/*! \brief The model that contains the files to edit in files summary editor.
 *
 */
class FilesModel : public QAbstractTableModel {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param files Contains the files to edit.
	 * \param parent Pointer to the owner of this model.
	 *
	*/
	FilesModel(QList<QSharedPointer<File> >& files,
		QObject *parent);
	
	//! \brief The destructor
	~FilesModel();

	/*! \brief Get the number of rows an item contains.
	 *
	 * \param parent Identifies the parent that's row count is requested.
	 *
	 * \return Number of rows the item has.
	*/
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Get the number of columns the item has to be displayed.
	 *
	 * \param parent Identifies the parent that's column count is requested.
	 *
	 * \return The number of columns to be displayed.
	*/
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Get the item flags that defines the possible operations for the item.
	*
	* \param index Model index that identifies the item.
	*
	* \return Qt::ItemFlags specify the possible operations for the item.
	*/
	Qt::ItemFlags flags(const QModelIndex& index) const;

	/*! \brief Get the header data for specified header.
	 *
	 * \param section The section specifies the row/column number for the header.
	 * \param orientation Specified if horizontal or vertical header is wanted.
	 * \param role Specifies the type of the requested data.
	 *
	 * \return QVariant Contains the requested data.
	*/
	virtual QVariant headerData(int section, Qt::Orientation orientation, 
		int role = Qt::DisplayRole) const;

	/*! \brief Get the data for specified item.
	*
	* \param index Specifies the item that's data is requested.
	* \param role The role that defines what kind of data is requested.
	*
	* \return QVariant Contains the data for the item.
	*/
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	/*! \brief Save the data to the model for specified item
	 *
	 * \param index The model index of the item that's data is to be saved.
	 * \param value The data that is to be saved.
	 * \param role The role specifies what kind of data should be saved.
	 *
	 * \return True if saving happened successfully.
	*/
	bool setData(const QModelIndex& index, const QVariant& value, 
		int role = Qt::EditRole);

public slots:

	/*! \brief Add a new item to the given index.
	 *
	 * \param index The index identifying the position for new item.
	 *
	*/
	virtual void onAddItem(const QModelIndex& index);

	/*! \brief Remove the item in the given index.
	 *
	 * \param index The index identifying the item to remove.
	 *
	*/
	virtual void onRemoveItem(const QModelIndex& index);

private:
	//! \brief No copying
	FilesModel(const FilesModel& other);

	//! \brief No assignment
	FilesModel& operator=(const FilesModel& other);

	//! \brief Contains the files to display.
	QList<QSharedPointer<File> >& files_;
};

#endif // FILESMODEL_H
