/* 
 *
 *  Created on: 20.12.2010
 *      Author: Antti Kamppi
 */

#ifndef LIBRARYTREEMODEL_H
#define LIBRARYTREEMODEL_H

#include "vlnv.h"
#include "libraryitem.h"
#include "librarydata.h"

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QSharedPointer>
#include <QList>

class LibraryInterface;

/*! \brief The model that contains the LibraryItems to display library hierarhically.
 *
 */
class LibraryTreeModel : public QAbstractItemModel {

    Q_OBJECT

public:

    /*! \brief The constructor
     *
     * \param handler Pointer to the instance that manages the library.
     * \param sourceModel Pointer to the data model to use as base model.
     * \param parent Pointer to the owner of this class
     *
    */
    LibraryTreeModel(LibraryInterface* handler,
		LibraryData* sourceModel, 
		QObject *parent = 0);

	//! \brief The destructor
    virtual ~LibraryTreeModel();

    /*! \brief Get the data for the headers of this model.
     *
     * \param section Specifies the column of the header.
	 * \param orientation The orientation of the header, only Qt::Horizontal supported.
     * \param role Specifies the role of the data.
     *
     * \return QVariant containing the data.
    */
    virtual QVariant headerData(int section, Qt::Orientation orientation,
    		int role = Qt::DisplayRole ) const;

    /*! \brief Get the number of columns in this model
     *
     * \param parent Model index identifying the item that's column count is 
     * wanted.
     *
     * \return Number of columns (always 1)
    */
    virtual int columnCount(const QModelIndex & parent = QModelIndex() ) const;

    /*! \brief Get the number of rows an item has.
     *
     * \param parent Model index identifying the item that's row count is wanted
     *
     * \return Number of rows the item has.
    */
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

    /*! \brief Get the model index of the specified object.
     *
     * \param row Row number of the object.
     * \param column Column number of the object.
     * \param parent Model index of the parent of the object.
     *
     * \return QModelIndex that identifies the object.
    */
    virtual QModelIndex index(int row, int column,
    		const QModelIndex &parent = QModelIndex()) const;

    /*! \brief Get the model index of the parent of the object
     *
     * \param child Model index that identifies the child of the object.
     *
     * \return QModelIndex that identifies the parent of the given object.
    */
    virtual QModelIndex parent(const QModelIndex &child) const;

    /*! \brief Get the data associated with given object.
     *
     * \param index Model index that identifies the object that's data is wanted.
     * \param role Specifies the type of data wanted.
     *
     * \return QVariant Containing the requested data.
    */
    virtual QVariant data(const QModelIndex& index,
    		int role = Qt::DisplayRole) const;

    /*! \brief Does the specified item have child items or not.
     *
     * \param parent Model index identifying the object that's children are asked.
     *
     * \return True if object has child objects.
    */
    virtual bool hasChildren(const QModelIndex& parent = QModelIndex()) const;

    /*! \brief Get the flags that identify possible methods for given object.
     *
     * \param index Model index identifying the object that's flags are requested.
     *
     * \return Qt::ItemFlags that specify how the object can be handled.
    */
    Qt::ItemFlags flags(const QModelIndex& index) const;

	/*! \brief Get the root item of the tree.
	 *
	 * \return Pointer to the root item
	*/
	LibraryItem* getRoot() const;

	/*! \brief Create a model index for a library item
	 *
	 * \param item Pointer to the LibraryItem that's model index is wanted
	 *
	 * \return QModelIndex that identifies the libraryItem.
	*/
	QModelIndex index(LibraryItem* item);

signals:

	//! \brief Emit an error message to be printed to user.
	void errorMessage(const QString& message);

	//! \brief Emit a notice message to be printed to user.
	void noticeMessage(const QString& message);

	//! \brief Open the design of a component.
	void openDesign(const VLNV& vlnv);

    //! \brief Open the SW design of a component.
    void openSWDesign(const VLNV& vlnv);

    //! \brief Open the system design of a component.
    void openSystemDesign(const VLNV& vlnv);

	//! \brief Open the component in a component editor.
	void editItem(const VLNV& vlnv);

	//! \brief Create a new bus definition.
	void createBus(const VLNV& vlnv);

	//! \brief Create a new abstraction definition for given bus definition.
	void createAbsDef(const VLNV& vlnv);

	//! \brief Create a new component with given vlnv.
	void createComponent(const VLNV& vlnv);

	//! \brief Create new design with given vlnv.
	void createDesign(const VLNV& vlnv);

    //! \brief Create new SW design with given vlnv.
    void createSWDesign(const VLNV& vlnv);

    //! \brief Create new system design with given vlnv.
    void createSystemDesign(const VLNV& vlnv);

    //! \brief Create a new COM definition.
    void createComDef(const VLNV& vlnv);

    //! \brief Create a new API definition.
    void createApiDef(const VLNV& vlnv);

	//! \brief Remove the specified VLNV from the library
	void removeVLNV(QList<VLNV> vlnvs);

	//! \brief Export a list of vlnv items to a new location.
	void exportItems(const QList<VLNV> list);

    //! Shows errors about the item at the given index.
    void showErrors(VLNV const& vlnv);

	//! \brief Refresh the item filtering because changes have been made
	void invalidateFilter();

	//! \brief Items have changed so th VLNVDialer should refresh itself.
	void refreshDialer();

public slots:

	//! \brief Open the selected hierarchical design
	void onOpenDesign(const QModelIndex& index);

    //! \brief Open the selected hierarchical SW design
    void onOpenSWDesign(const QModelIndex& index);

    //! \brief Open the selected hierarchical system design
    void onOpenSystemDesign(const QModelIndex& index);

	//! \brief When open is selected in search view
	void onOpenComponent(const QModelIndex& index);

	//! \brief Create new component
	void onCreateNewComponent(const QModelIndex& index);

	//! \brief Create new design
	void onCreateNewDesign(const QModelIndex& index);

    //! \brief Create new SW design
    void onCreateNewSWDesign(const QModelIndex& index);

    //! \brief Create new system design
    void onCreateNewSystemDesign(const QModelIndex& index);

	//! \brief When delete is selected in search view
	void onDeleteItem(const QModelIndex& index);

	//! \brief When export is selected in search view
	void onExportItem(const QModelIndex& index);

    //! Shows errors about the item at the given index.
    void onShowErrors(QModelIndex const& index);

	//! \brief Open bus definition in an editor
	void onOpenBus(const QModelIndex& index);

    //! \brief Open COM definition in an editor
    void onOpenComDef(const QModelIndex& index);

    //! \brief Open API definition in an editor
    void onOpenApiDef(const QModelIndex& index);

	//! \brief Create a new bus definition
	void onCreateBus(const QModelIndex& index);

	//! \brief Create a new abstraction definition for given bus definition.
	void onCreateAbsDef(const QModelIndex& index);

    //! \brief Create a new COM definition
    void onCreateComDef(const QModelIndex& index);

    //! \brief Create a new API definition
    void onCreateApiDef(const QModelIndex& index);

	//! \brief Remove the specified vlnv branch from the tree.
	void onRemoveVLNV(const VLNV& vlnv);

	//! \brief Add the specified vlnv branch to the tree.
	void onAddVLNV(const VLNV& vlnv);

	//! \brief Reset the model.
	void onResetModel();

private:

	//! \brief No copying
	LibraryTreeModel(const LibraryTreeModel& other);

	//! \brief No assignment
	LibraryTreeModel& operator=(const LibraryTreeModel& other);

	/*! \brief Removes an item from the treeModel
	 *
	 * \param toRemove Pointer to LibraryItem that is the highest-level item to 
	 * remove.
	 * \param emitSignals Specifies if the RemoveVLNV signal should be emitted
	 * or not.
	 *
	*/
	void removeLibraryItem(LibraryItem* toRemove, bool emitSignals);

	//! \brief Pointer to the data model that holds the library
	LibraryData* dataSource_;

	/*! \brief Pointer to the root of the tree.
	 *
	 * The LibraryTreeFilter owns the root item and the root item owns it's
	 * child items.
	 */
	QSharedPointer<LibraryItem> rootItem_;

	//! \brief Pointer to the instance that manages the library.
	LibraryInterface* handler_;

};

#endif // LIBRARYTREEMODEL_H
