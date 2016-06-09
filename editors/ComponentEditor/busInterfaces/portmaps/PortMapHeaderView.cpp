//-----------------------------------------------------------------------------
// File: PortMapHeaderView.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 08.06.2016
//
// Description:
// Header view for port map editor.
//-----------------------------------------------------------------------------

#include "PortMapHeaderView.h"

#include <editors/ComponentEditor/busInterfaces/portmaps/PortMapsColumns.h>

#include <QPainter>

//-----------------------------------------------------------------------------
// Function: PortMapHeaderView::PortMapHeaderView()
//-----------------------------------------------------------------------------
PortMapHeaderView::PortMapHeaderView(Qt::Orientation orientation, QWidget* parent):
QHeaderView(orientation, parent)
{

}

//-----------------------------------------------------------------------------
// Function: PortMapHeaderView::~PortMapHeaderView()
//-----------------------------------------------------------------------------
PortMapHeaderView::~PortMapHeaderView()
{

}

//-----------------------------------------------------------------------------
// Function: PortMapHeaderView::paintSection()
//-----------------------------------------------------------------------------
void PortMapHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    painter->save();
    QHeaderView::paintSection(painter, rect, logicalIndex);
    painter->restore();

    if ( logicalIndex == PortMapsColumns::LOGICAL_PRESENCE || logicalIndex == PortMapsColumns::INVERT)
    {
        QPen oldPen = painter->pen();
        QPen newPen(Qt::lightGray);
        newPen.setWidth(2);

        painter->setPen(newPen);
        painter->drawLine(rect.topRight(), rect.bottomRight());
        painter->setPen(oldPen);
    }
}
