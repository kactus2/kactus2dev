//-----------------------------------------------------------------------------
// File: DefaultRouting.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.09.2017
//
// Description:
// Default routing creation for design connections.
//-----------------------------------------------------------------------------

#include "DefaultRouting.h"

//-----------------------------------------------------------------------------
// Function: DefaultRouting::createRoute()
//-----------------------------------------------------------------------------
QList<QPointF> DefaultRouting::createRoute(QPointF const& p1, QPointF const& p2, 
    QVector2D const& dir1, QVector2D const& dir2)
{
    // Add the start position to the list of path points.
    QList<QPointF> points;
    points.append(p1);

    if (p1 == p2)
    {
        return points;
    }

    // Convert the points to vectors.
    QVector2D startPos = QVector2D(p1);
    QVector2D endPos = QVector2D(p2);

    // Sets the start position as the current position.
    QVector2D curPos = startPos;
    QVector2D curDir = dir1;

    // Set the target position based on the end point's direction.
    QVector2D targetPos = QVector2D(p2) + dir2 * MIN_START_LENGTH;

    // Find a route to the target.
    while (curPos != targetPos)
    {
        // Calculate the delta.
        QVector2D delta = targetPos - curPos;
        qreal dot = QVector2D::dotProduct(delta, curDir);
        qreal endDot = QVector2D::dotProduct(delta, dir2);

        // Calculate the projection onto the current direction and the perpendicular part.
        QVector2D proj = dot * curDir;
        QVector2D perp = delta - proj;

        // Check if we can draw a direct line to the target from the current position.
        if (dot > 0.0 && qFuzzyCompare(delta, proj) && endDot <= 0.0)
        {
            curPos = targetPos;
        }
        else 
        {
            // Otherwise draw at least some distance to the current direction.
            // Check if the target is not behind the current position/direction.
            if (qFuzzyCompare(curPos, startPos))
            {
                if (dot > 0.0 && !(endDot > 0.0 && qFuzzyCompare(delta, endDot * dir2)))
                {
                    // Draw the length of the projection to the current direction
                    // or at least the minimum length.
                    curPos = curPos + curDir * qMax(MIN_START_LENGTH, proj.length());
                }
                else
                {
                    // Otherwise we just draw the minimum length thub.
                    curPos = curPos + curDir * MIN_START_LENGTH;
                }
            }
            // Check if the target is in the opposite direction compared to the current
            // direction and we previously draw the starting thub.
            else if (dot < 0.0 && qFuzzyCompare(curPos, startPos + curDir * MIN_START_LENGTH))
            {
                // Draw to the perpendicular direction at least the minimum length.
                qreal length = qMax(perp.length(), MIN_LENGTH);
                QVector2D dir = perp.normalized();

                // Special case when the perpendicular vector would be zero-length.
                if (dir.isNull())
                {
                    // Rotate 90 degrees.
                    dir = QVector2D(curDir.y(), -curDir.x());
                }

                curPos = curPos + dir * length;
                curDir = dir;
            }
            else
            {
                // Otherwise we just draw to the perpendicular direction as much as we can.
                if (!perp.isNull())
                {
                    curPos = curPos + perp;
                    curDir = perp.normalized();
                }
                else
                {
                    // If the perpendicular vector was zero-length, we rotate the current
                    // direction 90 degrees and draw a minimum length thub to the new direction.
                    curDir = QVector2D(curDir.y(), -curDir.x());
                    curPos = curPos + curDir * MIN_LENGTH;
                }
            }

            // Check if we would end up in a position where we are right behind the target.
            QVector2D newDelta = targetPos - curPos;
            qreal endDot = QVector2D::dotProduct(newDelta, dir2);

            if (endDot > 0.0 && qFuzzyCompare(newDelta, endDot * dir2) &&
                !qFuzzyCompare(curPos, startPos + curDir * MIN_START_LENGTH))
            {
                // Make an adjustment to the current position.
                curPos += curDir * MIN_LENGTH;
            }
        }

        // Add the newly calculated current position to the path.
        points.append(curPos.toPointF());
    }

    // Add the last segment if the target position was not the end position.
    if (!qFuzzyCompare(targetPos, endPos))
    {
        points.append(endPos.toPointF());
    }

    // Simplify path.
    if (points.size() >= 3)
    {
        for (int i = 0; i < points.size() - 2; ++i)
        {
            QVector2D delta1 = QVector2D(points.at(i + 1) - points.at(i));
            QVector2D delta2 = QVector2D(points.at(i + 2) - points.at(i + 1));

            QVector2D deltaProj = QVector2D::dotProduct(delta2, delta1.normalized()) * delta1.normalized();

            // If the path was otherwise ok, just remove parallel lines.
            if (qFuzzyCompare(deltaProj, delta2))
            {
                points.removeAt(i + 1);
                i--;
            }                                
        }
    }

    return points;
}
