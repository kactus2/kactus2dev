//-----------------------------------------------------------------------------
// File: MemoryExtensionGraphicsItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 21.09.2016
//
// Description:
// Extension item for memory designer graphics items.
//-----------------------------------------------------------------------------

#ifndef MEMORYEXTENSIONGRAPHICSITEM_H
#define MEMORYEXTENSIONGRAPHICSITEM_H

#include <designEditors/MemoryDesigner/MemoryDesignerGraphicsItem.h>

//-----------------------------------------------------------------------------
//! Parent class for memory designer graphics items.
//-----------------------------------------------------------------------------
class MemoryExtensionGraphicsItem : public MemoryDesignerGraphicsItem
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] positionX           X position of the extension.
     *      @param [in] positionY           Y position of the extension.
     *      @param [in] extensionWidth      Width of the extension.
     *      @param [in] extensionHeight     Height of the extension.
     *      @param [in] parentItem          Parent item.
     */
    MemoryExtensionGraphicsItem(qreal positionX, qreal positionY, qreal extensionWidth, qreal extensionHeight,
        QGraphicsItem* parentItem);

	/*!
     *  The destructor.
     */
    virtual ~MemoryExtensionGraphicsItem();

    /*!
     *  Get the type of the parent item.
     */
    virtual int type() const;

private:
    // Disable copying.
    MemoryExtensionGraphicsItem(MemoryExtensionGraphicsItem const& rhs);
    MemoryExtensionGraphicsItem& operator=(MemoryExtensionGraphicsItem const& rhs);

    /*!
     *  Set the pen and brush for the memory extension.
     */
    void setPenAndBrush();

    /*!
     *  Set the positions of the information labels.
     */
    virtual void setLabelPositions();
};

//-----------------------------------------------------------------------------

#endif // MEMORYEXTENSIONGRAPHICSITEM_H
