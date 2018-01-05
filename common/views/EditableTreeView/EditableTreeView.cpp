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
clearAction_(new QAction(tr("Clear"), this))
{
    header()->setDefaultAlignment(Qt::AlignCenter);

    addItemAction_->setToolTip(addItemText);
    addItemAction_->setStatusTip(addItemText);
    addSubItemAction_->setToolTip(addSubItemText);
    addSubItemAction_->setStatusTip(addSubItemText);
    removeAction_->setToolTip(removeItemText);
    removeAction_->setStatusTip(removeItemText);
    removeAllSubItemsAction_->setToolTip(removeSubItemsText);
    removeAllSubItemsAction_->setStatusTip(removeSubItemsText);
    clearAction_->setToolTip(tr("Clear the contents of a cell."));
    clearAction_->setStatusTip(tr("Clear the contents of a cell."));

    setIndentation(10);
    setSortingEnabled(true);
    setSelectionBehavior(QAbstractItemView::SelectItems);

    addAction(clearAction_);
    clearAction_->setShortcut(QKeySequence::Delete);
    clearAction_->setShortcutContext(Qt::WidgetShortcut);

    connect(addItemAction_, SIGNAL(triggered()), this, SLOT(onAddItem()), Qt::UniqueConnection);
    connect(addSubItemAction_, SIGNAL(triggered()), this, SLOT(onAddSubItem()), Qt::UniqueConnection);

    connect(removeAction_, SIGNAL(triggered()), this, SLOT(onRemoveSelectedItems()), Qt::UniqueConnection);
    connect(removeAllSubItemsAction_, SIGNAL(triggered()),
        this, SLOT(onRemoveAllSubItems()), Qt::UniqueConnection);

    connectClearAction();
}

//-----------------------------------------------------------------------------
// Function: EditableTreeView::connectClearAction()
//-----------------------------------------------------------------------------
void EditableTreeView::connectClearAction()
{
    connect(clearAction_, SIGNAL(triggered()), this, SLOT(onClearAction()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: EditableTreeView::~EditableTreeView()
//-----------------------------------------------------------------------------
EditableTreeView::~EditableTreeView()
{

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

    QModelIndexList currentIndexes = selectedIndexes();

    emit addSubItem(currentIndexes);

    clearSelection();
    setCurrentIndex(QModelIndex());

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
// Function: EditableTreeView::onClearAction()
//-----------------------------------------------------------------------------
void EditableTreeView::onClearAction()
{
    QModelIndex selectedIndex = currentIndex();

    QString oldValue = model()->data(selectedIndex, Qt::EditRole).toString();
    QString newValue("");
    model()->setData(selectedIndex, newValue, Qt::EditRole);
}
