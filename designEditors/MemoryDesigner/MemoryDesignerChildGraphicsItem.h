//-----------------------------------------------------------------------------
// File: MemoryDesignerChildGraphicsItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 19.09.2016
//
// Description:
// Parent item for visualizing memory child items i.e. address blocks and segments.
//-----------------------------------------------------------------------------

#ifndef MEMORYDESIGNERCHILDGRAPHICSITEM_H
#define MEMORYDESIGNERCHILDGRAPHICSITEM_H

#include <designEditors/MemoryDesigner/MemoryDesignerGraphicsItem.h>

class MemoryItem;

#include <QSharedPointer>
#include <QGraphicsItem>

//-----------------------------------------------------------------------------
//! Parent item for visualizing memory child items i.e. address blocks and segments.
//-----------------------------------------------------------------------------
class MemoryDesignerChildGraphicsItem : public MemoryDesignerGraphicsItem
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] memoryItem      Memory item containing the graphics item data.
     *      @param [in] toolTipType     The type of the memory item.
     *      @param [in] baseAddress     Base address of the memory item.
     *      @param [in] range           Range of the memory item.
     *      @param [in] blockWidth      Width of the memory item.
     *      @param [in] parent          The parent item.
     */
    MemoryDesignerChildGraphicsItem(QSharedPointer<MemoryItem> memoryItem, QString const& toolTipType,
        quint64 baseAddress, quint64 range, qreal blockWidth, QGraphicsItem* parent);

	/*!
     *  The destructor.
     */
    virtual ~MemoryDesignerChildGraphicsItem();

    /*!
     *  Get the type of the memory graphics item.
     */
    virtual int type() const  = 0;

    /*!
     *  Change the address range of the memory item.
     *
     *      @param [in] offset  The offset of the parent item.
     */
    virtual void changeAddressRange(quint64 offset);

protected:

    /*!
     *  Set the colors for the memory item.
     *
     *      @param [in] itemColor       The main color for the memory item.
     *      @param [in] itemIsEmpty     Boolean for determining empty memory item.
     */
    void setColors(QColor itemColor, bool itemIsEmpty);

private:
    // Disable copying.
    MemoryDesignerChildGraphicsItem(MemoryDesignerChildGraphicsItem const& rhs);
    MemoryDesignerChildGraphicsItem& operator=(MemoryDesignerChildGraphicsItem const& rhs);
};

//-----------------------------------------------------------------------------

#endif // MEMORYDESIGNERCHILDGRAPHICSITEM_H
