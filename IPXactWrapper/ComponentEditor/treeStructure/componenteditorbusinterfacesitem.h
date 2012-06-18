/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorbusinterfacesitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORBUSINTERFACESITEM_H
#define COMPONENTEDITORBUSINTERFACESITEM_H

#include "componenteditoritem.h"
#include <IPXactWrapper/ComponentEditor/busInterfaces/businterfaceseditor.h>

/*! \brief The Bus interfaces-item in the component editor's navigation tree.
 *
 */
class ComponentEditorBusInterfacesItem : public ComponentEditorItem {
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
	ComponentEditorBusInterfacesItem(ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorBusInterfacesItem();

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

protected slots:

	/*! \brief Handler for editor's contentChanged signal.
	 *
	*/
	virtual void onEditorChanged();

private:
	//! \brief No copying
	ComponentEditorBusInterfacesItem(const ComponentEditorBusInterfacesItem& other);

	//! \brief No assignment
	ComponentEditorBusInterfacesItem& operator=(const ComponentEditorBusInterfacesItem& other);

	//! \brief Contains the bus interfaces to edit.
	QMap<QString, QSharedPointer<BusInterface> >& busifs_;

	//! \brief The editor to add/remove/edit bus interfaces
	BusInterfacesEditor editor_;
};

#endif // COMPONENTEDITORBUSINTERFACESITEM_H
