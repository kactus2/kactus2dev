//-----------------------------------------------------------------------------
// File: ComponentEditorSystemViewsItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 13.7.2012
//
// Description:
// The System Views-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORSYSTEMVIEWSITEM_H
#define COMPONENTEDITORSYSTEMVIEWSITEM_H

#include "componenteditoritem.h"

/*! \brief The System Views-item in the component editor's navigation tree.
 *
 */
class ComponentEditorSystemViewsItem : public ComponentEditorItem {
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
	ComponentEditorSystemViewsItem(ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorSystemViewsItem();

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

private:
	//! \brief No copying
	ComponentEditorSystemViewsItem(const ComponentEditorSystemViewsItem& other);

	//! \brief No assignment
	ComponentEditorSystemViewsItem& operator=(const ComponentEditorSystemViewsItem& other);

	//! \brief Contains the system views being edited.
	QList<QSharedPointer<SystemView> >& systemViews_;
};

#endif // COMPONENTEDITORSYSTEMVIEWSITEM_H
