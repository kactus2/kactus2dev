//-----------------------------------------------------------------------------
// File: componenteditorcominterfaceitem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 16.5.2012
//
// Description:
// The item for a single COM interface in component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORCOMINTERFACEITEM_H
#define COMPONENTEDITORCOMINTERFACEITEM_H

#include "componenteditoritem.h"

class ComInterface;
//-----------------------------------------------------------------------------
//! The item for a single COM interface in component editor's navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorComInterfaceItem : public ComponentEditorItem
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] comInterface    The COM interface being edited.
	 *      @param [in] model           The model that owns the items.
	 *      @param [in] libHandler      The instance that manages the library.
	 *      @param [in] component       The component being edited.
	 *      @param [in] parent          The parent item.
	 *
	*/
	ComponentEditorComInterfaceItem(QSharedPointer<ComInterface> comInterface,
		ComponentEditorTreeModel* model, LibraryInterface* libHandler,
		QSharedPointer<Component> component, ComponentEditorItem* parent);

	//! The destructor
	virtual ~ComponentEditorComInterfaceItem();

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

	/*! Get The editor of this item.
	 *
	 *      @return The editor to use for this item.
	*/
	virtual ItemEditor* editor();

	/*! Tells if the item can be opened or not.
	 * 
	 * If the com interface contains valid com definition reference then it can be opened.
	*/
	virtual bool canBeOpened() const;

    /*!
     *  Returns the possible actions for opening the item.
     *
     *      @return The actions to open the item.
     */
    virtual QList<QAction*> actions() const;

public slots:
	
	/*! Open the COM definition in an editor.
	 * 
	*/
	virtual void openItem();

private:
	//! No copying
	ComponentEditorComInterfaceItem(const ComponentEditorComInterfaceItem& other);

	//! No assignment
	ComponentEditorComInterfaceItem& operator=(const ComponentEditorComInterfaceItem& other);

	//! The com interface being edited.
	QSharedPointer<ComInterface> interface_;

    //! Action to open the COM interface for editing.
    QAction* editAction_;
};

#endif // COMPONENTEDITORCOMINTERFACEITEM_H
