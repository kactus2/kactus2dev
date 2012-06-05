//-----------------------------------------------------------------------------
// File: SystemColumn.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 31.5.2011
//
// Description:
// System column class for managing SW mapping components in a system design.
//-----------------------------------------------------------------------------

#ifndef SYSTEMCOLUMN_H
#define SYSTEMCOLUMN_H

#include <common/graphicsItems/GraphicsItemTypes.h>
#include <common/graphicsItems/GraphicsColumn.h>

#include <QGraphicsRectItem>
#include <QSet>
#include <QCursor>

class GraphicsColumnLayout;
class GraphicsConnection;

//-----------------------------------------------------------------------------
//! SystemColumn class.
//-----------------------------------------------------------------------------
class SystemColumn : public GraphicsColumn
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_SYSTEM_COLUMN };

    /*!
     *  Constructor.
     *
     *      @param [in] name          The column description.
     *      @param [in] layout        The parent column layout.
     *      @param [in] scene         The graphics scene.
     */
    SystemColumn(ColumnDesc const& desc, GraphicsColumnLayout* layout, QGraphicsScene* scene);

    /*!
     *  Destructor.
     */
    virtual ~SystemColumn();

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
    SystemColumn(SystemColumn const& rhs);
    SystemColumn& operator=(SystemColumn const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The connections that need to be also stored for undo.
    QSet<GraphicsConnection*> conns_;
};

//-----------------------------------------------------------------------------

#endif // SYSTEMCOLUMN_H
