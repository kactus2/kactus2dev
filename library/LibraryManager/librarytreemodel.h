//-----------------------------------------------------------------------------
// File: librarytreemodel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 20.12.2010
//
// Description:
// The model that contains the LibraryItems to display library hierarchically.
//-----------------------------------------------------------------------------

#ifndef LIBRARYTREEMODEL_H
#define LIBRARYTREEMODEL_H

#include "libraryitem.h"
#include "librarydata.h"

#include <IPXACTmodels/common/VLNV.h>

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QSharedPointer>
#include <QList>

class LibraryInterface;

//-----------------------------------------------------------------------------
//! The model that contains the LibraryItems to display library hierarchically.
//-----------------------------------------------------------------------------
class LibraryTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:

	/*! The constructor
	*
	*      @param [in] handler          The instance that manages the library.
	*      @param [in] sourceModel      The data model to use as base model.
	*      @param [in] parent           The owner of this class
	*
	*/
	LibraryTreeModel(LibraryInterface* handler,	LibraryData* sourceModel, QObject* parent = 0);

	//! The destructor
	virtual ~LibraryTreeModel();

	/*! Get the data for the headers of this model.
	*
	*      @param [in] section          Specifies the column of the header.
	*      @param [in] orientation      The orientation of the header, only Qt::Horizontal supported.
	*      @param [in] role             Specifies the role of the data.
	*
	*      @return QVariant containing the data.
	*/
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

	/*! Get the number of columns in this model
	*
	*      @param [in] parent Model index identifying the item that's column count is  wanted.
	*
	*      @return Number of columns.
	*/
	virtual int columnCount(QModelIndex const& parent = QModelIndex()) const;

	/*! Get the number of rows an item has.
	*
	*      @param [in] parent Model index identifying the item that's row count is wanted
	*
	*      @return Number of rows the item has.
	*/
	virtual int rowCount(QModelIndex const& parent = QModelIndex()) const;

	/*! Get the model index of the specified object.
	*
	*      @param [in] row      Row number of the object.
	*      @param [in] column   Column number of the object.
	*      @param [in] parent   Model index of the parent of the object.
	*
	*      @return QModelIndex that identifies the object.
	*/
	virtual QModelIndex index(int row, int column, QModelIndex const& parent = QModelIndex()) const;

	/*! Get the model index of the parent of the object
	*
	*      @param [in] child Model index that identifies the child of the object.
	*
	*      @return QModelIndex that identifies the parent of the given object.
	*/
	virtual QModelIndex parent(QModelIndex const& child) const;

	/*! Get the data associated with given object.
	*
	*      @param [in] index    Model index that identifies the object that's data is wanted.
	*      @param [in] role     Specifies the type of data wanted.
	*
	*      @return QVariant Containing the requested data.
	*/
	virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

	/*! Does the specified item have child items or not.
	*
	*      @param [in] parent Model index identifying the object that's children are asked.
	*
	*      @return True if object has child objects.
	*/
	virtual bool hasChildren(QModelIndex const& parent = QModelIndex()) const;

	/*! Get the flags that identify possible methods for given object.
	*
	*      @param [in] index Model index identifying the object that's flags are requested.
	*
	*      @return Qt::ItemFlags that specify how the object can be handled.
	*/
	Qt::ItemFlags flags(QModelIndex const& index) const;

	/*! Get the root item of the tree.
	*
	*      @return The root item
	*/
	LibraryItem* getRoot() const;

	/*! Create a model index for a library item
	*
	*      @param [in] item The LibraryItem that's model index is wanted
	*
	*      @return QModelIndex that identifies the libraryItem.
	*/
	QModelIndex index(LibraryItem* item);

signals:

	//! Emit an error message to be printed to user.
	void errorMessage(const QString& message);

	//! Emit a notice message to be printed to user.
	void noticeMessage(const QString& message);

	//! Open the design of a component.
	void openDesign(VLNV const& vlnv);

	//! Open the SW design of a component.
	void openSWDesign(VLNV const& vlnv);

	//! Open the system design of a component.
	void openSystemDesign(VLNV const& vlnv);

	//! Open the component in a component editor.
	void editItem(VLNV const& vlnv);

	//! Create a new bus definition.
	void createBus(VLNV const& vlnv);

	//! Create a new abstraction definition for given bus definition.
	void createAbsDef(VLNV const& vlnv);

	//! Create a new component with given vlnv.
	void createComponent(VLNV const& vlnv);

	//! Create new design with given vlnv.
	void createDesign(VLNV const& vlnv);

	//! Create new SW design with given vlnv.
	void createSWDesign(VLNV const& vlnv);

	//! Create new system design with given vlnv.
	void createSystemDesign(VLNV const& vlnv);

	//! Create a new COM definition.
	void createComDef(VLNV const& vlnv);

	//! Create a new API definition.
	void createApiDef(VLNV const& vlnv);

	//! Remove the specified VLNV from the library
	void removeVLNV(QList<VLNV> vlnvs);

	//! Export a list of vlnv items to a new location.
	void exportItems(const QList<VLNV> list);

	//! Shows errors about the item at the given index.
	void showErrors(VLNV const& vlnv);

	//! Refresh the item filtering because changes have been made
	void invalidateFilter();

	//! Items have changed so th VLNVDialer should refresh itself.
	void refreshDialer();

public slots:

	//! Open the selected hierarchical design
	void onOpenDesign(QModelIndex const& index);

	//! Open the selected hierarchical SW design
	void onOpenSWDesign(QModelIndex const& index);

	//! Open the selected hierarchical system design
	void onOpenSystemDesign(QModelIndex const& index);

	//! Open the selected document e.g. component or bus definition.
	void onOpenDocument(QModelIndex const& index);

	//! Create new component
	void onCreateNewComponent(QModelIndex const& index);

	//! Create new design
	void onCreateNewDesign(QModelIndex const& index);

	//! Create new SW design
	void onCreateNewSWDesign(QModelIndex const& index);

	//! Create new system design
	void onCreateNewSystemDesign(QModelIndex const& index);

	//! When delete is selected in search view
	void onDeleteItem(QModelIndex const& index);

	//! When export is selected in search view
	void onExportItem(QModelIndex const& index);

	//! Shows errors about the item at the given index.
	void onShowErrors(QModelIndex const& index);

	//! Create a new bus definition
	void onCreateBus(QModelIndex const& index);

	//! Create a new abstraction definition for given bus definition.
	void onCreateAbsDef(QModelIndex const& index);

	//! Create a new COM definition
	void onCreateComDef(QModelIndex const& index);

	//! Create a new API definition
	void onCreateApiDef(QModelIndex const& index);

	//! Remove the specified vlnv branch from the tree.
	void onRemoveVLNV(VLNV const& vlnv);

	//! Add the specified vlnv branch to the tree.
	void onAddVLNV(VLNV const& vlnv);

	//! Reset the model.
	void onResetModel();

    void onDocumentSaved(VLNV const& vlnv);

private:

	//! No copying
	LibraryTreeModel(const LibraryTreeModel& other);

	//! No assignment
	LibraryTreeModel& operator=(const LibraryTreeModel& other);

	/*! Removes an item from the treeModel
	*
	*      @param [in] toRemove     The LibraryItem that is the highest-level item to remove.
	*/
	void removeLibraryItem(LibraryItem* toRemove);
    
    /*!
     *  Validates the given item and possible sub-items.
     *
     *      @param [in] item   The item to validate.
     *
     *      @return True, if the item is valid, otherwise false.
     */
    bool validate(LibraryItem* item);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The data model that holds the library
	LibraryData* dataSource_;

	/*! The root of the tree.
	*
	* The LibraryTreeFilter owns the root item and the root item owns it's child items.
	*/
	LibraryItem* rootItem_;

	//! The instance that manages the library.
	LibraryInterface* handler_;
};

#endif // LIBRARYTREEMODEL_H
