/* 
 *  	Created on: 7.7.2011
 *      Author: Antti Kamppi
 * 		filename: logicallistview.cpp
 *		Project: Kactus 2
 */

#include "logicallistview.h"

#include <QMimeData>
#include <QDrag>

LogicalListView::LogicalListView(QWidget *parent):
PortListView(parent),
selectedIndex_(QModelIndex()) 
{
}

LogicalListView::~LogicalListView() 
{
}

void LogicalListView::dropEvent( QDropEvent* event )
{
	// make sure the source is not this view
	PortListView* source = qobject_cast<PortListView*>(event->source());

	// if source is neither of the supported
	if (!source) {
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
	QStringList logicals;

	// add the physical port that matches each selected index
	foreach (QModelIndex index, indexes) {
		if (index.isValid()) {
			// append each port with valid model index
			logicals.append(index.model()->data(index, Qt::DisplayRole).toString());
		}
	}

	// inform that connection should be made
	emit makeConnection(dropped, logicals);

	// accept the drop
	event->accept();
}

//-----------------------------------------------------------------------------
// Function: LogicalListView::onPortRestored()
//-----------------------------------------------------------------------------
void LogicalListView::onPortRestored(QString const& portName)
{    
    QModelIndex index = QModelIndex();
    bool found = false;
    for (int row = 0; row < model()->rowCount(); row++)
    {
        index = model()->index(row, 0);

        if (index.isValid() && model()->data(index).toString() == portName)
        {
            found = true;
            break;            
        }
    }

    if (found)
    {
        clearSelection();
        selectionModel()->select(index, QItemSelectionModel::Select);
        selectedIndex_ = index;
        emit clicked(index);
    }
}

//-----------------------------------------------------------------------------
// Function: LogicalListView::onPortRemoved()
//-----------------------------------------------------------------------------
void LogicalListView::onPortRemoved()
{
    QModelIndex nextIndex = model()->index(selectedIndex_.row(), selectedIndex_.column());
    if (!nextIndex.isValid())
    {
        nextIndex = model()->index(selectedIndex_.row() - 1, selectedIndex_.column());              
    }

    clearSelection();

    if (nextIndex.isValid())
    {    
        selectionModel()->select(nextIndex, QItemSelectionModel::Select);
        selectedIndex_ = nextIndex;
    } 
    else
    {
        selectedIndex_ = QModelIndex();      
    }
    
    emit clicked(selectedIndex_);
}

//-----------------------------------------------------------------------------
// Function: LogicalListView::mousePressEvent()
//-----------------------------------------------------------------------------
void LogicalListView::mousePressEvent(QMouseEvent* event)
{
    QListView::mousePressEvent(event);
    
    if (selectedIndexes().size() > 0)
    {
        selectedIndex_ = selectedIndexes().last();
    }       
}


