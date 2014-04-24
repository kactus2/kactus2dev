//-----------------------------------------------------------------------------
// File: PortMapsTreeView.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 7.10.2013
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#include "PortMapsTreeView.h"

#include "PortMapsTreeModel.h"
#include "PortMapsTreeItem.h"

#include <QHeaderView>
#include <QPainter>
#include <QPaintEvent>
#include <QScrollBar>
#include <QDebug>
#include <QMenu>
#include <QFileDialog>
#include <QModelIndex>

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::FileDependencyGraphWidget()
//-----------------------------------------------------------------------------
PortMapsTreeView::PortMapsTreeView(QWidget* parent)
    : QTreeView(parent),      
      model_(0),
      contextMenuIndex_(QModelIndex()),
      removeAction_(new QAction("Remove mapping", this)),
      restoreAction_(new QAction("Restore logical port to list", this))   
{

    // cells are resized to match contents 
    header()->setSectionResizeMode(QHeaderView::Interactive);

    //last column is stretched take the available space in the widget
    header()->setStretchLastSection(true);

    setUniformRowHeights(true);
    setIndentation(15);

    removeAction_->setShortcut(Qt::Key_Delete);
    connect(removeAction_, SIGNAL(triggered()), this, SLOT(onRemoveItem()), Qt::UniqueConnection);
    connect(restoreAction_, SIGNAL(triggered()), this, SLOT(onRestoreItem()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::~FileDependencyGraphWidget()
//-----------------------------------------------------------------------------
PortMapsTreeView::~PortMapsTreeView()
{
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::setModel()
//-----------------------------------------------------------------------------
void PortMapsTreeView::setModel(QAbstractItemModel* model)
{
    // Disconnect the previous model.
    if (model_ != 0)
    {
        model_->disconnect(this);
    }

    QTreeView::setModel(model);

    model_ = dynamic_cast<PortMapsTreeModel*>(model);
    if (model_)
    {
        connect(this, SIGNAL(removeItem(const QModelIndex&)),
            model_, SLOT(removeMapping(const QModelIndex&)), Qt::UniqueConnection);
        connect(this, SIGNAL(restoreItem(const QModelIndex&)),
            model_, SLOT(restoreItem(const QModelIndex&)), Qt::UniqueConnection);
    }    
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeView::keyPressEvent()
//-----------------------------------------------------------------------------
void PortMapsTreeView::keyPressEvent(QKeyEvent* event)
{
    // When delete is pressed, attempt to delete the selected mapping.
    if (event->key() == Qt::Key_Delete)
    {
        if (!selectedIndexes().isEmpty())
        {
            emit removeItem(selectedIndexes().first());
        }
    }
    else
    {
        QTreeView::keyPressEvent(event);
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeView::rowsInserted()
//-----------------------------------------------------------------------------
void PortMapsTreeView::rowsInserted(QModelIndex const& parent, int start, int end)
{
    QTreeView::rowsInserted(parent, start, end);

    if (parent.isValid())
    {
        setExpanded(parent, isExpanded(parent));
    } 
    else
    {
        setExpanded(parent, false);
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeView::onRemove()
//-----------------------------------------------------------------------------
void PortMapsTreeView::onRemoveItem()
{
    emit removeItem(contextMenuIndex_);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::drawRow()
//-----------------------------------------------------------------------------
void PortMapsTreeView::drawRow(QPainter* painter, QStyleOptionViewItem const& option,
    QModelIndex const& index) const
{
    // Handle base drawing.
    QTreeView::drawRow(painter, option, index);

    // Draw a line below the row.
    QRect rowRect = visualRect(index);

    painter->save();
    painter->setPen(QPen(QColor(200, 200, 200), 0));
    painter->drawLine(option.rect.left(), rowRect.bottom(), option.rect.right(), rowRect.bottom());
    painter->restore();
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeView::contextMenuEvent()
//-----------------------------------------------------------------------------
void PortMapsTreeView::contextMenuEvent(QContextMenuEvent* event)
{
    contextMenuIndex_ = indexAt(event->pos());
    if (!contextMenuIndex_.isValid())
    {
        event->accept();
        return;
    }
    else
    {
        PortMapsTreeItem* item = static_cast<PortMapsTreeItem*>(contextMenuIndex_.internalPointer());
        if (item && item->getType() == PortMapsTreeItem::ITEM_LOGICAL_PORT)
        {
            removeAction_->setEnabled(true);
            restoreAction_->setEnabled(true);
        }
        else
        {
            removeAction_->setEnabled(false);
            restoreAction_->setEnabled(false);
        }
    }

    QMenu contextMenu;
    contextMenu.addAction(removeAction_);
    contextMenu.addAction(restoreAction_);

    contextMenu.exec(event->globalPos());
    event->accept();
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeView::onRestoreItem()
//-----------------------------------------------------------------------------
void PortMapsTreeView::onRestoreItem()
{
    if (!contextMenuIndex_.isValid())
    {
        return;
    }

    emit restoreItem(contextMenuIndex_);   
}