//-----------------------------------------------------------------------------
// File: EditableTreeView.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 30.11.2017
//
// Description:
// A view for editable trees.
//-----------------------------------------------------------------------------

#include "EditableTreeView.h"

#include <common/KactusColors.h>

#include <QApplication>
#include <QPainter>
#include <QHeaderView>
#include <QMenu>

//-----------------------------------------------------------------------------
// Function: EditableTreeView::EditableTreeView()
//-----------------------------------------------------------------------------
EditableTreeView::EditableTreeView(bool allowNewRows, QString const& addItemText, QString const& addSubItemText,
    QString const& removeItemText, QString const& removeSubItemsText, QWidget* parent):
QTreeView(parent),
allowNewRows_(allowNewRows),
addItemAction_(new QAction(addItemText, this)),
addSubItemAction_(new QAction(addSubItemText, this)),
removeAction_(new QAction(removeItemText, this)),
removeAllSubItemsAction_(new QAction(removeSubItemsText, this)),
clearAction_(new QAction(tr("Clear"), this)),
expandAllItemsAction_(new QAction(tr("Expand all"), this)),
collapseAllItemsAction_(new QAction(tr("Collapse all"), this))
{
    header()->setDefaultAlignment(Qt::AlignCenter);

    setIndentation(10);
    setSelectionBehavior(QAbstractItemView::SelectItems);

    setupActions(addItemText, addSubItemText, removeItemText, removeSubItemsText);
}

//-----------------------------------------------------------------------------
// Function: EditableTreeView::setupActions()
//-----------------------------------------------------------------------------
void EditableTreeView::setupActions(QString const& addItemText, QString const& addSubItemText,
    QString const& removeItemText, QString const& removeSubItemsText)
{
    addItemAction_->setToolTip(addItemText);
    addItemAction_->setStatusTip(addItemText);
    addSubItemAction_->setToolTip(addSubItemText);
    addSubItemAction_->setStatusTip(addSubItemText);
    removeAction_->setToolTip(removeItemText);
    removeAction_->setStatusTip(removeItemText);
    removeAllSubItemsAction_->setToolTip(removeSubItemsText);
    removeAllSubItemsAction_->setStatusTip(removeSubItemsText);

    expandAllItemsAction_->setToolTip(tr("Expand all port maps."));
    expandAllItemsAction_->setStatusTip(tr("Expand all port maps."));
    collapseAllItemsAction_->setToolTip(tr("Collapse all port maps."));
    collapseAllItemsAction_->setStatusTip(tr("Collapse all port maps."));

    addAction(clearAction_);
    clearAction_->setShortcut(QKeySequence::Delete);
    clearAction_->setShortcutContext(Qt::WidgetShortcut);
    QString tooltipClear = tr("Clear the contents of a cell (%1)").arg(clearAction_->shortcut().toString(QKeySequence::NativeText));
    clearAction_->setToolTip(tooltipClear);
    clearAction_->setStatusTip(tooltipClear);

    connectActions();
}

//-----------------------------------------------------------------------------
// Function: EditableTreeView::connectActions()
//-----------------------------------------------------------------------------
void EditableTreeView::connectActions()
{
    connect(addItemAction_, SIGNAL(triggered()), this, SLOT(onAddItem()), Qt::UniqueConnection);
    connect(addSubItemAction_, SIGNAL(triggered()), this, SLOT(onAddSubItem()), Qt::UniqueConnection);

    connect(removeAction_, SIGNAL(triggered()), this, SLOT(onRemoveSelectedItems()), Qt::UniqueConnection);
    connect(removeAllSubItemsAction_, SIGNAL(triggered()),
        this, SLOT(onRemoveAllSubItems()), Qt::UniqueConnection);

    connect(clearAction_, SIGNAL(triggered()), this, SLOT(onClearAction()), Qt::UniqueConnection);

    connect(expandAllItemsAction_, SIGNAL(triggered()), this, SLOT(expandAll()), Qt::UniqueConnection);
    connect(collapseAllItemsAction_, SIGNAL(triggered()), this, SLOT(collapseAll()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: EditableTreeView::~EditableTreeView()
//-----------------------------------------------------------------------------
EditableTreeView::~EditableTreeView()
{

}

//-----------------------------------------------------------------------------
// Function: EditableTreeView::contextMenuEvent()
//-----------------------------------------------------------------------------
void EditableTreeView::contextMenuEvent(QContextMenuEvent* event)
{
    QModelIndex contextMenuIndex = indexAt(event->pos());

    QMenu contextMenu(this);

    contextMenu.addAction(addItemAction_);

    if (contextMenuIndex.isValid())
    {
        contextMenu.addAction(removeAction_);
        contextMenu.addAction(clearAction_);

        contextMenu.addSeparator();
        contextMenu.addAction(addSubItemAction_);
        contextMenu.addAction(removeAllSubItemsAction_);
    }

    contextMenu.addSeparator();

    contextMenu.addAction(expandAllItemsAction_);
    contextMenu.addAction(collapseAllItemsAction_);

    contextMenu.exec(event->globalPos());

    event->accept();

}

//-----------------------------------------------------------------------------
// Function: EditableTreeView::mouseDoubleClickEvent()
//-----------------------------------------------------------------------------
void EditableTreeView::mouseDoubleClickEvent(QMouseEvent* event)
{
    QModelIndex index = indexAt(event->pos());
    if (allowNewRows_ && !index.isValid())
    {
        onAddItem();
        event->accept();
        return;
    }
    else
    {
        QTreeView::mouseDoubleClickEvent(event);
    }
}

//-----------------------------------------------------------------------------
// Function: EditableTreeView::onAddItem()
//-----------------------------------------------------------------------------
void EditableTreeView::onAddItem()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    emit addItems(selectedIndexes());

    clearSelection();
    setCurrentIndex(QModelIndex());

    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: EditableTreeView::onAddSubItem()
//-----------------------------------------------------------------------------
void EditableTreeView::onAddSubItem()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    QModelIndexList currentIndexes;
    for (auto index : selectedIndexes())
    {
        currentIndexes.append(index.sibling(index.row(), 0));
    }

    emit addSubItem(currentIndexes);

    foreach (QModelIndex index, currentIndexes)
    {
        expand(index);
    }

    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: EditableTreeView::onRemoveItem()
//-----------------------------------------------------------------------------
void EditableTreeView::onRemoveSelectedItems()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    emit removeSelectedItems(selectedIndexes());

    clearSelection();
    setCurrentIndex(QModelIndex());

    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: EditableTreeView::onRemoveAllSubItems()
//-----------------------------------------------------------------------------
void EditableTreeView::onRemoveAllSubItems()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    emit removeAllSubItems(selectedIndexes());

    clearSelection();
    setCurrentIndex(QModelIndex());

    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: EditableTreeView::()
//-----------------------------------------------------------------------------
void EditableTreeView::drawRow(QPainter* painter, QStyleOptionViewItem const& option,
    QModelIndex const& index) const
{
    QTreeView::drawRow(painter, option, index);

    painter->save();
    painter->setPen(QPen(KactusColors::DISABLED_TEXT, 0));
    painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());
    painter->restore();
}

//-----------------------------------------------------------------------------
// Function: EditableTreeView::getAddItemAction()
//-----------------------------------------------------------------------------
QAction* EditableTreeView::getAddItemAction() const
{
    return addItemAction_;
}

//-----------------------------------------------------------------------------
// Function: EditableTreeView::getAddSubItemAction()
//-----------------------------------------------------------------------------
QAction* EditableTreeView::getAddSubItemAction() const
{
    return addSubItemAction_;
}

//-----------------------------------------------------------------------------
// Function: EditableTreeView::getRemoveAction()
//-----------------------------------------------------------------------------
QAction* EditableTreeView::getRemoveAction() const
{
    return removeAction_;
}

//-----------------------------------------------------------------------------
// Function: EditableTreeView::getRemoveAllSubItemsAction()
//-----------------------------------------------------------------------------
QAction* EditableTreeView::getRemoveAllSubItemsAction() const
{
    return removeAllSubItemsAction_;
}

//-----------------------------------------------------------------------------
// Function: EditableTreeView::getClearAction()
//-----------------------------------------------------------------------------
QAction* EditableTreeView::getClearAction() const
{
    return clearAction_;
}

//-----------------------------------------------------------------------------
// Function: EditableTreeView::getExpandAllAction()
//-----------------------------------------------------------------------------
QAction* EditableTreeView::getExpandAllAction() const
{
    return expandAllItemsAction_;
}

//-----------------------------------------------------------------------------
// Function: EditableTreeView::getCollapseAllAction()
//-----------------------------------------------------------------------------
QAction* EditableTreeView::getCollapseAllAction() const
{
    return collapseAllItemsAction_;
}

//-----------------------------------------------------------------------------
// Function: EditableTreeView::onClearAction()
//-----------------------------------------------------------------------------
void EditableTreeView::onClearAction()
{
    QModelIndex selectedIndex = currentIndex();

    QString oldValue = model()->data(selectedIndex, Qt::EditRole).toString();
    QString newValue("");
    model()->setData(selectedIndex, newValue, Qt::EditRole);
}
