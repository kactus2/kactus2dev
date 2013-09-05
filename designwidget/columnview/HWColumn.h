//-----------------------------------------------------------------------------
// File: HWColumn.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 4.4.2011
//
// Description:
// Diagram column class.
//-----------------------------------------------------------------------------

#ifndef HWCOLUMN_H
#define HWCOLUMN_H

#include <common/ColumnTypes.h>
#include <common/graphicsItems/GraphicsItemTypes.h>
#include <common/graphicsItems/GraphicsColumn.h>

#include <models/ColumnDesc.h>

#include <QGraphicsRectItem>
#include <QSet>
#include <QCursor>

class GraphicsConnection;
class HWConnectionEndpoint;
class Component;
class GenericEditProvider;

//-----------------------------------------------------------------------------
//! HWColumn class.
//-----------------------------------------------------------------------------
class HWColumn : public GraphicsColumn
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
    HWColumn(ColumnDesc const& desc, GraphicsColumnLayout* layout);

    /*!
     *  Destructor.
     */
    virtual ~HWColumn();

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
    HWColumn(HWColumn const& rhs);
    HWColumn& operator=(HWColumn const& rhs);
};

//-----------------------------------------------------------------------------

#endif // HWCOLUMN_H
