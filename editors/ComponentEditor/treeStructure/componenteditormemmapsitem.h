/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditormemmapsitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORMEMMAPSITEM_H
#define COMPONENTEDITORMEMMAPSITEM_H

#include "componenteditoritem.h"

class MemoryMapsEditor;
class ComponentEditorTreeModel;
class MemoryMapsVisualizer;

/*! \brief The Memory maps-item in the component navigation tree.
 *
 */
class ComponentEditorMemMapsItem : public ComponentEditorItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param model Pointer to the model that owns the items.
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the parent item.
	 *
	*/
	ComponentEditorMemMapsItem(ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorMemMapsItem();

	/*! \brief Get the font to be used for text of this item.
	*
	* The font is bold, if memory maps exist, otherwise not bold.
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

	/*! \brief Get pointer to the editor of this item.
	 *
	 * \return Pointer to the editor to use for this item.
	*/
	virtual ItemEditor* editor();

	/*! \brief Add a new child to the item.
	 * 
	 * \param index The index to add the child into.
	 *
	*/
	virtual void createChild(int index);

	/*! \brief Get pointer to the visualizer of memory maps.
	 * 
	 * \return Pointer to the visualizer to use for memory maps.
	*/
	virtual ItemVisualizer* visualizer();

private:
	//! \brief No copying
	ComponentEditorMemMapsItem(const ComponentEditorMemMapsItem& other);

	//! \brief No assignment
	ComponentEditorMemMapsItem& operator=(const ComponentEditorMemMapsItem& other);

	//! \brief Contains the memory maps being edited.
	QList<QSharedPointer<MemoryMap> >& memoryMaps_;

	//! \brief The visualizer to display the memory maps
	MemoryMapsVisualizer* visualizer_;
};

#endif // COMPONENTEDITORMEMMAPSITEM_H
