//-----------------------------------------------------------------------------
// File: PhysicalPortMappingTableView.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 19.05.2016
//
// Description:
// Table view for physical ports.
//-----------------------------------------------------------------------------

#include "PhysicalPortMappingTableView.h"

#include <QApplication>
#include <QMimeData>
#include <QDrag>
#include <QModelIndex>

//-----------------------------------------------------------------------------
// Function: PhysicalPortMappingTableView::PhysicalPortMappingTableView()
//-----------------------------------------------------------------------------
PhysicalPortMappingTableView::PhysicalPortMappingTableView(QWidget *parent):
PortMappingTableView(parent)
{
}

//-----------------------------------------------------------------------------
// Function: PhysicalPortMappingTableView::~PhysicalPortMappingTableView()
//-----------------------------------------------------------------------------
PhysicalPortMappingTableView::~PhysicalPortMappingTableView()
{
}

//-----------------------------------------------------------------------------
// Function: PhysicalPortMappingTableView::onFilterNameChanged()
//-----------------------------------------------------------------------------
void PhysicalPortMappingTableView::onFilterNameChanged(QString const& portName)
{    
    QModelIndex matchingIndex = model()->index(0, 0);
    
    if (!matchingIndex.isValid())
    {
        return;
    }

    int bestMatch =  QString::compare(matchingIndex.data().toString(), portName, Qt::CaseInsensitive);

    // Search for better matching port names.
    int rowCount = model()->rowCount();
    for (int row = 1; row < rowCount; row++)
    {
        QModelIndex index = model()->index(row, 0);
        if (index.isValid())
        {
            int diff = QString::compare(index.data().toString(), portName, Qt::CaseInsensitive);
            if (bestMatch > diff)
            {
                matchingIndex = index;
                bestMatch = diff;
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
// Function: PhysicalPortMappingTableView::dropEvent()
//-----------------------------------------------------------------------------
void PhysicalPortMappingTableView::dropEvent(QDropEvent* event) 
{
	// make sure the source is not this view
	PortMappingTableView* source = qobject_cast<PortMappingTableView*>(event->source());
	
	// if source is neither of the supported
	if (!source)
    {
		return;
	}

	QModelIndex index = indexAt(event->pos());

	// tell source that nothing is to be done to item
	event->setDropAction(Qt::IgnoreAction);

	// if no port name has been specified or drop index is invalid
	QString mimeText = event->mimeData()->text();
	if (mimeText.isEmpty())
    {
		event->accept();
		return;
	}

	// get the ports that were selected
	QStringList dropped = mimeText.split(QString(";"), QString::SkipEmptyParts);

	// if the item to drop is from this port list view
	if (source == this)
    {
		event->accept();
		return;
	}

	// get the selected indexes in this view
	QModelIndexList indexes = selectedIndexes();
	QStringList physicals;

	// add the physical port that matches each selected index
	foreach (QModelIndex const& index, indexes)
    {
		if (index.isValid())
        {
			physicals.append(index.model()->data(index, Qt::DisplayRole).toString());
		}
	}

	// accept the drop
	event->accept();
}

//-----------------------------------------------------------------------------
// Function: PhysicalPortMappingTableView::mouseMoveEvent()
//-----------------------------------------------------------------------------
void PhysicalPortMappingTableView::mouseMoveEvent(QMouseEvent* event)
{
    // if either mouse button is pressed
    if (event->buttons() == Qt::LeftButton || event->buttons() == Qt::RightButton)
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
// Function: PhysicalPortMappingTableView::mousePressEvent()
//-----------------------------------------------------------------------------
void PhysicalPortMappingTableView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton || event->buttons() == Qt::RightButton)
    {
        startPos_ = event->pos();
    }

    QTableView::mousePressEvent(event);
}
