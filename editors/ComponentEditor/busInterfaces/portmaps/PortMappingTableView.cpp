//-----------------------------------------------------------------------------
// File: PortMappingTableView.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 19.05.2016
//
// Description:
// View to display a table of ports for port mapping.
//-----------------------------------------------------------------------------

#include "PortMappingTableView.h"
#include "PortMappingColumns.h"

#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QModelIndex>
#include <QHeaderView>

//-----------------------------------------------------------------------------
// Function: PortMappingTableView::PortMappingTableView()
//-----------------------------------------------------------------------------
PortMappingTableView::PortMappingTableView(QWidget *parent):
QTableView(parent)
{
	setAlternatingRowColors(false);

    setSelectionBehavior(QAbstractItemView::SelectRows);

    setSelectionMode(QAbstractItemView::ExtendedSelection);

    verticalHeader()->hide();

    horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    horizontalHeader()->setStretchLastSection(true);

    verticalHeader()->setDefaultSectionSize(fontMetrics().height() + 8);

    setWordWrap(true);

    setSortingEnabled(true);

    setEditTriggers(QAbstractItemView::NoEditTriggers);

}

//-----------------------------------------------------------------------------
// Function: PortMappingTableView::~PortMappingTableView()
//-----------------------------------------------------------------------------
PortMappingTableView::~PortMappingTableView()
{

}

//-----------------------------------------------------------------------------
// Function: PortMappingTableView::dragEnterEvent()
//-----------------------------------------------------------------------------
void PortMappingTableView::dragEnterEvent(QDragEnterEvent* event)
{
	PortMappingTableView* source = qobject_cast<PortMappingTableView*>(event->source());
	
	// if source is port list view
	if (source && event->mimeData()->hasFormat("text/plain"))
    {
		// the item can be removed from the original model
		event->setDropAction(Qt::MoveAction);
		event->accept();
	}
}

//-----------------------------------------------------------------------------
// Function: PortMappingTableView::dragMoveEvent()
//-----------------------------------------------------------------------------
void PortMappingTableView::dragMoveEvent(QDragMoveEvent* e)
{
	PortMappingTableView* source = qobject_cast<PortMappingTableView*>(e->source());

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
// Function: PortMappingTableView::performDrag()
//-----------------------------------------------------------------------------
void PortMappingTableView::performDrag()
{
	QModelIndexList indexes = selectedIndexes();
	
    QModelIndexList changedIndexes = indexes;
    foreach (QModelIndex index, indexes)
    {
        if (index.column() != PortMappingColumns::NAME)
        {
            changedIndexes.removeAll(index);
        }
    }

    QStringList portNames;
	foreach (QModelIndex const& index, changedIndexes)
    {
		if (index.isValid())
        {
            portNames.append(index.data(Qt::DisplayRole).toString());
        }
	}

    QString mimeText = portNames.join(';');

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
// Function: PortMappingTableView::mouseMoveEvent()
//-----------------------------------------------------------------------------
void PortMappingTableView::mouseMoveEvent(QMouseEvent* event)
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
// Function: PortMappingTableView::mousePressEvent()
//-----------------------------------------------------------------------------
void PortMappingTableView::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
    {
		startPos_ = event->pos();
    }
	QTableView::mousePressEvent(event);
}

//-----------------------------------------------------------------------------
// Function: PortMappingTableView::getSelectedPorts()
//-----------------------------------------------------------------------------
QStringList PortMappingTableView::getSelectedPorts()
{
	QStringList portList;
	QModelIndexList indexList = selectedIndexes();

    QModelIndexList removedIndexList = indexList;
    foreach (QModelIndex index, indexList)
    {
        if (index.column() == PortMappingColumns::DIRECTION || index.column() == PortMappingColumns::SIZE)
        {
            removedIndexList.removeAll(index);
        }
    }

	// sort indexes so they are in order
	qSort(indexList);

    foreach (QModelIndex const& index, removedIndexList)
    {
		// dont append invalid indexes
		if (index.isValid())
        {
		    portList.append(index.data(Qt::DisplayRole).toString());
        }
	}

	return portList;
}
