//-----------------------------------------------------------------------------
// File: AbstractionPortsView.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 21.6.2011
//
// Description:
// The view that displays the contents of the BusPortsModel.
//-----------------------------------------------------------------------------

#ifndef ABSTRACTIONPORTSVIEW_H
#define ABSTRACTIONPORTSVIEW_H

#include <common/views/EditableTableView/editabletableview.h>

#include <QAction>
#include <QContextMenuEvent>
#include <QModelIndexList>
#include <QTableView>

//-----------------------------------------------------------------------------
//! The view that displays the contents of the BusPortsModel.
//-----------------------------------------------------------------------------
class AbstractionPortsView : public EditableTableView
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] parent  The owner of this widget.
     */
    AbstractionPortsView(QWidget* parent);
	
	/*!
     *  The destructor.
     */
	virtual ~AbstractionPortsView();

	/*!
     *  Get list of currently selected indexes.
	 *
	 *      @return The model indexes of the selected items.
     */
	QModelIndexList selected() const;

signals:

    /*!
     *  Emitted when a master mode should be added to the selected signal.
     */
    void addMaster();

    /*!
     *  Emitted when a slave mode should be added to the selected signal.
     */
    void addSlave();

    /*!
     *  Emitted when a system mode should be added to the selected signal.
     */
    void addSystem();

    /*!
     *  Emitted when the missing system group modes should be added to the selected signal.
     */
    void addAllSystems();

    /*!
     *  Emitted when the extend ports need to be reloaded from the extend abstraction definition.
     */
    void resetExtendPorts();

protected:

	//! Handler for context menu events
	virtual void contextMenuEvent(QContextMenuEvent* e);

private:
	//! No copying
    AbstractionPortsView(const AbstractionPortsView& other);

	//! No assignment
    AbstractionPortsView& operator=(const AbstractionPortsView& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Action for adding a master mode for a signal.
    QAction addMasterAction_;

    //! Action for adding a slave mode for a signal.
    QAction addSlaveAction_;

    //! Action for adding a system mode for a signal.
    QAction addSystemAction_;

    //! Action for adding the missing system modes for a signal.
    QAction addAllSystemsAction_;

    //! Action for reloading the extended abstraction definition ports.
    QAction resetExtendPortsAction_;
};

#endif // ABSTRACTIONPORTSVIEW_H
