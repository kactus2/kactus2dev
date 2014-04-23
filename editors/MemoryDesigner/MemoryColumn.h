//-----------------------------------------------------------------------------
// File: MemoryColumn.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 17.10.2012
//
// Description:
// Memory column.
//-----------------------------------------------------------------------------

#ifndef MEMORYCOLUMN_H
#define MEMORYCOLUMN_H

#include <common/graphicsItems/GraphicsItemTypes.h>
#include <common/graphicsItems/GraphicsColumn.h>

#include <QGraphicsRectItem>
#include <QSet>
#include <QCursor>

class GraphicsColumnLayout;
class GraphicsConnection;
class MemoryBaseItem;

//-----------------------------------------------------------------------------
//! MemoryColumn class.
//-----------------------------------------------------------------------------
class MemoryColumn : public GraphicsColumn
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_SYSTEM_COLUMN };

    /*!
     *  Constructor.
     *
     *      @param [in] name          The column description.
     *      @param [in] layout        The parent column layout.
     */
    MemoryColumn(ColumnDesc const& desc, GraphicsColumnLayout* layout);

    /*!
     *  Destructor.
     */
    virtual ~MemoryColumn();

    /*!
     *  Searches the item which contains the given y coordinate.
     *
     *      @param [i] y The y coordinate.
     *
     *      @return The item which contains the given y coordinate.
     */
    MemoryBaseItem* findItemAt(int y) const;

    int type() const { return Type; }

protected:
    /*!
     *  Returns true if the given item is allowed to reside in the column based on the allowed items.
     *
     *      @param [in] item          The item.
     *      @param [in] allowedItems  The allowed items flags.
     */
    virtual bool isItemAllowed(QGraphicsItem* item, unsigned int allowedItems) const;

private:
    // Disable copying.
    MemoryColumn(MemoryColumn const& rhs);
    MemoryColumn& operator=(MemoryColumn const& rhs);
};

//-----------------------------------------------------------------------------

#endif // MEMORYCOLUMN_H
