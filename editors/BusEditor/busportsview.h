//-----------------------------------------------------------------------------
// File: busportsview.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 21.6.2011
//
// Description:
// The view that displays the contents of the BusPortsModel.
//-----------------------------------------------------------------------------

#ifndef BUSPORTSVIEW_H
#define BUSPORTSVIEW_H

#include <common/views/EditableTableView/editabletableview.h>

#include <QAction>
#include <QContextMenuEvent>
#include <QModelIndexList>
#include <QTableView>

//-----------------------------------------------------------------------------
//! The view that displays the contents of the BusPortsModel.
//-----------------------------------------------------------------------------
class BusPortsView : public EditableTableView
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param parent The owner of this widget.
	*/
	BusPortsView(QWidget* parent);
	
	//! The destructor
	virtual ~BusPortsView();

	/*! Get list of currently selected indexes.
	 *
	 *      @return The model indexes of the selected items.
	*/
	QModelIndexList selected() const;

signals:

	//! Emitted when missing modes should be added to the selected signal.
    void addSignalOptions();

protected:

	//! Handler for context menu events
	virtual void contextMenuEvent(QContextMenuEvent* e);

private:
	//! No copying
	BusPortsView(const BusPortsView& other);

	//! No assignment
	BusPortsView& operator=(const BusPortsView& other);	

    //! Action for adding missing modes for a signal.
    QAction addOptionsAction_;
};

#endif // BUSPORTSVIEW_H
