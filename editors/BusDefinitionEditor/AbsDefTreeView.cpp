//-----------------------------------------------------------------------------
// File: AbsDefTreeView.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 27.04.2017
//
// Description:
// A view for abstraction definitions.
//-----------------------------------------------------------------------------

#include "AbsDefTreeView.h"

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
// Function: AbsDefTreeView::AbsDefTreeView()
//-----------------------------------------------------------------------------
AbsDefTreeView::AbsDefTreeView(QWidget* parent):
ConfigurableElementsView(parent),
addAction_(tr("Add port map"), this),
expandAllItemsAction_(tr("Expand all"), this),
collapseAllItemsAction_(tr("Collapse all"), this),
addOptionsAction_(tr("Add signal options"), this)
{
    getRemoveAction()->setToolTip(tr("Remove the selection."));

    header()->setSectionResizeMode(QHeaderView::Interactive);
    header()->setStretchLastSection(true);
    
    setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked |
        QAbstractItemView::EditKeyPressed | QAbstractItemView::AnyKeyPressed);

    setAlternatingRowColors(false);
    setSortingEnabled(true);

    setSelectionMode(QAbstractItemView::ExtendedSelection);

    setupActions();
}

//-----------------------------------------------------------------------------
// Function: AbsDefTreeView::~AbsDefTreeView()
//-----------------------------------------------------------------------------
AbsDefTreeView::~AbsDefTreeView()
{

}

//-----------------------------------------------------------------------------
// Function: AbsDefTreeView::setupActions()
//-----------------------------------------------------------------------------
void AbsDefTreeView::setupActions()
{
    getRemoveAction()->setToolTip(tr("Remove the selection."));
    getRemoveAction()->setStatusTip(tr("Remove the selection."));
    disconnect(getRemoveAction(), 0, this, 0);
    connect(getRemoveAction(), SIGNAL(triggered()), this, SLOT(onRemove()), Qt::UniqueConnection);

    connect(&addAction_, SIGNAL(triggered()), this, SLOT(onAdd()), Qt::UniqueConnection);

    expandAllItemsAction_.setToolTip(tr("Expand all port maps."));
    expandAllItemsAction_.setStatusTip(tr("Expand all port maps."));
    connect(&expandAllItemsAction_, SIGNAL(triggered()), this, SLOT(onExpandAll()), Qt::UniqueConnection);

    collapseAllItemsAction_.setToolTip(tr("Collapse all port maps."));
    collapseAllItemsAction_.setStatusTip(tr("Collapse all port maps."));
    connect(&collapseAllItemsAction_, SIGNAL(triggered()), this, SLOT(onCollapseAll()), Qt::UniqueConnection);

    connect(&addOptionsAction_, SIGNAL(triggered()), this, SIGNAL(addSignalOptions()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: AbsDefTreeView::keyPressEvent()
//-----------------------------------------------------------------------------
void AbsDefTreeView::keyPressEvent(QKeyEvent* event)
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
// Function: AbsDefTreeView::contextMenuEvent()
//-----------------------------------------------------------------------------
void AbsDefTreeView::contextMenuEvent(QContextMenuEvent* event)
{
    QModelIndex contextMenuIndex = indexAt(event->pos());

    QMenu contextMenu(this);

    contextMenu.addAction(&addAction_);

    if (contextMenuIndex.isValid())
    {
        contextMenu.addAction(&addOptionsAction_);

        contextMenu.addSeparator();

        contextMenu.addAction(getRemoveAction());
    }

    contextMenu.addAction(&expandAllItemsAction_);
    contextMenu.addAction(&collapseAllItemsAction_);

    contextMenu.exec(event->globalPos());

    event->accept();
}

//-----------------------------------------------------------------------------
// Function: AbsDefTreeView::onExpandAll()
//-----------------------------------------------------------------------------
void AbsDefTreeView::onExpandAll()
{
    expandAll();
}

//-----------------------------------------------------------------------------
// Function: AbsDefTreeView::onCollapseAll()
//-----------------------------------------------------------------------------
void AbsDefTreeView::onCollapseAll()
{
    collapseAll();
}

//-----------------------------------------------------------------------------
// Function: AbsDefTreeView::onAdd()
//-----------------------------------------------------------------------------
void AbsDefTreeView::onAdd()
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
// Function: AbsDefTreeView::onRemove()
//-----------------------------------------------------------------------------
void AbsDefTreeView::onRemove()
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
            QModelIndex index = removedIndexes.at(i);

            if (sortProxy != 0)
            {
                index = sortProxy->mapToSource(index);
            }

            emit removeItem(index);
        }

        clearSelection();
        setCurrentIndex(QModelIndex());

        QApplication::restoreOverrideCursor();
    }
}