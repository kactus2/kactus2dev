/* 
 *
 *  Created on: 27.4.2011
 *      Author: Antti Kamppi
 * 		filename: portmapsview.cpp
 */

#include "portmapsview.h"
#include "portmapsdelegate.h"

#include <QHeaderView>
#include <QCursor>
#include <QItemSelectionModel>
#include <QApplication>
#include <QMenu>
#include <QMimeData>
#include <QDrag>

PortMapsView::PortMapsView(QSortFilterProxyModel* proxy, QWidget *parent): 
QTableView(parent), 
proxy_(proxy),
removeAction_(tr("Remove mapping"), this),
restoreAction_(tr("Restore port to list"), this),
menuTarget_(),
startPos_() {

	// connect the action triggers to handlers
	connect(&removeAction_, SIGNAL(triggered()),
		this, SLOT(onRemoveItem()), Qt::UniqueConnection);
	connect(&restoreAction_, SIGNAL(triggered()),
		this, SLOT(onRestoreItem()), Qt::UniqueConnection);

	// items can be dragged but not dropped
	setDragDropMode(QAbstractItemView::DragOnly);

	setSortingEnabled(true);

	horizontalHeader()->setStretchLastSection(true);
	horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	horizontalHeader()->setMinimumSectionSize(50);

	verticalHeader()->setDefaultSectionSize(fontMetrics().height() + 8);

	setItemDelegate(new PortMapsDelegate(this));

	setSelectionMode(QAbstractItemView::SingleSelection);

	setAlternatingRowColors(true);
	setSelectionBehavior(QAbstractItemView::SelectItems);
	verticalHeader()->hide();
	setEditTriggers(QAbstractItemView::DoubleClicked | 
		QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);
	setWordWrap(true);

	// the view accepts drop events
	setAcceptDrops(true);
}

PortMapsView::~PortMapsView() {
}

void PortMapsView::keyPressEvent( QKeyEvent* event ) {

	// if delete was pressed
	if (event->key() == Qt::Key_Delete) {
		emit removeItems(proxy_->mapToSource(currentIndex()));
	}

	// call the default implementation
	QTableView::keyPressEvent(event);
}

void PortMapsView::contextMenuEvent( QContextMenuEvent* event ) {

	QMenu contextMenu(this);
	contextMenu.addAction(&removeAction_);

	menuTarget_ = indexAt(event->pos());
	
	// if the model index is invalid
	if (!menuTarget_.isValid())
		return;

	// if a port is selected
	if (menuTarget_.column() == 2 || menuTarget_.column() == 3) {
		contextMenu.addAction(&restoreAction_);
	}
	
	contextMenu.exec(event->globalPos());
	event->accept();
}

void PortMapsView::onRemoveItem() {

	if (menuTarget_.isValid())
		emit removeItems(menuTarget_);
}

void PortMapsView::onRestoreItem() {

	if (!menuTarget_.isValid())
		return;

	if (menuTarget_.column() == 2 || menuTarget_.column() == 3)
		emit restoreItem(menuTarget_);
}

void PortMapsView::dragMoveEvent( QDragMoveEvent* e ) {
	// make sure the source is not this model and that data is correct type
	PortMapsView* source = qobject_cast<PortMapsView*>(e->source());

	if (source && source != this) {

		// the item can be removed from the original model
		e->setDropAction(Qt::CopyAction);
		e->accept();
	}
}

void PortMapsView::mouseMoveEvent( QMouseEvent* event ) {
	// if left mouse button is pressed
	if (event->buttons() == Qt::LeftButton) {

		// calculate how much mouse was moved
		int distance = (event->pos() - startPos_).manhattanLength();

		// if the move distance is enough to start the drag
		if (distance >= QApplication::startDragDistance())
			performDrag();
	}
}

void PortMapsView::mousePressEvent( QMouseEvent* event ) {
	if (event->button() == Qt::LeftButton)
		startPos_ = event->pos();
	QTableView::mousePressEvent(event);
}

void PortMapsView::performDrag() {
	QModelIndex index = currentIndex();
	if (!index.isValid())
		return;

	// set the mime data
	QMimeData* mimeData = new QMimeData();
	mimeData->setText(index.model()->data(index, Qt::DisplayRole).toString());

	QDrag* drag = new QDrag(this);
	drag->setMimeData(mimeData);
	if (drag->exec(Qt::CopyAction) == Qt::CopyAction) {
		emit restoreItem(index);
	}
}
