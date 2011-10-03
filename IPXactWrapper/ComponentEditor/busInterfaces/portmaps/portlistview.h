/* 
 *
 *  Created on: 10.5.2011
 *      Author: Antti Kamppi
 * 		filename: portlistview.h
 */

#ifndef PORTLISTVIEW_H
#define PORTLISTVIEW_H

#include <QListView>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QModelIndex>
#include <QPoint>
#include <QSortFilterProxyModel>

/*! \brief View to display a list of ports.
 *
 */
class PortListView : public QListView {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	PortListView(QWidget *parent);
	
	//! \brief The destructor
	virtual ~PortListView();

	/*! \brief Get names of the currently selected ports.
	 * 
	 * \param remove If true then the selected ports are also removed.
	 * 
	 * \return QStringList containing the names of the ports that are selected.
	*/
	QStringList getSelectedPorts(bool remove);

signals:

	//! \brief Emitted when port is dropped on the widget.
	//void portDropped(const QString& droppedPort, const QModelIndex& dropIndex);

	//! \brief Emitted when port should be removed from the list.
	void removeItem(const QModelIndex& index) const;

	//! \brief Emitted when a group of ports should be removed from list.
	void removeItems(const QModelIndexList& indexes) const;

	//! \brief Emitted when group of ports should be moved to another location.
	void moveItems(const QStringList& portNames, const QModelIndex& targetIndex) const;

	//! \brief Emitted when drop event occurs to inform connection between signals.
	void makeConnection(const QStringList& physicals, const QStringList& logicals) const;

	//! \brief Emitted when one or more ports are dropped on the widget and mode is one to many
	//void portsDropped(const QStringList& ports, const QModelIndex& dropIndex);

	//! \brief Emitted when one or more ports are dropped on the widget and mode is one to one
	//void portsDropped(const QStringList& ports, const QModelIndexList& indexes);

protected:

	/*! \brief Handler for drag enter events
	 *
	 * \param event Pointer to the event object.
	 *
	*/
	virtual void dragEnterEvent(QDragEnterEvent* event);

	/*! \brief Handler for drag move events on drag & drop.
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
	
	/*! \brief Handler for mouse press events.
	 *
	 * \param event Pointer to the event object.
	 *
	*/
	virtual void mousePressEvent(QMouseEvent* event);

private:

	//! \brief Initiates a drag operation from this view.
	void performDrag();

	//! \brief No copying
	PortListView(const PortListView& other);

	//! No assignment
	PortListView& operator=(const PortListView& other);

	//! \brief The position where mouse press happened to initiate a drag event.
	QPoint startPos_;
};

#endif // PORTLISTVIEW_H
