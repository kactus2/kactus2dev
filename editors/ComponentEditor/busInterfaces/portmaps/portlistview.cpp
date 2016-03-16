//-----------------------------------------------------------------------------
// File: portlistview.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 10.05.2011
//
// Description:
// View to display a list of ports.
//-----------------------------------------------------------------------------

#include "portlistview.h"

#include <QApplication>
#include <QDrag>
#include <QMimeData>

//-----------------------------------------------------------------------------
// Function: PortListView::PortListView()
//-----------------------------------------------------------------------------
PortListView::PortListView(QWidget *parent): QListView(parent),
startPos_()
{
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

//-----------------------------------------------------------------------------
// Function: PortListView::~PortListView()
//-----------------------------------------------------------------------------
PortListView::~PortListView()
{
}

//-----------------------------------------------------------------------------
// Function: PortListView::dragEnterEvent()
//-----------------------------------------------------------------------------
void PortListView::dragEnterEvent(QDragEnterEvent* event)
{
	// make sure the source is port list view or port maps view and
	// that data is correct type
	PortListView* source = qobject_cast<PortListView*>(event->source());
	
	// if source is port list view
	if (source && event->mimeData()->hasFormat("text/plain"))
    {
		// the item can be removed from the original model
		event->setDropAction(Qt::MoveAction);
		event->accept();
	}
}

//-----------------------------------------------------------------------------
// Function: PortListView::dragMoveEvent()
//-----------------------------------------------------------------------------
void PortListView::dragMoveEvent(QDragMoveEvent* e)
{
	// make sure the data is correct type.
	PortListView* source = qobject_cast<PortListView*>(e->source());

	// if the source is another port list view then select the item under the cursor.
	if (source && source != this) 
    {
		setCurrentIndex(indexAt(e->pos()));
    }
	if (source && e->mimeData()->hasFormat("text/plain"))
    {
		// the item can be removed from the original model
		e->setDropAction(Qt::MoveAction);
		e->accept();
	}
}

//-----------------------------------------------------------------------------
// Function: PortListView::performDrag()
//-----------------------------------------------------------------------------
void PortListView::performDrag()
{
	QModelIndexList indexes = selectedIndexes();
	
	QString mimeText;
	foreach (QModelIndex const& index, indexes)
    {
		if (index.isValid())
        {
            mimeText += index.data(Qt::DisplayRole).toString() + ";";
        }
	}

	// set the mime data
	QMimeData* mimeData = new QMimeData();
	mimeData->setText(mimeText);

	QDrag* drag = new QDrag(this);
	drag->setMimeData(mimeData);

	if (drag->exec(Qt::MoveAction) == Qt::MoveAction)
    {
		// inform the model that the dragged items could be removed
		emit removeItems(indexes);
	}
}

//-----------------------------------------------------------------------------
// Function: PortListView::mouseMoveEvent()
//-----------------------------------------------------------------------------
void PortListView::mouseMoveEvent(QMouseEvent* event)
{
	// if left mouse button is pressed
	if (event->buttons() == Qt::LeftButton)
    {
		// calculate how much mouse was moved
		int distance = (event->pos() - startPos_).manhattanLength();

		// if the move distance is enough to start the drag
		if (distance >= QApplication::startDragDistance())
        {
			performDrag();
        }
	}
}

//-----------------------------------------------------------------------------
// Function: PortListView::mousePressEvent()
//-----------------------------------------------------------------------------
void PortListView::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
    {
		startPos_ = event->pos();
    }
	QListView::mousePressEvent(event);
}

//-----------------------------------------------------------------------------
// Function: PortListView::getSelectedPorts()
//-----------------------------------------------------------------------------
QStringList PortListView::getSelectedPorts()
{
	QStringList portList;
	QModelIndexList indexList = selectedIndexes();
	
	// sort indexes so they are in order
	qSort(indexList);

	foreach (QModelIndex const& index, indexList)
    {
		// dont append invalid indexes
		if (index.isValid())
        {
		    portList.append(index.data(Qt::DisplayRole).toString());
        }
	}

	return portList;
}
