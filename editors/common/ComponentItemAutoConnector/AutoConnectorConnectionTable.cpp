//-----------------------------------------------------------------------------
// File: AutoConnectorConnectionTable.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 11.02.2019
//
// Description:
// Table for connected items of two components.
//-----------------------------------------------------------------------------

#include "AutoConnectorConnectionTable.h"

#include <editors/common/ComponentItemAutoConnector/TableItemMatcher.h>

#include <IPXACTmodels/Component/Component.h>

#include <QHeaderView>
#include <QContextMenuEvent>
#include <QMenu>
#include <QMimeData>

//-----------------------------------------------------------------------------
// Function: AutoConnectorConnectionTable::AutoConnectorConnectionTable()
//-----------------------------------------------------------------------------
AutoConnectorConnectionTable::AutoConnectorConnectionTable(QSharedPointer<Component> firstComponent,
    QSharedPointer<Component> secondComponent, QListView* firstList, QListView* secondList,
    QString const& firstItemName, QString const& secondItemName, TableItemMatcher* itemMatcher, QWidget* parent):
QTableWidget(parent),
firstComponent_(firstComponent),
secondComponent_(secondComponent),
firstItemList_(firstList),
secondItemList_(secondList),
dragSourceList_(),
removeRowAction_(new QAction(tr("Remove row"), this)),
addRowAction_(new QAction(tr("Add row"), this)),
itemMatcher_(itemMatcher)
{
    setDragDropMode(QAbstractItemView::DropOnly);

    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader()->setDefaultSectionSize(22);

    setColumnCount(2);
    verticalHeader()->hide();
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    horizontalHeader()->setStretchLastSection(true);

    QString firstHeader = firstItemName;
    QString secondHeader = secondItemName;
    QStringList horizontalHeaders = { firstHeader, secondHeader };
    setHorizontalHeaderLabels(horizontalHeaders);

    connect(removeRowAction_, SIGNAL(triggered()), this, SLOT(onRemoveRow()), Qt::UniqueConnection);
    connect(addRowAction_, SIGNAL(triggered()), this, SLOT(onAddRow()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: AutoConnectorConnectionTable::~AutoConnectorConnectionTable()
//-----------------------------------------------------------------------------
AutoConnectorConnectionTable::~AutoConnectorConnectionTable()
{

}

//-----------------------------------------------------------------------------
// Function: AutoConnectorConnectionTable::getConnectedItems()
//-----------------------------------------------------------------------------
QVector<QPair<QString, QString> > AutoConnectorConnectionTable::getConnectedItems() const
{
    QVector<QPair<QString, QString> > fullConnections;

    for (int i = 0; i < rowCount(); ++i)
    {
        QTableWidgetItem* firstItem = item(i, 0);
        QTableWidgetItem* secondItem = item(i, 1);
        if (firstItem && !firstItem->text().isEmpty() && secondItem && !secondItem->text().isEmpty())
        {
            QPair<QString, QString> connection;
            connection.first = firstItem->text();
            connection.second = secondItem->text();

            fullConnections.append(connection);
        }
    }

    return fullConnections;
}

//-----------------------------------------------------------------------------
// Function: AutoConnectorConnectionTable::contextMenuEvent()
//-----------------------------------------------------------------------------
void AutoConnectorConnectionTable::contextMenuEvent(QContextMenuEvent *event)
{
    QModelIndex index = indexAt(event->pos());

    QMenu menu(this);
    menu.addAction(addRowAction_);

    if (index.isValid())
    {
        menu.addAction(removeRowAction_);
    }

    menu.exec(event->globalPos());
}

//-----------------------------------------------------------------------------
// Function: AutoConnectorConnectionTable::onAddRow()
//-----------------------------------------------------------------------------
void AutoConnectorConnectionTable::onAddRow()
{
    setRowCount(rowCount() + 1);
}

//-----------------------------------------------------------------------------
// Function: AutoConnectorConnectionTable::onRemoveRow()
//-----------------------------------------------------------------------------
void AutoConnectorConnectionTable::onRemoveRow()
{
    QModelIndexList indexlist = selectedIndexes();
    qSort(indexlist.begin(), indexlist.end(), qGreater<QModelIndex>());
    foreach(QModelIndex index, indexlist)
    {
        removeRow(index.row());
    }
}

//-----------------------------------------------------------------------------
// Function: AutoConnectorConnectionTable::dropMimeData()
//-----------------------------------------------------------------------------
bool AutoConnectorConnectionTable::dropMimeData(int row, int column, const QMimeData *data, Qt::DropAction action)
{
    if (dragSourceList_ && (dragSourceList_ == firstItemList_ || dragSourceList_ == secondItemList_))
    {
        int newColumn = column;
        if (dragSourceList_ == firstItemList_)
        {
            newColumn = 0;
        }
        else if (dragSourceList_ == secondItemList_)
        {
            newColumn = 1;
        }

        dragSourceList_ = nullptr;

        return QTableWidget::dropMimeData(row, newColumn, data, action);
    }
    else
    {
        dragSourceList_ = nullptr;

        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: AutoConnectorConnectionTable::dragEnterEvent()
//-----------------------------------------------------------------------------
void AutoConnectorConnectionTable::dragEnterEvent(QDragEnterEvent *event)
{
    QTableWidget::dragEnterEvent(event);

    dragSourceList_ = qobject_cast<QListView*>(event->source());
}

//-----------------------------------------------------------------------------
// Function: AutoConnectorConnectionTable::dragMoveEvent()
//-----------------------------------------------------------------------------
void AutoConnectorConnectionTable::dragMoveEvent(QDragMoveEvent *event)
{
    QTableWidget::dragMoveEvent(event);

    QModelIndex positionIndex = indexAt(event->pos());
    if (positionIndex.isValid() && itemMatcher_)
    {
        QSharedPointer<Component> sourceComponent = firstComponent_;
        QSharedPointer<Component> targetComponent = secondComponent_;
        int comparisonIndexColumn = 1;
        if (dragSourceList_ == secondItemList_)
        {
            sourceComponent = secondComponent_;
            targetComponent = firstComponent_;
            comparisonIndexColumn = 0;
        }

        if (positionIndex.column() != comparisonIndexColumn)
        {
            positionIndex = positionIndex.sibling(positionIndex.row(), comparisonIndexColumn);
        }

        if (itemMatcher_->canDrop(
                dragSourceList_->currentIndex(), positionIndex, sourceComponent, targetComponent))
        {
            event->setDropAction(Qt::MoveAction);
        }
        else
        {
            event->setDropAction(Qt::IgnoreAction);
        }
    }
    else
    {
        event->setDropAction(Qt::MoveAction);
    }
}
