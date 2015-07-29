//-----------------------------------------------------------------------------
// File: filesetsmodel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 25.5.2012
//
// Description:
// The model class to manage the objects for FileSetsEditor.
//-----------------------------------------------------------------------------

#ifndef FILESETSMODEL_H
#define FILESETSMODEL_H

#include <IPXACTmodels/fileset.h>
#include <IPXACTmodels/component.h>

#include <QAbstractTableModel>
#include <QList>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! The model class to manage the objects for FileSetsEditor.
//-----------------------------------------------------------------------------
class FileSetsModel : public QAbstractTableModel
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] component   The component being edited.
	 *      @param [in] parent      The owner of this model.
	*/
	FileSetsModel(QSharedPointer<Component> component, QObject *parent);
	
	//! The destructor
	~FileSetsModel();

    //! Refreshes the model.
    void refresh();

	/*! Get the number of rows an item contains.
	 *
	 *      @param [in] parent  Identifies the parent that's row count is requested.
	 *
	 *      @return Number of rows the item has.
	*/
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/*! Get the number of columns the item has to be displayed.
	 *
	 *      @param [in] parent  Identifies the parent that's column count is requested.
	 *
	 *      @return The number of columns to be displayed.
	*/
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

	/*! Get the data for specified item.
	 *
	 *      @param [in] index   Specifies the item that's data is requested.
	 *      @param [in] role    The role that defines what kind of data is requested.
	 *
	 *      @return QVariant Contains the data for the item.
	*/
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	/*! Get the header data for specified header.
	 *
	 *      @param [in] section         The section specifies the row/column number for the header.
	 *      @param [in] orientation     Specified if horizontal or vertical header is wanted.
	 *      @param [in] role            Specifies the type of the requested data.
	 *
	 *      @return QVariant Contains the requested data.
	*/
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	/*! Save the data to the model for specified item
	 *
	 *      @param [in] index   The model index of the item that's data is to be saved.
	 *      @param [in] value   The data that is to be saved.
	 *      @param [in] role    The role specifies what kind of data should be saved.
	 *
	 *      @return True if saving happened successfully.
	*/
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

	/*! Get the item flags that defines the possible operations for the item.
	 *
	 *      @param [in] index   Model index that identifies the item.
	 *
	 *      @return Flags specify the possible operations for the item.
	*/
	Qt::ItemFlags flags(const QModelIndex& index) const;

	/*! Check the validity of the items in the model.
	 *
	 *      @return True if all items are in valid state.
	*/
	virtual bool isValid() const;

public slots:

	/*! Add a new item to the given index.
	 *
	 *      @param [in] index   The index identifying the position for new item.
	 *
	*/
	virtual void onAddItem(const QModelIndex& index);

	/*! Remove the item in the given index.
	 *
	 *      @param [in] index   The index identifying the item to remove.
	 *
	*/
	virtual void onRemoveItem(const QModelIndex& index);

    /*!
     *  Updates the model when a file set has been added from outside the model.
     *
     *      @param [in] fileSet     The file set that was added.
     */
    void onFileSetAdded(FileSet* fileSet);

signals:

	//! Emitted when the contents of the model change.
	void contentChanged();

	/*! Emitted when a new file set is added to the model.
	 *
	 *      @param [in] index   The index of the added file set.
	 *
	*/
	void fileSetAdded(int index);

	/*! Emitted when a file set is removed from the model.
	 *
	 *      @param [in] index   The index of the removed file set.
	 *
	*/
	void fileSetRemoved(int index);

private:
	//! No copying
	FileSetsModel(const FileSetsModel& other);
	FileSetsModel& operator=(const FileSetsModel& other);

	//! The component being edited.
	QSharedPointer<Component> component_;

	//! The file sets to edit.
	QList<QSharedPointer<FileSet> >& fileSets_;
};

#endif // FILESETSMODEL_H
