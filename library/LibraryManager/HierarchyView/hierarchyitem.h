//-----------------------------------------------------------------------------
// File: hierarchyitem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 01.07.2011
//
// Description:
// Represents a single component in the library in hierarchy view.
//-----------------------------------------------------------------------------

#ifndef HIERARCHYITEM_H
#define HIERARCHYITEM_H

#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/kactusExtensions/ApiDefinition.h>
#include <IPXACTmodels/kactusExtensions/ComDefinition.h>
#include <IPXACTmodels/Design/Design.h>

#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

#include <QObject>
#include <QSharedPointer>
#include <QList>
#include <QMap>

class LibraryInterface;

//-----------------------------------------------------------------------------
//! Represents a single component in the library in hierarchy view.
//-----------------------------------------------------------------------------
class HierarchyItem : public QObject
{
	Q_OBJECT

public:

	//! Defines the type of the hierarchy item.
	enum ObjectType
    {
		ROOT = 0,
		COMPONENT,
		BUSDEFINITION,
		ABSDEFINITION,
        COMDEFINITION,
        APIDEFINITION,
		HW_DESIGN,
		SW_DESIGN,
		SYS_DESIGN
	};

	/*! The constructor
	 *
	 *      @param [in] handler         The instance that manages the library.
	 *      @param [in] parent          The owner of this object.
	 *      @param [in] vlnv            The vlnv to construct the item for.
	 *      @param [in] implementation  The implementation attribute needed by designs.
	 *      @param [in] viewName        The name of the view the design belongs to.
	 */
	HierarchyItem(LibraryInterface* handler, HierarchyItem* parent, VLNV const& vlnv,
		KactusAttribute::Implementation implementation = KactusAttribute::KTS_IMPLEMENTATION_COUNT,
		QString const& viewName = QString());

	/*! The constructor for the root item.
	 * 
	 * This constructor should only be used to create the root item and no other hierarchy items.
	 * 
	 *      @param [in] handler The instance that manages the library.
	 *      @param [in] parent The owner of this item.
	 */
	HierarchyItem(LibraryInterface* handler, QObject* parent);

    //! The destructor
    virtual ~HierarchyItem();

	/*! Create a child for the HierarchyItem that represents the given VLNV.
	 * 
	 * This function should only be called for the root item to create the tree.
	 * 
	 *      @param [in] vlnv Identifies the component to represent.
	 */
	void createChild(const VLNV& vlnv);


	/*! Get the parent of this item.
	 *
	 *      @return The parent of this item
	*/
	HierarchyItem* parent() const;

	/*! Get the vlnv of this item.
	 *
	 *      @return VLNV of the component this item represents.
	*/
	VLNV getVLNV() const;

	/*! Get pointer to the child item with given index.
	 * 
	 * If child with given index does not exist then return null pointer.
	 * 
	 *      @param [in] index Row number of the child that's pointer is wanted.
	 *
	 *      @return HierarchyItem*
	*/
	HierarchyItem* child(int index) const;

	/*!  Get the row number of this item on this item's parent.
	 *
	 *      @return The row number.
	*/
	int row();

	/*! Get the number of children this item has.
	 *
	 *      @return The number of child items this item has.
	*/
	int getNumberOfChildren() const;

	/*! Get the index of specified item.
	 *
	 *      @param [in] item The HierarchyItem that's row number is requested.
	 *
	 *      @return The row number of given item. Returns -1 if item is not found.
	*/
	int getIndexOf(HierarchyItem* item);

	/*! Check if this item has children or not.
	 *
	 *      @return True if at least one child item exists.
	*/
	bool hasChildren() const;

	/*! Check if this item is valid or not.
	 *
	 *      @return True if item is valid.
	*/
	bool isValid() const;

	/*! Set the validity of this item.
	 *
	 *      @param [in] valid Sets the valid value of the item.
	 *
	*/
	void setValidity(bool valid);

	/*! Checks if the component or one of it's sub-components contains the vlnv.
	 *
	 *      @param [in] vlnv The vlnv that is searched in hierarchy.
	 *
	 *      @return bool True if the vlnv is found.
	*/
	bool contains(const VLNV& vlnv) const;

	/*! Removes the child items that are contained in one or several other components.
	 * 
	 * Note: This function should be called only for the root item.
	 *
	 *      @return void
	*/
	void cleanUp();
	
	/*! Get pointer to the component that this item represents
	 *
	 *      @return QSharedPointer<Component> The component model.
	*/
	QSharedPointer<Component const> component() const;

	/*! Count how many instances of given vlnv is found under this item.
	 *
	 * This function does not check if the same owner is counted multiple times
	 * if same sub-component is instantiated in several components. If count that 
	 * contains only unique owners then you should use getOwners().
	 * 
	 *      @param [in] vlnv Identifies the component that's instances are searched.
	 *
	 *      @return Number of found instances.
	*/
	int referenceCount(const VLNV& vlnv) const;

	/*! Get list of objects that have instantiated the given vlnv.
	 *
	 * This function makes sure each owner is appended to the list only once.
	 *
	 *      @param [out] list QList where owner vlnvs is appended.
	 *      @param [in] vlnvToSearch The vlnv to that's parents are wanted.
	 * 
	 *      @return Number of unique owners found.
	*/
	int getOwners(QList<VLNV>& list, const VLNV& vlnvToSearch) const;

	/*! Remove the child items with given vlnv.
	 *
	 *      @param [in] vlnv Identifies the items to remove.
	 * 
	 *      @return Amount of child items removed from this item.
	*/
	int removeItems(const VLNV& vlnv);

	/*! Find the hierarchyItems that represent the given vlnv.
	 *
	 *      @param [in] vlnv Identifies the items.
	 *
	 *      @return QList<HierarchyItem*> contains pointers to the hierarchy items.
	*/
	QVector<HierarchyItem*> findItems(const VLNV& vlnv);

	/*! Update hierarchy items with given vlnv.
	 *
	 *      @param [in] vlnv Identifies the objects to update.
	 *
	*/
	void updateItems(const VLNV& vlnv);

	/*! Checks if the item has parent on some hierarchy-level with given vlnv.
	 * 
	 * Function does not just search the direct parent but the whole parent-chain
	 * up to the root item.
	 * 
	 *      @param [in] vlnv Identifies the parent to search.
	 *
	 *      @return bool True if parent with given vlnv is found.
	*/
	bool hasParent(const VLNV& vlnv);

	/*! Check if the parent of this item is the root item.
	 *
	 *      @return bool true if the parent item is root item. If this is the root or 
	 * parent is normal item then returns false.
	*/
	bool parentIsRoot() const;

	/*! Check if this is the root item or not.
	 *
	 *      @return bool True if this is the root item.
	*/
	bool isRoot() const;

	/*! Check if this item is instantiated in some item or not.
	 *
	 *      @return bool true if item is contained as some item's child.
	*/
	bool isDuplicate() const;

	/*! Set the duplicate value for this item.
	 *
	 *      @param [in] isDuplicate If true then this item is set as duplicate.
	 *
	*/
	void setDuplicate(bool isDuplicate);

	/*! Get the list of vlnvs this item and it's parents contain.
	 *
	 *      @return QList<VLNV> contains the list of vlnvs
	*/
	QVector<VLNV> getVLNVs() const;

	/*! Remove all children from item.
	 *
	*/
	void clear();

	/*! Checks if the component this item represents is hierarchical or not.
	 *
	 *      @return bool true if the component is hierarhical.
	*/
	bool isHierarchical() const;

	/*! Get the implementation of this item.
	 * 
	 * Note: This function can only be called for items of type component.
	 *
	 *      @return KactusAttribute::Implementation Specifies the implementation.
	*/
	KactusAttribute::Implementation getImplementation() const;

	/*! Get the type of this hierarchy item.
	 *
	 *      @return ObjectType defines the type of the item.
	*/
	ObjectType type() const;

	/*! Get the child items of given object.
	 * 
	 * This function must only be called to the root item.
	 * 
	 *      @param [in] childList The list where the vlnvs of the children are appended.
	 *      @param [in] owner Identifies the object that's children are searched.
	 *
	*/
	void getChildren(QList<VLNV>& childList, const VLNV& owner);

	/*! Get the vlnvs of the items that are this item's children.
	 *
	 *      @param [in] itemList The list where the vlnvs are appended to.
	 *
	*/
	void getChildItems(QList<VLNV>& itemList);

	/*! Count how many times this component has been instantiated in a containing design.
	 * 
	 * For items that are not components this function returns -1
	 *
	 *      @return The instance count in the containing design.
	*/
	int instanceCount() const;

	/*! Get the name of the view the design belongs to.
	 * 
	 * If type() is other than design an empty string is returned.
	 *
	 *      @return QString containing the view name.
	*/
	QString getViewName() const;

signals:

	//! Send a notification to be printed to user.
	void noticeMessage(QString const& msg);

	//! Send an error message to be printed to user.
	void errorMessage(QString const& msg);

protected:

	/*! Count the number of instances for the given vlnv.
	 *
	 *      @param [in] componentVLNV The vlnv of the component that's instance count is counted.
	 *
	 *      @return The number of instantiation times.
	*/
	int countInstances(const VLNV& componentVLNV);

private:
	//! No copying
	HierarchyItem(const HierarchyItem& other);

	//! No assignment
	HierarchyItem& operator=(const HierarchyItem& other);

	/*! Checks if this HierarchyItem already has child for given vlnv.
	 *
	 *      @param [in] vlnv Identifies the child.
	 *
	 *      @return bool True if child with given vlnv is found.
	*/
	bool hasChild(const VLNV& vlnv);

	/*! Parse this hierarchy item to match a component
	 *
	 *      @param [in] vlnv The vlnv of the component.
	 *
	*/
	void parseComponent(const VLNV& vlnv);

    /*!
     *  Finds the referenced design in a component view.
     *
     *      @param [in] view   The view whose reference to find.
     *
     *      @return The design referenced ín the view.
     */
    VLNV findDesignReference(QSharedPointer<View> view);

    /*!
     *  Creates a child item for the given design.
     *
     *      @param [in] designVLNV          The design to represent with the child.
     *      @param [in] implementation      The implementation type of the design.
     *      @param [in] viewName            The name of the view referencing the design.
     */
    void createChildItemForDesign(VLNV const& designVLNV, KactusAttribute::Implementation implementation,
        QString const& viewName);

	/*! Parse this hierarchy item to match a bus definition.
	 *
	 *      @param [in] vlnv The vlnv of the bus definition.
	 *
	*/
	void parseBusDefinition(const VLNV& vlnv);

	/*! Parse this hierarchy item to match an abstraction definition.
	 *
	 *      @param [in] vlnv The vlnv of the abstraction definition.
	 *
	*/
	void parseAbsDefinition(const VLNV& vlnv);

    /*! Parse this hierarchy item to match a COM definition.
	 *
	 *      @param [in] vlnv The vlnv of the COM definition.
	 *
	*/
	void parseComDefinition(const VLNV& vlnv);

    /*! Parse this hierarchy item to match an API definition.
	 *
	 *      @param [in] vlnv The vlnv of the API definition.
	 *
	*/
	void parseApiDefinition(const VLNV& vlnv);

	/*! Parse this hierarchy item to match a design.
	 *
	 *      @param [in] vlnv The vlnv of the design.
	*/
	void parseDesign(const VLNV& vlnv, KactusAttribute::Implementation implementation, QString const& viewName);

    /*!
     *  Finds the valid component references in a design item.
     *
     *      @return The valid VLVN references.
     */
    QVector<VLNV> getValidComponentsInDesign();

    /*!
     *  Checks if the given VLNV reference to a component is valid.
     *
     *      @param [in] componentVLNV   The VLNV to check.
     *
     *      @return True, if the VLVN reference is valid, otherwise false.
     */
    bool isValidComponentInstanceVLNV(VLNV const& componentVLNV);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The component that this hierarhcyItem represents.
	QSharedPointer<Component const> component_;

	//! The bus definition that this hierarchyItem represents.
	QSharedPointer<BusDefinition const> busDef_;

	//! The abstraction definition that this hierarchyItem represents.
	QSharedPointer<AbstractionDefinition const> absDef_;

    //! The COM definition that this hierarchyItem represents.
    QSharedPointer<ComDefinition const> comDef_;

    //! The API definition that this hierarchyItem represents.
    QSharedPointer<ApiDefinition const> apiDef_;

	//! The design that this hierarchyItem represents.
	QSharedPointer<Design const> design_;

	//! The object that manages the library.
	LibraryInterface* library_;

	//! List of children of this item.
	QList<HierarchyItem*> childItems_;

	//! The parent of this item.
	HierarchyItem* parentItem_;

	//! Specifies the validity of this item.
	bool isValid_;

	//! Contains info that tell is this item is contained in some item as child.
	bool isDuplicate_;

	//! Defines the type of this hierarchy item.
	ObjectType type_;

	//! Contains the instance count for the sub items.
	QMap<VLNV, int> instanceCount_;

	//! The name of the view a design belongs to.
	QString viewName_;
};

#endif // HIERARCHYITEM_H
