/* 
 *  	Created on: 1.6.2012
 *      Author: Antti Kamppi
 * 		filename: filesmodel.h
 *		Project: Kactus 2
 */

#ifndef FILESMODEL_H
#define FILESMODEL_H

#include <models/file.h>
#include <models/fileset.h>
#include <models/component.h>

#include <QAbstractTableModel>
#include <QList>
#include <QSharedPointer>

class LibraryInterface;

/*! \brief The model that contains the files to edit in files summary editor.
 *
 */
class FilesModel : public QAbstractTableModel {
	Q_OBJECT

public:

	/*! \brief The constructor
	 * 
	 * \param handler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param fileSet Pointer to the file set being edited.
	 * \param parent Pointer to the owner of this model.
	 *
	*/
	FilesModel(LibraryInterface* handler,
		QSharedPointer<Component> component,
		QSharedPointer<FileSet> fileSet,
		QObject* parent);
	
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
	 * \param index The index identifying the position for new file.
	 * \param filePath The file path to the file to be added.
	 *
	*/
	virtual void onAddItem(const QModelIndex& index, const QString& filePath);

	/*! \brief Remove the item in the given index.
	 *
	 * \param index The index identifying the item to remove.
	 *
	*/
	virtual void onRemoveItem(const QModelIndex& index);

	/*! \brief Move item from one position to another.
	 *
	 * \param originalPos The index identifying which item to move.
	 * \param newPos The position to move the item to.
	 *
	*/
	virtual void onMoveItem(const QModelIndex& originalPos, const QModelIndex& newPos);

signals:

	//! \brief Emitted when the model changes.
	void contentChanged();

	//! \brief Emitted when a file is added to the list.
	void fileAdded(int index);

	//! \brief Emitted when a file is removed from the list.
	void fileRemoved(int index);

	//! \brief Emitted when a file is moved from one position to another.
	void fileMoved(int source, int target);

private:
	//! \brief No copying
	FilesModel(const FilesModel& other);

	//! \brief No assignment
	FilesModel& operator=(const FilesModel& other);

	//! \brief Pointer to the instance that manages the library.
	LibraryInterface* handler_;

	//! \brief Pointer to the component being edited.
	QSharedPointer<Component> component_;

	//! \brief Pointer to the file set that contains the files.
	QSharedPointer<FileSet> fileSet_;

	//! \brief Contains the files to display.
	QList<QSharedPointer<File> >& files_;
};

#endif // FILESMODEL_H
