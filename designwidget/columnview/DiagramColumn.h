//-----------------------------------------------------------------------------
// File: DiagramColumn.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 4.4.2011
//
// Description:
// Diagram column class.
//-----------------------------------------------------------------------------

#ifndef DIAGRAMCOLUMN_H
#define DIAGRAMCOLUMN_H

#include <common/ColumnTypes.h>
#include <common/graphicsItems/GraphicsItemTypes.h>
#include <common/graphicsItems/GraphicsColumn.h>

#include <models/ColumnDesc.h>

#include <QGraphicsRectItem>
#include <QSet>
#include <QCursor>

class DiagramInterconnection;
class DiagramConnectionEndpoint;
class Component;
class GenericEditProvider;

//-----------------------------------------------------------------------------
//! DiagramColumn class.
//-----------------------------------------------------------------------------
class DiagramColumn : public GraphicsColumn
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_DIAGRAM_COLUMN };

    /*!
     *  Constructor.
     *
     *      @param [in] desc    The column description.
     *      @param [in] layout  The parent column layout.
     */
    DiagramColumn(ColumnDesc const& desc, GraphicsColumnLayout* layout, QGraphicsScene* scene);

    /*!
     *  Destructor.
     */
    virtual ~DiagramColumn();

    int type() const { return Type; }

protected:
    /*!
     *  Returns true if the given item is allowed to reside in the column based on the allowed items.
     *
     *      @param [in] item          The item.
     *      @param [in] allowedItems  The allowed items flags.
     */
    virtual bool isItemAllowed(QGraphicsItem* item, unsigned int allowedItems) const;

    /*!
     *  Prepares for column move.
     */
    virtual void prepareColumnMove();

    /*!
     *  Creates an undo command for the column move.
     *
     *      @return The created undo command.
     */
    virtual QSharedPointer<QUndoCommand> createMoveUndoCommand();

private:
    // Disable copying.
    DiagramColumn(DiagramColumn const& rhs);
    DiagramColumn& operator=(DiagramColumn const& rhs);

    /*!
     *  Begins the position updates for the connections that start from the given end point.
     *
     *      @param [in] endpoint The end point.
     */
    void beginUpdateConnPositions(DiagramConnectionEndpoint* endpoint);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The connections that need to be also stored for undo.
    QSet<DiagramInterconnection*> conns_;
};

//-----------------------------------------------------------------------------

#endif // DIAGRAMCOLUMN_H
