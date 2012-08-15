/* 
 *  Created on: 24.6.2010
 *  Author: Antti Kamppi
 */

#ifndef LIBRARYITEM_H_
#define LIBRARYITEM_H_

#include "librarydata.h"

#include <QList>
#include <QString>
#include <QObject>
#include <QRegExpValidator>

class VLNV;
class LibraryTreeModel;

/*! \brief LibraryItem class is used to create the hierarchical structure of
 * component library
 *
 * component library is a hierarchical structure consisting of vendors,
 * libraries, names and versions. One instance knows its children and
 * can provide access to them.
 */
class LibraryItem : public QObject {

	Q_OBJECT

public:

	/*! \brief enum Level is used in constructing new items in library
	 *
	 * Level is used to give tell what information to use from VLNV tag in
	 * creating new component in hierarchy
	 */
	enum Level {ROOT = 0, TYPE = 1, VENDOR = 2, LIBRARY = 3, NAME = 4,
		VERSION = 5};

	/*! \brief The default constructor
	*
	* Constructs LibraryItem as empty item. Only used when creating
	* a root item for a tree structure
	*/
	LibraryItem(const QString& name, QObject* parent);

	/*! \brief The constructor
	*
	* \exception runtime_error Occurs if an invalid level is specified.
	* \param vlnv A VLNV tag that is to be added to database
	* \param level The level of the item in the hierarchy
	* \param parent A pointer to the parent of this item
	*/
	LibraryItem(const VLNV& vlnv, Level level, LibraryItem *parent);

	/*! \brief The destructor of the class LibraryItem
	*
	*/
	virtual ~LibraryItem();

	/*! \brief Create a new child for an existing item
	*
	* \param vlnv A VLNV tag that is to be added to database
	* \param level The level of the item in the hierarchy
	* 
	*/
	void createChild(const VLNV& vlnv, Level level);

	/*! \brief Get the parent of the current node
	 *
	 * \return The pointer to the parent of the item
	 */
	LibraryItem *parent() const;

	/*! \brief Get the name of the item
	 *
	 * \return The name of the item
	 */
	QString getName() const;

	/*! \brief Get the level of the item in the hierarchy
	 *
	 * \return the level the item is in
	 */
	Level getLevel() const;

	/*! \brief Get a pointer to the child item
	 *
	 * \param index index of the child item
	 * \return pointer to the child
	 */
	LibraryItem *child(int index) const;

	/*! \brief get the row number of the child item
	 *
	 * \return the row number where the item is located in its parents list.
	 */
	int row();

	/*! \brief Get the number of child items the item has
	 *
	 * \return How many children item owns.
	 */
	int getNumberOfChildren() const;

	/*! \brief get the VLNV tag of this item.
	 *
	 * Valid VLNV pointer can only be returned if this item is low level
	 * item that can identify a single VLNV tag. If level is other than
	 * VERSION then a non valid vlnv is returned.
	 * 
	 * \return A VLNV instance that is represented by this item.
	 */
	VLNV getVLNV() const;

	/*! \brief Get the index of the specified item
	 *
	 * \param item Pointer to the library item that's row number is searched.
	 *
	 * \return The row number of the specified item
	*/
	int getIndexOf(LibraryItem* item);

	/*! \brief Get the row number of this item in item's parent.
	 *
	 * \return The row number of this item.
	*/
	int getRow();

	/*! \brief Check if this item has children or not
	 *
	 * \return True if item has child-items.
	*/
	bool hasChildren() const;

	/*! \brief Get list of all VLNV tags that exist under this item
	 *
	 * \param vlnvList QList where all VLNVs are to be saved
	 *
	*/
	void getVLNVs(QList<VLNV>& vlnvList);

	/*! \brief Remove a child of this item
	 *
	 * \param childItem Pointer to the child item that should be removed.
	 *
	*/
	void removeChild(LibraryItem* childItem);

	/*! \brief Find the highest level LibraryItem that still identifies a single item
	 * 
	 * This function checks if there are other children. If no other children
	 * are found then this function calls parent item to check if it can be
	 * used to identify a single item.
	 * 
	 * \param childItem Pointer to item that is currently the highest possible
	 * item.
	 *
	 * \return LibraryItem* Pointer to the item that is to be removed
	*/
	LibraryItem* findHighestUnique(LibraryItem* childItem = 0);

	/*! \brief Find the highest level LibraryItem that still identifies a single item
	 * 
	 * This function finds the highest level possible to identify a single item.
	 *
	 * \param vlnv The vlnv element that must be matched.
	 *
	 * \return LibraryItem* pointer to the item that is the highest level item.
	*/
	LibraryItem* findHighestUnique(const VLNV& vlnv);

	/*! \brief Find the library item that uniquely represents the specified vlnv.
	 *
	 * \param vlnv The vlnv that identifies the item to find.
	 *
	 * \return Pointer to the LibraryItem that represents the vlnv in the tree.
	*/
	LibraryItem* findItem(const VLNV& vlnv);

	/*! \brief Get the type of items that this library item represents.
	 *
	 *
	 * \return QString containing the name of the type (component, busDefinition, jne)
	*/
	QString getTypeStr() const;

	/*! \brief Set the vlnv to match the tree structure of this item
	 *
	 * \param vlnv The vlnv instance to set.
	 *
	*/
	void setVlnv(VLNV& vlnv);

	/*! \brief Remove the children of this item.
	 *
	*/
	void clear();

	/*! \brief Get the vendor items that's name passes the validator.
	 *
	 * \param validator Reference to the validator to use.
	 *
	 * \return QList<LibraryItem*> List of items that passed the validator.
	*/
	QList<LibraryItem*> getVendors(const QRegExpValidator& validator) const;

	/*! \brief Get all vendor items
	 *
	 * \return QList<LibraryItem*> contains pointers to all vendor items under this item.
	*/
	QList<LibraryItem*> getVendors() const;

	/*! \brief Get the library items that's name passes the validator.
	 *
	 * \param validator Reference to the validator to use.
	 *
	 * \return QList<LibraryItem*> List of items that passed the validator.
	*/
	QList<LibraryItem*> getLibraries(const QRegExpValidator& validator) const;

	/*! \brief Get all library items.
	 *
	 * \return QList<LibraryItem*> contains pointers to all library items under this item.
	*/
	QList<LibraryItem*> getLibraries() const;

	/*! \brief Get all name items.
	 *
	 * \return QList<LibraryItem*> contains pointers to all name items under this item.
	*/
	QList<LibraryItem*> getNames() const;

	/*! \brief Get all version items.
	 *
	 * \return QList<LibraryItem*> contains pointers to all version items under this item.
	*/
	QList<LibraryItem*> getVersions() const;

private:

	//! \brief No copying
	LibraryItem(const LibraryItem &other);

	//! \brief No assignment
	LibraryItem &operator=(const LibraryItem &other);

	/*! \brief The name of the item on the level (i.e. TUT in vendor-level)
	 *
	 */
	QString name_;

	/*! \brief The level of the item
	 *
	 */
	Level level_;

	/*! \brief QList contains child items of current item in the hierarchy
	 *
	 */
	QList<LibraryItem*> childItems_;

	/*! \brief Pointer to the parent of the item
	 *
	 */
	LibraryItem *parentItem_;

	/*! \brief A VLNV tag that matches the version
	 *
	 */
	VLNV vlnv_;
};

#endif /* LIBRARYITEM_H_ */
