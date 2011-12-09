/* 
 *  	Created on: 30.11.2011
 *      Author: Antti Kamppi
 * 		filename: listview.cpp
 *		Project: Kactus 2
 */

#include "listview.h"

#include <QMenu>
#include <QCursor>
#include <QSize>

ListView::ListView(QWidget *parent):
QListView(parent),
addAction_(tr("Add new"), this),
removeAction_(tr("Remove"), this) {

	setMovement(QListView::Free);

	setupActions();
}

ListView::~ListView() {
}

void ListView::mouseMoveEvent( QMouseEvent* e ) {
	
	// if left mouse button was pressed 
	if (e->buttons() & Qt::LeftButton) {

		QModelIndex startIndex = indexAt(pressedPoint_);
		QModelIndex thisIndex = indexAt(e->pos());

		// if the item was dragged to new location
		if (startIndex.isValid() && startIndex != thisIndex) {
			setCursor(QCursor(Qt::ClosedHandCursor));

			emit moveItem(startIndex, thisIndex);

			// update the pressed point so the dragging works also
			// when moving further to another index
			pressedPoint_ = e->pos();
		}
	}

	QListView::mouseMoveEvent(e);
}

void ListView::mousePressEvent( QMouseEvent* event ) {
	pressedPoint_ = event->pos();
	QListView::mousePressEvent(event);
}


void ListView::mouseReleaseEvent( QMouseEvent* event ) {
	setCursor(QCursor(Qt::ArrowCursor));
	QListView::mouseReleaseEvent(event);
}

void ListView::keyPressEvent( QKeyEvent* event ) {

	// if delete was pressed for an item
	QModelIndex index = currentIndex();
	if (index.isValid() && event->key() == Qt::Key_Delete) {
		emit removeItem(index);
	}

	QListView::keyPressEvent(event);
}

void ListView::mouseDoubleClickEvent( QMouseEvent* event ) {

	// if there is no item on the clicked position then a new item should be added
	QModelIndex index = indexAt(event->pos());
	if (!index.isValid()) {
		emit addItem(index);
		event->accept();
		return;
	}

	QListView::mouseDoubleClickEvent(event);
}

void ListView::contextMenuEvent( QContextMenuEvent* event ) {
	
	pressedPoint_ = event->pos();

	QMenu menu(this);
	menu.addAction(&addAction_);
	menu.addAction(&removeAction_);
	menu.exec(event->globalPos());

	event->accept();
}

void ListView::setupActions() {

	addAction_.setToolTip(tr("Add a new item to list"));
	addAction_.setStatusTip(tr("Add a new item to list"));
	connect(&addAction_, SIGNAL(triggered()),
		this, SLOT(onAddAction()), Qt::UniqueConnection);

	removeAction_.setToolTip(tr("Remove an item from the list"));
	removeAction_.setStatusTip(tr("Remove an item from the list"));
	connect(&removeAction_, SIGNAL(triggered()),
		this, SLOT(onRemoveAction()), Qt::UniqueConnection);
}

void ListView::onAddAction() {
	QModelIndex index = indexAt(pressedPoint_);
	emit addItem(index);
}

void ListView::onRemoveAction() {
	QModelIndex index = currentIndex();
	emit removeItem(index);
}
