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

class GraphicsColumnLayout;
class GraphicsConnection;
class MainMemoryGraphicsItem;

#include <QGraphicsRectItem>
#include <QSet>
#include <QCursor>

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
     *      @param [in] name            The column description.
     *      @param [in] layout          The parent column layout.
     *      @param [in] itemSpacing     Spacing for the graphics items.
     */
    MemoryColumn(QSharedPointer<ColumnDesc> desc, GraphicsColumnLayout* layout, int itemSpacing = 30);

    /*!
     *  The destructor.
     */
    virtual ~MemoryColumn();

    /*!
     *  Searches the item which contains the given y coordinate.
     *
     *      @param [i] y The y coordinate.
     *
     *      @return The item which contains the given y coordinate.
     */
    MainMemoryGraphicsItem* findItemAt(int y) const;

    /*!
     *  Find a graphics item at a given position.
     *
     *      @param [in] itemYPosition   The given position.
     */
    MainMemoryGraphicsItem* findGraphicsItemAt(int itemYPosition) const;

    /*!
     *  Find a graphics item by a given name.
     *
     *      @param [in] itemName        Name of the searched item.
     *      @param [in] instanceName    Name of the component instance containing the graphics item.
     */
    MainMemoryGraphicsItem* findGraphicsItemByName(QString const& itemName, QString const& instanceName) const;

    /*!
     *  Get the type of this item.
     */
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
