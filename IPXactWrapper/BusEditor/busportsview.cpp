/*
 *  Created on: 21.6.2011
 *      Author: Antti Kamppi
 * 		filename: busportsview.cpp
 */

#include "busportsview.h"

#include "busportsdelegate.h"

#include <QHeaderView>
#include <QMenu>

BusPortsView::BusPortsView(QWidget *parent):
QTableView(parent),
removeAction_(tr("Remove"), this),
copyAction_(tr("Copy"), this) {

	horizontalHeader()->setStretchLastSection(true);
	horizontalHeader()->setResizeMode(QHeaderView::Interactive);
	horizontalHeader()->setMinimumSectionSize(70);

	setItemDelegate(new BusPortsDelegate(this));

	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setSelectionBehavior(QAbstractItemView::SelectRows);

	setEditTriggers(QAbstractItemView::DoubleClicked | 
		QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);
	setWordWrap(true);
	
	setSortingEnabled(false);

	setupActions();
}

BusPortsView::~BusPortsView() {
}

void BusPortsView::keyPressEvent( QKeyEvent* event ) {
	// if delete was pressed
	if (event->key() == Qt::Key_Delete) {
		emit removeItems(selectedIndexes());
	}

	// call the default implementation
	QTableView::keyPressEvent(event);
}

void BusPortsView::contextMenuEvent( QContextMenuEvent* e ) {
	QMenu contextMenu(this);
	contextMenu.addAction(&copyAction_);
	contextMenu.addAction(&removeAction_);

	QModelIndexList indexes = selectedIndexes();

	// if no indexes are selected
	if (indexes.isEmpty())
		return;

	contextMenu.exec(e->globalPos());
	e->accept();
}

void BusPortsView::setupActions() {

	connect(&removeAction_, SIGNAL(triggered()),
		this, SLOT(onRemove()), Qt::UniqueConnection);

	connect(&copyAction_, SIGNAL(triggered()),
		this, SLOT(onCopy()), Qt::UniqueConnection);
}

void BusPortsView::onRemove() {
	emit removeItems(selectedIndexes());
}

void BusPortsView::onCopy() {
	emit copyItems(selectedIndexes());
}

QModelIndexList BusPortsView::selected() const {
	return selectedIndexes();
}
