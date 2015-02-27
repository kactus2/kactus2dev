//-----------------------------------------------------------------------------
// File: PortsView.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 2.12.2013
//
// Description:
// View for port editor.
//-----------------------------------------------------------------------------

#include "PortsView.h"

#include "PortColumns.h"

#include <QMenu>

//-----------------------------------------------------------------------------
// Function: PortsView::PortsView()
//-----------------------------------------------------------------------------
PortsView::PortsView(QWidget *parent ):
    ColumnFreezableTable(2, QSharedPointer<EditableTableView> (new EditableTableView(parent)), parent),
    createBus_(tr("Create new bus definition"), this),
    createExistingBus_(tr("Use existing bus definition"),this)
{
    connect(&createBus_, SIGNAL(triggered()),
        this, SLOT(onCreateNewBus()), Qt::UniqueConnection);

    connect(&createExistingBus_, SIGNAL(triggered()),
        this, SLOT(onCreateExistingBus()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: PortsView::~PortsView()
//-----------------------------------------------------------------------------
PortsView::~PortsView() 
{
}

//-----------------------------------------------------------------------------
// Function: PortsView::onCreateBus()
//-----------------------------------------------------------------------------
void PortsView::onCreateNewBus()
{
    QStringList selectedPorts;
    foreach(QModelIndex portIndex, selectedIndexes())
    {   

        QString portName =  portIndex.sibling(portIndex.row(), PortColumns::NAME).data().toString();
        if (!selectedPorts.contains(portName))
        {
            selectedPorts.append(portName);
        }
    }

    emit createBus(selectedPorts);
}

//-----------------------------------------------------------------------------
// Function: PortsView::onCreateExistingBus()
//-----------------------------------------------------------------------------
void PortsView::onCreateExistingBus()
{
    QStringList selectedPorts;
    foreach(QModelIndex portIndex, selectedIndexes())
    {   

        QString portName =  portIndex.sibling(portIndex.row(), PortColumns::NAME).data().toString();
        if (!selectedPorts.contains(portName))
        {
            selectedPorts.append(portName);
        }
    }

    emit createExistingBus(selectedPorts);
}

//-----------------------------------------------------------------------------
// Function: PortsView::contextMenuEvent()
//-----------------------------------------------------------------------------
void PortsView::contextMenuEvent(QContextMenuEvent* event)
{
    pressedPoint_ = event->pos();

    QModelIndex index = indexAt(pressedPoint_);

    QMenu menu(this);
    menu.addAction(&addAction_);

    // if at least one valid item is selected
    if (index.isValid()) {
        menu.addAction(&removeAction_);
        menu.addAction(&clearAction_);
        menu.addAction(&copyAction_);
    }

    menu.addAction(&pasteAction_);
    
    if (index.isValid())
    {
        QMenu* createMenu = menu.addMenu(tr("New bus interface"));
        createMenu->addAction(&createBus_);    
        createMenu->addAction(&createExistingBus_);           
    }

    if (impExportable_) {
        menu.addAction(&importAction_);
        menu.addAction(&exportAction_);
    }

    menu.exec(event->globalPos());

    event->accept();
}



