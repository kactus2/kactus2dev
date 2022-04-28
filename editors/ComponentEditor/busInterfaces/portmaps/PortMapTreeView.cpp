//-----------------------------------------------------------------------------
// File: PortMapTreeView.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 23.05.2016
//
// Description:
// A view for bus interface port maps.
//-----------------------------------------------------------------------------

#include "PortMapTreeView.h"

#include <editors/ComponentEditor/busInterfaces/portmaps/PortMappingTableView.h>
#include <editors/ComponentEditor/busInterfaces/portmaps/PortMapsColumns.h>
#include <editors/ComponentEditor/busInterfaces/portmaps/interfaces/PortMapInterface.h>

#include <IPXACTmodels/Component/PortMap.h>

#include <QHeaderView>
#include <QPainter>
#include <QMenu>
#include <QApplication>
#include <QDropEvent>
#include <QMimeData>
#include <QVariant>
#include <QSortFilterProxyModel>

//-----------------------------------------------------------------------------
// Function: PortMapTreeView::PortMapTreeView()
//-----------------------------------------------------------------------------
PortMapTreeView::PortMapTreeView(PortMapInterface* portMapInterface, QWidget* parent):
EditableTreeView(false, QString(""), QString(tr("Add port map")), QString(tr("Remove port map")),
    QString(tr("Remove all port maps")), parent),
autoConnectAction_(tr("Auto connect"), this),
mapInterface_(portMapInterface)
{
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
    getAddSubItemAction()->setToolTip(tr("Add a new port map to the selected logical port."));
    getAddSubItemAction()->setStatusTip(tr("Add a new port map to the selected logical port."));

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
        QSortFilterProxyModel* proxyModel = dynamic_cast<QSortFilterProxyModel*>(model());
        if (proxyModel)
        {
            contextMenuIndex = proxyModel->mapToSource(contextMenuIndex);
        }

        std::string abstractPortName = "";
        if (!contextMenuIndex.parent().isValid())
        {
            abstractPortName = mapInterface_->getIndexedItemName(contextMenuIndex.row());
        }
        else
        {
            abstractPortName = mapInterface_->getIndexedItemName(contextMenuIndex.parent().row());
        }

        if (mapInterface_->logicalPortExists(abstractPortName))
        {
            contextMenu.addAction(getAddSubItemAction());
        }

        if (!contextMenuIndex.parent().isValid())
        {
            contextMenu.addAction(getRemoveAllSubItemsAction());
            contextMenu.addAction(&autoConnectAction_);
        }
        else
        {
            contextMenu.addAction(getRemoveAction());
        }

        contextMenu.addSeparator();
    }

    contextMenu.addAction(getExpandAllAction());
    contextMenu.addAction(getCollapseAllAction());

    contextMenu.exec(event->globalPos());

    event->accept();
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

    EditableTreeView::dragEnterEvent(event);
}
