//-----------------------------------------------------------------------------
// File: portlistview.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 10.05.2011
//
// Description:
// View to display a list of ports.
//-----------------------------------------------------------------------------

#ifndef PORTLISTVIEW_H
#define PORTLISTVIEW_H

#include <QListView>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QModelIndex>
#include <QPoint>

//-----------------------------------------------------------------------------
//! View to display a list of ports.
//-----------------------------------------------------------------------------
class PortListView : public QListView
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] parent The owner of this widget.
	*/
	PortListView(QWidget *parent);
	
	//! The destructor
	virtual ~PortListView();

	/*! Get names of the currently selected ports.
	 * 
	 *      @return QStringList containing the names of the ports that are selected.
	*/
    QStringList getSelectedPorts();

signals:

	//! Emitted when port should be removed from the list.
	void removeItem(QModelIndex const& index) const;

	//! Emitted when a group of ports should be removed from list.
	void removeItems(const QModelIndexList& indexes) const;

	//! Emitted when group of ports should be moved to another location.
	void moveItems(const QStringList& portNames, QModelIndex const& targetIndex) const;

	//! Emitted when drop event occurs to inform connection between signals.
	void makeConnection(const QStringList& physicals, const QStringList& logicals) const;

protected:

	/*! Handler for drag enter events
	 *
	 *      @param [in] event The event object.
	*/
	virtual void dragEnterEvent(QDragEnterEvent* event);

	/*! Handler for drag move events on drag & drop.
	 *
	 *      @param [in] e The event object.
	*/
	virtual void dragMoveEvent(QDragMoveEvent* e);

	/*! Handler for mouse move events.
	 *
	 *      @param [in] event The event object.
	*/
	virtual void mouseMoveEvent(QMouseEvent* event);
	
	/*! Handler for mouse press events.
	 *
	 *      @param [in] event The event object.
	*/
	virtual void mousePressEvent(QMouseEvent* event);

    //! Initiates a drag operation from this view.
    virtual void performDrag();

    //! The position where mouse press happened to initiate a drag event.
    QPoint startPos_;

private:

	//! No copying
	PortListView(const PortListView& other);
	PortListView& operator=(const PortListView& other);

};

#endif // PORTLISTVIEW_H
