//-----------------------------------------------------------------------------
// File: AbstractionPortsView.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 21.6.2011
//
// Description:
// The view that displays the contents of the BusPortsModel.
//-----------------------------------------------------------------------------

#include "AbstractionPortsView.h"

#include "AbstractionPortsModel.h"

#include <QMenu>

//-----------------------------------------------------------------------------
// Function: AbstractionPortsView::AbstractionPortsView()
//-----------------------------------------------------------------------------
AbstractionPortsView::AbstractionPortsView(QWidget *parent):
EditableTableView(parent),
addMasterAction_(tr("Add master signal"), this),
addSlaveAction_(tr("Add slave signal"), this),
addSystemAction_(tr("Add system signal"), this),
addAllSystemsAction_(tr("Add all unconnected system signals"), this),
resetExtendPortsAction_(tr("Reset extend ports"), this)
{
    connect(&addMasterAction_, SIGNAL(triggered()), this, SIGNAL(addMaster()), Qt::UniqueConnection);
    connect(&addSlaveAction_, SIGNAL(triggered()), this, SIGNAL(addSlave()), Qt::UniqueConnection);
    connect(&addSystemAction_, SIGNAL(triggered()), this, SIGNAL(addSystem()), Qt::UniqueConnection);
    connect(&addAllSystemsAction_, SIGNAL(triggered()), this, SIGNAL(addAllSystems()), Qt::UniqueConnection);
    connect(&resetExtendPortsAction_, SIGNAL(triggered()), this, SIGNAL(resetExtendPorts()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsView::~AbstractionPortsView()
//-----------------------------------------------------------------------------
AbstractionPortsView::~AbstractionPortsView()
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsView::selected()
//-----------------------------------------------------------------------------
QModelIndexList AbstractionPortsView::selected() const
{
    return selectedIndexes();
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsView::contextMenuEvent()
//-----------------------------------------------------------------------------
void AbstractionPortsView::contextMenuEvent(QContextMenuEvent* event)
{
    pressedPoint_ = event->pos();

    QModelIndex index = indexAt(pressedPoint_);

    if (index.data(AbstractionPortsModel::isExtendLockedRole).toBool())
    {
        removeAction_.setDisabled(true);
    }
    else
    {
        removeAction_.setEnabled(true);
    }

    if (!(index.flags() & Qt::ItemIsEditable))
    {
        clearAction_.setDisabled(true);
    }
    else
    {
        clearAction_.setEnabled(true);
    }

    QMenu menu(this);
    if (index.isValid())
    {
        menu.addAction(&addMasterAction_);
        menu.addAction(&addSlaveAction_);
        menu.addAction(&addSystemAction_);
        menu.addAction(&addAllSystemsAction_);
        menu.addSeparator();
    }

    menu.addAction(&resetExtendPortsAction_);
    menu.addSeparator();

    menu.addAction(&addAction_);
    
    // if at least one valid item is selected
    if (index.isValid())
    {
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

