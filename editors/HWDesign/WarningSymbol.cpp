//-----------------------------------------------------------------------------
// File: WarningSymbol.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 02.11.2021
//
// Description:
// Warning symbol constructed from QPolygonf
//-----------------------------------------------------------------------------

#include "WarningSymbol.h"

#include <editors/common/diagramgrid.h>

#include <QPen>

//-----------------------------------------------------------------------------
// Function: WarningSymbol::WarningSymbol()
//-----------------------------------------------------------------------------
WarningSymbol::WarningSymbol(QGraphicsItem* parent) :
QGraphicsPolygonItem(parent)
{
    setToolTip(QObject::tr("No portmaps found"));

    QPen warningPen(Qt::black, 0);
    setPen(warningPen);

    QBrush warningBrush(QColor(255, 223, 0));
    setBrush(warningBrush);

    constexpr int squareSize = GridSize;
    constexpr qreal startingY = squareSize - 2;
    constexpr qreal pointDistance = 2 * squareSize / 3;

    QPolygonF warningShape({
        QPointF(0, startingY),
        QPointF(-pointDistance, startingY + pointDistance),
        QPointF(-pointDistance * 2, startingY),
        QPointF(-pointDistance, startingY - pointDistance)
    });

    setPolygon(warningShape);

    QPolygonF exclamationShape({ QPointF(-squareSize / 5, startingY), QPointF(-squareSize + 3, startingY) });
    exclamationLine_->setPolygon(exclamationShape);

    QPolygonF exclamationDotShape({ QPointF(-squareSize, startingY), QPointF(-squareSize + 1 , startingY) });
    exclamationDot_->setPolygon(exclamationDotShape);
}

//-----------------------------------------------------------------------------
// Function: WarningSymbol::moveSymbol()
//-----------------------------------------------------------------------------
void WarningSymbol::moveSymbol(QVector2D const& dir)
{
    if (qFuzzyCompare(dir, QVector2D(1.0f, 0.0f)))
    {
        setRotation(180.0);
        setPos(QPointF(0,  2* (GridSize - 2)));
    }

    else if (qFuzzyCompare(dir, QVector2D(-1.0f, 0.0f)))
    {
        setRotation(0);
        setPos(QPointF(0.0, 0.0));
    }
}
