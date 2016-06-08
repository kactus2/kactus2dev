//-----------------------------------------------------------------------------
// File: hierarchymodel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 01.07.2011
//
// Description:
// Contains the items to display the library component hierarchy to user.
//-----------------------------------------------------------------------------

#ifndef HIERARCHYMODEL_H
#define HIERARCHYMODEL_H

#include "hierarchyitem.h"

#include <QAbstractItemModel>
#include <QSharedPointer>

class LibraryInterface;
class LibraryData;

//-----------------------------------------------------------------------------
//! Contains the items to display the library component hierarchy to user.
//-----------------------------------------------------------------------------
class HierarchyModel : public QAbstractItemModel
{
	Q_OBJECT

public:

	//! Defines the columns for the model.
	enum Column
    {
		OBJECT_COLUMN = 0,
		INSTANCE_COLUMN,
		COLUMN_COUNT
	};

	/*! The constructor
	 * 
	 *      @param [in] sourceModel     The data model to use as base model.
	 *      @param [in] handler         The instance that manages the library.
	 *      @param [in] parent          The owner of this model.
	 */
	HierarchyModel(LibraryData* sourceModel, LibraryInterface* handler, QObject* parent);
	
	//! The destructor
	virtual ~HierarchyModel();

	/*! Get the data for the headers of this model.
     *
     *      @param [in] section         Specifies the column of the header.
	 *      @param [in] orientation     The orientation of the header, only Qt::Horizontal supported.
     *      @param [in] role            Specifies the role of the data.
     *
     *      @return QVariant containing the data.
    */
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    /*! Get the number of columns in this model
     *
     *      @param [in] parent Model index identifying the item that's column count is 
     * wanted.
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
     *      @param [in] row         Row number of the object.
     *      @param [in] column      Column number of the object.
     *      @param [in] parent      Model index of the parent of the object.
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
     *      @param [in] index Model index that identifies the object that's data is wanted.
     *      @param [in] role Specifies the type of data wanted.
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

	/*! Create a model index for a library item
	 *
	 *      @param [in] item The LibraryItem that's model index is wanted
	 *
	 *      @return QModelIndex that identifies the libraryItem.
	*/
	QModelIndex index(HierarchyItem* item) const;

	/*! Count how many times the given component is instantiated in the library.
	 * 
	 * This function does not check if the same owner is counted multiple times
	 * if same sub-component is instantiated in several components. If count that 
	 * contains only unique owners then you should use getOwners().
	 * 
	 *      @param [in] vlnv Identifies the component that's instances are searched.
	 *
	 *      @return Number of found instances.
	 */
	int referenceCount(VLNV const& vlnv) const;

	/*! Get the components that have instantiated the given vlnv in their design.
	 * 
	 * This function makes sure each owner is appended to the list only once.
	 *
	 *      @param [out] list           QList where the search results are appended.
	 *      @param [in] vlnvToSearch    Identifies the component to search for.
	 * 
	 *      @return Number of owners found.
	 *
	*/
	virtual int getOwners(QList<VLNV>& list, VLNV const& vlnvToSearch) const;

	/*! Find model indexes of items that represent given vlnv.
	 *
	 *      @param [in] vlnv Identifies the objects to search for.
	 *
	 *      @return QModelIndexList contains indexes of items with given vlnv.
	*/
	QModelIndexList findIndexes(VLNV const& vlnv);

	/*! Get the child items of given object.
	 *
	 *      @param [out] childList  The list where the vlnvs of the children are appended.
	 *      @param [in] owner       Identifies the object that's children are searched.
	 *
	*/
	void getChildren(QList<VLNV>& childList, VLNV const& owner);

public slots:

	//! Reset the model
	void onResetModel();

	//! Open the selected hierarchical design
	void onOpenDesign(QModelIndex const& index);

    //! Open the memory design of the selected design.
    void onOpenMemoryDesign(QModelIndex const& index);

    //! Open the selected SW design
    void onOpenSWDesign(QModelIndex const& index);

    //! Open the selected system design
    void onOpenSystemDesign(QModelIndex const& index);

	//! When open is selected in search view
	void onOpenComponent(QModelIndex const& index);

	//! Create new component
	void onCreateNewComponent(QModelIndex const& index);

	//! Create new design
	void onCreateNewDesign(QModelIndex const& index);

    //! Create new SW design
    void onCreateNewSWDesign(QModelIndex const& index);

    //! Create new system design
    void onCreateNewSystemDesign(QModelIndex const& index);

	//! Create new bus
	void onCreateNewBus(QModelIndex const& index);

	//! Create new abstraction definition
	void onCreateNewAbsDef(QModelIndex const& index);

    //! Create new COM definition
    void onCreateNewComDef(QModelIndex const& index);

    //! Create new API definition
    void onCreateNewApiDef(QModelIndex const& index);

	//! When export is selected in search view
	void onExportItem(QModelIndex const& index);

    //! Shows errors about the item at the given index.
    void onShowErrors(QModelIndex const& index);

	//! Remove the specified vlnv from the tree.
	void onRemoveVLNV(VLNV const& vlnv);
	
	/*! This function should be called when an IP-XACT document has changed.
	 * 
	 * Function updates the hierarchical model so that changes made to the document are visible.
	 * 
	 *      @param [in] vlnv Identifies the document that was saved.
	*/
	void onDocumentSaved(VLNV const& vlnv);

signals:

	//! Send an error message to be printed to user.
	void errorMessage(const QString& msg);

	//! Send a notification to be printed to user.
	void noticeMessage(const QString& msg);

	//! Open the design of a component.
	void openDesign(VLNV const& vlnv, const QString& viewName);

    //! Open the memory design of a component.
    void openMemoryDesign(VLNV const& vlnv, const QString& viewName);

    //! Open the SW design of a component.
    void openSWDesign(VLNV const& vlnv, const QString& viewName);

    //! Open the system design of a component.
    void openSystemDesign(VLNV const& vlnv, const QString& viewName);

	//! Open the component in a component editor.
	void editItem(VLNV const& vlnv);

	//! Create a new bus definition.
	void createBusDef(VLNV const& vlnv);

	//! Create a new component with given vlnv.
	void createComponent(VLNV const& vlnv);

	//! Create a new bus with given vlnv.
	void createBus(VLNV const& vlnv);

	//! Create a new abstraction definition for given bus definition.
	void createAbsDef(VLNV const& busDefVLNV);

	//! Create new design with given vlnv.
	void createDesign(VLNV const& vlnv);

    //! Create new SW design with given vlnv.
    void createSWDesign(VLNV const& vlnv);

    //! Create new system design with given vlnv.
    void createSystemDesign(VLNV const& vlnv);

    //! Create a new COM definition with given vlnv.
    void createComDef(VLNV const& vlnv);

    //! Create a new API definition with given vlnv.
    void createApiDef(VLNV const& vlnv);

	//! Export an item to a new location.
	void exportItem(const VLNV vlnv);

    //! Shows errors about the item at the given index.
    void showErrors(VLNV const& vlnv);

	//! Refresh the item filtering because changes have been made
	void invalidateFilter();

private:
	//! No copying
	HierarchyModel(const HierarchyModel& other);

	//! No assignment
	HierarchyModel& operator=(const HierarchyModel& other);

	//! The data model that holds the library.
	LibraryData* dataModel_;

	//! The root item of the model
	HierarchyItem* rootItem_;

	//! The instance that manages the library.
	LibraryInterface* handler_;
};

#endif // HIERARCHYMODEL_H
