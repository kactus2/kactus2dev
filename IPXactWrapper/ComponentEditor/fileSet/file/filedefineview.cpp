/* 
 *
 *  Created on: 17.2.2011
 *      Author: Antti Kamppi
 * 		filename: filedefineview.cpp
 */

#include "filedefineview.h"

#include <QHeaderView>

FileDefineView::FileDefineView(QWidget *parent): QTableView(parent) {

	setCornerButtonEnabled(false);
	setShowGrid(true);
	setSortingEnabled(true);
	setWordWrap(true);
	setAlternatingRowColors(true);
	setEditTriggers(QAbstractItemView::DoubleClicked |
		QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);
	horizontalHeader()->setStretchLastSection(true);
}

FileDefineView::~FileDefineView() {
}
