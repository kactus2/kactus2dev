//-----------------------------------------------------------------------------
// File: CatalogFileModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 02.02.2017
//
// Description:
// Model class for ipxactFiles in a catalog.
//-----------------------------------------------------------------------------

#ifndef CATALOGFILEMODEL_H
#define CATALOGFILEMODEL_H

#include <IPXACTmodels/common/VLNV.h>

#include <QModelIndex>
#include <QIcon>
#include <QAbstractItemModel>
#include <QSharedPointer>

class LibraryInterface;

class Catalog;
class IpxactFile;

//-----------------------------------------------------------------------------
//! Model class for ipxactFiles in a catalog.
//-----------------------------------------------------------------------------
class CatalogFileModel : public QAbstractItemModel
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] library     The library of available IP-XACT documents.
     *      @param [in] parent      The parent object.
     */
    CatalogFileModel(LibraryInterface* library, QObject *parent);

    /*!
     *  The destructor.
     */
    virtual ~CatalogFileModel();

    /*!
     *  Sets the protection against changes.
     *
     *      @param [in] locked   Lock against changes or not.
     */
    void setProtection(bool locked);

    /*!
     *  Refreshes the model content from the given catalog.
     *
     *      @param [in] catalog   The catalog whose files to display.
     */
    void refresh(QSharedPointer<Catalog> catalog);

    /*!
     *  Returns the number of columns in this model.
     *
     *      @param [in] parent  Model index identifying the item whose column count is requested.
     *
     *      @return The number of columns.
     */
    virtual int columnCount(QModelIndex const&  parent = QModelIndex() ) const;

    /*! 
     *  Returns the number of rows an item has.
     *
     *      @param [in] parent  Model index identifying the item whose row count is wanted.
     *
     *      @return The number of rows the item has.
     */
    virtual int rowCount(QModelIndex const& parent = QModelIndex()) const;

    /*!
     *  Get the model index of the specified object.
     *
     *      @param [in] row     Row number of the object.
     *      @param [in] column  Column number of the object.
     *      @param [in] parent  Model index of the parent of the object.
     *
     *      @return QModelIndex that identifies the object.
     */
    virtual QModelIndex index(int row, int column, QModelIndex const& parent = QModelIndex()) const;

    /*!
     *  Get the model index of the parent of the object
     *
     *      @param [in] child   Model index that identifies the child of the object.
     *
     *      @return QModelIndex that identifies the parent of the given object.
     */
    virtual QModelIndex parent(QModelIndex const& child) const;
        
    /*!
     *  Return header data for the given header column.
     *
     *      @param [in] section      The index of the header column.
	 *      @param [in] orientation  The orientation of the header, only Qt::Horizontal supported.
     *      @param [in] role         The role of the data.
     *
     *      @return QVariant containing the data.
     */
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    /*!
     *  Get the data associated with given object.
     *
     *      @param [in] index Model index that identifies the object that's data is wanted.
     *      @param [in] role Specifies the type of data wanted.
     *
     *      @return QVariant Containing the requested data.
     */
    virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

	/*!
     *  Set the data for specified item.
	 *
	 *      @param [in]  index  Specifies the item that's data is modified
	 *      @param [in]  value  The value to be set.
	 *      @param [in]  role   The role that is trying to modify the data. Only Qt::EditRole is supported.
	 *
	 *      @return  True, if data was successfully set, otherwise false.
	 */
    virtual bool setData(QModelIndex const& index, const QVariant &value, int role /* = Qt::EditRole */);

    /*!
     *  Get the flags that identify possible methods for given object.
     *
     *      @param [in] index   Model index identifying the object that's flags are requested.
     *
     *      @return Qt::ItemFlags that specify how the object can be handled.
     */
    Qt::ItemFlags flags(QModelIndex const& index) const;

    /*!
     *  Get the list of acceptable mime types.
     *
     *      @return The list of acceptable mime types.
     */
    QStringList mimeTypes() const;

    /*!
     *  Get the supported drop actions.
     *
     *      @return The supported drop actions.
     */
    virtual Qt::DropActions supportedDropActions() const;

    /*!
     *  Drop the mime data.
     *
     *      @param [in] data    The actual data.
     *      @param [in] action  The used drop action.
     *      @param [in] row     The selected row.
     *      @param [in] column  The selected column.
     *      @param [in] parent  Parent index of the new data.
     */
    virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
        QModelIndex const& parent);

signals:

    //! Emitted when the contents have changed.
    void contentChanged();

    //! Emitted when a catalog should be opened.
    void openCatalog(VLNV const& vlnv);

    //! Emitted when a component should be opened.
    void openComponent(VLNV const& vlnv);

    //! Emitted when a bus should be opened.
    void openBus(VLNV const& busVLNV);

    //! Emitted when a bus should be opened.
    void openAbsDef(VLNV const& absDefVLNV);

    /*!
     *  Emitted when an IP-XACT file using different std revision than the catalog is dropped in the editor.
     */
    void stdRevisionMismatch() const;

public slots:

    //! Called when an item should be added in the given index.
    void onAddItem(QModelIndex const& index);

    //! Called when an item should be removed from the given index.
    void onRemoveItem(QModelIndex const& index);

    //! Called when an item should be opened in the given index.
    void onOpenItem(QModelIndex const& index);

private:

    // Disable copying.
    CatalogFileModel(CatalogFileModel const& rhs);
    CatalogFileModel& operator=(CatalogFileModel const& rhs);

    /*!
     *  Adds the given file to correct category in the model.
     *
     *      @param [in] fileToAdd   The file to add.
     */
    void addFile(QSharedPointer<IpxactFile> fileToAdd);

    /*!
     *  Checks if the given VLNV is already contained in the catalog.
     *
     *      @param [in] vlnv   The VLNV to find.
     *
     *      @return True, if the VLNV is already in the catalog, otherwise false.
     */
    bool contains(VLNV const& vlnv) const;

    /*!
     *  Checks if the given path is valid.
     *
     *      @param [in] path   The path to check.
     *
     *      @return True, if the path is valid, otherwise false.
     */
    bool isValidPath(QString const& path) const;


    //-----------------------------------------------------------------------------
    // Data.
    //----------------------------------------------------------------------------- 

    //! The catalog whose files to model.
    QSharedPointer<Catalog> catalog_;

    //! The instance that manages the library.
    LibraryInterface* library_;

    //! All the files in the model.
    QVector<QSharedPointer<QList<QSharedPointer<IpxactFile> > > > topLevelRows_;

    //! Lock for editing.
    bool locked_;
};

//-----------------------------------------------------------------------------

#endif // CATALOGFILEMODEL_H
