/* 
 *  	Created on: 1.7.2011
 *      Author: Antti Kamppi
 * 		filename: hierarchyitem.h
 *		Project: Kactus 2
 */

#ifndef HIERARCHYITEM_H
#define HIERARCHYITEM_H

#include <models/component.h>
#include <models/busdefinition.h>
#include <models/abstractiondefinition.h>
#include <models/ApiDefinition.h>
#include <models/ComDefinition.h>

#include <common/KactusAttribute.h>

#include <QObject>
#include <QSharedPointer>
#include <QList>

class LibraryInterface;

/*! \brief Represents a single component in the library in hierarchy view.
 *
 */
class HierarchyItem : public QObject {
	Q_OBJECT

public:

	//! \brief Defines the type of the hierarchy item.
	enum ObjectType {
		ROOT = 0,
		COMPONENT,
		BUSDEFINITION,
		ABSDEFINITION,
        COMDEFINITION,
        APIDEFINITION
	};

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this object.
	 *
	*/
	HierarchyItem(LibraryInterface* handler, 
		HierarchyItem* parent, VLNV* vlnvP);

	/*! \brief The constructor for the root item.
	 * 
	 * This constructor should only be used to create the root item and no other
	 * hierarchy items.
	 * 
	 * \param handler Pointer to the instance that manages the library.
	 * \param parent Pointer to the owner of this item.
	 *
	*/
	HierarchyItem(LibraryInterface* handler, 
		QObject* parent);

	/*! \brief Create a child for the HierarchyItem that represents the given VLNV.
	 * 
	 * This function should only be called for the root item to create the tree.
	 * 
	 * \param vlnv Identifies the component to represent.
	 *
	*/
	void createChild(VLNV* vlnv);
	
	//! \brief The destructor
	virtual ~HierarchyItem();

	/*! \brief Get the parent of this item.
	 *
	 * \return Pointer to the parent of this item
	*/
	HierarchyItem* parent() const;

	/*! \brief Get the vlnv of this item.
	 *
	 * \return VLNV of the component this item represents.
	*/
	VLNV getVLNV() const;

	/*! \brief Get pointer to the child item with given index.
	 * 
	 * If child with given index does not exist then return null pointer.
	 * 
	 * \param index Row number of the child that's pointer is wanted.
	 *
	 * \return HierarchyItem*
	*/
	HierarchyItem* child(int index) const;

	/*! \brief  Get the row number of this item on this item's parent.
	 *
	 * \return The row number.
	*/
	int row();

	/*! \brief Get the number of children this item has.
	 *
	 * \return The number of child items this item has.
	*/
	int getNumberOfChildren() const;

	/*! \brief Get the index of specified item.
	 *
	 * \param item Pointer to the HierarchyItem that's row number is requested.
	 *
	 * \return The row number of given item. Returns -1 if item is not found.
	*/
	int getIndexOf(HierarchyItem* item);

	/*! \brief Check if this item has children or not.
	 *
	 * \return True if atleast one child item exists.
	*/
	bool hasChildren() const;

	/*! \brief Check if this item is valid or not.
	 *
	 * \return True if item is valid.
	*/
	bool isValid() const;

	/*! \brief Set the validity of this item.
	 *
	 * \param valid Sets the valid value of the item.
	 *
	*/
	void setValidity(bool valid);

	/*! \brief Checks if the component or one of it's sub-components contains the vlnv.
	 *
	 * \param vlnv The vlnv that is searched in hierarchy.
	 *
	 * \return bool True if the vlnv is found.
	*/
	bool contains(const VLNV& vlnv) const;

	/*! \brief Removes the child items that are contained in one or several other components.
	 * 
	 * Note: This function should be called only for the root item.
	 *
	 * \return void
	*/
	void cleanUp();
	
	/*! \brief Get pointer to the component that this item represents
	 *
	 * \return QSharedPointer<Component> Pointer to the component model.
	*/
	QSharedPointer<Component> component() const;

	/*! \brief Get pointer to the original vlnv instance owned by LibraryData.
	 *
	 * \return Pointer to the vlnv instance
	*/
	VLNV* getVLNVPointer() const;

	/*! \brief Count how many instances of given vlnv is found under this item.
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

	/*! \brief Get list of objects that have instantiated the given vlnv.
	 *
	 * This function makes sure each owner is appended to the list only once.
	 *
	 * \param list QList where owner vlnvs is appended.
	 * \param vlnvToSearch The vlnv to that's parents are wanted.
	 * 
	 * \return Number of unique owners found.
	 *
	*/
	int getOwners(QList<VLNV>& list, const VLNV& vlnvToSearch) const;

	/*! \brief Remove the child items with given vlnv.
	 *
	 * \param vlnv Identifies the items to remove.
	 * 
	 * \return Amount of child items removed from this item.
	*/
	int removeItems(const VLNV& vlnv);

	/*! \brief Find the hierarchyItems that represent the given vlnv.
	 *
	 * \param vlnv Identifies the items.
	 *
	 * \return QList<HierarchyItem*> contains pointers to the hierarchy items.
	*/
	QList<HierarchyItem*> findItems(const VLNV& vlnv);

	/*! \brief Update hierarchy items with given vlnv.
	 *
	 * \param vlnv Identifies the objects to update.
	 *
	*/
	void updateItems(const VLNV& vlnv);

	/*! \brief Checks if the item has parent on some hierarchy-level with given vlnv.
	 * 
	 * Function does not just search the direct parent but the whole parent-chain
	 * up to the root item.
	 * 
	 * \param vlnv Identifies the parent to search.
	 *
	 * \return bool True if parent with given vlnv is found.
	*/
	bool hasParent(const VLNV& vlnv);

	/*! \brief Check if the parent of this item is the root item.
	 *
	 * \return bool true if the parent item is root item. If this is the root or 
	 * parent is normal item then returns false.
	*/
	bool parentIsRoot() const;

	/*! \brief Check if this is the root item or not.
	 *
	 * \return bool True if this is the root item.
	*/
	bool isRoot() const;

	/*! \brief Check if this item is instantated in some item or not.
	 *
	 * \return bool true if item is contained as some item's child.
	*/
	bool isDuplicate() const;

	/*! \brief Set the duplicate value for this item.
	 *
	 * \param isDuplicate If true then this item is set as duplicate.
	 *
	*/
	void setDuplicate(bool isDuplicate);

	/*! \brief Get the list of vlnvs this item and it's parents contain.
	 *
	 * \return QList<VLNV> contains the list of vlnvs
	*/
	QList<VLNV> getVLNVs() const;

	/*! \brief Remove all children from item.
	 *
	*/
	void clear();

	/*! \brief Checks if the component this item represents is hierarchical or not.
	 *
	 * \return bool true if the component is hierarhical.
	*/
	bool isHierarchical() const;

	/*! \brief Get the implementation of this item.
	 * 
	 * Note: This function can only be called for items of type component.
	 *
	 * \return KactusAttribute::Implementation Specifies the implementation.
	*/
	KactusAttribute::Implementation getImplementation() const;

	/*! \brief Get the software type of this item.
	 *
	 * This function must be called only for items that represent software item.
	 *
	 * \return KactusAttribute::SWType Specifies the software type.
	*/
	KactusAttribute::SWType getSoftwareType() const;

	/*! \brief Get the type of the bus definition.
	 *
	 *
	 * \return KactusAttribute::BusDefType Specified the type of bus definition.
	*/
	KactusAttribute::BusDefType getBusDefType();

	/*! \brief Get the type of this hierarchy item.
	 *
	 * \return ObjectType defines the type of the item.
	*/
	ObjectType type() const;

	/*! \brief Get the child items of given object.
	 * 
	 * This function must only be called to the root item.
	 * 
	 * \param childList The list where the vlnvs of the children are appended.
	 * \param owner Identifies the object that's children are searched.
	 *
	*/
	void getChildren(QList<VLNV>& childList, const VLNV& owner);

	/*! \brief Get the vlnvs of the items that are this item's children.
	 *
	 * \param itemList The list where the vlnvs are appended to.
	 *
	*/
	void getChildItems(QList<VLNV>& itemList);

signals:

	//! \brief Send a notification to be printed to user.
	void noticeMessage(const QString& msg);

	//! \brief Send an error message to be printed to user.
	void errorMessage(const QString& msg);

private:
	//! \brief No copying
	HierarchyItem(const HierarchyItem& other);

	//! \brief No assignment
	HierarchyItem& operator=(const HierarchyItem& other);

	/*! \brief Checks if this HierarchyItem already has child for given vlnv.
	 *
	 * \param vlnv Identifies the child.
	 *
	 * \return bool True if child with given vlnv is found.
	*/
	bool hasChild(const VLNV& vlnv);

	/*! \brief Parse this hierarchy item to match a component
	 *
	 * \param vlnv The vlnv of the component.
	 *
	*/
	void parseComponent(const VLNV& vlnv);

	/*! \brief Parse this hierarchy item to match a bus definition.
	 *
	 * \param vlnv The vlnv of the bus definition.
	 *
	*/
	void parseBusDefinition(const VLNV& vlnv);

	/*! \brief Parse this hierarchy item to match an abstraction definition.
	 *
	 * \param vlnv The vlnv of the abstraction definition.
	 *
	*/
	void parseAbsDefinition(const VLNV& vlnv);

    /*! \brief Parse this hierarchy item to match a COM definition.
	 *
	 * \param vlnv The vlnv of the COM definition.
	 *
	*/
	void parseComDefinition(const VLNV& vlnv);

    /*! \brief Parse this hierarchy item to match an API definition.
	 *
	 * \param vlnv The vlnv of the API definition.
	 *
	*/
	void parseApiDefinition(const VLNV& vlnv);

	//! \brief Pointer to the component that this hierarhcyItem represents.
	QSharedPointer<Component> component_;

	//! \brief Pointer to the bus definition that this hierarchyItem represents.
	QSharedPointer<BusDefinition> busDef_;

	//! \brief Pointer to the abstraction definition that this hierarchyItem represents.
	QSharedPointer<AbstractionDefinition> absDef_;

    //! \brief Pointer to the COM definition that this hierarchyItem represents.
    QSharedPointer<ComDefinition> comDef_;

    //! \brief Pointer to the API definition that this hierarchyItem represents.
    QSharedPointer<ApiDefinition> apiDef_;

	//! \brief Pointer to the object that manages the library.
	LibraryInterface* handler_;

	//! \brief List of children of this item.
	QList<HierarchyItem*> childItems_;

	//! \brief Pointer to the parent of this item.
	HierarchyItem* parentItem_;

	//! \brief Specifies the validity of this item.
	bool isValid_;

	//! \brief Contains info that tell is this item is contained in some item as child.
	bool isDuplicate_;

	//! \brief Defines the type of this hierarchy item.
	ObjectType type_;
};

#endif // HIERARCHYITEM_H
