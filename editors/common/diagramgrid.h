#ifndef DIAGRAMGRID_H
#define DIAGRAMGRID_H

#include <QPointF>

static constexpr int GridSize = 10;

/*! \brief Snaps the given QPointF to the diagram grid
 *
 */
inline constexpr QPointF snapPointToGrid(const QPointF& point)
{
    return QPointF((point / GridSize).toPoint() * GridSize);
}

/*! \brief Snaps the given coordinates to the diagram grid
 *
 */
inline constexpr QPointF snapPointToGrid(qreal x, qreal y)
{
    return snapPointToGrid(QPointF(x, y));
}

#endif // DIAGRAMGRID_H
