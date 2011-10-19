//-----------------------------------------------------------------------------
// File: DiagramColumnLayout.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 4.4.2011
//
// Description:
// Diagram column layout which manages the ordering and positioning
// of diagram columns.
//-----------------------------------------------------------------------------

#ifndef DIAGRAMCOLUMNLAYOUT_H
#define DIAGRAMCOLUMNLAYOUT_H

#include <QGraphicsScene>
#include <QList>

#include "DiagramColumn.h"

class GenericEditProvider;

//-----------------------------------------------------------------------------
//! DiagramColumnLayout class.
//-----------------------------------------------------------------------------
class DiagramColumnLayout : public QObject
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] editProvider  The edit provider for undo/redo.
     *      @param [in] scene         The parent graphics scene.
     */
    DiagramColumnLayout(GenericEditProvider& editProvider, QGraphicsScene* scene = 0);

    /*!
     *  Destructor.
     */
    virtual ~DiagramColumnLayout();

    /*!
     *  Adds a column to the layout.
     *
     *      @param [in] name          The name of the column.
     *      @param [in] type          The content type.
     *      @param [in] allowedItems  The allowed items, if the content type is custom.
     *                                This value is discarded if the content type is something else.
     *                                See ColumnItemType for possible values.
     */
    void addColumn(QString const& name, ColumnContentType contentType,
                   unsigned int allowedItems = CIT_NONE);

    /*!
     *  Adds a column to the layout.
     *
     *      @param [in] column The column to add.
     */
    void addColumn(DiagramColumn* column);

    /*!
     *  Adds an item to the first column where it is allowed to be placed.
     *
     *      @param [in] item The item to add.
     */
    void addItem(QGraphicsItem* item);

    /*!
     *  Removes a column from the layout.
     *
     *      @param [in] column The column to remove.
     */
    void removeColumn(DiagramColumn* column);

    /*!
     *  Removes a column from the layout at the given index.
     *
     *      @param [in] index The index of the column to remove.
     */
    void removeColumn(int index);

    /*!
     *  Updates the column positions (due to e.g. a width change).
     */
    void updateColumnPositions();

    /*!
     *  Searches for a column containing the given point.
     *
     *      @param [in] pt The point.
     *
     *      @return The column containing the given point, if found. Otherwise null.
     */
    DiagramColumn* findColumnAt(QPointF pt);

    /*!
     *  Called when a column is moved within the layout.
     *
     *      @param [in] column The column that has been moved.
     */
    void onMoveColumn(DiagramColumn* column);

    /*!
     *  Called when a column is released from being moved by mouse.
     *
     *      @param [in] column The column that has been released.
     */
    void onReleaseColumn(DiagramColumn* column);

    /*!
     *  Sets the drawing offset for the title bars.
     *
     *      @param [in] y The y coordinate offset.
     */
    void setOffsetY(qreal y);

    /*!
     *  Returns the columns in the column layout.
     */
    QList<DiagramColumn*> const& getColumns() const;

    /*!
     *  Returns the edit provider.
     */
    GenericEditProvider& getEditProvider();

signals:
    //! Signals that the contents of the column layout have changed.
    void contentChanged();

private:
    // Disable copying.
    DiagramColumnLayout(DiagramColumnLayout const& rhs);
    DiagramColumnLayout& operator=(DiagramColumnLayout const& rhs);
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The edit provider.
    GenericEditProvider& editProvider_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! The columns.
    QList<DiagramColumn*> columns_;

    //! The current layout width.
    qreal layoutWidth_;

    //! The y offset.
    unsigned int offsetY_;
};

//-----------------------------------------------------------------------------

#endif // DIAGRAMCOLUMNLAYOUT_H