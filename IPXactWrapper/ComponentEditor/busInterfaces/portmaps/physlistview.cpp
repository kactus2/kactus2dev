/* 
 *  	Created on: 7.7.2011
 *      Author: Antti Kamppi
 * 		filename: physlistview.cpp
 *		Project: Kactus 2
 */

#include "physlistview.h"

#include "portmapsview.h"

#include <QMimeData>
#include <QDrag>

PhysListView::PhysListView(QWidget *parent):
PortListView(parent) {

}

PhysListView::~PhysListView() {
}

void PhysListView::dropEvent( QDropEvent* event ) {
	// make sure the source is not this view
	PortListView* source = qobject_cast<PortListView*>(event->source());
	PortMapsView* mapSource = qobject_cast<PortMapsView*>(event->source());

	// if source is neither of the supported
	if (!source && !mapSource) {
		return;
	}

	if (mapSource && event->proposedAction() == Qt::CopyAction) {
		event->acceptProposedAction();
		return;
	}

	QModelIndex index = indexAt(event->pos());

	// tell source that nothing is to be done to item
	event->setDropAction(Qt::IgnoreAction);

	// if no port name has been specified or drop index is invalid
	QString mimeText = event->mimeData()->text();
	if (mimeText.isEmpty()) {
		event->accept();
		return;
	}

	// get the ports that were selected
	QStringList dropped = mimeText.split(QString(";"), QString::SkipEmptyParts);

	// if the item to drop is from this port list view
	if (source == this) {
		emit moveItems(dropped, index);
		event->accept();
		return;
	}

	// get the selected indexes in this view
	QModelIndexList indexes = selectedIndexes();
	QStringList physicals;

	// add the physical port that matches each selected index
	foreach (QModelIndex index, indexes) {
		if (index.isValid()) {
			// append each port with valid model index
			physicals.append(index.model()->data(index, Qt::DisplayRole).toString());
		}
	}

	// inform that connection should be made
	emit makeConnection(physicals, dropped);

	// accept the drop
	event->accept();
}
