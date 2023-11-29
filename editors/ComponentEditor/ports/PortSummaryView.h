//-----------------------------------------------------------------------------
// File: PortSummaryView.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 29.11.2023
//
// Description:
// View for a summary of all ports in a component.
//-----------------------------------------------------------------------------

#ifndef PORT_SUMMARY_VIEW_H
#define PORT_SUMMARY_VIEW_H

#include "PortsView.h"

#include <QAction>

class BusInterfaceInterface;

//-----------------------------------------------------------------------------
// View for a summary of all ports in a component.
//-----------------------------------------------------------------------------
class PortSummaryView : public PortsView
{
	Q_OBJECT
public:


	/*!
	 *  The constructor.
	 *
     *      @param [in] nameColumn      Name column index of.
     *      @param [in] busInterface    Interface for accessing bus interfaces.
	 *      @param [in] parent          The parent widget.
	 */
    PortSummaryView(int nameColumn, BusInterfaceInterface* busInterface, QWidget *parent);

	//! The destructor.
	~PortSummaryView() = default;
	
signals:

	//! Emitted when a new wire should be created.
    void addWire();

    //! Emitted when a new transactional should be created.
	void addTransactional();

protected:

    //! Handler for context menu events.
	void contextMenuEvent(QContextMenuEvent* event) override;

private:

	//! Action for adding new wire ports.
	QAction wireAction_{ tr("Add wire") };

    //! Action for adding new transactional ports.
	QAction transactionalAction_{ tr("Add transactional") };


};

#endif // PORT_SUMMARY_VIEW_H
