//-----------------------------------------------------------------------------
// File: PortMappingTableView.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 19.05.2016
//
// Description:
// View to display a table of ports for port mapping.
//-----------------------------------------------------------------------------

#ifndef PORTMAPPINGTABLEVIEW_H
#define PORTMAPPINGTABLEVIEW_H

#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QModelIndex>
#include <QPoint>
#include <QTableView>

//-----------------------------------------------------------------------------
//! View to display a table of ports for port mapping.
//-----------------------------------------------------------------------------
class PortMappingTableView : public QTableView
{
	Q_OBJECT

public:

	/*!
     *  The constructor
	 *
	 *      @param [in] parent  The owner of this widget.
	 */
	PortMappingTableView(QWidget *parent);
	
	/*!
     *  The destructor.
     */
    virtual ~PortMappingTableView();

	/*!
     *  Get names of the currently selected ports.
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

	/*!
     *  Handler for drag enter events.
	 *
	 *      @param [in] event   The event object.
	 */
	virtual void dragEnterEvent(QDragEnterEvent* event);

	/*!
     *  Handler for drag move events on drag & drop.
	 *
	 *      @param [in] e   The event object.
	 */
	virtual void dragMoveEvent(QDragMoveEvent* e);

	/*!
     *  Handler for mouse move events.
	 *
	 *      @param [in] event   The event object.
	 */
	virtual void mouseMoveEvent(QMouseEvent* event);
	
	/*!
     *  Handler for mouse press events.
	 *
	 *      @param [in] event   The event object.
	 */
	virtual void mousePressEvent(QMouseEvent* event);

    //! Initiates a drag operation from this view.
    virtual void performDrag();

    //! The position where mouse press happened to initiate a drag event.
    QPoint startPos_;

private:

	//! No copying.
	PortMappingTableView(const PortMappingTableView& other);
	PortMappingTableView& operator=(const PortMappingTableView& other);
};

#endif // PORTMAPPINGTABLEVIEW_H
