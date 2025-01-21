//-----------------------------------------------------------------------------
// File: filesmodel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 1.6.2012
//
// Description:
// The model that contains the files to edit in files summary editor.
//-----------------------------------------------------------------------------

#ifndef FILESMODEL_H
#define FILESMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QSharedPointer>

class LibraryInterface;
class File;
class FileSet;
class Component;
class FileInterface;

//-----------------------------------------------------------------------------
//! The model that contains the files to edit in files summary editor.
//-----------------------------------------------------------------------------
class FilesModel : public QAbstractTableModel
{
	Q_OBJECT

public:


	/*! The constructor.
	 * 
	 *    @param [in] handler         The instance that manages the library.
	 *    @param [in] component       The component being edited.
     *    @param [in] fileInterface   Interface for accessing files.
	 *    @param [in] parent          The owner of this model.
	 *
	 */
    FilesModel(LibraryInterface* handler,
        QSharedPointer<Component> component,
        FileInterface* fileInterface,
        QObject* parent);

	/*!
     *  The destructor.
     */
	~FilesModel() = default;

	/*!
     *  Get the number of rows an item contains.
	 *
	 *    @param [in] parent Identifies the parent that's row count is requested.
	 *
	 *    @return Number of rows the item has.
	 */
	virtual int rowCount(QModelIndex const& parent = QModelIndex()) const;

	/*!
     *  Get the number of columns the item has to be displayed.
	 *
	 *    @param [in] parent Identifies the parent that's column count is requested.
	 *
	 *    @return The number of columns to be displayed.
	 */
	virtual int columnCount(QModelIndex const& parent = QModelIndex()) const;

	/*!
     *  Get the item flags that defines the possible operations for the item.
	 *
	 *    @param [in] index Model index that identifies the item.
	 *
	 *    @return Flags specifying the possible operations for the item.
	 */
	Qt::ItemFlags flags(QModelIndex const& index) const;

	/*!
     *  Get the header data for specified header.
	 *
	 *    @param [in] section     The section specifies the row/column number for the header.
	 *    @param [in] orientation Specified if horizontal or vertical header is wanted.
	 *    @param [in] role        Specifies the type of the requested data.
	 *
	 *    @return The requested data.
	 */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	/*!
     *  Get the data for specified item.
	 *
	 *    @param [in] index    Specifies the item that's data is requested.
	 *    @param [in] role     The role that defines what kind of data is requested.
	 *
	 *    @return  The data for the item.
	 */
	virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

	/*!
     *  Save the data to the model for specified item
	 *
	 *    @param [in] index   The model index of the item that's data is to be saved.
	 *    @param [in] value   The data that is to be saved.
	 *    @param [in] role    The role specifies what kind of data should be saved.
	 *
	 *    @return True if save was successful, otherwise false.
	 */
	bool setData(QModelIndex const& index, const QVariant& value, int role = Qt::EditRole);
    
    /*!
     *  Returns a list of supported MIME data types.
     *
     *    @return The supported MIME types.
     */
    virtual QStringList mimeTypes() const;
   
    /*!
     *  Returns the supported actions of a drop.
     *
     *    @return The drop actions supported by the model.
     */
    virtual Qt::DropActions supportedDropActions() const;

    /*!
     *  Handler for the dropped MIME data.
     *
     *    @param [in] data    The data associated to the drop.
     *    @param [in] action  The drop action.  
     *    @param [in] row     The row beneath the drop position.
     *    @param [in] column  The column beneath the drop position.
     *    @param [in] parent  The parent index of the drop position.
     *
     *    @return True, if the model could handle the data, otherwise false.
     */
    virtual bool dropMimeData(QMimeData const* data, Qt::DropAction action, int row, int column, 
        QModelIndex const& parent);

public slots:

	/*!
     *  Add a new item to the given index.
	 *
	 *    @param [in] index       The index identifying the position for new file.
	 */
    virtual void onAddItem(QModelIndex const& index);

	/*!
     *  Add a new item to the given index.
	 *
	 *    @param [in] index       The index identifying the position for new file.
	 *    @param [in] filePath    The file path to the file to be added.
	 */
	virtual void onAddItem(QModelIndex const& index, const QString& filePath);

	/*!
     *  Remove the item in the given index.
	 *
	 *    @param [in] index The index identifying the item to remove.
	 */
	virtual void onRemoveItem(QModelIndex const& index);

	/*!
     *  Move item from one position to another.
	 *
	 *    @param [in] originalPos The index identifying which item to move.
	 *    @param [in] newPos The position to move the item to.
	 */
	virtual void onMoveItem(QModelIndex const& originalPos, QModelIndex const& newPos);

signals:

	//! Emitted when the model changes.
	void contentChanged();

	//! Emitted when a file is added to the list.
	void fileAdded(int index);

	//! Emitted when a file is removed from the list.
	void fileRemoved(int index);

	//! Emitted when a file is moved from one position to another.
	void fileMoved(int source, int target);

    /*!
     *  Informs of a file name change.
     *
     *    @param [in] oldName     Old name of the file.
     *    @param [in] newName     New name of the file.
     */
    void fileRenamed(std::string const& oldName, std::string const& newName);

private:
	//! No copying
	FilesModel(const FilesModel& other);
	FilesModel& operator=(const FilesModel& other);

    /*!
     *  Get the file types of the selected file as a combined string.
     *
     *    @param [in] fileName    Name of the selected file.
     *
     *    @return File types of the selected file as a combined string.
     */
    QString getFileTypesString(std::string const& fileName) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The instance that manages the library.
	LibraryInterface* handler_;

	//! The component being edited.
	QSharedPointer<Component> component_;

    //! Interface for accessing files.
    FileInterface* fileInterface_;
};

#endif // FILESMODEL_H
