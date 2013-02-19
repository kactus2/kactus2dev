/* 
 *	Created on: 19.2.2013
 *	Author:		Antti Kamppi
 * 	File name:	filesavedialogview.cpp
 * 	Project:	Kactus 2
*/

#include "filesavedialogview.h"

#include <QMenu>

#include <QDebug>

FileSaveDialogView::FileSaveDialogView(QWidget *parent):
QTableView(parent),
changeAction_(new QAction(tr("Change"), this)) {

	changeAction_->setToolTip(tr("Change the file name and path"));
	changeAction_->setStatusTip(tr("Change the file name and path"));
	connect(changeAction_, SIGNAL(triggered()),
		this, SLOT(onChange()), Qt::UniqueConnection);
}

FileSaveDialogView::~FileSaveDialogView() {
}

void FileSaveDialogView::mouseDoubleClickEvent( QMouseEvent* event ) {

	QTableView::mouseDoubleClickEvent(event);
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
	qDebug() << "Change path";
}
