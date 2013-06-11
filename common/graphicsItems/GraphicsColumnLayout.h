//-----------------------------------------------------------------------------
// File: GraphicsColumnLayout.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 1.6.2012
//
// Description:
// Graphics column layout which manages the ordering and positioning
// of graphics columns.
//-----------------------------------------------------------------------------

#ifndef GRAPHICSCOLUMNLAYOUT_H
#define GRAPHICSCOLUMNLAYOUT_H

#include "GraphicsColumn.h"

#include <common/layouts/IHGraphicsLayout.h>

#include <QGraphicsScene>
#include <QList>

//-----------------------------------------------------------------------------
//! GraphicsColumnLayout class.
//-----------------------------------------------------------------------------
class GraphicsColumnLayout : public QObject
{
    Q_OBJECT

public:
    typedef GraphicsColumn* (*AutoCreateColumnFunc)(GraphicsColumnLayout*);

    /*!
     *  Constructor.
     *
     *      @param [in] scene The parent graphics scene.
     */
    GraphicsColumnLayout(QGraphicsScene* scene = 0);

    /*!
     *  Destructor.
     */
    virtual ~GraphicsColumnLayout();

    /*!
     *  Adds an item to the first column where it is allowed to be placed.
     *
     *      @param [in] item The item to add.
     */
    void addItem(QGraphicsItem* item);

    /*!
     *  Adds a column to the layout.
     *
     *      @param [in] column  The column to add.
     *      @param [in] append  If true, the column is appended to the end of the layout.
     */
    void addColumn(GraphicsColumn* column, bool append = false);

    /*!
     *  Removes a column from the layout.
     *
     *      @param [in] column The column to remove.
     */
    void removeColumn(GraphicsColumn* column);

    /*!
     *  Searches for a column containing the given point.
     *
     *      @param [in] pt The point.
     *
     *      @return The column containing the given point, if found. Otherwise null.
     */
    GraphicsColumn* findColumnAt(QPointF pt);

    /*!
     *  Called when a column is moved within the layout.
     *
     *      @param [in] column The column that has been moved.
     */
    void onMoveColumn(GraphicsColumn* column);

    /*!
     *  Called when a column is released from being moved by mouse.
     *
     *      @param [in] column The column that has been released.
     */
    void onReleaseColumn(GraphicsColumn* column);

    /*!
     *  Updates the column positions.
     */
    void updateColumnPositions();

    /*!
     *  Sets auto-reorganize on/off.
     *
     *      @param [in] autoReorganized If true, auto-reorganize is set on.
     */
    void setAutoReorganized(bool autoReorganized);

    /*!
     *  Sets the column auto-creation function.
     */
    void setAutoCreateColumnFunction(AutoCreateColumnFunc func);

    /*!
     *  Creates a column with default properties by calling the auto-create function.
     */
    int autoCreateColumn();

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
    QList<GraphicsColumn*> const& getColumns() const;

    /*!
     *  Returns true if the layout is auto-reorganized.
     */
    bool isAutoReorganized() const;

private:
    // Disable copying.
    GraphicsColumnLayout(GraphicsColumnLayout const& rhs);
    GraphicsColumnLayout& operator=(GraphicsColumnLayout const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Layouter for columns.
    QSharedPointer< IHGraphicsLayout<GraphicsColumn> > layout_;

    //! The columns.
    QList<GraphicsColumn*> columns_;

    //! The current layout width.
    qreal layoutWidth_;

    //! The y offset.
    unsigned int offsetY_;

    //! If true, the column layout can auto-reorganize (incl. growing and shrinking).
    bool autoReorganized_;

    //! The function pointer to the column auto-creation function.
    AutoCreateColumnFunc autoCreateColumnFunc_;
};

//-----------------------------------------------------------------------------

#endif // GRAPHICSCOLUMNLAYOUT_H