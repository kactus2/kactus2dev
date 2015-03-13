//-----------------------------------------------------------------------------
// File: ArrayView.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 12.03.2015
//
// Description:
// Editor for changing values in arrays.
//-----------------------------------------------------------------------------

#include "ArrayView.h"

#include <QHeaderView>
#include <QEvent>

//-----------------------------------------------------------------------------
// Function: ArrayView::ArrayView()
//-----------------------------------------------------------------------------
ArrayView::ArrayView(QWidget* parent):
QTableView(parent)
{
    setAlternatingRowColors(false);
    verticalHeader()->setDefaultSectionSize(fontMetrics().height() + 8);
    verticalHeader()->hide();
    horizontalHeader()->setStretchLastSection(true);
}

//-----------------------------------------------------------------------------
// Function: ArrayView::~ArrayView()
//-----------------------------------------------------------------------------
ArrayView::~ArrayView()
{

}

//-----------------------------------------------------------------------------
// Function: ArrayView::keyboardSearch()
//-----------------------------------------------------------------------------
void ArrayView::keyboardSearch(QString const& /*search*/)
{
    return;
}

//-----------------------------------------------------------------------------
// Function: ArrayView::eventFilter()
//-----------------------------------------------------------------------------
bool ArrayView::eventFilter(QObject* /*filterObject*/, QEvent *filterEvent)
{
    if (filterEvent->type() == QEvent::KeyPress)
    {
        return true;
    }

    return false;
}
