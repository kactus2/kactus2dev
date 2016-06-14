//-----------------------------------------------------------------------------
// File: PortMapTreeView.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.05.2016
//
// Description:
// A view for bus interface port maps.
//-----------------------------------------------------------------------------

#include "PortMapTreeView.h"

#include <editors/ComponentEditor/busInterfaces/portmaps/PortMappingTableView.h>
#include <editors/ComponentEditor/busInterfaces/portmaps/PortMapsColumns.h>

#include <IPXACTmodels/Component/PortMap.h>

#include <QHeaderView>
#include <QPainter>
#include <QMenu>
#include <QApplication>
#include <QDropEvent>
#include <QMimeData>
#include <QSortFilterProxyModel>

//-----------------------------------------------------------------------------
// Function: PortMapTreeView::PortMapTreeView()
//-----------------------------------------------------------------------------
PortMapTreeView::PortMapTreeView(QWidget* parent):
ConfigurableElementsView(parent),
removeAllMapsAction_(tr("Remove all port maps"), this),
addMapAction_(tr("Add port map"), this),
expandAllItemsAction_(tr("Expand all"), this),
collapseAllItemsAction_(tr("Collapse all"), this),
autoConnectAction_(tr("Auto connect"), this)
{
    getRemoveAction()->setText("Remove port map");

    header()->setSectionResizeMode(QHeaderView::Interactive);
    header()->setStretchLastSection(true);
    
    setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked |
        QAbstractItemView::EditKeyPressed | QAbstractItemView::AnyKeyPressed);

    setAlternatingRowColors(false);
    setSortingEnabled(true);
    setAcceptDrops(true);
    viewport()->setAcceptDrops(true);
    setDropIndicatorShown(true);   
    setDragDropMode(QAbstractItemView::DropOnly);

    setSelectionMode(QAbstractItemView::ExtendedSelection);

    setupActions();
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeView::~PortMapTreeView()
//-----------------------------------------------------------------------------
PortMapTreeView::~PortMapTreeView()
{

}

//-----------------------------------------------------------------------------
// Function: PortMapTreeView::setupActions()
//-----------------------------------------------------------------------------
void PortMapTreeView::setupActions()
{
    removeAllMapsAction_.setToolTip(tr("Remove all port maps."));
    removeAllMapsAction_.setStatusTip(tr("Remove all port maps."));
    connect(&removeAllMapsAction_, SIGNAL(triggered()), this, SLOT(onRemoveAllPortMaps()), Qt::UniqueConnection);

    getRemoveAction()->setToolTip(tr("Remove the selected port map."));
    getRemoveAction()->setStatusTip(tr("Remove the selected port map."));
    disconnect(getRemoveAction(), 0, this, 0);
    connect(getRemoveAction(), SIGNAL(triggered()), this, SLOT(onRemovePortMap()), Qt::UniqueConnection);

    addMapAction_.setToolTip(tr("Add a new port map to the selected logical port."));
    addMapAction_.setStatusTip(tr("Add a new port map to the selected logical port."));
    connect(&addMapAction_, SIGNAL(triggered()), this, SLOT(onAddPortMap()), Qt::UniqueConnection);

    expandAllItemsAction_.setToolTip(tr("Expand all port maps."));
    expandAllItemsAction_.setStatusTip(tr("Expand all port maps."));
    connect(&expandAllItemsAction_, SIGNAL(triggered()), this, SLOT(onExpandAll()), Qt::UniqueConnection);

    collapseAllItemsAction_.setToolTip(tr("Collapse all port maps."));
    collapseAllItemsAction_.setStatusTip(tr("Collapse all port maps."));
    connect(&collapseAllItemsAction_, SIGNAL(triggered()), this, SLOT(onCollapseAll()), Qt::UniqueConnection);

    autoConnectAction_.setToolTip(
        tr("Automatically create a port map from this logical signal and physical port."));
    autoConnectAction_.setStatusTip(
        tr("Automatically create a port map from this logical signal and physical port."));
    connect(&autoConnectAction_, SIGNAL(triggered()), this, SLOT(onAutoConnect()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeView::keyPressEvent()
//-----------------------------------------------------------------------------
void PortMapTreeView::keyPressEvent(QKeyEvent* event)
{
    // When delete is pressed, attempt to delete the selected mapping.
    if (event->key() == Qt::Key_Delete)
    {
        QModelIndexList indexList = selectedIndexes();
        if (!indexList.isEmpty())
        {
            foreach (QModelIndex index, indexList)
            {
                model()->setData(index, QVariant(), Qt::EditRole);
            }
        }
    }
    else
    {
        QTreeView::keyPressEvent(event);
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeView::contextMenuEvent()
//-----------------------------------------------------------------------------
void PortMapTreeView::contextMenuEvent(QContextMenuEvent* event)
{
    QModelIndex contextMenuIndex = indexAt(event->pos());

    QMenu contextMenu(this);

    if (contextMenuIndex.isValid())
    {
        contextMenu.addAction(&addMapAction_);

        if (!contextMenuIndex.parent().isValid())
        {
            contextMenu.addAction(&removeAllMapsAction_);
            contextMenu.addAction(&autoConnectAction_);
        }
        else
        {
            contextMenu.addAction(getRemoveAction());
        }

        contextMenu.addSeparator();
    }

    contextMenu.addAction(&expandAllItemsAction_);
    contextMenu.addAction(&collapseAllItemsAction_);

    contextMenu.exec(event->globalPos());

    event->accept();
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeView::onRemoveAllPortMaps()
//-----------------------------------------------------------------------------
void PortMapTreeView::onRemoveAllPortMaps()
{
    QModelIndexList indexList = selectedIndexes();

    if (!indexList.isEmpty())
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);

        qSort(indexList);

        clearSelection();

        QSortFilterProxyModel* sortProxy = dynamic_cast<QSortFilterProxyModel*>(model());
        foreach (QModelIndex index, indexList)
        {
            if (sortProxy != 0)
            {
                index = sortProxy->mapToSource(index);
            }

            if (!index.parent().isValid())
            {
                emit removeAllChildItemsFromIndex(index);
            }
        }

        setCurrentIndex(QModelIndex());

        QApplication::restoreOverrideCursor();
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeView::onRemovePortMap()
//-----------------------------------------------------------------------------
void PortMapTreeView::onRemovePortMap()
{
    QModelIndexList removedIndexes = selectedIndexes();

    if (!removedIndexes.isEmpty())
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);

        qSort(removedIndexes);

        int previousRow = removedIndexes.first().row();
        int rowCount = 1;

        foreach (QModelIndex index, removedIndexes)
        {
            if (previousRow != index.row())
            {
                ++rowCount;
            }
            previousRow = index.row();
        }

        QSortFilterProxyModel* sortProxy = dynamic_cast<QSortFilterProxyModel*>(model());

        for (int i = 0; i < rowCount; ++i)
        {
            QModelIndex index = removedIndexes.first();

            if (sortProxy != 0)
            {
                index = sortProxy->mapToSource(index);
            }

            if (index.parent().isValid())
            {
                emit removeItem(index);
            }
        }

        clearSelection();
        setCurrentIndex(QModelIndex());

        QApplication::restoreOverrideCursor();
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeView::onAddPortMap()
//-----------------------------------------------------------------------------
void PortMapTreeView::onAddPortMap()
{
    QModelIndexList indexes = selectedIndexes();
    QModelIndex positionToAdd;

    int rowCount = 1;

    if (!indexes.isEmpty())
    {
        qSort(indexes);

        positionToAdd = indexes.first();

        QSortFilterProxyModel* sortProxy = dynamic_cast<QSortFilterProxyModel*>(model());
        if (sortProxy)
        {
            positionToAdd = sortProxy->mapToSource(positionToAdd);
        }

        int previousRow = indexes.first().row();
        foreach (QModelIndex index, indexes)
        {
            if (index.row() != previousRow)
            {
                ++rowCount;
            }

            previousRow = index.row();
        }
    }

    for (int i = 0; i < rowCount; ++i)
    {
        emit (addItem(positionToAdd));
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeView::onExpandAll()
//-----------------------------------------------------------------------------
void PortMapTreeView::onExpandAll()
{
    expandAll();
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeView::onCollapseAll()
//-----------------------------------------------------------------------------
void PortMapTreeView::onCollapseAll()
{
    collapseAll();
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeView::onAutoConnect()
//-----------------------------------------------------------------------------
void PortMapTreeView::onAutoConnect()
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
// Function: PortMapTreeView::dragEnterEvent()
//-----------------------------------------------------------------------------
void PortMapTreeView::dragEnterEvent(QDragEnterEvent *event)
{
    PortMappingTableView* source = qobject_cast<PortMappingTableView*>(event->source());

    // if source is port list view
    if (source && event->mimeData()->hasFormat("text/plain"))
    {
        event->acceptProposedAction();
    }

    ConfigurableElementsView::dragEnterEvent(event);
}
