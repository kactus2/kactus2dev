//-----------------------------------------------------------------------------
// File: ComponentEditorTreeView.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 21.01.2011
//
// Description:
// brief ComponentTreeView is a widget to display the navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTTREEVIEW_H
#define COMPONENTTREEVIEW_H

#include <common/views/ExpandingTreeView/ExpandingTreeView.h>

#include <IPXACTmodels/common/VLNV.h>

#include <QTreeView>
#include <QModelIndex>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QContextMenuEvent>
#include <QAction>

class LibraryInterface;
class ComponentEditorItem;
/*! \brief ComponentTreeView is a widget to display the navigation tree.
 * 
 * This tree view displays a navigation tree in IPXactComponentEditor that
 * is used to navigate between IP-Xact elements.
 */

//-----------------------------------------------------------------------------
//! ComponentTreeView is a widget to display the navigation tree.
//  This tree view displays a navigation tree in IPXactComponentEditor that
//  is used to navigate between IP-Xact elements.
//-----------------------------------------------------------------------------
class ComponentTreeView : public ExpandingTreeView
{
    Q_OBJECT

public:
    
	//! The default width of the navigation tree.
    enum
    {
        DEFAULT_WIDTH = 180
    };

    /*!
     *  The constructor.
     *
     *      @param [in] handler     Pointer to the instance that manages the library.
     *      @param [in] compVLNV    The VLNV of the component being edited.
     *      @param [in] parent      Pointer to the owner of this widget
     */
    ComponentTreeView(LibraryInterface* handler,
		const VLNV& compVLNV,
		QWidget *parent);

	/*!
     *  The destructor.
     */
     ~ComponentTreeView() final = default;

	/*!
     *  Set the locked state(enables / disables the dragging).
     * 
     *      @param [in] locked  The new locked status.
     */
	void setLocked(bool locked);

	/*!
     *  Select the specified item and deselect all others.
	 *
	 *      @param [in] index   Identifies the item to select.
     */
	void selectItem(const QModelIndex& index);

signals:

	/*!
     *  Emitted when user wants to move item from one place to another.
     */
	void moveItem(const QModelIndex& source, const QModelIndex& target);

protected:

	/*!
     *  Called when a new item becomes the current item.
	 *
	 *  This is reimplemented function that calls the base class implementation 
	 *  and also emits activated() signal for the current model index.
	 * 
	 *      @param [in] current     ModelIndex of the new item
	 *      @param [in] previous    ModelIndex of the previous item
     */
	virtual void currentChanged(const QModelIndex & current, const QModelIndex & previous);

	/*!
     *  Handler for mouse press events.
     */
	virtual void mousePressEvent(QMouseEvent* event);

	/*!
     *  Handler for mouse release events.
     */
	virtual void mouseReleaseEvent(QMouseEvent* event);

	/*!
     *  Handler for mouse move events.
     */
	virtual void mouseMoveEvent(QMouseEvent* event);

	/*!
     *  The context menu event handler.
     */
	virtual void contextMenuEvent(QContextMenuEvent* event);

	/*!
     *  The handler for mouse double clicks.
     */
	virtual void mouseDoubleClickEvent(QMouseEvent* event);

private slots:

    /*!
     *  Handle the expand of the selected item.
     */
    void onExpandItem();

    /*!
     *  Handle the collapse of the selected item.
     */
    void onCollapseItem();

private:

	//! No copying
	ComponentTreeView(const ComponentTreeView& other);

	//! No assignment
	ComponentTreeView& operator=(const ComponentTreeView& other);

    /*!
     *  Finds the clicked item.     
     *
     *      @return The clicked item.
     */
    ComponentEditorItem* getPressedItem();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
    
    //! The mouse position where the mouse was pressed.
	QPoint pressedPoint_;

    //! The currently selected context menu index.
    QModelIndex menuIndex_;

	//! The current locked state (enables/disables dragging)
	bool locked_;

	//! Pointer to the instance that manages the library.
	LibraryInterface* handler_;

	//! The vlnv of the component being edited.
	VLNV componentVLNV_;

    //! Action for displaying all child items.
    QAction* expandAllItemsAction_;

    //! Action for hiding all child items.
    QAction* collapseAllItemsAction_;

    //! Action for displaying all the child items of the selected item.
    QAction* itemExpandAction_;

    //! Action for hiding all the child items of the selected item.
    QAction* itemCollapseAction_;
};

#endif // COMPONENTTREEVIEW_H
