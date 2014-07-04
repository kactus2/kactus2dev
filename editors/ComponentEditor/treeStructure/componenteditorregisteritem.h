/* 
 *  	Created on: 24.8.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorregisteritem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORREGISTERITEM_H
#define COMPONENTEDITORREGISTERITEM_H

#include "componenteditoritem.h"
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/register.h>
#include <IPXACTmodels/field.h>

#include <QSharedPointer>
#include <QList>

class RegisterEditor;
class MemoryMapsVisualizer;
class RegisterGraphItem;

/*! \brief The item for single register in component editor's navigation tree.
 *
 */
class ComponentEditorRegisterItem : public ComponentEditorItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param reg Pointer to the register being edited.
	 * \param model Pointer to the model that owns the items.
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the parent item.
	 *
	*/
	ComponentEditorRegisterItem(QSharedPointer<Register> reg, 
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorRegisterItem();

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

	/*! \brief Add a new child to the item.
	 * 
	 * \param index The index to add the child into.
	 *
	*/
	virtual void createChild(int index);

	/*! \brief Get pointer to the visualizer of this item.
	 * 
	 * 
	 * \return Pointer to the visualizer to use for this item.
	*/
	virtual ItemVisualizer* visualizer();

	/*! \brief Set the visualizer for this item.
	 *
	 * \param visualizer Pointer to the visualizer.
	 *
	*/
	virtual void setVisualizer(MemoryMapsVisualizer* visualizer);

	/*! \brief Get the visualizer graphics item for the register.
	 *
	 * \return QGraphicsItem* Pointer to the graphics item.
	*/
	virtual QGraphicsItem* getGraphicsItem();

	/*! \brief Update the graphics item of the register.
	 *
	*/
	virtual void updateGraphics();

	/*! \brief Remove the graphics item of the register.
	 *
	*/
	virtual void removeGraphicsItem();

protected slots:

	/*! \brief Handler for editor's contentChanged signal.
	 *
	*/
	virtual void onEditorChanged();

private:
	
	//! \brief No copying
	ComponentEditorRegisterItem(const ComponentEditorRegisterItem& other);

	//! \brief No assignment
	ComponentEditorRegisterItem& operator=(const ComponentEditorRegisterItem& other);

	//! \brief Pointer to the register being edited.
	QSharedPointer<Register> reg_;

	//! \brief Contains the fields that belong to this register.
	QList<QSharedPointer<Field> >& fields_;

	//! \brief Pointer to the visualizer of memory maps.
	MemoryMapsVisualizer* visualizer_;

	//! \brief Pointer to the graph item that visualizes the register.
	RegisterGraphItem* graphItem_;
};

#endif // COMPONENTEDITORREGISTERITEM_H
