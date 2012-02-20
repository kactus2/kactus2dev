//-----------------------------------------------------------------------------
// File: DiagramConnectionEndPoint.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 18.3.2011
//
// Description:
// Interface class for the diagram connection end points.
//-----------------------------------------------------------------------------

#include "DiagramConnectionEndpoint.h"

#include <QPen>

//-----------------------------------------------------------------------------
// Function: DiagramConnectionEndPoint()
//-----------------------------------------------------------------------------
DiagramConnectionEndPoint::DiagramConnectionEndPoint(QGraphicsItem* parent,
                                                     QVector2D const& dir) : QGraphicsPolygonItem(parent),
                                                                             connections_(), dir_()
{
    setDirection(dir);
}

//-----------------------------------------------------------------------------
// Function: ~DiagramConnectionEndPoint()
//-----------------------------------------------------------------------------
DiagramConnectionEndPoint::~DiagramConnectionEndPoint() {
	emit destroyed(this);
}

//-----------------------------------------------------------------------------
// Function: setHighlight()
//-----------------------------------------------------------------------------
void DiagramConnectionEndPoint::setHighlight(HighlightMode mode)
{
    switch (mode)
    {
    case HIGHLIGHT_OFF:
        {
            setPen(QPen(Qt::black, 1));
            break;
        }

    case HIGHLIGHT_ALLOWED:
        {
            setPen(QPen(Qt::blue, 2));
            break;
        }

    case HIGHLIGHT_HOVER:
        {
            setPen(QPen(Qt::red, 2));
            break;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: addInterconnection()
//-----------------------------------------------------------------------------
void DiagramConnectionEndPoint::addInterconnection(DiagramInterconnection* connection)
{
    connections_.append(connection);
}

//-----------------------------------------------------------------------------
// Function: removeInterconnection()
//-----------------------------------------------------------------------------
void DiagramConnectionEndPoint::removeInterconnection(DiagramInterconnection* connection)
{
    connections_.removeAll(connection);
}

//-----------------------------------------------------------------------------
// Function: getInterconnections()
//-----------------------------------------------------------------------------
QList<DiagramInterconnection*> const& DiagramConnectionEndPoint::getInterconnections() const
{
    return connections_;
}

//-----------------------------------------------------------------------------
// Function: isConnected()
//-----------------------------------------------------------------------------
bool DiagramConnectionEndPoint::isConnected() const
{
    return (!connections_.empty());
}

//-----------------------------------------------------------------------------
// Function: setDirection()
//-----------------------------------------------------------------------------
void DiagramConnectionEndPoint::setDirection(QVector2D const& dir)
{
    // Determine the rotation based on the new direction.
    if (qFuzzyCompare(dir, QVector2D(0.0f, -1.0f)))
    {
        setRotation(0.0);
    }
    else if (qFuzzyCompare(dir, QVector2D(0.0f, 1.0f)))
    {
        setRotation(180.0);
    }
    else if (qFuzzyCompare(dir, QVector2D(1.0f, 0.0f)))
    {
        setRotation(90.0);
    }
    else if (qFuzzyCompare(dir, QVector2D(-1.0f, 0.0f)))
    {
        setRotation(-90.0);
    }

    dir_ = dir;
}

//-----------------------------------------------------------------------------
// Function: isDirectionFixed()
//-----------------------------------------------------------------------------
bool DiagramConnectionEndPoint::isDirectionFixed() const
{
    // The end point direction is not fixed by default.
    return false;
}

//-----------------------------------------------------------------------------
// Function: getDirection()
//-----------------------------------------------------------------------------
QVector2D const& DiagramConnectionEndPoint::getDirection() const
{
    return dir_;
}

//-----------------------------------------------------------------------------
// Function: getOffPageConnector()
//-----------------------------------------------------------------------------
DiagramConnectionEndPoint* DiagramConnectionEndPoint::getOffPageConnector()
{
    return 0;
}

//-----------------------------------------------------------------------------
// Function: DiagramConnectionEndPoint::isAdHoc()
//-----------------------------------------------------------------------------
bool DiagramConnectionEndPoint::isAdHoc() const
{
    return (!isBus());
}
