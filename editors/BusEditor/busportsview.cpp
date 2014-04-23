/*
 *  Created on: 21.6.2011
 *      Author: Antti Kamppi
 * 		filename: busportsview.cpp
 */

#include "busportsview.h"



#include <QHeaderView>
#include <QMenu>

BusPortsView::BusPortsView(QWidget *parent):
EditableTableView(parent),
addOptionsAction_(tr("Add signal options"), this)
//removeAction_(tr("Remove"), this),
//copyAction_(tr("Copy"), this) 
{

	/*horizontalHeader()->setStretchLastSection(true);
	horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	horizontalHeader()->setMinimumSectionSize(70);
*/

/*
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setSelectionBehavior(QAbstractItemView::SelectRows);

	setEditTriggers(QAbstractItemView::DoubleClicked | 
		QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);
	setWordWrap(true);
	
	setSortingEnabled(false);
*/
	setupActions();
}

BusPortsView::~BusPortsView() {
}

QModelIndexList BusPortsView::selected() const {
    return selectedIndexes();
}
/*
void BusPortsView::keyPressEvent( QKeyEvent* event ) {
	// if delete was pressed
	if (event->key() == Qt::Key_Delete) {
		emit removeItems(selectedIndexes());
	}

	// call the default implementation
	QTableView::keyPressEvent(event);
}
*/
void BusPortsView::contextMenuEvent( QContextMenuEvent* event ) {
    pressedPoint_ = event->pos();

    QModelIndex index = indexAt(pressedPoint_);

    QMenu menu(this);
    if (index.isValid()) {
        menu.addAction(&addOptionsAction_);
        menu.addSeparator();
    }
    menu.addAction(&addAction_);
    
    // if at least one valid item is selected
    if (index.isValid()) {        
        menu.addAction(&removeAction_);
        menu.addAction(&clearAction_);
        menu.addAction(&copyAction_);    
    }
    menu.addAction(&pasteAction_);

    if (impExportable_) {
        menu.addSeparator();
        menu.addAction(&importAction_);
        menu.addAction(&exportAction_);
    }

    menu.exec(event->globalPos());

    event->accept();
}

void BusPortsView::setupActions() {

    connect(&addOptionsAction_, SIGNAL(triggered()), this, 
        SIGNAL(addSignalOptions()), Qt::UniqueConnection);
    
}
