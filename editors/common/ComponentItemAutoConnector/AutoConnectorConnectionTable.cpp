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

#include <QHeaderView>
#include <QContextMenuEvent>
#include <QMenu>
#include <QMimeData>

//-----------------------------------------------------------------------------
// Function: AutoConnectorConnectionTable::AutoConnectorConnectionTable()
//-----------------------------------------------------------------------------
AutoConnectorConnectionTable::AutoConnectorConnectionTable(QListWidget* firstList, QListWidget* secondList,
    QWidget* parent):
QTableWidget(parent),
firstItemList_(firstList),
secondItemList_(secondList),
dragSourceList_(),
removeRowAction_(new QAction(tr("Remove row"), this))
{
    setDragDropMode(QAbstractItemView::DropOnly);

    setColumnCount(2);
    verticalHeader()->hide();
    horizontalHeader()->setStretchLastSection(true);

    connect(removeRowAction_, SIGNAL(triggered()), this, SLOT(onRemoveRow()), Qt::UniqueConnection);
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

    if (index.isValid())
    {
        QMenu menu(this);

        menu.addAction(removeRowAction_);

        menu.exec(event->globalPos());
    }
}

//-----------------------------------------------------------------------------
// Function: AutoConnectorConnectionTable::onRemoveRow()
//-----------------------------------------------------------------------------
void AutoConnectorConnectionTable::onRemoveRow()
{
    removeRow(currentIndex().row());
}

//-----------------------------------------------------------------------------
// Function: AutoConnectorConnectionTable::dropEvent()
//-----------------------------------------------------------------------------
void AutoConnectorConnectionTable::dropEvent(QDropEvent *event)
{
    dragSourceList_ = qobject_cast<QListWidget*>(event->source());

    QTableWidget::dropEvent(event);
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
