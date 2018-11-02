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

#include <QAbstractButton>
#include <QAbstractTableModel>
#include <QApplication>
#include <QClipboard>
#include <QFile>
#include <QFileDialog>
#include <QFontMetrics>
#include <QHeaderView>
#include <QKeySequence>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>
#include <QModelIndexList>
#include <QSize>
#include <QSortFilterProxyModel>
#include <QStylePainter>
#include <QTextStream>

//-----------------------------------------------------------------------------
// Function: EditableTableView::EditableTableView()
//-----------------------------------------------------------------------------
EditableTableView::EditableTableView(QWidget *parent):
QTableView(parent),
    pressedPoint_(),
    addAction_(tr("Add row"), this),
    removeAction_(tr("Remove row"), this),
    cutAction_(tr("Cut"), this),
    copyAction_(tr("Copy"), this),
    pasteAction_(tr("Paste"), this),
    clearAction_(tr("Clear"), this),
    importAction_(tr("Import csv-file"), this),
    exportAction_(tr("Export csv-file"), this),
    copyElementAction_(tr("Copy element"), this),
    pasteElementAction_(tr("Paste element"), this),
    importExportEnabled_(false),
    elementCopyIsAllowed_(false),
    defImportExportPath_(),
    itemsDraggable_(true)
{
    horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    horizontalHeader()->setStretchLastSection(true);

    verticalHeader()->hide();

    // set the height of a row to be smaller than default
    verticalHeader()->setDefaultSectionSize(fontMetrics().height() + 8);

    QAbstractButton* cornerButton = findChild<QAbstractButton*>();
    if (cornerButton != 0)
    {
        cornerButton->setText(tr("Name"));
        cornerButton->setToolTip(tr("Click to select all"));
        cornerButton->installEventFilter(this);
    }

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
// Function: editabletableview::setAllowElementCopying()
//-----------------------------------------------------------------------------
void EditableTableView::setAllowElementCopying(bool allow)
{
    elementCopyIsAllowed_ = allow;
}

//-----------------------------------------------------------------------------
// Function: EditableTableView::setCornerButtonText()
//-----------------------------------------------------------------------------
void EditableTableView::setCornerButtonText(QString const& text)
{
    QAbstractButton* cornerButton = findChild<QAbstractButton*>();
    if (cornerButton != 0)
    {
        cornerButton->setText(text);
    }
}

//-----------------------------------------------------------------------------
// Function: EditableTableView::mouseMoveEvent()
//-----------------------------------------------------------------------------
void EditableTableView::mouseMoveEvent(QMouseEvent* event)
{
    QModelIndex selected = indexAt(event->pos());

	if (itemsDraggable_)
    {
		// if left mouse button was pressed 
		if (event->buttons() & Qt::LeftButton)
        {
			QModelIndex startIndex = indexAt(pressedPoint_);
			QModelIndex thisIndex = selected;

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

				// update the pressed point so the dragging works also when moving further to another index
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
    addBasicActionsForContextMenu(menu, index);
    addElementCopyActionForContextMenu(menu, index);
    addImportExportActionsForContextMenu(menu);

	menu.exec(event->globalPos());

	event->accept();
}

//-----------------------------------------------------------------------------
// Function: editabletableview::addBasicActionsForContextMenu()
//-----------------------------------------------------------------------------
void EditableTableView::addBasicActionsForContextMenu(QMenu& menu, QModelIndex const& index)
{
    menu.addAction(&addAction_);

    if (index.isValid())
    {
        menu.addAction(&removeAction_);
        menu.addAction(&clearAction_);
        menu.addAction(&cutAction_);
        menu.addAction(&copyAction_);
        menu.addAction(&pasteAction_);
    }
}

//-----------------------------------------------------------------------------
// Function: editabletableview::addElementCopyActionForContextMenu()
//-----------------------------------------------------------------------------
void EditableTableView::addElementCopyActionForContextMenu(QMenu& menu, QModelIndex const& index)
{
    bool validIndex = index.isValid();

    if (elementCopyIsAllowed_)
    {
        const QMimeData* clipMimeData = QApplication::clipboard()->mimeData();
        QString modelAcceptedMimeType = model()->mimeTypes().last();

        bool containsMimeData = (clipMimeData->hasImage() && clipMimeData->hasFormat(modelAcceptedMimeType));

        if (validIndex || containsMimeData)
        {
            menu.addSeparator();
            if (validIndex)
            {
                menu.addAction(&copyElementAction_);
            }
            if (containsMimeData)
            {
                menu.addAction(&pasteElementAction_);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: editabletableview::addImportExportActionsForContextMenu()
//-----------------------------------------------------------------------------
void EditableTableView::addImportExportActionsForContextMenu(QMenu& menu)
{
    if (importExportAllowed())
    {
        menu.addSeparator();
        menu.addAction(&importAction_);
        menu.addAction(&exportAction_);
    }
}

//-----------------------------------------------------------------------------
// Function: EditableTableView::eventFilter()
//-----------------------------------------------------------------------------
bool EditableTableView::eventFilter(QObject* target, QEvent* event)
{
    if (event->type() == QEvent::Paint)
    {
        QAbstractButton* cornerButton = qobject_cast<QAbstractButton*>(target);
        if (cornerButton)
        {
            // paint by hand (borrowed from QTableCornerButton)
            QStyleOptionHeader opt;
            opt.init(cornerButton);

            QStyle::State styleState = QStyle::State_None;

            if (cornerButton->isEnabled())
            {
                styleState |= QStyle::State_Enabled;
            }
            if (cornerButton->isActiveWindow())
            {
                styleState |= QStyle::State_Active;
            }
            if (cornerButton->isDown())
            {
                styleState |= QStyle::State_Sunken;
            }

            opt.state = styleState;
            opt.rect = cornerButton->rect();
            opt.text = cornerButton->text();
            opt.textAlignment = Qt::AlignCenter;
            opt.fontMetrics = horizontalHeader()->fontMetrics();
            opt.position = QStyleOptionHeader::OnlyOneSection;

            QStylePainter painter(cornerButton);
            painter.drawControl(QStyle::CE_Header, opt);
            return true;
        }
    }

    return false;
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
// Function: editabletableview::onCutAction()
//-----------------------------------------------------------------------------
void EditableTableView::onCutAction()
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
    
    QString textType = QStringLiteral("plain");
    QString pasteText = QApplication::clipboard()->text(textType);

	// Split the string from clip board into rows.
	QStringList rowsToAdd = pasteText.split("\n", QString::SkipEmptyParts);

    bool useDynamicSorting = false;
    QSortFilterProxyModel* proxyModel = qobject_cast<QSortFilterProxyModel*>(model());
    if (proxyModel)
    {
        useDynamicSorting = proxyModel->dynamicSortFilter();
        proxyModel->setDynamicSortFilter(false);
    }

	foreach (QString const& row, rowsToAdd)
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
// Function: editabletableview::onCopyElementAction()
//-----------------------------------------------------------------------------
void EditableTableView::onCopyElementAction()
{
    // if nothing was selected then don't copy anything
    if (!currentIndex().isValid())
    {
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    QModelIndexList indexList = selectedIndexes();
    qSort(indexList);

    QSortFilterProxyModel* sortProxy = dynamic_cast<QSortFilterProxyModel*>(model());

    QVector<int> usedRows;

    QModelIndexList singleRowIndexList;
    for (int i = 0; i < indexList.size(); i++)
    {
        QModelIndex index = indexList.at(i);

        if (sortProxy != 0)
        {
            index = sortProxy->mapToSource(index);
        }

        if (!usedRows.contains(index.row()))
        {
            usedRows.append(index.row());
            singleRowIndexList.append(index);
        }
    }

    emit copyRows(singleRowIndexList);

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
            QString data = index.data(Qt::EditRole).toString();

            if (data.isEmpty())
            {
                data = index.data(Qt::DisplayRole).toString();
            }

            bool multiline = data.contains(QStringLiteral("\n"));
            if (multiline)
            {
                stream << "\"" << data << "\"" << ";";
            }
            else
            {
                stream << data << ";";
            }
			
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
	QAbstractTableModel* targetModel = NULL;
	QSortFilterProxyModel* proxyModel = qobject_cast<QSortFilterProxyModel*>(model());
	
	// if view is connected to proxy model
	if (proxyModel)
    {
		targetModel = qobject_cast<QAbstractTableModel*>(proxyModel->sourceModel());
	}
	// if view is connected directly to actual model
	else
    {
		targetModel = qobject_cast<QAbstractTableModel*>(model());
	}
	Q_ASSERT(targetModel);

	int columnCount = targetModel->columnCount(QModelIndex());

	while (!stream.atEnd())
    {
		QString line = stream.readLine();
        while (line.count(QLatin1Char(';')) < columnCount - 1 && !stream.atEnd())
        {
            line.append(QStringLiteral("\n"));
            line.append(stream.readLine());
        }

		QStringList items = line.split(";");

        // add a new empty row
        // data is always added to the last row
		emit addItem(QModelIndex());
		int rowCount = targetModel->rowCount(QModelIndex());

		for (int column = 0; column < columnCount && column < items.size(); ++column)
        {
            QString item = items.at(column);
            if (item.startsWith(QLatin1Char('"')) && item.endsWith(QLatin1Char('"')))
            {
                item = item.mid(1, item.length() - 2);
            }

			QModelIndex index = targetModel->index(rowCount - 1, column, QModelIndex());
			targetModel->setData(index, item, Qt::EditRole);
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
// Function: EditableTableView::setupActions()
//-----------------------------------------------------------------------------
void EditableTableView::setupActions()
{
    addAction(&addAction_);
    addAction_.setToolTip(tr("Add a new row to table"));
    addAction_.setStatusTip(tr("Add a new row to table"));
    addAction_.setShortcut(QKeySequence::InsertLineSeparator);
    addAction_.setShortcutContext(Qt::WidgetShortcut);
    connect(&addAction_, SIGNAL(triggered()), this, SLOT(onAddAction()), Qt::UniqueConnection);

    addAction(&removeAction_);
    removeAction_.setToolTip(tr("Remove a row from the table"));
    removeAction_.setStatusTip(tr("Remove a row from the table"));    
    removeAction_.setShortcut(Qt::SHIFT + Qt::Key_Delete);
    removeAction_.setShortcutContext(Qt::WidgetShortcut);
    connect(&removeAction_, SIGNAL(triggered()), this, SLOT(onRemoveAction()), Qt::UniqueConnection);

    addAction(&cutAction_);
    cutAction_.setToolTip(tr("Cut the contents of a cell from the table"));
    cutAction_.setStatusTip(tr("Cut the contents of a cell from the table"));
    cutAction_.setShortcut(QKeySequence::Cut);
    cutAction_.setShortcutContext(Qt::WidgetShortcut);
    connect(&cutAction_, SIGNAL(triggered()), this, SLOT(onCutAction()), Qt::UniqueConnection);

    addAction(&copyAction_);
    copyAction_.setToolTip(tr("Copy the contents of a cell from the table"));
    copyAction_.setStatusTip(tr("Copy the contents of a cell from the table"));
    copyAction_.setShortcut(QKeySequence::Copy);
    copyAction_.setShortcutContext(Qt::WidgetShortcut);
    connect(&copyAction_, SIGNAL(triggered()), this, SLOT(onCopyAction()), Qt::UniqueConnection);

    addAction(&pasteAction_);
    pasteAction_.setToolTip(tr("Paste the contents of a cell to the table"));
    pasteAction_.setStatusTip(tr("Paste the contents of a cell to the table"));
    pasteAction_.setShortcut(QKeySequence::Paste);
    pasteAction_.setShortcutContext(Qt::WidgetShortcut);
    connect(&pasteAction_, SIGNAL(triggered()), this, SLOT(onPasteAction()), Qt::UniqueConnection);

    addAction(&clearAction_);
    clearAction_.setToolTip(tr("Clear the contents of a cell"));
    clearAction_.setStatusTip(tr("Clear the contents of a cell"));
    clearAction_.setShortcut(QKeySequence::Delete);
    clearAction_.setShortcutContext(Qt::WidgetShortcut);
    connect(&clearAction_, SIGNAL(triggered()),	this, SLOT(onClearAction()), Qt::UniqueConnection);

    addAction(&importAction_);
    importAction_.setToolTip(tr("Import a csv-file to table"));
    importAction_.setStatusTip(tr("Import a csv-file to table"));    
    connect(&importAction_, SIGNAL(triggered()), this, SLOT(onCSVImport()), Qt::UniqueConnection);

    addAction(&exportAction_);
    exportAction_.setToolTip(tr("Export table to a csv-file"));
    exportAction_.setStatusTip(tr("Export table to a csv-file"));
    connect(&exportAction_, SIGNAL(triggered()), this, SLOT(onCSVExport()), Qt::UniqueConnection);

    addAction(&copyElementAction_);
    copyElementAction_.setToolTip(tr("Copy a row from the table"));
    copyElementAction_.setStatusTip(tr("Copy a row from the table"));
    connect(&copyElementAction_, SIGNAL(triggered()), this, SLOT(onCopyElementAction()), Qt::UniqueConnection);

    addAction(&pasteElementAction_);
    pasteElementAction_.setToolTip(tr("Paste a row from the table"));
    pasteElementAction_.setStatusTip(tr("Paste a row from the table"));
    connect(&pasteElementAction_, SIGNAL(triggered()), this, SIGNAL(pasteRows()), Qt::UniqueConnection);
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
