//-----------------------------------------------------------------------------
// File: MemoryMapsView.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
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

//-----------------------------------------------------------------------------
// Function: MemoryMapsView::MemoryMapsView()
//-----------------------------------------------------------------------------
MemoryMapsView::MemoryMapsView(QWidget* parent):
ConfigurableElementsView(parent),
pressedPoint_(),
addMemoryRemapAction_(tr("Add memory remap"), this),
addAction_(tr("Add memory map"), this),
copyAction_(tr("Copy"), this),
pasteAction_(tr("Paste"), this),
clearAction_(tr("Clear"), this),
importAction_(tr("Import csv-file"), this),
exportAction_(tr("Export csv-file"), this),
importExportable_(false),
defaultImportExportPath_()
{
    getRemoveAction()->setText("Remove row");

    setAlternatingRowColors(false);

    setSelectionMode(QAbstractItemView::ContiguousSelection);

    setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked |
        QAbstractItemView::EditKeyPressed | QAbstractItemView::AnyKeyPressed);

    setToolTip("Double click to add a new memory map.");

    setupActions();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapsView::~MemoryMapsView()
//-----------------------------------------------------------------------------
MemoryMapsView::~MemoryMapsView()
{

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
    if (event->matches(QKeySequence::Delete))
    {
        onClearAction();
    }
    if (event->matches(QKeySequence::InsertLineSeparator))
    {
        onAddAction();
    }
    if (event->matches(QKeySequence::Cut))
    {
        onRemoveAction();
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapsView::mouseDoubleClickEvent()
//-----------------------------------------------------------------------------
void MemoryMapsView::mouseDoubleClickEvent(QMouseEvent* event)
{
    QModelIndex index = indexAt(event->pos());
    if (!index.isValid())
    {
        emit addItem(index);
        event->accept();
        //return;
    }
    else
    {
        QTreeView::mouseDoubleClickEvent(event);
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

    menu.addAction(&addAction_);
    if (!index.parent().isValid() && index.isValid())
    {
        menu.addAction(&addMemoryRemapAction_);
    }

    if (index.isValid())
    {
        menu.addAction(getRemoveAction());
    }

    menu.addAction(&pasteAction_);

    if (index.isValid())
    {
        menu.addAction(&clearAction_);
        menu.addAction(&copyAction_);
    }

    if (importExportable_)
    {
        menu.addSeparator();
        menu.addAction(&importAction_);
        menu.addAction(&exportAction_);
    }

    menu.exec(event->globalPos());
    event->accept();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapsView::setupActions()
//-----------------------------------------------------------------------------
void MemoryMapsView::setupActions()
{
    addAction_.setToolTip(tr("Add a new row to the table."));
    addAction_.setStatusTip(tr("Add a new row to the table."));
    connect(&addAction_, SIGNAL(triggered()), this, SLOT(onAddAction()), Qt::UniqueConnection);

    getRemoveAction()->setToolTip(tr("Remove a row from the table."));
    getRemoveAction()->setStatusTip(tr("Remove a row from the table."));
    disconnect(getRemoveAction(), 0, this, 0);
    connect(getRemoveAction(), SIGNAL(triggered()), this, SLOT(onRemoveAction()), Qt::UniqueConnection);

    addMemoryRemapAction_.setToolTip(tr("Add a new memory remap to the selected memory map."));
    addMemoryRemapAction_.setStatusTip(tr("Add a new memory remap to the selected memory map."));
    connect(&addMemoryRemapAction_, SIGNAL(triggered()), this, SLOT(onAddMemoryRemapAction()), Qt::UniqueConnection);

    copyAction_.setToolTip(tr("Copy a row from the table."));
    copyAction_.setStatusTip(tr("Copy a row from the table."));
    connect(&copyAction_, SIGNAL(triggered()), this, SLOT(onCopyAction()), Qt::UniqueConnection);

    pasteAction_.setToolTip(tr("Paste a row to the table."));
    pasteAction_.setStatusTip(tr("Paste a row to the table."));
    connect(&pasteAction_, SIGNAL(triggered()), this, SLOT(onPasteAction()), Qt::UniqueConnection);

    clearAction_.setToolTip(tr("Clear the contents of a cell."));
    clearAction_.setStatusTip(tr("Clear the contents of a cell."));
    connect(&clearAction_, SIGNAL(triggered()), this, SLOT(onClearAction()), Qt::UniqueConnection);

    importAction_.setToolTip(tr("Import a csv-file to the table."));
    importAction_.setStatusTip(tr("Import a csv-file to the table."));
    connect(&importAction_, SIGNAL(triggered()), this, SLOT(onCSVImport()), Qt::UniqueConnection);

    exportAction_.setToolTip(tr("Export table to a csv-file."));
    exportAction_.setStatusTip(tr("Export table to a csv-file."));
    connect(&exportAction_, SIGNAL(triggered()), this, SLOT(onCSVExport()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapsView::onAddActions()
//-----------------------------------------------------------------------------
void MemoryMapsView::onAddAction()
{
    QModelIndexList indexes = selectedIndexes();
    QModelIndex posToAdd;

    int rowCount = 1;

    QSortFilterProxyModel* sortProxy = dynamic_cast<QSortFilterProxyModel*>(model());

    if (!indexes.isEmpty())
    {
        qSort(indexes);

        posToAdd = indexes.first();
        
        if (sortProxy)
        {
            posToAdd = sortProxy->mapToSource(posToAdd);
        }

        int previousRow = indexes.first().row();
        foreach (QModelIndex index, indexes)
        {
            if (index.row() != previousRow)
            {
                ++rowCount;
            }

            previousRow = index.row();
        }
    }

    for (int i = 0; i < rowCount; ++i)
    {
        emit(addItem(posToAdd));
    }
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
        qSort(indexes);

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
        qSort(indexes);
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

        QModelIndex newRow = origModel->index(startRow, columnCounter, QModelIndex());
        emit addItem(newRow);

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
// Function: MemoryMapsView::onClearAction()
//-----------------------------------------------------------------------------
void MemoryMapsView::onClearAction()
{
    QModelIndexList indexes = selectedIndexes();

    // clear the contents of each cell
    foreach (QModelIndex index, indexes)
    {
        model()->setData(index, QVariant(), Qt::EditRole);
    }
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
        emit addItem(QModelIndex());

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
    stream << endl;

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
        stream << endl;
    }
    file.close();

    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapsView::onAddMemoryRemapAction()
//-----------------------------------------------------------------------------
void MemoryMapsView::onAddMemoryRemapAction()
{
    QModelIndex parentMemoryMapIndex = currentIndex();

    QSortFilterProxyModel* sortProxy = dynamic_cast<QSortFilterProxyModel*>(model());
    if (sortProxy)
    {
        parentMemoryMapIndex = sortProxy->mapToSource(parentMemoryMapIndex);
    }

    QModelIndex parentNameIndex = 
        parentMemoryMapIndex.sibling(parentMemoryMapIndex.row(), MemoryMapsColumns::NAME_COLUMN);

    emit addMemoryRemapItem(parentNameIndex);

    expand(parentNameIndex);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapsView::onRemoveAction()
//-----------------------------------------------------------------------------
void MemoryMapsView::onRemoveAction()
{
    QModelIndexList indexes = selectedIndexes();
    if (indexes.isEmpty())
    {
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    qSort(indexes);

    // count how many rows the user wants to remove
    int previousRow = indexes.first().row();
    int rowCount = 1;
    foreach (QModelIndex index, indexes)
    {
        if (previousRow != index.row())
        {
            ++rowCount;
        }
        previousRow = index.row();
    }

    // remove as many rows as wanted
    QSortFilterProxyModel* sortProxy = dynamic_cast<QSortFilterProxyModel*>(model());

    for (int i = 0; i < rowCount; ++i)
    {
        QModelIndex index = indexes.first();

        if (sortProxy != 0)
        {
            index = sortProxy->mapToSource(index);
        }

        emit removeItem(index);
    }

    clearSelection();
    setCurrentIndex(QModelIndex());

    QApplication::restoreOverrideCursor();

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