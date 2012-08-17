/* 
 *  	Created on: 1.7.2011
 *      Author: Antti Kamppi
 * 		filename: hierarchymodel.h
 *		Project: Kactus 2
 */

#ifndef HIERARCHYMODEL_H
#define HIERARCHYMODEL_H

#include "hierarchyitem.h"

#include <QAbstractItemModel>
#include <QSharedPointer>

class LibraryInterface;
class LibraryData;

/*! \brief Contains the items to display the library component hierarchy to user.
 *
 */
class HierarchyModel : public QAbstractItemModel {
	Q_OBJECT

public:

	/*! \brief The constructor
	 * 
	 * \param sourceModel Pointer to the data model to use as base model.
	 * \param handler Pointer to the instance that manages the library.
	 * \param parent Pointer to the owner of this model.
	 *
	*/
	HierarchyModel(LibraryData* sourceModel,
		LibraryInterface* handler,
		QObject *parent);
	
	//! \brief The destructor
	virtual ~HierarchyModel();

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
     * \return Number of columns (always 2)
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

	/*! \brief Create a model index for a library item
	 *
	 * \param item Pointer to the LibraryItem that's model index is wanted
	 *
	 * \return QModelIndex that identifies the libraryItem.
	*/
	QModelIndex index(HierarchyItem* item) const;

	/*! \brief Count how many times the given component is instantiated in the library.
	 * 
	 * This function does not check if the same owner is counted multiple times
	 * if same sub-component is instantiated in several components. If count that 
	 * contains only unique owners then you should use getOwners().
	 * 
	 * \param vlnv Identifies the component that's instances are searched.
	 *
	 * \return Number of found instances.
	 */
	int referenceCount(const VLNV& vlnv) const;

	/*! \brief Get the components that have instantiated the given vlnv in their design.
	 * 
	 * This function makes sure each owner is appended to the list only once.
	 *
	 * \param list QList where the search results are appended.
	 * \param vlnvToSearch Identifies the component to search for.
	 * 
	 * \return Number of owners found.
	 *
	*/
	virtual int getOwners(QList<VLNV>& list, const VLNV& vlnvToSearch) const;

	/*! \brief Find model indexes of items that represent given vlnv.
	 *
	 * \param vlnv Identifies the objects to search for.
	 *
	 * \return QModelIndexList contains indexes of items with given vlnv.
	*/
	QModelIndexList findIndexes(const VLNV& vlnv);

	/*! \brief Get the child items of given object.
	 *
	 * \param childList The list where the vlnvs of the children are appended.
	 * \param owner Identifies the object that's children are searched.
	 *
	*/
	void getChildren(QList<VLNV>& childList, const VLNV& owner);

public slots:

	//! \brief Reset the model
	void onResetModel();

	//! \brief Open the selected hierarchical design
	void onOpenDesign(const QModelIndex& index);

    //! \brief Open the selected SW design
    void onOpenSWDesign(const QModelIndex& index);

    //! \brief Open the selected system design
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

	//! \brief Create new bus
	void onCreateNewBus(const QModelIndex& index);

	//! \brief Create new abstraction definition
	void onCreateNewAbsDef(const QModelIndex& index);

    //! \brief Create new COM definition
    void onCreateNewComDef(const QModelIndex& index);

    //! \brief Create new API definition
    void onCreateNewApiDef(const QModelIndex& index);

	//! \brief When export is selected in search view
	void onExportItem(const QModelIndex& index);

	//! \brief Remove the specified vlnv from the tree.
	void onRemoveVLNV(const VLNV& vlnv);
	
	/*! \brief This function should be called when a component has changed.
	 * 
	 * Function updates the hierarchical model so that changes made to the 
	 * component are visible.
	 * 
	 * \param vlnv Identifies the component that was saved.
	 *
	*/
	void onComponentSaved(const VLNV& vlnv);

signals:

	//! \brief Send an error message to be printed to user.
	void errorMessage(const QString& msg);

	//! \brief Send a notification to be printed to user.
	void noticeMessage(const QString& msg);

	//! \brief Open the design of a component.
	void openDesign(const VLNV& vlnv, const QString& viewName);

    //! \brief Open the SW design of a component.
    void openSWDesign(const VLNV& vlnv);

    //! \brief Open the system design of a component.
    void openSystemDesign(const VLNV& vlnv);

	//! \brief Open the component in a component editor.
	void editItem(const VLNV& vlnv);

	//! \brief Create a new bus definition.
	void createBusDef(const VLNV& vlnv);

	//! \brief Create a new component with given vlnv.
	void createComponent(const VLNV& vlnv);

	//! \brief Create a new bus with given vlnv.
	void createBus(const VLNV& vlnv);

	//! \brief Create a new abstraction definition for given bus definition.
	void createAbsDef(const VLNV& busDefVLNV);

	//! \brief Create new design with given vlnv.
	void createDesign(const VLNV& vlnv);

    //! \brief Create new SW design with given vlnv.
    void createSWDesign(const VLNV& vlnv);

    //! \brief Create new system design with given vlnv.
    void createSystemDesign(const VLNV& vlnv);

    //! \brief Create a new COM definition with given vlnv.
    void createComDef(const VLNV& vlnv);

    //! \brief Create a new API definition with given vlnv.
    void createApiDef(const VLNV& vlnv);

	//! \brief Export an item to a new location.
	void exportItem(const VLNV vlnv);

	//! \brief Refresh the item filtering because changes have been made
	void invalidateFilter();

private:
	//! \brief No copying
	HierarchyModel(const HierarchyModel& other);

	//! \brief No assignment
	HierarchyModel& operator=(const HierarchyModel& other);

	//! \brief Pointer to the data model that holds the library.
	LibraryData* dataModel_;

	//! \brief Pointer to the root item of the model
	QSharedPointer<HierarchyItem> rootItem_;

	//! \brief Pointer to the instance that manages the library.
	LibraryInterface* handler_;
};

#endif // HIERARCHYMODEL_H
