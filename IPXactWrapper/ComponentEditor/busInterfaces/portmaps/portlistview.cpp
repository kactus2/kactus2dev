/* 
 *
 *  Created on: 10.5.2011
 *      Author: Antti Kamppi
 * 		filename: portlistview.cpp
 */

#include "portlistview.h"

#include "portmapsview.h"

#include <QMimeData>
#include <QDrag>
#include <QApplication>

#include <QDebug>

PortListView::PortListView(QWidget *parent): 
QListView(parent),
startPos_() {

	// the view accepts drops from drag & drop actions
	setAcceptDrops(true);

	setDropIndicatorShown(true);

	// the accepts dragging of it's items
	//setDragEnabled(true);

	setAlternatingRowColors(false);

	// user can not move items 
	//setMovement(QListView::Static);

	setSelectionBehavior(QAbstractItemView::SelectItems);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
}

PortListView::~PortListView() {
}

void PortListView::dragEnterEvent( QDragEnterEvent* event ) {

	// make sure the source is port list view or port maps view and
	// that data is correct type
	PortListView* source = qobject_cast<PortListView*>(event->source());
	PortMapsView* mapSource = qobject_cast<PortMapsView*>(event->source());
	
	// if source is port list view
	if (source && event->mimeData()->hasFormat("text/plain")) {

		// the item can be removed from the original model
		event->setDropAction(Qt::MoveAction);
		event->accept();
	}
	// if source is port maps view
	else if (mapSource && event->mimeData()->hasFormat("text/plain")) {
		event->acceptProposedAction();
	}

}

void PortListView::dragMoveEvent( QDragMoveEvent* e ) {

	// make sure the data is correct type
	PortListView* source = qobject_cast<PortListView*>(e->source());
	PortMapsView* mapSource = qobject_cast<PortMapsView*>(e->source());

	// if the source is another port list view then select the item under
	// the cursor
	if (source && source != this) 
		setCurrentIndex(indexAt(e->pos()));

	if (source && e->mimeData()->hasFormat("text/plain")) {

		// the item can be removed from the original model
		e->setDropAction(Qt::MoveAction);
		e->accept();
	}
	else if (mapSource && e->mimeData()->hasFormat("text/plain")) {
		e->acceptProposedAction();
	}
}

void PortListView::performDrag() {

	QModelIndexList indexes = selectedIndexes();
	
	QString mimeText;
	foreach (QModelIndex index, indexes) {

		// if the index is not valid
		if (!index.isValid())
			continue;

		// add the port name of the indexed item to mime data
		mimeText += index.model()->data(index, Qt::DisplayRole).toString();
		mimeText += ";";
	}

	// set the mime data
	QMimeData* mimeData = new QMimeData();
	mimeData->setText(mimeText);

	QDrag* drag = new QDrag(this);
	drag->setMimeData(mimeData);
	if (drag->exec(Qt::MoveAction) == Qt::MoveAction) {
		
		// inform the model that the dragged items could be removed
		emit removeItems(indexes);
	}

}

void PortListView::mouseMoveEvent( QMouseEvent* event ) {

	// if left mouse button is pressed
	if (event->buttons() == Qt::LeftButton) {
		
		// calculate how much mouse was moved
		int distance = (event->pos() - startPos_).manhattanLength();

		// if the move distance is enough to start the drag
		if (distance >= QApplication::startDragDistance())
			performDrag();
	}
}

void PortListView::mousePressEvent( QMouseEvent* event ) {

	if (event->button() == Qt::LeftButton)
		startPos_ = event->pos();
	QListView::mousePressEvent(event);
}

QStringList PortListView::getSelectedPorts(bool remove) {
	QStringList portList;
	QModelIndexList indexList = selectedIndexes();
	
	// sort indexes so they are in order
	qSort(indexList);

	foreach (QModelIndex index, indexList) {

		// dont append invalid indexes
		if (!index.isValid())
			continue;
		// append the names of the selected ports
		portList.append(index.model()->data(index, Qt::DisplayRole).toString());
	}

	// if items are to be removed
	if (remove)
		emit removeItems(indexList);

	return portList;
}
