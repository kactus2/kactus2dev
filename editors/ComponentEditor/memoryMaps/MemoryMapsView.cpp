//-----------------------------------------------------------------------------
// File: MemoryMapsView.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 17.04.2015
//
// Description:
// View for memory maps editor.
//-----------------------------------------------------------------------------

#include "MemoryMapsView.h"
#include "MemoryMapsColumns.h"

#include <QMenu>
#include <QKeySequence>
#include <QClipboard>
#include <QMessageBox>
#include <QTextStream>
#include <QFileDialog>
#include <QSortFilterProxyModel>
#include <QApplication>
#include <QPainter>
#include <QPen>
#include <QMimeData>
#include <QHeaderView>

//-----------------------------------------------------------------------------
// Function: MemoryMapsView::MemoryMapsView()
//-----------------------------------------------------------------------------
MemoryMapsView::MemoryMapsView(QWidget* parent):
EditableTreeView(true, QString(tr("Add memory map")), QString(tr("Add memory remap")), QString(tr("Remove row")),
    QString(tr("Remove all memory remaps")), parent),
    defaultImportExportPath_(), 
    pressedPoint_(),
    copyAction_(tr("Copy"), this),
    pasteAction_(tr("Paste"), this),
    importAction_(tr("Import csv-file"), this),
    exportAction_(tr("Export csv-file"), this),
    copyRowsAction_(tr("Copy elements"), this),
    pasteRowsAction_(tr("Paste elements"), this),
    importExportable_(false)
{
    setAlternatingRowColors(false);

    setSelectionMode(QAbstractItemView::ContiguousSelection);

    setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked |
        QAbstractItemView::EditKeyPressed | QAbstractItemView::AnyKeyPressed);

    setToolTip("Double click to add a new memory map.");

    setupActions();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapsView::setAllowImportExport()
//-----------------------------------------------------------------------------
void MemoryMapsView::setAllowImportExport(bool allow)
{
    importExportable_ = allow;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapsView::setDefaultImportExportPath()
//-----------------------------------------------------------------------------
void MemoryMapsView::setDefaultImportExportPath(const QString& path)
{
    defaultImportExportPath_ = path;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapsView::keyPressEvent()
//-----------------------------------------------------------------------------
void MemoryMapsView::keyPressEvent(QKeyEvent* event)
{
    QTreeView::keyPressEvent(event);

    if (event->matches(QKeySequence::Copy))
    {
        onCopyAction();
    }
    if (event->matches(QKeySequence::Paste))
    {
        onPasteAction();
    }
    if (event->matches(QKeySequence::InsertLineSeparator))
    {
        onAddItem();
    }
    if (event->matches(QKeySequence::Cut))
    {
        onCopyAction();
        onRemoveSelectedItems();
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapsView::contextMenuEvent()
//-----------------------------------------------------------------------------
void MemoryMapsView::contextMenuEvent(QContextMenuEvent* event)
{
    pressedPoint_ = event->pos();

    QModelIndex index = indexAt(pressedPoint_);

    QMenu menu(this);

    menu.addAction(getAddItemAction());
    if (!index.parent().isValid() && index.isValid())
    {
        menu.addAction(getAddSubItemAction());
    }

    menu.addSeparator();

    if (index.isValid())
    {
        menu.addAction(getRemoveAction());
        if (!index.parent().isValid())
        {
            menu.addAction(getRemoveAllSubItemsAction());
        }

        menu.addSeparator();
    }

    menu.addAction(&pasteAction_);

    if (index.isValid())
    {
        menu.addAction(getClearAction());
        menu.addAction(&copyAction_);
    }

    bool validIndex = index.isValid();
    bool containsCorrectMimeData = false;

    const QMimeData* clipMime = QApplication::clipboard()->mimeData();
    QStringList modelMimes = model()->mimeTypes();
    QString remapMime = modelMimes.last();
    QString mapMime = modelMimes.at(modelMimes.size() - 2);

    if (clipMime->hasImage())
    {
        if ((index.isValid() && (clipMime->hasFormat(remapMime) || clipMime->hasFormat(mapMime))) ||
            (!index.isValid() && clipMime->hasFormat(mapMime)))
        {
            containsCorrectMimeData = true;
        }
    }

    if (validIndex || containsCorrectMimeData)
    {
        menu.addSeparator();
        if (validIndex)
        {
            menu.addAction(&copyRowsAction_);
        }
        if (containsCorrectMimeData)
        {
            menu.addAction(&pasteRowsAction_);
        }
    }

    if (importExportable_)
    {
        menu.addSeparator();
        menu.addAction(&importAction_);
        menu.addAction(&exportAction_);
        menu.addSeparator();
    }

    menu.addAction(getExpandAllAction());
    menu.addAction(getCollapseAllAction());

    menu.exec(event->globalPos());
    event->accept();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapsView::setupActions()
//-----------------------------------------------------------------------------
void MemoryMapsView::setupActions()
{
    getAddItemAction()->setToolTip(tr("Add a new row to the table."));
    getAddItemAction()->setStatusTip(tr("Add a new row to the table."));

    getAddSubItemAction()->setToolTip(tr("Add a new memory remap to the selected memory map."));
    getAddSubItemAction()->setStatusTip(tr("Add a new memory remap to the selected memory map."));

    copyAction_.setToolTip(tr("Copy the contents of a cell from the table."));
    copyAction_.setStatusTip(tr("Copy the contents of a cell from the table."));
    connect(&copyAction_, SIGNAL(triggered()), this, SLOT(onCopyAction()), Qt::UniqueConnection);

    pasteAction_.setToolTip(tr("Paste the contents of a cell to the table."));
    pasteAction_.setStatusTip(tr("Paste the contents of a cell to the table."));
    connect(&pasteAction_, SIGNAL(triggered()), this, SLOT(onPasteAction()), Qt::UniqueConnection);

    importAction_.setToolTip(tr("Import a csv-file to the table."));
    importAction_.setStatusTip(tr("Import a csv-file to the table."));
    connect(&importAction_, SIGNAL(triggered()), this, SLOT(onCSVImport()), Qt::UniqueConnection);

    exportAction_.setToolTip(tr("Export table to a csv-file."));
    exportAction_.setStatusTip(tr("Export table to a csv-file."));
    connect(&exportAction_, SIGNAL(triggered()), this, SLOT(onCSVExport()), Qt::UniqueConnection);

    copyRowsAction_.setToolTip(tr("Copy a row from the table"));
    copyRowsAction_.setStatusTip(tr("Copy a row from the table"));
    connect(&copyRowsAction_, SIGNAL(triggered()), this, SLOT(onCopyRowsAction()), Qt::UniqueConnection);

    pasteRowsAction_.setToolTip(tr("Paste a row from the table"));
    pasteRowsAction_.setStatusTip(tr("Paste a row from the table"));
    connect(&pasteRowsAction_, SIGNAL(triggered()), this, SLOT(onPasteRowsAction()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapsView::onCopyAction()
//-----------------------------------------------------------------------------
void MemoryMapsView::onCopyAction()
{
    if (currentIndex().isValid())
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);

        QModelIndexList indexes = selectedIndexes();
        std::sort(indexes.begin(), indexes.end());

        QString copyText;

        for (int row = indexes.first().row(); row <= indexes.last().row(); ++row)
        {
            for (int column = indexes.first().column(); column <= indexes.last().column(); ++column)
            {
                QModelIndex itemToAdd = model()->index(row, column, QModelIndex());

                copyText.append(model()->data(itemToAdd, Qt::DisplayRole).toString());

                if (column < indexes.last().column())
                {
                    copyText.append("\t");
                }
                else
                {
                    copyText.append("\n");
                }
            }
        }

        QClipboard* clipBoard = QApplication::clipboard();
        clipBoard->setText(copyText);
        QApplication::restoreOverrideCursor();
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapsView::onPasteAction()
//-----------------------------------------------------------------------------
void MemoryMapsView::onPasteAction()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    // find the highest row to start adding to
    QModelIndexList indexes = selectedIndexes();
    QModelIndex posToPaste;
    if (!indexes.isEmpty())
    {
        std::sort(indexes.begin(), indexes.end());
        posToPaste = indexes.first();
    }

    int startRow = posToPaste.row();
    int startCol = posToPaste.column();

    QClipboard* clipBoard = QApplication::clipboard();
    QString subtype("plain");
    QString pasteText = clipBoard->text(subtype);

    // split the string from clip board into rows
    QStringList rowsToAdd = pasteText.split("\n");

    // the model containing the actual data
    QAbstractItemModel* origModel = NULL;

    QSortFilterProxyModel* proxyModel = qobject_cast<QSortFilterProxyModel*>(model());

    // if view is connected to proxy model
    if (proxyModel)
    {
        origModel = qobject_cast<QAbstractItemModel*>(proxyModel->sourceModel());
    }
    // if view is connected directly to actual model
    else
    {
        origModel = qobject_cast<QAbstractItemModel*>(model());
    }
    Q_ASSERT(origModel);

    foreach (QString row, rowsToAdd)
    {
        if (row.isEmpty())
        {
            continue;
        }

        // new row starts always on same column
        int columnCounter = qMax(0, startCol);

        onAddItem();

        // split the row into columns
        QStringList columnsToAdd = row.split("\t");
        foreach (QString column, columnsToAdd)
        {
            QModelIndex itemToSet = origModel->index(startRow, columnCounter, QModelIndex());
            // if the index is not valid then the data is written to last item
            if (!itemToSet.isValid())
            {
                int lastRow = origModel->rowCount(QModelIndex()) - 1;

                itemToSet = origModel->index(lastRow, columnCounter, QModelIndex());
            }

            // Check for name conflicts.
            if ( columnCounter == MemoryMapsColumns::NAME_COLUMN && column.size() > 0 )
            {
                column = getUniqueName(column, origModel);
            }

            origModel->setData(itemToSet, column, Qt::EditRole);
            ++columnCounter;
        }

        if (startRow >= 0)
        {
            // when row is done then increase the row counter
            ++startRow;
        }
    }

    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapsView::onCSVImport()
//-----------------------------------------------------------------------------
void MemoryMapsView::onCSVImport(const QString& filePath /* = QString() */)
{
    QString target(filePath);

    if (filePath.isEmpty())
    {
        target = QFileDialog::getOpenFileName(this, tr("Open file"), defaultImportExportPath_,
            tr("csv-files (*.csv)"));
    }

    if (target.isEmpty())
    {
        return;
    }

    QFile file(target);

    // if file can not be opened 
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, tr("Error opening file"), tr("Could not open file %1 for reading").arg(target));
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    // create a stream to read from
    QTextStream stream(&file);

    // read the headers from the file
    QString headers = stream.readLine();

    // the model containing the actual data
    QAbstractTableModel* origModel = NULL;

    QSortFilterProxyModel* proxyModel = qobject_cast<QSortFilterProxyModel*>(model());

    // if view is connected to proxy model
    if (proxyModel)
    {
        origModel = qobject_cast<QAbstractTableModel*>(proxyModel->sourceModel());
    }
    // if view is connected directly to actual model
    else
    {
        origModel = qobject_cast<QAbstractTableModel*>(model());
    }
    Q_ASSERT(origModel);

    int columnCount = origModel->columnCount(QModelIndex());

    while (!stream.atEnd())
    {
        QString line = stream.readLine();
        QStringList columns = line.split(";");

        // add a new empty row
        onAddItem();

        // data is always added to the last row
        int rowCount = origModel->rowCount(QModelIndex());

        for (int col = 0; col < columnCount && col < columns.size(); ++col)
        {
            QModelIndex index = origModel->index(rowCount - 1, col, QModelIndex());
            origModel->setData(index, columns.at(col), Qt::EditRole);
        }
    }

    file.close();

    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapsView::onCSVExport()
//-----------------------------------------------------------------------------
void MemoryMapsView::onCSVExport(const QString& filePath /* = QString() */)
{
    QString target(filePath);

    if (filePath.isEmpty())
    {
        target = QFileDialog::getSaveFileName(this, tr("Set name and location for csv-file"),
            defaultImportExportPath_, tr("csv-files (*.csv)"));
    }

    if (target.isEmpty())
    {
        return;
    }

    QFile file(target);

    // if file can not be opened 
    if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly))
    {
        QMessageBox::critical(this, tr("Error opening file"), tr("Could not open file %1 for writing").arg(target));
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    int columnCount = model()->columnCount(QModelIndex());
    int rowCount = model()->rowCount(QModelIndex());

    // create a stream to write into
    QTextStream stream(&file);

    // write the headers
    for (int i = 0; i < columnCount; ++i)
    {
        stream << model()->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString().simplified();
        stream << ";";
    }
    stream << Qt::endl;

    // write each row
    for (int row = 0; row < rowCount; ++row)
    {
        // write each column
        for (int col = 0; col < columnCount; ++col)
        {
            QModelIndex index = model()->index(row, col, QModelIndex());
            stream << model()->data(index, Qt::DisplayRole).toString();
            stream << ";";
        }
        stream << Qt::endl;
    }
    file.close();

    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapsView::onCopyRowsAction()
//-----------------------------------------------------------------------------
void MemoryMapsView::onCopyRowsAction()
{
    // if nothing was selected then don't copy anything
    if (!currentIndex().isValid())
    {
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    QModelIndexList indexList = selectedIndexes();
    std::sort(indexList.begin(), indexList.end());

    QSortFilterProxyModel* sortProxy = dynamic_cast<QSortFilterProxyModel*>(model());

    QModelIndexList singleRowIndexList;
    for (int i = 0; i < indexList.size(); i++)
    {
        QModelIndex index = indexList.at(i);

        if (sortProxy != 0)
        {
            index = sortProxy->mapToSource(index);
        }

        singleRowIndexList.append(index);
    }

    emit copyRows(singleRowIndexList);

    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapsView::onPasteRowsAction()
//-----------------------------------------------------------------------------
void MemoryMapsView::onPasteRowsAction()
{
    QModelIndex pasteTarget = indexAt(pressedPoint_);

    QSortFilterProxyModel* sortProxy = dynamic_cast<QSortFilterProxyModel*>(model());
    if (sortProxy != 0)
    {
        pasteTarget = sortProxy->mapToSource(pasteTarget);
    }

    if (pasteTarget.parent().isValid())
    {
        pasteTarget = pasteTarget.parent();
    }

    emit pasteRows(pasteTarget);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapsView::getUniqueName()
//-----------------------------------------------------------------------------
QString MemoryMapsView::getUniqueName(QString const& original, QAbstractItemModel* model)
{
    QString name = original;    
    int trailingNumber = 1;
    bool match =  true;
    while ( match )
    {
        match = false;
        for(int row = 0; row < model->rowCount(); row++ )
        {
            QModelIndex index = model->index(row, MemoryMapsColumns::NAME_COLUMN);
            if ( name == model->data(index ,Qt::DisplayRole ).toString() )
            {
                match = true;
                name = original + "_" + QString::number(trailingNumber);
                trailingNumber++;
                break;
            }
        }       
    }

    return name;
}