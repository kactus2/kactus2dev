//-----------------------------------------------------------------------------
// File: PortSummaryView.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 29.11.2023
//
// Description:
// View for a summary of all ports in a component.
//-----------------------------------------------------------------------------

#include "PortSummaryView.h"

#include <QMenu>

//-----------------------------------------------------------------------------
// Function: PortSummaryView::PortSummaryView()
//-----------------------------------------------------------------------------
PortSummaryView::PortSummaryView(int nameColumn, BusInterfaceInterface* busInterface, QWidget *parent):
    PortsView(nameColumn, busInterface, parent)
{
    addAction_.setDisabled(true);

    connect(&wireAction_, SIGNAL(triggered(bool)), this, SIGNAL(addWire()), Qt::UniqueConnection);
    connect(&transactionalAction_, SIGNAL(triggered(bool)), this, SIGNAL(addTransactional()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: PortSummaryView::contextMenuEvent()
//-----------------------------------------------------------------------------
void PortSummaryView::contextMenuEvent(QContextMenuEvent* event)
{
    pressedPoint_ = event->pos();

    QModelIndex index = indexAt(pressedPoint_);

    QMenu menu(this);
    menu.addAction(&wireAction_);
    menu.addAction(&transactionalAction_);

    if (index.isValid())
    {
        menu.addAction(&removeAction_);
        menu.addAction(&clearAction_);
        menu.addAction(&cutAction_);
        menu.addAction(&copyAction_);
        menu.addAction(&pasteAction_);
    }

    menu.exec(event->globalPos());

    event->accept();
}
