//-----------------------------------------------------------------------------
// File: DefaultRouting.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.09.2017
//
// Description:
// Default routing creation for design connections.
//-----------------------------------------------------------------------------

#ifndef DEFAULTROUTING_H
#define DEFAULTROUTING_H

#include <QList>
#include <QPointF>
#include <QVector2D>


namespace DefaultRouting 
{
    float const MIN_LENGTH = 10.0f;
    float const MIN_START_LENGTH = 20.0f;

    /*!
     *  Creates an optimal route from point p1 to point p2, considering the given start and exit directions.
     */
    QList<QPointF> createRoute(QPointF const& p1, QPointF const& p2, QVector2D const& dir1, QVector2D const& dir2);

};

#endif // DEFAULTROUTING_H