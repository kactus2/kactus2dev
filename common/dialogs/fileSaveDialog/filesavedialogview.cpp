/* 
 *	Created on: 19.2.2013
 *	Author:		Antti Kamppi
 * 	File name:	filesavedialogview.cpp
 * 	Project:	Kactus 2
*/

#include "filesavedialogview.h"

#include <QMenu>
#include <QHeaderView>
#include <QFontMetrics>
#include <QFileDialog>

#include <QDebug>

FileSaveDialogView::FileSaveDialogView(QWidget *parent):
QTableView(parent),
changeAction_(new QAction(tr("Change"), this)) {

	changeAction_->setToolTip(tr("Change the file name and path"));
	changeAction_->setStatusTip(tr("Change the file name and path"));
	connect(changeAction_, SIGNAL(triggered()),
		this, SLOT(onChange()), Qt::UniqueConnection);

	// cells are resized to match contents 
	horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

	//last column is stretched take the available space in the widget
	horizontalHeader()->setStretchLastSection(true);

	// vertical headers are not visible
	verticalHeader()->hide();

	// set the height of a row to be smaller than default
	verticalHeader()->setDefaultSectionSize(fontMetrics().height() + 8);

	// words are wrapped in the cells to minimize space usage
	setWordWrap(true);

	// easies to see the different rows from one another
	setAlternatingRowColors(true);
}

FileSaveDialogView::~FileSaveDialogView() {
}

void FileSaveDialogView::mouseDoubleClickEvent( QMouseEvent* event ) {
	pressedPoint_ = event->pos();
	QTableView::mouseDoubleClickEvent(event);

	// if the item is valid but not editable then double click works
	QModelIndex selectedIndex = indexAt(pressedPoint_);
	if (selectedIndex.isValid() && (selectedIndex.model()->flags(selectedIndex) & Qt::ItemIsEditable) == 0) {
		onChange();
	}

}

void FileSaveDialogView::contextMenuEvent( QContextMenuEvent* event ) {
	pressedPoint_ = event->pos();

	QModelIndex index = indexAt(pressedPoint_);

	// only valid items can be changed
	if (!index.isValid()) {
		return;
	}

	// display the menu
	QMenu menu(this);
	menu.addAction(changeAction_);
	menu.exec(event->globalPos());

	event->accept();
}

void FileSaveDialogView::onChange() {
	
	// find out which item was selected
	QModelIndex selectedIndex = indexAt(pressedPoint_);
	if (!selectedIndex.isValid()) {
		return;
	}

	// the previous path to the file
	QString previousPath = model()->data(selectedIndex, Qt::UserRole).toString();

	// ask user to select the file to save
	QString targetName = QFileDialog::getSaveFileName(this, tr("Select file to save"), previousPath, tr("header files (*.h)"));
	
	// is user clicked cancel
	if (targetName.isEmpty()) {
		return;
	}

	// save the new path to the model
	model()->setData(selectedIndex, targetName, Qt::UserRole);
}

void FileSaveDialogView::setModel( QAbstractItemModel* model ) {
	// the base class implementation does most of the work
	QTableView::setModel(model);

	// contains info on the used font
	QFontMetrics fMetrics = fontMetrics();

	// set the widths for the columns
	int columnCount = model->columnCount(QModelIndex());
	for (int i = 0; i < columnCount; ++i) {

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
		headerSize += 45;

		// set the width for the column
		setColumnWidth(i, qMax(contentSize, headerSize));
	}
}
