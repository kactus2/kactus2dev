/* 
 *  	Created on: 10.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditoraddrspaceitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORADDRSPACEITEM_H
#define COMPONENTEDITORADDRSPACEITEM_H

#include "componenteditoritem.h"
#include <models/addressspace.h>
#include <models/memorymap.h>
#include <IPXactWrapper/ComponentEditor/addressSpaces/addressspaceeditor.h>

class LocalMemoryMapGraphItem;
class MemoryMapsVisualizer;

/*! \brief The item for a single address space in the component editor's navigation tree.
 *
 */
class ComponentEditorAddrSpaceItem : public ComponentEditorItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 * 
	 * \param addrSpace Pointer to the address space being edited.
	 * \param model Pointer to the model that owns the items.
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the parent item.
	 *
	*/
	ComponentEditorAddrSpaceItem(QSharedPointer<AddressSpace> addrSpace,
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorAddrSpaceItem();

	/*! \brief Get the font to be used for text of this item.
	*
	* \return QFont instance that defines the font to be used.
	*/
	virtual QFont getFont() const;

	/*! \brief Get the tool tip for the item.
	 * 
	 * \return The text for the tool tip to print to user.
	*/
	virtual QString getTooltip() const;

	/*! \brief Get the text to be displayed to user in the tree for this item.
	 *
	 * \return QString Contains the text to display.
	*/
	virtual QString text() const;

	/*! \brief Check the validity of this item and sub items.
	 *
	 * \return bool True if item is in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Get pointer to the editor of this item.
	 *
	 * \return Pointer to the editor to use for this item.
	*/
	virtual ItemEditor* editor();

	/*! \brief Get pointer to the editor of this item.
	 *
	 * \return Pointer to the editor to use for this item.
	*/
	virtual const ItemEditor* editor() const;

	/*! \brief Add a new child to the item.
	 * 
	 * \param index The index to add the child into.
	 *
	*/
	virtual void createChild(int index);
	
	/*! \brief Get the visualizer graphics item for the memory map.
	 *
	 * \return QGraphicsItem* Pointer to the graphics item.
	*/
	virtual QGraphicsItem* getGraphicsItem();

	/*! \brief Update the graphics item of the memory map.
	 *
	*/
	virtual void updateGraphics();

	/*! \brief Remove the graphics item of the memory map.
	 *
	*/
	virtual void removeGraphicsItem();

private:
	//! \brief No copying
	ComponentEditorAddrSpaceItem(const ComponentEditorAddrSpaceItem& other);

	//! \brief No assignment
	ComponentEditorAddrSpaceItem& operator=(const ComponentEditorAddrSpaceItem& other);

	//! \brief Pointer to the address space being edited.
	QSharedPointer<AddressSpace> addrSpace_;

	//! \brief Pointer to the local memory map of the address space.
	QSharedPointer<MemoryMap> localMemMap_;

	//! \brief Contains the address blocks that are children of this tree item.
	QList<QSharedPointer<MemoryMapItem> >& items_;

	//! \brief The editor used to edit the address space.
	AddressSpaceEditor editor_;

	//! \brief Pointer to the graph item of the local memory map.
	LocalMemoryMapGraphItem* graphItem_;

	//! \brief Pointer to the visualizer for the local memory maps.
	MemoryMapsVisualizer* localMemMapVisualizer_;
};

#endif // COMPONENTEDITORADDRSPACEITEM_H
