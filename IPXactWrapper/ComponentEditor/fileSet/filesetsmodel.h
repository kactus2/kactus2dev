/* 
 *  	Created on: 25.5.2012
 *      Author: Antti Kamppi
 * 		filename: filesetsmodel.h
 *		Project: Kactus 2
 */

#ifndef FILESETSMODEL_H
#define FILESETSMODEL_H

#include <models/fileset.h>
#include <models/component.h>

#include <QAbstractTableModel>
#include <QList>
#include <QSharedPointer>

/*! \brief The model class to manage the objects for FileSetsEditor.
 *
 */
class FileSetsModel : public QAbstractTableModel {
	Q_OBJECT

public:

	//! \brief The number of columns the model contains.
	static const int COLUMN_COUNT = 3;

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the owner of this model.
	 *
	*/
	FileSetsModel(QSharedPointer<Component> component, 
		QObject *parent);
	
	//! \brief The destructor
	~FileSetsModel();

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

	/*! \brief Get the data for specified item.
	 *
	 * \param index Specifies the item that's data is requested.
	 * \param role The role that defines what kind of data is requested.
	 *
	 * \return QVariant Contains the data for the item.
	*/
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

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

	/*! \brief Get the item flags that defines the possible operations for the item.
	 *
	 * \param index Model index that identifies the item.
	 *
	 * \return Qt::ItemFlags specify the possible operations for the item.
	*/
	Qt::ItemFlags flags(const QModelIndex& index) const;

	/*! \brief Check the validity of the items in the model.
	 *
	 * \return True if all items are in valid state.
	*/
	virtual bool isValid() const;

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

signals:

	//! \brief Emitted when the contents of the model change.
	void contentChanged();

	/*! \brief Emitted when a new file set is added to the model.
	 *
	 * \param index The index of the added file set.
	 *
	*/
	void fileSetAdded(int index);

	/*! \brief Emitted when a file set is removed from the model.
	 *
	 * \param index The index of the remove file set.
	 *
	*/
	void fileSetRemoved(int index);

private:
	//! \brief No copying
	FileSetsModel(const FileSetsModel& other);

	//! \brief No assignment
	FileSetsModel& operator=(const FileSetsModel& other);

	//! \brief Pointer to the component being edited.
	QSharedPointer<Component> component_;

	//! \brief The file sets to edit.
	QList<QSharedPointer<FileSet> >& fileSets_;
};

#endif // FILESETSMODEL_H
