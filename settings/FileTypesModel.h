//-----------------------------------------------------------------------------
// File: FileTypesModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 20.02.2013
//
// Description:
// Model for file types data.
//-----------------------------------------------------------------------------

#ifndef FILETYPESMODEL_H
#define FILETYPESMODEL_H

#include <QAbstractTableModel>
#include <QMap>
#include <QList>
#include <QString>
#include <QSettings>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! File types column enumeration.
//-----------------------------------------------------------------------------
enum FileTypesColumn
{
	FILE_TYPES_COL_NAME,
	FILE_TYPES_COL_EDIT_IN_KACTUS,
    FILE_TYPES_COL_EXTENSIONS,
	FILE_TYPES_COL_EXECUTABLE,
    FILE_TYPES_COL_COUNT
};


//-----------------------------------------------------------------------------
//! Data model for the address editor.
//-----------------------------------------------------------------------------
class FileTypesModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    /*!
     *  Constructor which loads the model from settings.
     *
     *      @param [in] settings  The settings store.
     *      @param [in] parent    The owner of this model.
     *
     */
    FileTypesModel(QSettings& settings, QObject* parent);
    
    /*!
     *  The destructor.
     */
    ~FileTypesModel();

    /*!
     *  Saves the model to settings.
     *
     *      @param [in,out] settings  The settings store.
     */
    void apply(QSettings& settings);

    /*!
     *  Returns the number of rows to be displayed.
     *
     *      @param [in] parent Identifies the parent. Must always be a invalid index.
     */
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

    /*!
     *  Returns the number of columns to display.
     *
     *      @param [in] parent Identifies the parent. Must always be a invalid index.
     */
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

    /*!
     *  Returns that data for specified index in given data role.
     *  
     *  Qt::UserRole returns QStringList containing the file types that are already defined.
     *
     *      @param [in] index  Identifies the object that's data is wanted.
     *      @param [in] role   Identifies the type of data being requested.
     *
     *      @return QVariant containing the requested data.
     */
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    /*!
     *  Returns the data for given header in given data role.
     *
     *      @param [in] section      Identifies the header section.
     *      @param [in] orientation  Only Qt::Horizontal is supported.
     *      @param [in] role         Identifies the type of the data being requested.
     *
     *      @return QVariant contains the requested data.
     */
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    /*!
     *  Sets the data for specified index.
     *
     *      @param [in] index  Identifies the object that's data is to be saved.
     *      @param [in] value  Contains the data to be saved.
     *      @param [in] role   Identifies the type of the data to be saved.
     *
     *      @return True if data was saved successfully.
     */
    virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole );

    /*!
     *  Retrieves info on what operations are possible for specified item.
     *
     *      @param [in] index Identifies the item that's operations are requested.
     *
     *      @return Item flags containing info on which operations are available for the given index.
     */
    virtual Qt::ItemFlags flags(const QModelIndex& index) const;

public slots:
	/*!
     *  Adds a new empty row to the model.
	 */
	void onAddRow();

    /*!
     *  Removes a row from the model.
	 *
	 *      @param [in] row Specifies the row to remove.
	 */
	void onRemoveRow(int row);

	/*!
     *  Called when a new item should be added to given index.
	 *
	 *      @param [in] index The position where new item should be added at.
	 */
	void onAddItem(const QModelIndex& index);

	/*! 
     *  An item should be removed from the model.
	 * 
	 *      @param [in] index Identifies the item that should be removed.
	 */
	void onRemoveItem(const QModelIndex& index);

private:
    // Disable copying.
    FileTypesModel(FileTypesModel const& rhs);
    FileTypesModel& operator=(FileTypesModel const& rhs);
    
    //-----------------------------------------------------------------------------
    //! FileTypeEntry structure.
    //-----------------------------------------------------------------------------
    struct FileTypeEntry
    {
		QString name;       //!< The file type name.
		bool editInKactus;	//!< True, if edited in Kactus rather than some other application.
        QString extensions; //!< The extensions for this file type.
        QString executable; //!< The executable to run this file type.

        /*!
         *  Default constructor.
         */
        FileTypeEntry() : name(), editInKactus(false), extensions(), executable()
        {
        }
    };

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The file type entries.
    QList<FileTypeEntry> entries_;
};

//-----------------------------------------------------------------------------

#endif // FILETYPESMODEL_H
