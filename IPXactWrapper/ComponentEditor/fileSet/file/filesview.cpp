/* 
 *  	Created on: 4.6.2012
 *      Author: Antti Kamppi
 * 		filename: filesview.cpp
 *		Project: Kactus 2
 */

#include "filesview.h"
#include <LibraryManager/libraryinterface.h>

#include <QSortFilterProxyModel>
#include <QStringList>
#include <QFileDialog>

FilesView::FilesView(LibraryInterface* handler,
					 QSharedPointer<Component> component,
					 QWidget *parent):
EditableTableView(parent),
handler_(handler),
component_(component) {
}

FilesView::~FilesView() {
}

void FilesView::mouseDoubleClickEvent( QMouseEvent* event ) {
	// if there is no item on the clicked position then a new item should be added
	QModelIndex index = indexAt(event->pos());
	if (!index.isValid()) {

		// ask user to select the files to add
		QStringList files = QFileDialog::getOpenFileNames(this, tr("Select files to add."),
			handler_->getPath(*component_->getVlnv()));
		
		// if user clicked cancel
		if (files.isEmpty()) {
			event->accept();
			return;
		}

		// add each file
		foreach (QString file, files) {
			emit addItem(index, file);
		}

		event->accept();
		return;
	}

	QTableView::mouseDoubleClickEvent(event);
}

void FilesView::onAddAction() {
	QModelIndexList indexes = selectedIndexes();
	QModelIndex posToAdd;

	QSortFilterProxyModel* sortProxy = dynamic_cast<QSortFilterProxyModel*>(model());

	// if there were indexes selected
	if (!indexes.isEmpty()) {
		qSort(indexes);

		// the position to add the files to
		posToAdd = indexes.first();
	}

	// if proxy is used then map the index to source indexes
	if (sortProxy) {
		posToAdd = sortProxy->mapToSource(posToAdd);
	}

	// ask user to select the files to add
	QStringList files = QFileDialog::getOpenFileNames(this, tr("Select files to add."),
		handler_->getPath(*component_->getVlnv()));

	// if user clicked cancel
	if (files.isEmpty()) {
		return;
	}

	// add each file
	foreach (QString file, files) {
		emit addItem(posToAdd, file);
	}
}
