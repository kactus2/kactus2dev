//-----------------------------------------------------------------------------
// File: physlistview.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 7.7.2011
//
// Description:
// List view for physical ports.
//-----------------------------------------------------------------------------

#include "physlistview.h"

#include "portmapsview.h"

#include <QApplication>
#include <QMimeData>
#include <QDrag>

//-----------------------------------------------------------------------------
// Function: physlistview::PhysListView()
//-----------------------------------------------------------------------------
PhysListView::PhysListView(QWidget *parent):
PortListView(parent)
{
}

//-----------------------------------------------------------------------------
// Function: physlistview::~PhysListView()
//-----------------------------------------------------------------------------
PhysListView::~PhysListView()
{
}

//-----------------------------------------------------------------------------
// Function: PhysListView::onFilterNameChanged()
//-----------------------------------------------------------------------------
void PhysListView::onFilterNameChanged(QString const& portName)
{    
    QModelIndex matchingIndex = model()->index(0, 0);
    
    if (!matchingIndex.isValid())
    {
        return;
    }

    int match =  QString::compare(model()->data(matchingIndex).toString(), portName, Qt::CaseInsensitive);

    // Search for better matching port names.
    for (int row = 1; row < model()->rowCount(); row++)
    {
        QModelIndex index = model()->index(row, 0);
        if (index.isValid())
        {
            int diff = QString::compare(model()->data(index).toString(), portName, Qt::CaseInsensitive);
            if (match > diff)
            {
                matchingIndex = index;
                match = diff;
            }
        }
    }

    clearSelection();

    // Select the best matching port.
    if (matchingIndex.isValid())
    {        
        selectionModel()->select(matchingIndex, QItemSelectionModel::Select);        
    }    
}

//-----------------------------------------------------------------------------
// Function: physlistview::dropEvent()
//-----------------------------------------------------------------------------
void PhysListView::dropEvent( QDropEvent* event ) 
{
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

//-----------------------------------------------------------------------------
// Function: physlistview::mouseMoveEvent()
//-----------------------------------------------------------------------------
void PhysListView::mouseMoveEvent( QMouseEvent* event ) {

    // if either mouse button is pressed
    if (event->buttons() == Qt::LeftButton || event->buttons() == Qt::RightButton) {

        // calculate how much mouse was moved
        int distance = (event->pos() - startPos_).manhattanLength();

        // if the move distance is enough to start the drag
        if (distance >= QApplication::startDragDistance())
            performDrag();
    }
}

//-----------------------------------------------------------------------------
// Function: physlistview::mousePressEvent()
//-----------------------------------------------------------------------------
void PhysListView::mousePressEvent( QMouseEvent* event ) {

    if (event->button() == Qt::LeftButton || event->buttons() == Qt::RightButton)
        startPos_ = event->pos();
    QListView::mousePressEvent(event);
}