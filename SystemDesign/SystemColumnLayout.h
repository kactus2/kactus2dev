//-----------------------------------------------------------------------------
// File: SystemColumnLayout.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 31.5.2011
//
// Description:
// Node column layout which manages the ordering and positioning
// of node columns.
//-----------------------------------------------------------------------------

#ifndef SYSTEMCOLUMNLAYOUT_H
#define SYSTEMCOLUMNLAYOUT_H

#include <QGraphicsScene>
#include <QList>

#include "SystemColumn.h"

//-----------------------------------------------------------------------------
//! SystemColumnLayout class.
//-----------------------------------------------------------------------------
class SystemColumnLayout : public QObject
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] scene The parent graphics scene.
     */
    SystemColumnLayout(QGraphicsScene* scene = 0);

    /*!
     *  Destructor.
     */
    virtual ~SystemColumnLayout();

    /*!
     *  Adds a column to the layout.
     *
     *      @param [in] name The name of the column.
     */
    void addColumn(QString const& name);

    /*!
     *  Adds a column to the layout.
     *
     *      @param [in] column The column to add.
     */
    void addColumn(SystemColumn* column);

    /*!
     *  Removes a column from the layout.
     *
     *      @param [in] column The column to remove.
     */
    void removeColumn(SystemColumn* column);

    /*!
     *  Removes a column from the layout at the given index.
     *
     *      @param [in] index The index of the column to remove.
     */
    void removeColumn(int index);

    /*!
     *  Searches for a column containing the given point.
     *
     *      @param [in] pt The point.
     *
     *      @return The column containing the given point, if found. Otherwise null.
     */
    SystemColumn* findColumnAt(QPointF pt);

    /*!
     *  Called when a column is moved within the layout.
     *
     *      @param [in] column The column that has been moved.
     */
    void onMoveColumn(SystemColumn* column);

    /*!
     *  Called when a column is released from being moved by mouse.
     *
     *      @param [in] column The column that has been released.
     */
    void onReleaseColumn(SystemColumn* column);

    /*!
     *  Sets the drawing offset for the title bars.
     *
     *      @param [in] y The y coordinate offset.
     */
    void setOffsetY(qreal y);

    /*!
     *  Updates the positions of components in all columns.
     */
    void updatePositions();

    /*!
     *  Returns the columns in the column layout.
     */
    QList<SystemColumn*> const& getColumns() const;

signals:
    //! Signals that the contents of the column layout have changed.
    void contentChanged();

private:
    // Disable copying.
    SystemColumnLayout(SystemColumnLayout const& rhs);
    SystemColumnLayout& operator=(SystemColumnLayout const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! The columns.
    QList<SystemColumn*> columns_;

    //! The current layout width.
    qreal layoutWidth_;

    //! The y offset.
    unsigned int offsetY_;
};

//-----------------------------------------------------------------------------

#endif // SYSTEMCOLUMNLAYOUT_H