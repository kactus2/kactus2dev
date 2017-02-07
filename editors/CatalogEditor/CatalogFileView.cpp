//-----------------------------------------------------------------------------
// File: CatalogFileView.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 03.02.2017
//
// Description:
// The widget to display the ipxact files of a catalog.
//-----------------------------------------------------------------------------

#include "CatalogFileView.h"

#include "CatalogFileColumns.h"

#include <QApplication>
#include <QClipboard>
#include <QDrag>
#include <QFileInfo>
#include <QHeaderView>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>
#include <QVector>

//-----------------------------------------------------------------------------
// Function: CatalogFileView::CatalogFileView()
//-----------------------------------------------------------------------------
CatalogFileView::CatalogFileView(QWidget* parent): QTreeView(parent),
    addAction_(new QAction(tr("Add row"), this)),
    removeAction_(new QAction(tr("Remove row"), this)),
    openAction_(new QAction(tr("Open"), this)),
    cutAction_(new QAction(tr("Cut"), this)),
    copyAction_(new QAction(tr("Copy"), this)),
    pasteAction_(new QAction(tr("Paste"), this)),
    clearAction_(new QAction(tr("Clear"), this)),
    expandAllIAction_(new QAction(tr("Expand all"), this)),
    collapseAllIAction_(new QAction(tr("Collapse all"), this))
{
	setSelectionBehavior(QAbstractItemView::SelectItems);
	setSelectionMode(QAbstractItemView::ContiguousSelection);

    setDropIndicatorShown(true);   
    viewport()->setAcceptDrops(true); 
    setDragDropMode(QAbstractItemView::DropOnly);

    setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked |
        QAbstractItemView::EditKeyPressed | QAbstractItemView::AnyKeyPressed);

	setupActions();
}

//-----------------------------------------------------------------------------
// Function: CatalogFileView::~CatalogFileView()
//-----------------------------------------------------------------------------
CatalogFileView::~CatalogFileView()
{

}

//-----------------------------------------------------------------------------
// Function: CatalogFileView::contextMenuEvent()
//-----------------------------------------------------------------------------
void CatalogFileView::contextMenuEvent(QContextMenuEvent* event)
{
    QModelIndex index = indexAt(event->pos());

    QMenu menu(this);

    if (index.isValid() && index.parent().isValid() && index.parent().row() <= CatalogFileColumns::COMPONENTS)
    {
        menu.addAction(openAction_);
        openAction_->setEnabled(selectedIndexes().count() == 1);
        menu.addSeparator();
    }

    menu.addAction(addAction_);

    bool validIndex = index.isValid() && index.parent().isValid();

    // if at least one valid item is selected.
    if (validIndex)
    {
        menu.addAction(removeAction_);

        menu.addSeparator();
        menu.addAction(cutAction_);
        menu.addAction(copyAction_);
        menu.addAction(pasteAction_);
        menu.addAction(clearAction_);
    }

    menu.addSeparator();
    menu.addAction(expandAllIAction_);
    menu.addAction(collapseAllIAction_);

    menu.exec(event->globalPos());

    event->accept();
}

//-----------------------------------------------------------------------------
// Function: CatalogFileView::onAddAction()
//-----------------------------------------------------------------------------
void CatalogFileView::onAddAction()
{
    QModelIndexList indexes = selectedIndexes();
    QModelIndex posToAdd;

    if (!indexes.isEmpty())
    {
        posToAdd = indexes.first();
    }

    emit addItem(posToAdd);

    clearSelection();
}

//-----------------------------------------------------------------------------
// Function: CatalogFileView::onRemoveAction()
//-----------------------------------------------------------------------------
void CatalogFileView::onRemoveAction()
{
    QModelIndexList toRemove = sortAndMinimize(selectedIndexes());
    if (toRemove.isEmpty())
    {
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    QModelIndex removeIndex = toRemove.first();

    for (int i = 0; i < toRemove.count(); i++)
    {
        if (removeIndex.parent() != toRemove.at(i).parent())
        {
            removeIndex = toRemove.at(i);
        }

        emit removeItem(removeIndex);
    }

    clearSelection();
    setCurrentIndex(QModelIndex());

    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: CatalogFileView::onCopyAction()
//-----------------------------------------------------------------------------
void CatalogFileView::onCopyAction()
{
    // if nothing was selected then don't copy anything
    if (!currentIndex().isValid())
    {
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    QModelIndexList indexes = selectedIndexes();
    qSort(indexes);

    int lastColumn = indexes.last().column();

    QString copyText;
    foreach(QModelIndex sourceIndex, indexes)
    {
        copyText.append(sourceIndex.data(Qt::EditRole).toString());

        if (sourceIndex.column() < lastColumn)
        {
            copyText.append("\t");
        }
        else
        {
            copyText.append("\n");
        }
    }

    QApplication::clipboard()->setText(copyText);
    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: CatalogFileView::onPasteAction()
//-----------------------------------------------------------------------------
void CatalogFileView::onPasteAction()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    // find the highest row to start adding to
    QModelIndexList indexes = sortAndMinimize(selectedIndexes());

    if (indexes.isEmpty())
    {
        QApplication::restoreOverrideCursor();
        return;
    }

    QString subtype("plain");
    QString pasteText = QApplication::clipboard()->text(subtype);

    // Split the string from clip board into rows.
    QStringList rowsToAdd = pasteText.split("\n", QString::SkipEmptyParts);

    int current = 0;
    QPersistentModelIndex currentIndex = indexes.first();
    int startColumn = indexes.first().column();
    
    foreach (QString const& row, rowsToAdd)
    {
        int targetColumn = qMax(0, startColumn);

        // Split the row into columns.
        QStringList columnsToAdd = row.split("\t");
        foreach (QString const& columnData, columnsToAdd)
        {
            QModelIndex itemToSet = currentIndex.sibling(currentIndex.row(), targetColumn);
            if (itemToSet.isValid())
            {
                model()->setData(itemToSet, columnData, Qt::EditRole);
            }

            targetColumn++;
        }

        if (current < indexes.count() -1)
        {           
            current++;
            currentIndex = indexes.at(current);
        }
        else
        {
            currentIndex = currentIndex.sibling(currentIndex.row() + 1, startColumn);
        }
    }

    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: CatalogFileView::onCutAction()
//-----------------------------------------------------------------------------
void CatalogFileView::onCutAction()
{
    QModelIndexList indexes = selectedIndexes();
    if (indexes.isEmpty())
    {
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    qSort(indexes);
    int lastColumn = indexes.last().column();

    QString copyText;

    foreach (QModelIndex index, indexes)
    {
        copyText.append(index.data(Qt::EditRole).toString());

        model()->setData(index, QVariant(), Qt::EditRole);

        if (index.column() < lastColumn)
        {
            copyText.append("\t");
        }
        else
        {
            copyText.append("\n");
        }
    }

    QApplication::clipboard()->setText(copyText);
    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: CatalogFileView::onClearAction()
//-----------------------------------------------------------------------------
void CatalogFileView::onClearAction()
{	
    QModelIndexList indexes = selectedIndexes();

    // clear the contents of each cell
    foreach (QModelIndex const& index, indexes)
    {
        model()->setData(index, QVariant(), Qt::EditRole);
    }
}

//-----------------------------------------------------------------------------
// Function: CatalogFileView::onOpenAction()
//-----------------------------------------------------------------------------
void CatalogFileView::onOpenAction()
{	
    QModelIndexList indexes = selectedIndexes();

    if (indexes.count() != 1) 
    {
        return;
    }

    emit openItem(indexes.first());
}

//-----------------------------------------------------------------------------
// Function: CatalogFileView::setupActions()
//-----------------------------------------------------------------------------
void CatalogFileView::setupActions()
{
    addAction(addAction_);
    addAction_->setShortcut(QKeySequence::InsertLineSeparator);
    addAction_->setShortcutContext(Qt::WidgetShortcut);
	connect(addAction_, SIGNAL(triggered()), this, SLOT(onAddAction()), Qt::UniqueConnection);
    
    addAction(removeAction_);
    removeAction_->setShortcut(Qt::SHIFT + Qt::Key_Delete);
    removeAction_->setShortcutContext(Qt::WidgetShortcut);
    connect(removeAction_, SIGNAL(triggered()), this, SLOT(onRemoveAction()), Qt::UniqueConnection);
    
    addAction(openAction_);
    connect(openAction_, SIGNAL(triggered()), this, SLOT(onOpenAction()), Qt::UniqueConnection);

    addAction(copyAction_);
    copyAction_->setShortcut(QKeySequence::Copy);
    copyAction_->setShortcutContext(Qt::WidgetShortcut);
    connect(copyAction_, SIGNAL(triggered()), this, SLOT(onCopyAction()), Qt::UniqueConnection);

    addAction(pasteAction_);
    pasteAction_->setShortcut(QKeySequence::Paste);
    pasteAction_->setShortcutContext(Qt::WidgetShortcut);
    connect(pasteAction_, SIGNAL(triggered()), this, SLOT(onPasteAction()), Qt::UniqueConnection);

    addAction(cutAction_);
    cutAction_->setShortcut(QKeySequence::Cut);
    cutAction_->setShortcutContext(Qt::WidgetShortcut);
    connect(cutAction_, SIGNAL(triggered()), this, SLOT(onCutAction()), Qt::UniqueConnection);

    addAction(clearAction_);
    clearAction_->setShortcut(QKeySequence::Delete);
    clearAction_->setShortcutContext(Qt::WidgetShortcut);
    connect(clearAction_, SIGNAL(triggered()), this, SLOT(onClearAction()), Qt::UniqueConnection);

    addAction(expandAllIAction_);
    connect(expandAllIAction_, SIGNAL(triggered()), this, SLOT(expandAll()), Qt::UniqueConnection);

    addAction(collapseAllIAction_);
    connect(collapseAllIAction_, SIGNAL(triggered()), this, SLOT(collapseAll()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: CatalogFileView::sortAndMinimize()
//-----------------------------------------------------------------------------
QModelIndexList CatalogFileView::sortAndMinimize(QModelIndexList const& indexes)
{
    QModelIndexList sorted = indexes;
    qSort(sorted);

    QModelIndex previousRow = QModelIndex();

    foreach (QModelIndex index, indexes)
    {
        if (!index.parent().isValid() || 
            (index.row() == previousRow.row() && index.parent() == previousRow.parent()))
        {
            sorted.removeAll(index);            
        }
        else
        {
            previousRow = index;
        }
    }

    return sorted;
}
