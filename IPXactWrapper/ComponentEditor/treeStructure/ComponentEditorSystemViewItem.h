//-----------------------------------------------------------------------------
// File: ComponentEditorSystemViewItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 13.7.2012
//
// Description:
// The item for a single system view in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORSYSTEMVIEWITEM_H
#define COMPONENTEDITORSYSTEMVIEWITEM_H

#include "componenteditoritem.h"
#include <IPXactWrapper/ComponentEditor/software/SystemViewEditor.h>
#include <models/SystemView.h>

/*! \brief The item for a single system view in the component editor's navigation tree.
 *
 */
class ComponentEditorSystemViewItem : public ComponentEditorItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 * 
	 * \param systemView Pointer to the system view to edit.
	 * \param model Pointer to the model that owns the items.
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the parent item.
	 *
	*/
	ComponentEditorSystemViewItem(QSharedPointer<SystemView> systemView,
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorSystemViewItem();

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

	/*! \brief Tells if the item can be opened or not.
	 * 
	 * If the view hasn't changed then it can be opened.
	*/
	virtual bool canBeOpened() const;

public slots:
	
	/*! \brief Open the view in a design editor.
	 * 
	*/
	virtual void openItem(bool builtinEditor = false);

private:
	//! \brief No copying
	ComponentEditorSystemViewItem(const ComponentEditorSystemViewItem& other);

	//! \brief No assignment
	ComponentEditorSystemViewItem& operator=(const ComponentEditorSystemViewItem& other);

	//! \brief Pointer to the view being edited.
	QSharedPointer<SystemView> systemView_;

	//! \brief The editor to edit the system view.
	SystemViewEditor editor_;
};

#endif // COMPONENTEDITORSYSTEMVIEWITEM_H
