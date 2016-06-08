//-----------------------------------------------------------------------------
// File: libraryitem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 24.06.2010
//
// Description:
// LibraryItem class is used to create the hierarchical structure of IP library.
//-----------------------------------------------------------------------------

#ifndef LIBRARYITEM_H
#define LIBRARYITEM_H

#include "librarydata.h"

#include <QVector>
#include <QString>
#include <QObject>

class VLNV;

//-----------------------------------------------------------------------------
//! LibraryItem class is used to create the hierarchical structure of IP library.
//-----------------------------------------------------------------------------
class LibraryItem : public QObject
{
	Q_OBJECT

public:

	/*! enum Level is used in constructing new items in library.
	 *
	 * Level is used to tell what information to use from VLNV tag in creating new component in hierarchy.
	 */
	enum Level
    {
        ROOT = 0, 
		VENDOR, 
		LIBRARY,
		NAME,
		VERSION
    };

	/*! The default constructor.
	*
	*       @remark Only used when creating a root item for a tree structure.
	*/
	LibraryItem(QObject* parent);

	/*! The constructor
	*
	*      @param [in] vlnv     A VLNV tag that is to be added to database.
	*      @param [in] level    The level of the item in the hierarchy.
	*      @param [in] parent   The parent of this item.
	*/
	LibraryItem(VLNV const& vlnv, Level level, LibraryItem *parent);

	//! The destructor of the class LibraryItem.
	virtual ~LibraryItem();

	/*! Create a new child for an existing item.
	*
	*      @param [in] vlnv A VLNV tag that is to be added to database.
	*      @param [in] level The level of the item in the hierarchy.
	*/
	void createChild(VLNV const& vlnv, Level level);

	/*! Get the parent of the current node.
	 *
	 *      @return The parent of the item.
	 */
	LibraryItem* parent() const;

	/*! Get the name of the item.
	 *
	 *      @return The name of the item.
	 */
	QString name() const;

	/*! Get the level of the item in the hierarchy.
	 *
	 *      @return The level the item is in.
	 */
	Level getLevel() const;

	/*! Get the child item.
	 *
	 *      @param [in] index   index of the child item.
     *
	 *      @return The child.
	 */
	LibraryItem* child(int index) const;

	/*! Get the row number of the child item.
	 *
	 *      @return The row number where the item is located in its parents list.
	 */
	int row();

	/*! Get the number of child items the item has.
	 *
	 *      @return How many children item owns.
	 */
	int getNumberOfChildren() const;

	/*! Get the VLNV tag of this item.
	 *
     *      @return A VLNV instance that is represented by this item.
     *
	 *      @remark Valid VLNV pointer can only be returned if this item is level VERSION.
	 */
	VLNV getVLNV() const;

	/*! Get the index of the specified item.
	 *
	 *      @param [in] item    The library item that's row number is searched.
	 *
	 *      @return The row number of the specified item.
	 */
	int getIndexOf(LibraryItem* item);

	/*! Get the row number of this item in item's parent.
	 *
	 *      @return The row number of this item.
	 */
	int getRow();

	/*! Check if this item has children or not.
	 *
	 *      @return True if item has child-items, otherwise false.
	 */
	bool hasChildren() const;

	/*! Get list of all VLNV tags that exist under this item.
	 *
	 *      @return List of all VLNVs.
	 */
	QVector<VLNV> getVLNVs() const;

	/*! Remove a child of this item.
	 *
	 *      @param [in] childItem The child item that should be removed.
	 */
	void removeChild(LibraryItem* childItem);

	/*! Find the highest level LibraryItem that still identifies a single item.
	 * 
	 * This function checks if there are other children. If no other children
	 * are found then this function calls parent item to check if it can be
	 * used to identify a single item.
	 * 
	 *      @param [in] childItem   The item that is currently the highest possible item.
	 *
	 *      @return LibraryItem* The item that is to be removed.
	 */
	LibraryItem* findHighestUnique(LibraryItem* childItem = 0);

	/*! Find the highest level LibraryItem that still identifies a single item.
	 * 
	 * This function finds the highest level possible to identify a single item.
	 *
	 *      @param [in] vlnv The vlnv element that must be matched.
	 *
	 *      @return The item that is the highest level item.
	 */
	LibraryItem* findHighestUnique(VLNV const& vlnv);

	/*! Find the library item that uniquely represents the specified vlnv.
	 *
	 *      @param [in] vlnv The vlnv that identifies the item to find.
	 *
	 *      @return The LibraryItem that represents the vlnv in the tree.
	 */
	LibraryItem* findItem(VLNV const& vlnv);

	/*! Set the vlnv to match the tree structure of this item
	 *
	 *      @param [out] vlnv The vlnv instance to set.
	 */
	void setVlnv(VLNV& vlnv);

	//! Remove the children of this item.
	void clear();

	/*! Get all vendor items.
	 *
	 *      @return All vendor items under this item.
	*/
	QVector<LibraryItem*> getVendors() const;

	/*! Get all library items.
	 *
	 *      @return All library items under this item.
	*/
	QVector<LibraryItem*> getLibraries() const;

	/*! Get all name items.
	 *
	 *      @return All name items under this item.
	*/
	QVector<LibraryItem*> getNames() const;

	/*! Get all version items.
	 *
	 *      @return All version items under this item.
	*/
	QVector<LibraryItem*> getVersions() const;

    /*!
     *  Checks if the item is valid.
     *
     *      @return True, if the item is valid, otherwise false.
     */
    bool isValid() const;

    /*!
     *  Sets the validity of the item.
     *
     *      @param [in] valid   The validity to set.
     */
    void setValid(bool valid);

private:

	//! No copying
	LibraryItem(const LibraryItem &other);

	//! No assignment
	LibraryItem &operator=(const LibraryItem &other);


    /*!
     *  Finds the child item on the given level from the given VLNV.
     *
     *      @param [in] level   The level to get the item for.
     *      @param [in] vlnv    The VLNV to get the item from.
     *
     *      @return The child item on the given level.
     */
    LibraryItem* findChildForLevel(Level level, VLNV const& vlnv);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The name of the item on the level (i.e. TUT in vendor-level).
	QString name_;

	//! The level of the item.
	Level level_;

	//! The child items of current item in the hierarchy.
	QVector<LibraryItem*> childItems_;

	//! The parent of the item.
	LibraryItem *parentItem_;

	//! A VLNV tag that matches the version.
	VLNV vlnv_;

    bool valid_;
};

#endif // LIBRARYITEM_H
