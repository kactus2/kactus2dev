/* 
 *  	Created on: 12.12.2011
 *      Author: Antti Kamppi
 * 		filename: editablelistview.h
 *		Project: Kactus 2
 */

#ifndef EDITABLELISTVIEW_H
#define EDITABLELISTVIEW_H

#include <QListView>
#include <QPoint>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QContextMenuEvent>
#include <QAction>

class EditableListView : public QListView {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this view.
	 *
	*/
	EditableListView(QWidget *parent);
	
	//! \brief The destructor
	virtual ~EditableListView();

signals:

	/*! \brief A new item should be added to given index.
	 *
	 * \param index The position where new item should be added at.
	 *
	*/
	void addItem(const QModelIndex& index);

	/*! \brief An item should be removed from the model.
	 * 
	 * \param index Identifies the item that should be removed.
	 *
	*/
	void removeItem(const QModelIndex& index);

	/*! \brief Move item to another position.
	 *
	 * \param originalPos Identifies the item that should be moved.
	 * \param newPos The new position the item should be moved to.
	 *
	*/
	void moveItem(const QModelIndex& originalPos, const QModelIndex& newPos);

protected:

	//! \brief Handler for mouse move events
	virtual void mouseMoveEvent(QMouseEvent* e);

	//! \brief Handler for key press events
	virtual void keyPressEvent(QKeyEvent* event);

	//! \brief Handler for mouse double click events
	virtual void mouseDoubleClickEvent(QMouseEvent* event);

	//! \brief Handler for mouse press events
	virtual void mousePressEvent(QMouseEvent* event);

	//! \brief Handler for mouse release events
	virtual void mouseReleaseEvent(QMouseEvent* event);

	//! \brief Handler for context menu events
	virtual void contextMenuEvent(QContextMenuEvent* event);

private slots:

	//! \brief Handler for add action
	void onAddAction();

	//! \brief Handler for remove action
	void onRemoveAction();

private:
	//! \brief No copying
	EditableListView(const EditableListView& other);

	//! \brief No assignment
	EditableListView& operator=(const EditableListView& other);

	//! \brief Set up the actions for the context menu
	void setupActions();

	//! \brief The point where mouse was clicked
	QPoint pressedPoint_;

	//! \brief Action to add new item
	QAction addAction_;

	//! \brief Action to remove item
	QAction removeAction_;
};

#endif // EDITABLELISTVIEW_H
