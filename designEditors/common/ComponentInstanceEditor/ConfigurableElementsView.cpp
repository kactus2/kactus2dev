//-----------------------------------------------------------------------------
// File: ConfigurableElementsView.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 30.03.2015
//
// Description:
// A view for configurable element tree model.
//-----------------------------------------------------------------------------

#include "ConfigurableElementsView.h"

#include <QMenu>
#include <QPainter>

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsView::ConfigurableElementsView()
//-----------------------------------------------------------------------------
ConfigurableElementsView::ConfigurableElementsView(QWidget* parent):
QTreeView(parent),
contextMenuIndex_(QModelIndex()),
removeAction_(new QAction("Remove configurable element", this))
{
    setSortingEnabled(true);
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setSelectionMode(QAbstractItemView::SingleSelection);

    setIndentation(10);

    connect(removeAction_, SIGNAL(triggered()), this, SLOT(onRemoveItem()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsView::~ConfigurableElementsView()()
//-----------------------------------------------------------------------------
ConfigurableElementsView::~ConfigurableElementsView()
{

}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsView::onRemoveItem()
//-----------------------------------------------------------------------------
void ConfigurableElementsView::onRemoveItem()
{
    emit removeItem(contextMenuIndex_);
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsView::()
//-----------------------------------------------------------------------------
void ConfigurableElementsView::drawRow(QPainter* painter, QStyleOptionViewItem const& option,
    QModelIndex const& index) const
{
    QTreeView::drawRow(painter, option, index);

    painter->save();
    painter->setPen(QPen(QColor(Qt::lightGray), 0));
    painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());
    painter->restore();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsView::contextMenuEvent()
//-----------------------------------------------------------------------------
void ConfigurableElementsView::contextMenuEvent(QContextMenuEvent* event)
{
    contextMenuIndex_ = indexAt(event->pos());
    if (!contextMenuIndex_.isValid())
    {
        event->accept();
        return;
    }

    if (model()->data(contextMenuIndex_, Qt::UserRole).toBool())
    {
        removeAction_->setEnabled(true);

        QMenu contextMenu;
        contextMenu.addAction(removeAction_);

        contextMenu.exec(event->globalPos());
        event->accept();
    }
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsView::getRemoveAction()
//-----------------------------------------------------------------------------
QAction* ConfigurableElementsView::getRemoveAction() const
{
    return removeAction_;
}