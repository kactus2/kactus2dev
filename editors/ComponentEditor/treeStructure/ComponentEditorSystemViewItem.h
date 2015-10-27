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

class SystemView;
//-----------------------------------------------------------------------------
//! The item for a single system view in the component editor's navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorSystemViewItem : public ComponentEditorItem
{
	Q_OBJECT

public:

	/*! The constructor
	 * 
	 *      @param [in] systemView  The system view to edit.
	 *      @param [in] model       The model that owns the items.
	 *      @param [in] libHandler  The instance that manages the library.
	 *      @param [in] component   The component being edited.
	 *      @param [in] parent      The parent item.
	*/
	ComponentEditorSystemViewItem(QSharedPointer<SystemView> systemView, ComponentEditorTreeModel* model,
		LibraryInterface* libHandler, QSharedPointer<Component> component, ComponentEditorItem* parent);

	//! The destructor
	virtual ~ComponentEditorSystemViewItem();

	/*! Get the tool tip for the item.
	 * 
	 *      @return The text for the tool tip to print to user.
	*/
	virtual QString getTooltip() const;

	/*! Get the text to be displayed to user in the tree for this item.
	 *
	 *      @return QString Contains the text to display.
	*/
	virtual QString text() const;

	/*! Check the validity of this item and sub items.
	 *
	 *      @return bool True if item is in valid state.
	*/
	virtual bool isValid() const;

	/*! Get pointer to the editor of this item.
	 *
	 *      @return The editor to use for this item.
	*/
	virtual ItemEditor* editor();

	/*! Tells if the item can be opened or not.
	 * 
	 * If the view hasn't changed then it can be opened.
	*/
	virtual bool canBeOpened() const;

    /*!
     *  Returns the possible actions for opening the item.
     *
     *      @return The actions to open the item.
     */
    virtual QList<QAction*> actions() const;

public slots:
	
	/*! Open the view in a design editor.
	 * 
	*/
	virtual void openItem();

private:
	//! No copying
	ComponentEditorSystemViewItem(const ComponentEditorSystemViewItem& other);

	//! No assignment
	ComponentEditorSystemViewItem& operator=(const ComponentEditorSystemViewItem& other);
   
	//! The view being edited.
	QSharedPointer<SystemView> systemView_;

    //! Action to open the system view for editing.
    QAction* editAction_;
};

#endif // COMPONENTEDITORSYSTEMVIEWITEM_H
