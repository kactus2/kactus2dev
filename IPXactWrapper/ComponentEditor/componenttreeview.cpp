/* 
 *
 *  Created on: 21.1.2011
 *      Author: Antti Kamppi
 */

#include "componenttreeview.h"

#include <QEvent>
#include <QCursor>
#include <QApplication>

#include <QDebug>

ComponentTreeView::ComponentTreeView(QWidget *parent):
QTreeView(parent),
locked_(true) {
	
	// header is not shown 
	setHeaderHidden(true);

	setIndentation(20);

	// alternating row colors to make reading easier
	//setAlternatingRowColors(true);

	// only one item can be selected at a time
	setSelectionMode(QAbstractItemView::SingleSelection);

	setSelectionBehavior(QAbstractItemView::SelectItems);
}

ComponentTreeView::~ComponentTreeView() {

}

void ComponentTreeView::currentChanged(const QModelIndex & current, 
									   const QModelIndex & previous ) 
{
	// call the base class implementation
	QTreeView::currentChanged(current, previous);

	// inform that the current index has changed
	emit activated(current);
}

void ComponentTreeView::keyPressEvent( QKeyEvent* event ) {
	QTreeView::keyPressEvent(event);
// 	QModelIndex index = currentIndex();
// 	emit activated(index);
}

void ComponentTreeView::mousePressEvent( QMouseEvent* event ) {
	pressedPoint_ = event->pos();
	QTreeView::mousePressEvent(event);
// 	QModelIndex index = indexAt(pressedPoint_);
// 	setCurrentIndex(index);
}

void ComponentTreeView::mouseReleaseEvent( QMouseEvent* event ) {

	// calculate the distance of the drag
	int distance = (event->pos() - pressedPoint_).manhattanLength();

	// make sure the drag distance is large enough to start the drag and widget is not locked
	if (distance >= QApplication::startDragDistance() && !locked_) {

		QModelIndex pressedIndex = indexAt(pressedPoint_);

		QModelIndex releaseIndex = indexAt(event->pos());
		if (!releaseIndex.isValid() || !pressedIndex.isValid()) {
			
			// do the norman release event functionality
			QTreeView::mouseReleaseEvent(event);

			// restore the normal cursor
			setCursor(QCursor(Qt::ArrowCursor));
			return;
		}

		QModelIndex parent1 = pressedIndex.model()->parent(pressedIndex);
		QModelIndex parent2 = releaseIndex.model()->parent(releaseIndex);

		if (parent1.isValid() && parent1 == parent2) {
			emit moveItem(pressedIndex, releaseIndex);

			QModelIndex newIndex = indexAt(event->pos());
			setCurrentIndex(newIndex);
			emit activated(newIndex);
		}
	}

	// restore the normal cursor
	setCursor(QCursor(Qt::ArrowCursor));

	QTreeView::mouseReleaseEvent(event);
}

void ComponentTreeView::mouseMoveEvent( QMouseEvent* event ) {

	// if not dragging an item or item is locked
	if (event->buttons() == Qt::NoButton || locked_)
		QTreeView::mouseMoveEvent(event);
	
	// if dragging item
	else {
		QModelIndex startIndex = indexAt(pressedPoint_);
		QModelIndex thisIndex = indexAt(event->pos());

		// if either index is invalid then dragging is not possible
		if (!startIndex.isValid() || !thisIndex.isValid()) {
			setCursor(QCursor(Qt::ForbiddenCursor));
			return;
		}

		QModelIndex parent1 = startIndex.model()->parent(startIndex);
		QModelIndex parent2 = thisIndex.model()->parent(thisIndex);

		// if both have same parent
		if (parent1 == parent2) {
			setCursor(QCursor(Qt::ClosedHandCursor));
		}
		else {
			setCursor(QCursor(Qt::ForbiddenCursor));
		}
	}
}

void ComponentTreeView::setLocked( bool locked ) {
	locked_ = locked;
}

void ComponentTreeView::selectItem( const QModelIndex& index ) {
	selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
}


