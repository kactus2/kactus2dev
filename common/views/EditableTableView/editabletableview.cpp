//-----------------------------------------------------------------------------
// File: editabletableview.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 10.12.2011
//
// Description:
// This view can be used to display contents of a table model and supports adding and removing items.
//-----------------------------------------------------------------------------

#include "editabletableview.h"

#include <QHeaderView>
#include <QMenu>
#include <QKeySequence>
#include <QClipboard>
#include <QApplication>
#include <QModelIndexList>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QSortFilterProxyModel>
#include <QAbstractTableModel>
#include <QApplication>
#include <QFontMetrics>
#include <QSize>
#include <QHeaderView>

//-----------------------------------------------------------------------------
// Function: EditableTableView::EditableTableView()
//-----------------------------------------------------------------------------
EditableTableView::EditableTableView(QWidget *parent)
    : QTableView(parent),
      pressedPoint_(),
      addAction_(tr("Add row"), this),
      removeAction_(tr("Remove row"), this),
      copyAction_(tr("Copy"), this),
      pasteAction_(tr("Paste"), this),
      clearAction_(tr("Clear"), this),
      importAction_(tr("Import csv-file"), this),
      exportAction_(tr("Export csv-file"), this),
      importExportEnabled_(false),
      defImportExportPath_(),
      itemsDraggable_(true)
{
	// cells are resized to match contents 
	horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

	//last column is stretched take the available space in the widget
	horizontalHeader()->setStretchLastSection(true);

	// vertical headers are not visible
	verticalHeader()->hide();

	// set the height of a row to be smaller than default
	verticalHeader()->setDefaultSectionSize(fontMetrics().height() + 8);

	// easies to see the different rows from one another
	setAlternatingRowColors(true);

	// words are wrapped in the cells to minimize space usage
	setWordWrap(true);

	// user can select several items at a time
	setSelectionMode(QAbstractItemView::ContiguousSelection);

	setEditTriggers(QAbstractItemView::DoubleClicked |QAbstractItemView::SelectedClicked |
		QAbstractItemView::EditKeyPressed |	QAbstractItemView::AnyKeyPressed);

	setToolTip(tr("Double click to add a new item."));
	
	setupActions();

	setMinimumHeight(MINIMUM_TABLE_HEIGHT);
}

//-----------------------------------------------------------------------------
// Function: EditableTableView::~EditableTableView()
//-----------------------------------------------------------------------------
EditableTableView::~EditableTableView()
{
}

//-----------------------------------------------------------------------------
// Function: EditableTableView::setAllowImportExport()
//-----------------------------------------------------------------------------
void EditableTableView::setAllowImportExport(bool allow)
{
    importExportEnabled_ = allow;
}

//-----------------------------------------------------------------------------
// Function: EditableTableView::importExportAllowed()
//-----------------------------------------------------------------------------
bool EditableTableView::importExportAllowed() const
{
    return importExportEnabled_;
}

//-----------------------------------------------------------------------------
// Function: EditableTableView::mouseMoveEvent()
//-----------------------------------------------------------------------------
void EditableTableView::mouseMoveEvent(QMouseEvent* event)
{
    QModelIndex selected;

	if (itemsDraggable_)
    {
		// if left mouse button was pressed 
		if (event->buttons() & Qt::LeftButton)
        {
			QModelIndex startIndex = indexAt(pressedPoint_);
			QModelIndex thisIndex = indexAt(event->pos());
            selected = thisIndex;

			// if the model is a sort proxy then convert the indexes to source indexes
			QSortFilterProxyModel* sortProxy = dynamic_cast<QSortFilterProxyModel*>(model());
			if (sortProxy)
            {
				startIndex = sortProxy->mapToSource(startIndex);
				thisIndex = sortProxy->mapToSource(thisIndex);
			}

			// if the item was dragged to new location
			if (startIndex.isValid() && startIndex != thisIndex)
            {
				setCursor(QCursor(Qt::ClosedHandCursor));
				emit moveItem(startIndex, thisIndex);

				// update the pressed point so the dragging works also
				// when moving further to another index
				pressedPoint_ = event->pos();
			}
		}
	}

	QTableView::mouseMoveEvent(event);

	// if item is being dragged then do not select an area
	if (itemsDraggable_)
    {
		clearSelection();
		setCurrentIndex(selected);
	}
}

//-----------------------------------------------------------------------------
// Function: EditableTableView::keyPressEvent()
//-----------------------------------------------------------------------------
void EditableTableView::keyPressEvent(QKeyEvent* event)
{
	QTableView::keyPressEvent(event);

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
// Function: EditableTableView::mouseDoubleClickEvent()
//-----------------------------------------------------------------------------
void EditableTableView::mouseDoubleClickEvent(QMouseEvent* event)
{
	// if there is no item on the clicked position then a new item should be added
	QModelIndex index = indexAt(event->pos());
	if (!index.isValid())
    {
		emit addItem(index);
		event->accept();
		return;
	}

	QTableView::mouseDoubleClickEvent(event);
}

//-----------------------------------------------------------------------------
// Function: EditableTableView::mousePressEvent()
//-----------------------------------------------------------------------------
void EditableTableView::mousePressEvent(QMouseEvent* event)
{
	pressedPoint_ = event->pos();

	// if user clicked area that has no item
	QModelIndex pressedIndex = indexAt(pressedPoint_);
	if (!pressedIndex.isValid())
    {
		setCurrentIndex(pressedIndex);
	}

	QTableView::mousePressEvent(event);
}

//-----------------------------------------------------------------------------
// Function: EditableTableView::mouseReleaseEvent()
//-----------------------------------------------------------------------------
void EditableTableView::mouseReleaseEvent(QMouseEvent* event)
{
	setCursor(QCursor(Qt::ArrowCursor));
	QTableView::mouseReleaseEvent(event);
}

//-----------------------------------------------------------------------------
// Function: EditableTableView::contextMenuEvent()
//-----------------------------------------------------------------------------
void EditableTableView::contextMenuEvent(QContextMenuEvent* event)
{
	pressedPoint_ = event->pos();

	QModelIndex index = indexAt(pressedPoint_);

	QMenu menu(this);
	menu.addAction(&addAction_);

	// if at least one valid item is selected
	if (index.isValid())
    {
		menu.addAction(&removeAction_);
		menu.addAction(&clearAction_);
		menu.addAction(&copyAction_);
        menu.addAction(&pasteAction_);
    }
	if (importExportAllowed())
    {
        menu.addSeparator();
		menu.addAction(&importAction_);
		menu.addAction(&exportAction_);
	}

	menu.exec(event->globalPos());

	event->accept();
}

//-----------------------------------------------------------------------------
// Function: EditableTableView::onAddAction()
//-----------------------------------------------------------------------------
void EditableTableView::onAddAction()
{
	QModelIndexList indexes = selectedIndexes();
	QModelIndex posToAdd;

    QSortFilterProxyModel* sortProxy = dynamic_cast<QSortFilterProxyModel*>(model());

	if (!indexes.isEmpty())
    {
		qSort(indexes);
		posToAdd = indexes.first();

		if (sortProxy)
        {
			posToAdd = sortProxy->mapToSource(posToAdd);
		}
	}
    
    int rowsToAdd = qMax(1, countRows(indexes));
	for (int i = 0; i < rowsToAdd; ++i)
    {
		emit addItem(posToAdd);
	}
   
    QModelIndex firstCreatedIndex;
    int nameColumn = NAME_COLUMN;
    if (posToAdd.isValid() && (posToAdd.sibling(posToAdd.row(), NAME_COLUMN).flags() & Qt::ItemIsEditable) == 0)
    {
        nameColumn++;
    }

    if (posToAdd.isValid())
    {
        firstCreatedIndex = posToAdd.sibling(posToAdd.row(), nameColumn); 
        if (sortProxy)
        {
            firstCreatedIndex = sortProxy->mapFromSource(firstCreatedIndex);
        }
    }
    else
    {
        firstCreatedIndex = model()->index(model()->rowCount() - 1, nameColumn);
        if (sortProxy)
        {
            QModelIndex indexOnLastRow = sortProxy->sourceModel()->index(model()->rowCount() - 1, nameColumn);
            firstCreatedIndex = sortProxy->mapFromSource(indexOnLastRow);
        }
    }

    clearSelection();
    setCurrentIndex(firstCreatedIndex);
}

//-----------------------------------------------------------------------------
// Function: EditableTableView::countRows()
//-----------------------------------------------------------------------------
int EditableTableView::countRows(QModelIndexList const& indexes)
{
    if (indexes.empty())
    {
        return 0;
    }

    int rows = 1;

    int previousRow = indexes.first().row();
    foreach (QModelIndex index, indexes)
    {
        if (index.row() != previousRow)
        {
            rows++;
        }

        previousRow = index.row();
    }
    
    return rows;
}

//-----------------------------------------------------------------------------
// Function: EditableTableView::onRemoveAction()
//-----------------------------------------------------------------------------
void EditableTableView::onRemoveAction()
{
	QModelIndexList indexes = selectedIndexes();
	if (indexes.isEmpty())
    {
		return;
	}

	QApplication::setOverrideCursor(Qt::WaitCursor);

	qSort(indexes);
	int rowCount = qMax(1, countRows(indexes));

	// Remove as many rows as wanted.
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
// Function: EditableTableView::setupActions()
//-----------------------------------------------------------------------------
void EditableTableView::setupActions()
{
	addAction_.setToolTip(tr("Add a new row to table"));
	addAction_.setStatusTip(tr("Add a new row to table"));
	connect(&addAction_, SIGNAL(triggered()), this, SLOT(onAddAction()), Qt::UniqueConnection);
	addAction_.setShortcut(QKeySequence::InsertLineSeparator);

	removeAction_.setToolTip(tr("Remove a row from the table"));
	removeAction_.setStatusTip(tr("Remove a row from the table"));
	connect(&removeAction_, SIGNAL(triggered()), this, SLOT(onRemoveAction()), Qt::UniqueConnection);
    removeAction_.setShortcut(Qt::SHIFT + Qt::Key_Delete);

	copyAction_.setToolTip(tr("Copy a row from the table"));
	copyAction_.setStatusTip(tr("Copy a row from the table"));
	connect(&copyAction_, SIGNAL(triggered()), this, SLOT(onCopyAction()), Qt::UniqueConnection);
	copyAction_.setShortcut(QKeySequence::Copy);

	pasteAction_.setToolTip(tr("Paste a row to the table"));
	pasteAction_.setStatusTip(tr("Paste a row to the table"));
	connect(&pasteAction_, SIGNAL(triggered()), this, SLOT(onPasteAction()), Qt::UniqueConnection);
	pasteAction_.setShortcut(QKeySequence::Paste);

	clearAction_.setToolTip(tr("Clear the contents of a cell"));
	clearAction_.setStatusTip(tr("Clear the contents of a cell"));
	connect(&clearAction_, SIGNAL(triggered()),	this, SLOT(onClearAction()), Qt::UniqueConnection);
	clearAction_.setShortcut(QKeySequence::Delete);

	importAction_.setToolTip(tr("Import a csv-file to table"));
	importAction_.setStatusTip(tr("Import a csv-file to table"));
	connect(&importAction_, SIGNAL(triggered()), this, SLOT(onCSVImport()), Qt::UniqueConnection);

	exportAction_.setToolTip(tr("Export table to a csv-file"));
	exportAction_.setStatusTip(tr("Export table to a csv-file"));
	connect(&exportAction_, SIGNAL(triggered()), this, SLOT(onCSVExport()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: EditableTableView::setItemsDraggable()
//-----------------------------------------------------------------------------
void EditableTableView::setItemsDraggable(bool draggable)
{
	itemsDraggable_ = draggable;
}

//-----------------------------------------------------------------------------
// Function: EditableTableView::onCopyAction()
//-----------------------------------------------------------------------------
void EditableTableView::onCopyAction()
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
// Function: EditableTableView::onPasteAction()
//-----------------------------------------------------------------------------
void EditableTableView::onPasteAction()
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

	int targetRow = posToPaste.row();
	int startColumn = posToPaste.column();

    QString subtype("plain");
    QString pasteText = QApplication::clipboard()->text(subtype);

	// Split the string from clip board into rows.
	QStringList rowsToAdd = pasteText.split("\n", QString::SkipEmptyParts);

    bool useDynamicSorting = false;
    QSortFilterProxyModel* proxyModel = qobject_cast<QSortFilterProxyModel*>(model());
    if (proxyModel)
    {
        useDynamicSorting = proxyModel->dynamicSortFilter();
        proxyModel->setDynamicSortFilter(false);
    }

	foreach (QString row, rowsToAdd)
    {
		// New row starts always on same column.
		int targetColumn = qMax(0, startColumn);

		// Split the row into columns.
		QStringList columnsToAdd = row.split("\t");
		foreach (QString column, columnsToAdd)
        {
			QModelIndex itemToSet = model()->index(targetRow, targetColumn, QModelIndex());
			if (itemToSet.isValid())
            {
                // Check for name conflicts.   
                if (targetColumn == NAME_COLUMN && column.size() > 0)
                {
                    column = getUniqueName(column);
                }

                model()->setData(itemToSet, column, Qt::EditRole);
			}

			targetColumn++;
		}
		
		if (targetRow >= 0)
        {
			targetRow++;
		}
	}

    if (proxyModel)
    {
        proxyModel->setDynamicSortFilter(useDynamicSorting);
    }

	QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: EditableTableView::onClearAction()
//-----------------------------------------------------------------------------
void EditableTableView::onClearAction()
{	
	QModelIndexList indexes = selectedIndexes();

	// clear the contents of each cell
	foreach (QModelIndex index, indexes)
    {
		model()->setData(index, QVariant(), Qt::EditRole);
	}
}

//-----------------------------------------------------------------------------
// Function: EditableTableView::onCSVExport()
//-----------------------------------------------------------------------------
void EditableTableView::onCSVExport(const QString& filePath)
{
	QString target(filePath);

	if (filePath.isEmpty())
    {
		target = QFileDialog::getSaveFileName(this, tr("Set name and location for csv-file"),
			defImportExportPath_, tr("csv-files (*.csv)"));
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
	for (int i = 0; i < columnCount; i++)
    {
		stream << model()->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString().simplified() << ";";
	}
	stream << endl;

	// write each row
	for (int row = 0; row < rowCount; row++)
    {	
		// write each column
		for (int column = 0; column < columnCount; column++)
        {
			QModelIndex index = model()->index(row, column, QModelIndex());
			stream << index.data(Qt::DisplayRole).toString();
			stream << ";";
		}
		stream << endl;
	}
	file.close();

	QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: EditableTableView::onCSVImport()
//-----------------------------------------------------------------------------
void EditableTableView::onCSVImport(const QString& filePath)
{
	QString target(filePath);
	
	if (filePath.isEmpty())
    {
		target = QFileDialog::getOpenFileName(this, tr("Open file"), defImportExportPath_, tr("csv-files (*.csv)"));
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
// Function: EditableTableView::setDefaultImportExportPath()
//-----------------------------------------------------------------------------
void EditableTableView::setDefaultImportExportPath(const QString& path)
{
	defImportExportPath_ = path;
}

//-----------------------------------------------------------------------------
// Function: EditableTableView::setModel()
//-----------------------------------------------------------------------------
void EditableTableView::setModel(QAbstractItemModel* model)
{
	// the base class implementation does most of the work
	QTableView::setModel(model);

	// contains info on the used font
	QFontMetrics fMetrics = fontMetrics();

	// set the widths for the columns
	int columnCount = model->columnCount(QModelIndex());
	for (int i = 0; i < columnCount; ++i)
    {
		// the width required by the contents of the model
		int contentSize = sizeHintForColumn(i);

		// get the header for the section
		QString headerText = model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
		// if the header contains several lines
		QStringList headerLines = headerText.split("\n", QString::SkipEmptyParts);
		int headerSize = 0;

		// find the line that needs most space
		foreach (QString headerLine, headerLines)
        {
			headerSize = qMax(headerSize, fMetrics.width(headerLine));
		}
        headerSize += 45;

        // set the width for the column
        setColumnWidth(i, qMax(contentSize, headerSize));
    }
}

//-----------------------------------------------------------------------------
// Function: EditableTableView::getUniqueName()
//-----------------------------------------------------------------------------
QString EditableTableView::getUniqueName(QString const& original)
{
    int rowCount = model()->rowCount();

    QString name = original;    
    int trailingNumber = 1;
    
    bool match =  true;
    while (match)
    {
        match = false;        
        for(int row = 0; row < rowCount; row++)
        {
            QModelIndex index = model()->index(row, NAME_COLUMN);
            if (name == index.data(Qt::DisplayRole).toString())
            {
                match = true;
                name = original + "_" + QString::number(trailingNumber);
                trailingNumber++;
            }
        }       
    }
    
    return name;
}
