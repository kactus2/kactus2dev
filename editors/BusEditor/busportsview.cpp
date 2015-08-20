//-----------------------------------------------------------------------------
// File: busportsview.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 21.6.2011
//
// Description:
// The view that displays the contents of the BusPortsModel.
//-----------------------------------------------------------------------------

#include "busportsview.h"

#include <QMenu>

//-----------------------------------------------------------------------------
// Function: BusPortsView::BusPortsView()
//-----------------------------------------------------------------------------
BusPortsView::BusPortsView(QWidget *parent):
EditableTableView(parent),
addOptionsAction_(tr("Add signal options"), this)
{
    connect(&addOptionsAction_, SIGNAL(triggered()), this, SIGNAL(addSignalOptions()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: BusPortsView::~BusPortsView()
//-----------------------------------------------------------------------------
BusPortsView::~BusPortsView()
{

}

//-----------------------------------------------------------------------------
// Function: BusPortsView::selected()
//-----------------------------------------------------------------------------
QModelIndexList BusPortsView::selected() const
{
    return selectedIndexes();
}

//-----------------------------------------------------------------------------
// Function: BusPortsView::contextMenuEvent()
//-----------------------------------------------------------------------------
void BusPortsView::contextMenuEvent(QContextMenuEvent* event)
{
    pressedPoint_ = event->pos();

    QModelIndex index = indexAt(pressedPoint_);

    QMenu menu(this);
    if (index.isValid())
    {
        menu.addAction(&addOptionsAction_);
        menu.addSeparator();
    }
    menu.addAction(&addAction_);
    
    // if at least one valid item is selected
    if (index.isValid()) {        
        menu.addAction(&removeAction_);
        menu.addAction(&clearAction_);
        menu.addAction(&copyAction_);    
    }
    menu.addAction(&pasteAction_);

    if (importExportAllowed())
    {
        menu.addSeparator();
        menu.addAction(&importAction_);
        menu.addAction(&exportAction_);
    }

    menu.exec(event->globalPos());

    event->accept();
}

