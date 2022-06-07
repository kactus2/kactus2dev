//-----------------------------------------------------------------------------
// File: PortMapView.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 31.05.2022
//
// Description:
// A view for bus interface port maps.
//-----------------------------------------------------------------------------

#include "PortMapView.h"

#include <editors/ComponentEditor/busInterfaces/portmaps/PortMappingTableView.h>
#include <editors/ComponentEditor/busInterfaces/portmaps/PortMapsColumns.h>

#include <QHeaderView>
#include <QMenu>
#include <QMimeData>

//-----------------------------------------------------------------------------
// Function: PortMapView::PortMapView()
//-----------------------------------------------------------------------------
PortMapView::PortMapView(PortMapInterface* portMapInterface, QWidget* parent):
EditableTableView(parent),
autoConnectAction_(tr("Auto connect"), this),
mapInterface_(portMapInterface)
{
    horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    horizontalHeader()->setStretchLastSection(true);
    
    setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked |
        QAbstractItemView::EditKeyPressed | QAbstractItemView::AnyKeyPressed);

    setAlternatingRowColors(false);
    setSortingEnabled(true);
    setAcceptDrops(true);
    viewport()->setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::DropOnly);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setItemsDraggable(false);

    setupActions();
}

//-----------------------------------------------------------------------------
// Function: PortMapView::setupActions()
//-----------------------------------------------------------------------------
void PortMapView::setupActions()
{
    autoConnectAction_.setToolTip(
        tr("Automatically create a port map from this logical signal and physical port."));
    autoConnectAction_.setStatusTip(
        tr("Automatically create a port map from this logical signal and physical port."));
    connect(&autoConnectAction_, SIGNAL(triggered()), this, SLOT(onAutoConnect()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: PortMapView::contextMenuEvent()
//-----------------------------------------------------------------------------
void PortMapView::contextMenuEvent(QContextMenuEvent* event)
{
    QModelIndex index = indexAt(event->pos());

    QMenu menu(this);
    menu.addAction(&addAction_);

    // if at least one valid item is selected
    if (index.isValid())
    {
        menu.addAction(&removeAction_);
        menu.addAction(&clearAction_);
        menu.addAction(&cutAction_);
        menu.addAction(&copyAction_);
        menu.addAction(&pasteAction_);
    }

    menu.addSeparator();
    menu.addAction(&autoConnectAction_);

    if (importExportAllowed())
    {
        menu.addSeparator();
        menu.addAction(&importAction_);
        menu.addAction(&exportAction_);
    }

    menu.exec(event->globalPos());

    event->accept();
}

//-----------------------------------------------------------------------------
// Function: PortMapView::onAutoConnect()
//-----------------------------------------------------------------------------
void PortMapView::onAutoConnect()
{
    QModelIndexList indexList = selectedIndexes();

    QStringList selectedLogicalSignals;

    foreach (QModelIndex index, indexList)
    {
        if (!index.parent().isValid())
        {
            QModelIndex logicalSibling = index.sibling(index.row(), PortMapsColumns::LOGICAL_PORT);
            QString logicalName = logicalSibling.data(Qt::DisplayRole).toString();

            selectedLogicalSignals.append(logicalName);
        }
    }

    emit autoConnecteLogicalSignals(selectedLogicalSignals);
}

//-----------------------------------------------------------------------------
// Function: PortMapView::dragEnterEvent()
//-----------------------------------------------------------------------------
void PortMapView::dragEnterEvent(QDragEnterEvent *event)
{
    PortMappingTableView* source = qobject_cast<PortMappingTableView*>(event->source());

    // if source is port list view
    if (source && event->mimeData()->hasFormat("text/plain"))
    {
        event->acceptProposedAction();
    }

    EditableTableView::dragEnterEvent(event);
}
