/* 
 *	Created on:	4.4.2013
 *	Author:		Antti Kamppi
 *	File name:	filetypesview.cpp
 *	Project:		Kactus 2
*/

#include "filetypesview.h"
#include <IPXACTmodels/generaldeclarations.h>

#include <QMenu>

FileTypesView::FileTypesView(QWidget *parent):
EditableTableView(parent) {

	// user can only select one item at a time
	setSelectionMode(QAbstractItemView::SingleSelection);
}

FileTypesView::~FileTypesView() {
}

void FileTypesView::contextMenuEvent( QContextMenuEvent* event ) {
	pressedPoint_ = event->pos();

	QModelIndex index = indexAt(pressedPoint_);

	QMenu menu(this);
	menu.addAction(&addAction_);

	// if at least one valid item is selected
	if (index.isValid()) {

		// find the index for the file type column, this is in case the user clicked some
		// other column than file type
		QModelIndex fileTypeIndex = index.model()->index(index.row(), 0, QModelIndex());
		QString selectedFileType = fileTypeIndex.model()->data(fileTypeIndex, Qt::DisplayRole).toString();

		// if the file type is user defined and not one of the file types specified
		// by IP-XACT standard
		if (!General::isIpXactFileType(selectedFileType)) {
			menu.addAction(&removeAction_); 
			
			menu.addAction(&clearAction_);
		}
		// if the file type was defined by IP-XACT standard
		else {

			// if user selected some other column than the file type column, it can be cleared
			if (index.column() != 0) {
				menu.addAction(&clearAction_);
			}
		}

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

void FileTypesView::keyPressEvent( QKeyEvent* event ) {
	// call the base class implementation
	QTableView::keyPressEvent(event);

	QModelIndex selected = currentIndex();

	if (event->matches(QKeySequence::Copy)) {
		onCopyAction();
	}
	if (event->matches(QKeySequence::Paste)) {
		onPasteAction();
	}
	if (event->matches(QKeySequence::Delete)) {

		// if user had selected the file type column
		if (selected.column() == 0) {

			// if the file type is one of the standard ones, it can't be cleared
			QString selectedFileType = selected.model()->data(selected, Qt::DisplayRole).toString();
			if (General::isIpXactFileType(selectedFileType)) {
				return;
			}
		}

		onClearAction();
	}
	if (event->matches(QKeySequence::InsertLineSeparator)) {
		onAddAction();
	}
}
