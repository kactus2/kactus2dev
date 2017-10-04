//-----------------------------------------------------------------------------
// File: ConfigurableElementsView.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 30.03.2015
//
// Description:
// A view for configurable element tree model.
//-----------------------------------------------------------------------------

#include "ConfigurableElementsView.h"

#include <common/KactusColors.h>

#include <QMenu>
#include <QPainter>

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsView::ConfigurableElementsView()
//-----------------------------------------------------------------------------
ConfigurableElementsView::ConfigurableElementsView(QWidget* parent):
QTreeView(parent),
contextMenuIndex_(QModelIndex()),
removeAction_(new QAction("Remove configurable element", this)),
removeAllAction_(new QAction("Remove all unknown configurable elements", this)),
clearAction_(new QAction(tr("Clear"), this))
{
    setSortingEnabled(true);
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setSelectionMode(QAbstractItemView::SingleSelection);

    setIndentation(10);

    connect(removeAction_, SIGNAL(triggered()), this, SLOT(onRemoveItem()), Qt::UniqueConnection);
    connect(removeAllAction_, SIGNAL(triggered()), this, SLOT(onRemoveAllSubItems()), Qt::UniqueConnection);

    addAction(clearAction_);
    clearAction_->setShortcut(QKeySequence::Delete);
    clearAction_->setShortcutContext(Qt::WidgetShortcut);
    connect(clearAction_, SIGNAL(triggered()), this, SLOT(onClearAction()), Qt::UniqueConnection);
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
// Function: ConfigurableElementsView::onRemoveAllSubItems()
//-----------------------------------------------------------------------------
void ConfigurableElementsView::onRemoveAllSubItems()
{
    emit removeAllSubItems(contextMenuIndex_);
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsView::()
//-----------------------------------------------------------------------------
void ConfigurableElementsView::drawRow(QPainter* painter, QStyleOptionViewItem const& option,
    QModelIndex const& index) const
{
    QTreeView::drawRow(painter, option, index);

    painter->save();
    painter->setPen(QPen(KactusColors::DISABLED_TEXT, 0));
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
    else if (!contextMenuIndex_.parent().isValid() &&
        model()->data(contextMenuIndex_, Qt::ForegroundRole) == KactusColors::ERROR)
    {
        removeAllAction_->setEnabled(true);

        QMenu contextMenu;
        contextMenu.addAction(removeAllAction_);

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

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsView::onClearAction()
//-----------------------------------------------------------------------------
void ConfigurableElementsView::onClearAction()
{
    QModelIndex selectedIndex = currentIndex();

    QString oldValue = model()->data(selectedIndex, Qt::EditRole).toString();
    QString newValue("");
    model()->setData(selectedIndex, newValue, Qt::EditRole);

    if (selectedIndex.parent().isValid())
    {
        emit createElementChangeCommand(oldValue, newValue, selectedIndex, model());
    }
}
