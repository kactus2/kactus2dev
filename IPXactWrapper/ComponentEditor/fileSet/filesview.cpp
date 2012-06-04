/* 
 *  	Created on: 4.6.2012
 *      Author: Antti Kamppi
 * 		filename: filesview.cpp
 *		Project: Kactus 2
 */

#include "filesview.h"

#include <QSortFilterProxyModel>

FilesView::FilesView(QWidget *parent):
EditableTableView(parent) {
}

FilesView::~FilesView() {
}

void FilesView::mouseDoubleClickEvent( QMouseEvent* event ) {
	// if there is no item on the clicked position then a new item should be added
	QModelIndex index = indexAt(event->pos());
	if (!index.isValid()){
		emit addItem(index, QString("joku.test"));
		event->accept();
		return;
	}

	QTableView::mouseDoubleClickEvent(event);
}

void FilesView::onAddAction() {
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
		emit addItem(posToAdd, QString("add.test"));
	}
}
