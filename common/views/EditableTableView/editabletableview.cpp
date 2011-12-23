/* 
 *  	Created on: 10.12.2011
 *      Author: Antti Kamppi
 * 		filename: editabletableview.cpp
 *		Project: Kactus 2
 */

#include "editabletableview.h"

#include <QHeaderView>
#include <QMenu>

EditableTableView::EditableTableView(QWidget *parent):
QTableView(parent),
addAction_(tr("Add new"), this),
removeAction_(tr("Remove"), this),
itemsDraggable_(true) {

	// cells are resized to match contents 
	horizontalHeader()->setResizeMode(QHeaderView::Interactive);

	//last column is stretched take the available space in the widget
	horizontalHeader()->setStretchLastSection(true);

	// vertical headers are not visible
	verticalHeader()->hide();

	// easies to see the different rows from one another
	setAlternatingRowColors(true);

	// words are wrapped in the cells to minimize space usage
	setWordWrap(true);

	// user can only select one item at a time
	setSelectionMode(QAbstractItemView::SingleSelection);

	setEditTriggers(QAbstractItemView::DoubleClicked |
		QAbstractItemView::SelectedClicked |
		QAbstractItemView::EditKeyPressed |
		QAbstractItemView::AnyKeyPressed);
	
	setupActions();
}

EditableTableView::~EditableTableView() {
}

void EditableTableView::mouseMoveEvent( QMouseEvent* e ) {
	if (itemsDraggable_) {

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
	}

	QTableView::mouseMoveEvent(e);
}

void EditableTableView::keyPressEvent( QKeyEvent* event ) {
	// if delete was pressed for an item
	QModelIndex index = currentIndex();
	if (index.isValid() && event->key() == Qt::Key_Delete) {
		emit removeItem(index);
	}

	QTableView::keyPressEvent(event);
}

void EditableTableView::mouseDoubleClickEvent( QMouseEvent* event ) {
	// if there is no item on the clicked position then a new item should be added
	QModelIndex index = indexAt(event->pos());
	if (!index.isValid()) {
		emit addItem(index);
		event->accept();
		return;
	}

	QTableView::mouseDoubleClickEvent(event);
}

void EditableTableView::mousePressEvent( QMouseEvent* event ) {
	pressedPoint_ = event->pos();
	QTableView::mousePressEvent(event);
}

void EditableTableView::mouseReleaseEvent( QMouseEvent* event ) {
	setCursor(QCursor(Qt::ArrowCursor));
	QTableView::mouseReleaseEvent(event);
}

void EditableTableView::contextMenuEvent( QContextMenuEvent* event ) {
	pressedPoint_ = event->pos();

	QMenu menu(this);
	menu.addAction(&addAction_);
	menu.addAction(&removeAction_);
	menu.exec(event->globalPos());

	event->accept();
}

void EditableTableView::onAddAction() {
	QModelIndex index = indexAt(pressedPoint_);
	emit addItem(index);
}

void EditableTableView::onRemoveAction() {
	QModelIndex index = currentIndex();
	emit removeItem(index);
}

void EditableTableView::setupActions() {
	addAction_.setToolTip(tr("Add a new item to table"));
	addAction_.setStatusTip(tr("Add a new item to table"));
	connect(&addAction_, SIGNAL(triggered()),
		this, SLOT(onAddAction()), Qt::UniqueConnection);

	removeAction_.setToolTip(tr("Remove an item from the table"));
	removeAction_.setStatusTip(tr("Remove an item from the table"));
	connect(&removeAction_, SIGNAL(triggered()),
		this, SLOT(onRemoveAction()), Qt::UniqueConnection);
}

void EditableTableView::setItemsDraggable( bool draggable ) {
	itemsDraggable_ = draggable;
}
