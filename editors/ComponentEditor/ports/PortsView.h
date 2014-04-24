//-----------------------------------------------------------------------------
// File: PortsView.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 2.12.2013
//
// Description:
// View for port editor.
//-----------------------------------------------------------------------------
#ifndef PortsView_H
#define PortsView_H

#include <common/views/EditableTableView/editabletableview.h>
#include <IPXACTmodels/port.h>
#include <IPXACTmodels/component.h>

#include <QAction>

//-----------------------------------------------------------------------------
// View for port editor.
//-----------------------------------------------------------------------------
class PortsView : public EditableTableView {
	Q_OBJECT

public:


	/*!
	 *  The constructor.
	 *
	 *      @param [in] parent   The parent widget.	 
	 */
	PortsView(QWidget *parent);

	/*!
	 *  The destructor.
	 */
	virtual ~PortsView();

signals:

    //! Emitted when new bus interface is selected from the context menu and a new bus definition must be created.
    void createBus(QStringList const& portNames);

    //! Emitted when new bus interface is selected from the context menu.
    void createExistingBus(QStringList const& portNames);

private slots:

    //! Handler for new bus definition and interface creation.     
    virtual void onCreateNewBus();

    //! Handler for new bus interface creation.
    virtual void onCreateExistingBus();

protected:

    //! Handler for context menu events.
    virtual void contextMenuEvent(QContextMenuEvent* event);

private:

    //! Action for creating a new bus definition and interface.
    QAction createBus_;

    //! Action for creating a new bus interface.
    QAction createExistingBus_;
};

#endif // PortsView_H
