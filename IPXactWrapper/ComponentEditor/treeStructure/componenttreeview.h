/* 
 *
 *  Created on: 21.1.2011
 *      Author: Antti Kamppi
 */

#ifndef COMPONENTTREEVIEW_H
#define COMPONENTTREEVIEW_H

#include <LibraryManager/vlnv.h>

#include <QTreeView>
#include <QModelIndex>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QContextMenuEvent>
#include <QAction>

class LibraryInterface;

/*! \brief ComponentTreeView is a widget to display the navigation tree.
 * 
 * This tree view displays a navigation tree in IPXactComponentEditor that
 * is used to navigate between IP-Xact elements.
 */
class ComponentTreeView : public QTreeView {
    Q_OBJECT

public:

    /*! \brief The constructor
     *
     * \param handler Pointer to the instance that manages the library.
     * \param compVLNV The VLNV of the component being edited.
     * \param parent Pointer to the owner of this widget
     * 
     */
    ComponentTreeView(LibraryInterface* handler,
		const VLNV& compVLNV,
		QWidget *parent = 0);

	//! \brief The destructor
    virtual ~ComponentTreeView();

	//! \brief Set the locked state (enables/disables the dragging)
	void setLocked(bool locked);

	/*! \brief Select the specified item and deselect all others.
	 *
	 * \param index Identifies the item to select.
	 *
	*/
	void selectItem(const QModelIndex& index);

signals:

	//! \brief Emitted when user wants to move item from one place to another.
	void moveItem(const QModelIndex& source, const QModelIndex& target);

protected:

	/*! \brief Called when a new item becomes the current item.
	 *
	 * This is reimplemented function that calls the base class implementation 
	 * and also emits activated() signal for the current model index.
	 * 
	 * \param current ModelIndex of the new item
	 * \param previous ModelIndex of the previous item
	 *
	*/
	virtual void currentChanged(const QModelIndex & current, 
		const QModelIndex & previous);

	//! \brief Handler for mouse press events.
	virtual void mousePressEvent(QMouseEvent* event);

	//! \brief Handler for mouse release events.
	virtual void mouseReleaseEvent(QMouseEvent* event);

	//! \brief Handler for mouse move events
	virtual void mouseMoveEvent(QMouseEvent* event);

	//! \brief Handler for key press events
	virtual void keyPressEvent(QKeyEvent* event);

	//! \brief The context menu event handler
	virtual void contextMenuEvent(QContextMenuEvent* event);

	//! \brief The handler for mouse double clicks
	virtual void mouseDoubleClickEvent(QMouseEvent* event);

private slots:

	//! \brief Open an item at position pressedPoint_
	void onFileOpen();

private:

	//! No copying
	ComponentTreeView(const ComponentTreeView& other);

	//! No assignment
	ComponentTreeView& operator=(const ComponentTreeView& other);

	//! \brief The mouse position where the mouse was pressed.
	QPoint pressedPoint_;

	//! \brief The current locked state (enables/disables dragging)
	bool locked_;

	//! \brief The action to open files from the component editor.
	QAction fileOpenAction_;

	//! \brief Pointer to the instance that manages the library.
	LibraryInterface* handler_;

	//! \brief The vlnv of the component being edited.
	VLNV componentVLNV_;
};

#endif // COMPONENTTREEVIEW_H
