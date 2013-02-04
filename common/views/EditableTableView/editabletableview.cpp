/* 
 *  	Created on: 10.12.2011
 *      Author: Antti Kamppi
 * 		filename: editabletableview.cpp
 *		Project: Kactus 2
 */

#include "editabletableview.h"
#include <LibraryManager/vlnv.h>

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

EditableTableView::EditableTableView(QWidget *parent):
QTableView(parent),
defImportExportPath_(),
addAction_(tr("Add row"), this),
removeAction_(tr("Remove row"), this),
copyAction_(tr("Copy"), this),
pasteAction_(tr("Paste"), this),
clearAction_(tr("Clear"), this),
importAction_(tr("Import csv-file"), this),
exportAction_(tr("Export csv-file"), this),
itemsDraggable_(true),
impExportable_(false) {

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

	// user can only select one item at a time
	setSelectionMode(QAbstractItemView::ContiguousSelection);

	setEditTriggers(QAbstractItemView::DoubleClicked |
		QAbstractItemView::SelectedClicked |
		QAbstractItemView::EditKeyPressed |
		QAbstractItemView::AnyKeyPressed);

	setToolTip(tr("Double click to add a new item."));
	
	setupActions();

	setMinimumHeight(MINIMUM_TABLE_HEIGHT);
}

EditableTableView::~EditableTableView() {
}

void EditableTableView::mouseMoveEvent( QMouseEvent* e ) {

		QModelIndex selected;

	if (itemsDraggable_) {

		// if left mouse button was pressed 
		if (e->buttons() & Qt::LeftButton) {

			QModelIndex startIndex = indexAt(pressedPoint_);
			QModelIndex thisIndex = indexAt(e->pos());
			selected = thisIndex;

			// if the model is a sort proxy then convert the indexes to source indexes
			QSortFilterProxyModel* sortProxy = dynamic_cast<QSortFilterProxyModel*>(model());
			if (sortProxy) {

				startIndex = sortProxy->mapToSource(startIndex);
				thisIndex = sortProxy->mapToSource(thisIndex);
			}

			// if the item was dragged to new location
			if (startIndex.isValid() && startIndex != thisIndex) {
				setCursor(QCursor(Qt::ClosedHandCursor));

				emit moveItem(startIndex, thisIndex);

				// update the pressed point so the dragging works also
				// when moving further to another index
				pressedPoint_ = e->pos();
			}
		}
	}

	QTableView::mouseMoveEvent(e);

	// if item is being dragged then do not select an area
	if (itemsDraggable_) {
		clearSelection();
		setCurrentIndex(selected);
	}
}

void EditableTableView::keyPressEvent( QKeyEvent* event ) {

	// call the base class implementation
	QTableView::keyPressEvent(event);

	if (event->matches(QKeySequence::Copy)) {
		onCopyAction();
	}
	if (event->matches(QKeySequence::Paste)) {
		onPasteAction();
	}
	if (event->matches(QKeySequence::Delete)) {
		onClearAction();
	}
	if (event->matches(QKeySequence::InsertLineSeparator)) {
		onAddAction();
	}
}

void EditableTableView::mouseDoubleClickEvent( QMouseEvent* event ) {
	// if there is no item on the clicked position then a new item should be added
	QModelIndex index = indexAt(event->pos());
	if (!index.isValid()){
		emit addItem(index);
		event->accept();
		return;
	}

	QTableView::mouseDoubleClickEvent(event);
}

void EditableTableView::mousePressEvent( QMouseEvent* event ) {
	pressedPoint_ = event->pos();

	// if user clicked area that has no item
	QModelIndex pressedIndex = indexAt(pressedPoint_);
	if (!pressedIndex.isValid()) {
		setCurrentIndex(pressedIndex);
	}

	QTableView::mousePressEvent(event);
}

void EditableTableView::mouseReleaseEvent( QMouseEvent* event ) {
	setCursor(QCursor(Qt::ArrowCursor));
	QTableView::mouseReleaseEvent(event);
}

void EditableTableView::contextMenuEvent( QContextMenuEvent* event ) {
	pressedPoint_ = event->pos();

	QModelIndex index = indexAt(pressedPoint_);

	QMenu menu(this);
	menu.addAction(&addAction_);

	// if at least one valid item is selected
	if (index.isValid()) {
		menu.addAction(&removeAction_);
		menu.addAction(&clearAction_);
		menu.addAction(&copyAction_);
	}
	menu.addAction(&pasteAction_);

	if (impExportable_) {
		menu.addAction(&importAction_);
		menu.addAction(&exportAction_);
	}

	menu.exec(event->globalPos());

	event->accept();
}

void EditableTableView::onAddAction() {

	QModelIndexList indexes = selectedIndexes();
	QModelIndex posToAdd;
	int rowCount = 1;

	QSortFilterProxyModel* sortProxy = dynamic_cast<QSortFilterProxyModel*>(model());

	// if there were indexes selected
	if (!indexes.isEmpty()) {
		qSort(indexes);

		posToAdd = indexes.first();

		if (sortProxy) {
			posToAdd = sortProxy->mapToSource(posToAdd);
		}

		// count how many rows are to be added
		int previousRow = indexes.first().row();
		foreach (QModelIndex index, indexes) {
			
			if (index.row() != previousRow) {
				++rowCount;
			}

			previousRow = index.row();
		}
	}

	for (int i = 0; i < rowCount; ++i) {
		emit addItem(posToAdd);
	}
}

void EditableTableView::onRemoveAction() {
	QModelIndexList indexes = selectedIndexes();

	if (indexes.isEmpty()) {
		return;
	}

	QApplication::setOverrideCursor(Qt::WaitCursor);

	qSort(indexes);
	
	// count how many rows the user wants to remove
	int previousRow = indexes.first().row();
	int rowCount = 1;
	foreach (QModelIndex index, indexes) {
		if (previousRow != index.row()) {
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

void EditableTableView::setupActions() {
	addAction_.setToolTip(tr("Add a new row to table"));
	addAction_.setStatusTip(tr("Add a new row to table"));
	connect(&addAction_, SIGNAL(triggered()),
		this, SLOT(onAddAction()), Qt::UniqueConnection);
	addAction_.setShortcut(QKeySequence::InsertLineSeparator);

	removeAction_.setToolTip(tr("Remove a row from the table"));
	removeAction_.setStatusTip(tr("Remove a row from the table"));
	connect(&removeAction_, SIGNAL(triggered()),
		this, SLOT(onRemoveAction()), Qt::UniqueConnection);

	copyAction_.setToolTip(tr("Copy a row from the table"));
	copyAction_.setStatusTip(tr("Copy a row from the table"));
	connect(&copyAction_, SIGNAL(triggered()),
		this, SLOT(onCopyAction()), Qt::UniqueConnection);
	copyAction_.setShortcut(QKeySequence::Copy);

	pasteAction_.setToolTip(tr("Paste a row to the table"));
	pasteAction_.setStatusTip(tr("Paste a row to the table"));
	connect(&pasteAction_, SIGNAL(triggered()),
		this, SLOT(onPasteAction()), Qt::UniqueConnection);
	pasteAction_.setShortcut(QKeySequence::Paste);

	clearAction_.setToolTip(tr("Clear the contents of a cell"));
	clearAction_.setStatusTip(tr("Clear the contents of a cell"));
	connect(&clearAction_, SIGNAL(triggered()),
		this, SLOT(onClearAction()), Qt::UniqueConnection);
	clearAction_.setShortcut(QKeySequence::Delete);

	importAction_.setToolTip(tr("Import a csv-file to table"));
	importAction_.setStatusTip(tr("Import a csv-file to table"));
	connect(&importAction_, SIGNAL(triggered()),
		this, SLOT(onCSVImport()), Qt::UniqueConnection);

	exportAction_.setToolTip(tr("Export table to a csv-file"));
	exportAction_.setStatusTip(tr("Export table to a csv-file"));
	connect(&exportAction_, SIGNAL(triggered()),
		this, SLOT(onCSVExport()), Qt::UniqueConnection);
}

void EditableTableView::setItemsDraggable( bool draggable ) {
	itemsDraggable_ = draggable;
}

void EditableTableView::onCopyAction() {

	// if nothing was selected then don't copy anything
	if (!currentIndex().isValid()) {
		return;
	}

	QApplication::setOverrideCursor(Qt::WaitCursor);

	QModelIndexList indexes = selectedIndexes();
	qSort(indexes);

	QString copyText;

	for (int row = indexes.first().row(); row <= indexes.last().row(); ++row) {
		for (int column = indexes.first().column(); column <= indexes.last().column(); ++column) {

			QModelIndex itemToAdd = model()->index(row, column, QModelIndex());

			copyText.append(model()->data(itemToAdd, Qt::DisplayRole).toString());

			// separate columns with tab
			if (column < indexes.last().column()) {
				copyText.append("\t");
			}
			// for the last column don't add tab but new line instead
			else {
				copyText.append("\n");
			}
		}
	}

	QClipboard* clipBoard = QApplication::clipboard();

	clipBoard->setText(copyText);

	QApplication::restoreOverrideCursor();
}

void EditableTableView::onPasteAction() {

	QApplication::setOverrideCursor(Qt::WaitCursor);

	// find the highest row to start adding to
	QModelIndexList indexes = selectedIndexes();
	QModelIndex posToPaste;
	if (!indexes.isEmpty()) {
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
	QAbstractTableModel* origModel = NULL;

	QSortFilterProxyModel* proxyModel = qobject_cast<QSortFilterProxyModel*>(model());

	// if view is connected to proxy model
	if (proxyModel) {
		origModel = qobject_cast<QAbstractTableModel*>(proxyModel->sourceModel());
	}
	// if view is connected directly to actual model
	else {
		origModel = qobject_cast<QAbstractTableModel*>(model());
	}
	Q_ASSERT(origModel);

	foreach (QString row, rowsToAdd) {

		if (row.isEmpty()) {
			continue;
		}

		// new row starts always on same column
		int columnCounter = qMax(0, startCol);

		QModelIndex newRow = origModel->index(startRow, columnCounter, QModelIndex());
		emit addItem(newRow);

		// split the row into columns
		QStringList columnsToAdd = row.split("\t");
		foreach (QString column, columnsToAdd) {

			QModelIndex itemToSet = origModel->index(startRow, columnCounter, QModelIndex());
			// if the index is not valid then the data is written to last item
			if (!itemToSet.isValid()) {
				int lastRow = origModel->rowCount(QModelIndex()) - 1;

				itemToSet = origModel->index(lastRow, columnCounter, QModelIndex());
			}
			origModel->setData(itemToSet, column, Qt::EditRole);
			++columnCounter;
		}
		
		if (startRow >= 0) {
			// when row is done then increase the row counter
			++startRow;
		}
	}

	QApplication::restoreOverrideCursor();
}

void EditableTableView::onClearAction() {
	
	QModelIndexList indexes = selectedIndexes();

	// clear the contents of each cell
	foreach (QModelIndex index, indexes) {
		model()->setData(index, QVariant(), Qt::EditRole);
	}
}

void EditableTableView::onCSVExport( const QString& filePath ) {

	QString target(filePath);

	if (filePath.isEmpty()) {
		target = QFileDialog::getSaveFileName(this, 
			tr("Set name and location for csv-file"),
			defImportExportPath_, tr("csv-files (*.csv)"));
	}

	if (target.isEmpty()) {
		return;
	}

	QFile file(target);

	// if file can not be opened 
	if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
		QMessageBox::critical(this, tr("Error opening file"),
			tr("Could not open file %1 for writing").arg(target));
		return;
	}

	QApplication::setOverrideCursor(Qt::WaitCursor);

	int columnCount = model()->columnCount(QModelIndex());
	int rowCount = model()->rowCount(QModelIndex());

	// create a stream to write into
	QTextStream stream(&file);

	// write the headers
	for (int i = 0; i < columnCount; ++i) {
		stream << model()->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString().simplified();
		stream << ";";
	}
	stream << endl;

	// write each row
	for (int row = 0; row < rowCount; ++row) {
		
		// write each column
		for (int col = 0; col < columnCount; ++col) {

			QModelIndex index = model()->index(row, col, QModelIndex());
			stream << model()->data(index, Qt::DisplayRole).toString();
			stream << ";";
		}
		stream << endl;
	}
	file.close();

	QApplication::restoreOverrideCursor();
}

void EditableTableView::onCSVImport( const QString& filePath ) {
	
	QString target(filePath);
	
	if (filePath.isEmpty()) {
		target = QFileDialog::getOpenFileName(this, tr("Open file"),
			defImportExportPath_, tr("csv-files (*.csv)"));
	}

	if (target.isEmpty()) {
		return;
	}
	
	QFile file(target);

	// if file can not be opened 
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::critical(this, tr("Error opening file"),
			tr("Could not open file %1 for reading").arg(target));
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
	if (proxyModel) {
		origModel = qobject_cast<QAbstractTableModel*>(proxyModel->sourceModel());
	}
	// if view is connected directly to actual model
	else {
		origModel = qobject_cast<QAbstractTableModel*>(model());
	}
	Q_ASSERT(origModel);

	int columnCount = origModel->columnCount(QModelIndex());

	while (!stream.atEnd()) {
		QString line = stream.readLine();
		QStringList columns = line.split(";");

		// add a new empty row
		emit addItem(QModelIndex());

		// data is always added to the last row
		int rowCount = origModel->rowCount(QModelIndex());

		for (int col = 0; col < columnCount && col < columns.size(); ++col) {

			QModelIndex index = origModel->index(rowCount - 1, col, QModelIndex());

			origModel->setData(index, columns.at(col), Qt::EditRole);
		}
	}

	file.close();

	QApplication::restoreOverrideCursor();
}

void EditableTableView::setDefaultImportExportPath( const QString& path ) {
	defImportExportPath_ = path;
}

void EditableTableView::setAllowImportExport( bool allow ) {
	impExportable_ = allow;
}

void EditableTableView::setModel( QAbstractItemModel* model ) {
	
	// the base class implementation does most of the work
	QTableView::setModel(model);

	// contains info on the used font
	QFontMetrics fMetrics = fontMetrics();

	// set the widths for the columns
	int columnCount = model->columnCount(QModelIndex());
	for (int i = 0; i < columnCount - 1; ++i) {

		// the width required by the contents of the model
		int contentSize = sizeHintForColumn(i);

		// get the header for the section
		QString headerText = model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
		// if the header contains several lines
		QStringList headerLines = headerText.split("\n", QString::SkipEmptyParts);
		int headerSize = 0;

		// find the line that needs most space
		foreach (QString headerLine, headerLines) {
			headerSize = qMax(headerSize, fMetrics.width(headerLine));
		}
		headerSize += 40;

		// the width required by the headers of the model
		//int headerSize = horizontalHeader()->sectionSizeHint(i);

		// set the width for the column
		setColumnWidth(i, qMax(contentSize, headerSize));
	}
}
