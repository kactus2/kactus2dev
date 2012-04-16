/* 
 *
 *  Created on: 23.1.2011
 *      Author: Antti Kamppi
 */

#ifndef COMPONENTTREEITEM_H_
#define COMPONENTTREEITEM_H_

#include <models/component.h>

#include <QObject>
#include <QString>
#include <QList>
#include <QFont>

class LibraryInterface;
class ItemEditor;

/*! \brief ComponentTreeItem represents a single item in the ComponentTreeView.
*
* This class is used to handle a single item in ComponentTreeView and 
* ComponentTreeModel.
*/
class ComponentTreeItem: public QObject {

	Q_OBJECT

public:

	//! \brief TreeItemType specifies the type of the ComponentTreeItem.
	enum TreeItemType {
		COMPONENT,			// for the root item
		GENERAL,			// for the "General" item
		FILESETS,			// for "File sets" item
		FILESET,			// for an item for a single file set
		FILES,				// for "Files" item
		FILE,				// for a single file item
		FUNCTIONS,			// for "Functions" item
		FUNCTION,			// for a single Function item
		DEFAULTFILEBUILDERS,// for "Default file builders" item
		CHOICES,			// for "Choices" item
		CHOICE,				// for a single choice item
		MODELPARAMETERS,	// for "Model parameters" item
		PARAMETERS,			// for "Parameters" item
		MEMORYMAPS,			// for "Memory maps" item
		MEMORYMAP,			// for a single memory map item
		ADDRESSBLOCKS,		// for "Address blocks" item
		ADDRESSBLOCK,		// for a single AddressBlock item
		BANKS,				// for "Banks" item
		BANK,				// for a single Bank item
		SUBSPACEMAPS,		// for "Subspace maps" item
		SUBSPACEMAP,		// for a single SubspaceMap item
		ADDRESSSPACES,		// for "Address spaces" item
		ADDRESSSPACE,		// for a single address space item
		REMAPSTATES,		// for "Remap states" item
		REMAPSTATE,			// for a single remap state item
		VIEWS,				// for "Views" item
		VIEW,				// for a single view item
		PORTS,				// for "Ports" item
		BUSINTERFACES,		// for "Bus interfaces" item
		BUSINTERFACE,		// for a single bus interface item
		CHANNELS,			// for "Channels" item
		CHANNEL,			// for a single channel item
		CPUS,				// for "Cpus" item
		CPU,				// for a single cpu item
		OTHERCLOCKDRIVERS,	// for "Other clock drivers" item
		COMPONENTGENERATORS,// for "Component generators" item
        SOFTWARE,           // for "Software Mappings" item
        COMINTERFACES,      // for "COM interfaces" item
        COMINTERFACE,       // for a single COM interface item
        APIINTERFACES,      // for "API interfaces" item
        APIINTERFACE,       // for a single API interface item
	};

	
	/*! \brief The constructor
	 *
	 * \param type The type for the component tree item.
	 * \param elementPointer Pointer to the IP-Xact element represented by this item.
	 * \param component Pointer to the component being edited.
	 * \param handler Pointer to the instance that manages the library.
	 * \param parentItem Pointer to the owner of this item.
	 *
	*/
	ComponentTreeItem(TreeItemType type, 
		void* elementPointer,
			QSharedPointer<Component> component,
			LibraryInterface* handler,
			ComponentTreeItem* parentItem);

	//! \brief The destructor
	virtual ~ComponentTreeItem();

	/*! \brief Get pointer to the child in specified index.
	 *
	 * \param index Specifies the index of the child.
	 */
	ComponentTreeItem* child(int index);

	//! \brief Get pointer to the parent of this instance.
	ComponentTreeItem* parent();

	/*! \brief Get row of this item.
	 *
	 * \return The row number of this item.
	 */
	int row();

	/*! \brief Get index of specified child.
	 *
	 * \param child Pointer to the child that's index is requested.
	 *
	 * \return The row number of the child.
	 */
	int getIndexOf(ComponentTreeItem* child);

	/*! \brief Get the text that is to be displayed by this item.
	 *
	 * \return QString containing the text.
	 */
	QString text() const;

	/*! \brief Set a new text to be displayed for the item.
	*
	* \param newText The new text to be displayed by the item.
	*/
	void setText(const QString newText);

	/*! \brief Get the number of rows that exist under this item.
	 *
	 * \return The number of children this item has.
	 */
	int rowCount() const;

	/*! \brief Tells if the item has child-items or not.
	 *
	 * \return True if children exist, otherwise false.
	 */
	bool hasChildren();

	/*! \brief Get pointer to the data-element represented by this item.
	 *
	 * \return void-pointer to the data-element.
	 */
	void* data();

	/*! \brief Get the font to be used for text of this item.
	*
	* \return QFont instance that defines the font to be used.
	*/
	QFont getFont() const;

	/*! \brief Add a new child for the item.
	*
	* After calling this function this ComponentTreeItem takes ownership of the 
	* given child.
	*
	* \param child Pointer to the ComponentTreeItem instance that is to be added
	* to the childs of this item.
	*/
	void addChild(ComponentTreeItem* child);

	/*! \brief Get the type of this item.
	*
	* \return TreeItemType that specifies the type.
	*/
	TreeItemType type() const;

	/*! \brief Get list of ComponentTreeItem's child items.
	*
	* \return QList containing pointers to the item's children.
	*/
	QList<ComponentTreeItem*> getChildItems();

	/*! \brief Check if the matching IP-Xact model is in valid state
	*
	* \return True if the model is valid. False if invalid.
	*/
	bool isModelValid() const;

	/*! \brief Check if both the IP-Xact model and the registered editor are valid.
	 *
	 * \return bool True if both are valid. False if either is invalid.
	*/
	bool isValid() const;

	/*! \brief Get the data pointer to the item that this item represents
	*
	* \return void* pointer that points to the correct IP-Xact element model.
	*/
	void* getDataPointer() const;

	/*! \brief Remove the specified child from the parent.
	 *
	 * Also all the child's own children will be deleted.

	 * \param childPointer Pointer to the child-instance to remove
	 *
	*/
	void removeChild(ComponentTreeItem* childPointer);

	/*! \brief Get the dataPointers of this item and it's sub items.
	 * 
	 * This function adds all the not null dataPointers of this item and it's 
	 * sub items. This function should be used when removing one item in the 
	 * tree so all the editors for removed tree-items can also be removed.
	 * 
	 * \param pointers Reference to the QList containing the dataPointers where
	 * each item appends it's dataPointer.
	 *
	*/
	void getDataPointers(QList<void*>& pointers);

	/*! \brief Create a new child for the item.
	 *
	 * This function creates a new child for the item. It also constructs a 
	 * new IP-Xact element for the child and sets the child's dataPointer to 
	 * point to this new IP-Xact model.
	 * 
	 * \return True if child was successfully created.
	*/
	bool createChild();

	/*! \brief Tells if the item can have children or not
	 *
	 * Some type of items can not have children. i.e type FILE is only a leaf
	 * item and it can't have children of it's own.
	 *
	 * \return True if the item can have children.
	*/
	bool canHaveChildren() const;

	/*! \brief Can the item represented by this item be removed from the tree
	 *
	 * \return True if the item is type that can be removed.
	*/
	bool canBeRemoved() const;

	/*! \brief Move this item's child from source index to target index.
	 *
	 * \param sourceIndex The index identifying the child to be move.
	 * \param targetIndex The target index where the child is moved to.
	 *
	*/
	void moveChild(int sourceIndex, int targetIndex);

	/*! \brief Sort the underlying IP-Xact model according to given id-list.
	 *
	 * \param idList Identifies the order to which sort the IP-Xact model.
	 *
	*/
	void sortModel(const QStringList& idList);

	/*! \brief Register the editor for the item.
	 * 
	 * Editor is used to check the validity of the item.
	 * 
	 * \param editor Pointer to the editor that matches this item.
	 *
	*/
	void registerEditor(ItemEditor* editor);

private:

	//! No copying
	ComponentTreeItem(const ComponentTreeItem& other);

	//! No assignment
	ComponentTreeItem& operator=(const ComponentTreeItem& other);

	//! \brief The type of the element that this item represents in a tree.
	TreeItemType type_;

	//! \brief The text to be displayed in the tree for this item.
	QString text_;
	
	//! \brief Pointer to the element that this item represents.
	void* dataPointer_;

	//! \brief Pointer to the component instance that is represented in model.
	QSharedPointer<Component> component_;

	//! \brief Pointer to the parent of this item.
	ComponentTreeItem* parent_;

	//! \brief List of child items for this item.
	QList<ComponentTreeItem*> childItems_;

	//! \brief Pointer to the instance that manages the library
	LibraryInterface* handler_;	

	//! \brief Pointer to the editor of the item.
	ItemEditor* editor_;
};

#endif /* COMPONENTTREEITEM_H_ */
