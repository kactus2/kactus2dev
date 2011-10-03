/* 
 *
 *  Created on: 27.4.2011
 *      Author: Antti Kamppi
 * 		filename: portmapsview.h
 */

#ifndef PORTMAPSVIEW_H
#define PORTMAPSVIEW_H

#include <QTableView>
#include <QModelIndex>
#include <QKeyEvent>
#include <QSortFilterProxyModel>
#include <QContextMenuEvent>
#include <QAction>

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDragMoveEvent>
#include <QPoint>

/*! \brief The view to display the port maps of a bus interface.
 *
 */
class PortMapsView : public QTableView {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param proxy Pointer to the proxy model that does the sorting of items.
	 * \param parent Pointer to the owner of this view.
	 *
	*/
	PortMapsView(QSortFilterProxyModel* proxy, QWidget *parent);
	
	//! \brief The destructor
	virtual ~PortMapsView();

signals:

	/*! \brief Remove the given index.
	 *
	 * \param index The model index identifying the item to remove.
	*/
	void removeItems(const QModelIndex& index);

	/*! \brief Emitted when user wants to restore a port back to port list.
	 *
	 * \param index Model index that indentifies the selected port.
	 *
	*/
	void restoreItem(const QModelIndex& index);

protected:

	/*! \brief Handler for key press events
	 *
	 * \param event Pointer to the event object.
	 *
	*/
	virtual void keyPressEvent(QKeyEvent* event);

	/*! \brief Handler for context menu events
	 *
	 * \param event Pointer to the event object
	 *
	*/
	virtual void contextMenuEvent(QContextMenuEvent* event);

	/*! \brief Handler for drag move event on drag & drop
	 *
	 * \param e Pointer to the event object.
	 *
	*/
	virtual void dragMoveEvent(QDragMoveEvent* e);

	/*! \brief Handler for mouse move events.
	 *
	 * \param event Pointer to the event object.
	 *
	*/
	virtual void mouseMoveEvent(QMouseEvent* event);

	/*! \brief Handler for mouse press events
	 *
	 * \param event Pointer to the event object.
	 *
	*/
	virtual void mousePressEvent(QMouseEvent* event);

private slots:

	//! \brief Handler for remove action triggers.
	void onRemoveItem();

	//! \brief Handler for restore action triggers.
	void onRestoreItem();

private:

	//! \brief No copying
	PortMapsView(const PortMapsView& other);

	//! No assignment
	PortMapsView& operator=(const PortMapsView& other);

	//! \brief Initiates a drag operation from this view.
	void performDrag();
	
	//! \brief Pointer to the instance that does the sorting for this view.
	QSortFilterProxyModel* proxy_;

	//! \brief The action to remove the selected item.
	QAction removeAction_;

	//! \brief The action to restore a port to the original list.
	QAction restoreAction_;

	//! \brief The index that was selected when menu was requested.
	QModelIndex menuTarget_;

	//! \brief The position where mouse press happened to initiate a drag event.
	QPoint startPos_;
};

#endif // PORTMAPSVIEW_H
