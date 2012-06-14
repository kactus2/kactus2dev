/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorcpusitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORCPUSITEM_H
#define COMPONENTEDITORCPUSITEM_H

#include "componenteditoritem.h"
#include <IPXactWrapper/ComponentEditor/cpus/cpuseditor.h>

#include <QList>

/*! \brief The Cpus-item in the component editor's navigation tree.
 *
 */
class ComponentEditorCpusItem : public ComponentEditorItem {
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
	ComponentEditorCpusItem(ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorCpusItem();

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

private:
	//! \brief No copying
	ComponentEditorCpusItem(const ComponentEditorCpusItem& other);

	//! \brief No assignment
	ComponentEditorCpusItem& operator=(const ComponentEditorCpusItem& other);

	//! \brief The cpus being edited
	QList<QSharedPointer<Cpu> >& cpus_;

	//! \brief The editor to edit the cpus of the component.
	CpusEditor editor_;
};

#endif // COMPONENTEDITORCPUSITEM_H
