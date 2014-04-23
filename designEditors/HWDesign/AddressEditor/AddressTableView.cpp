//-----------------------------------------------------------------------------
// File: AddressTableView.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 03.09.2012
//
// Description:
// Table view with reordering support.
//-----------------------------------------------------------------------------

#include "AddressTableView.h"

#include <QMouseEvent>

//-----------------------------------------------------------------------------
// Function: AddressTableView::AddressTableView()
//-----------------------------------------------------------------------------
AddressTableView::AddressTableView(QWidget* parent)
    : QTableView(parent)
{

}

//-----------------------------------------------------------------------------
// Function: AddressTableView::~AddressTableView()
//-----------------------------------------------------------------------------
AddressTableView::~AddressTableView()
{

}

//-----------------------------------------------------------------------------
// Function: AddressTableView::mouseMoveEvent()
//-----------------------------------------------------------------------------
void AddressTableView::mouseMoveEvent(QMouseEvent* e)
{
    QModelIndex selected;

    // if left mouse button was pressed 
    if (e->buttons() & Qt::LeftButton)
    {
        QModelIndex startIndex = indexAt(pressedPoint_);
        QModelIndex thisIndex = indexAt(e->pos());
        selected = thisIndex;

        // if the item was dragged to new location
        if (startIndex.isValid() && startIndex != thisIndex)
        {
            setCursor(QCursor(Qt::ClosedHandCursor));

            emit moveItem(startIndex, thisIndex);

            // update the pressed point so the dragging works also
            // when moving further to another index
            pressedPoint_ = e->pos();
        }
    }

    QTableView::mouseMoveEvent(e);

    clearSelection();
    setCurrentIndex(selected);
}

//-----------------------------------------------------------------------------
// Function: AddressTableView::mousePressEvent()
//-----------------------------------------------------------------------------
void AddressTableView::mousePressEvent(QMouseEvent* event)
{
    pressedPoint_ = event->pos();

    // if user clicked area that has no item
    QModelIndex pressedIndex = indexAt(pressedPoint_);
    if (!pressedIndex.isValid()) {
        setCurrentIndex(pressedIndex);
    }

    QTableView::mousePressEvent(event);
}

//-----------------------------------------------------------------------------
// Function: AddressTableView::mouseReleaseEvent()
//-----------------------------------------------------------------------------
void AddressTableView::mouseReleaseEvent(QMouseEvent* event)
{
    setCursor(QCursor(Qt::ArrowCursor));
    QTableView::mouseReleaseEvent(event);
}
