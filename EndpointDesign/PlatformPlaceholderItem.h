//-----------------------------------------------------------------------------
// File: PlatformPlaceholderItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 27.9.2011
//
// Description:
// Placeholder box for the platform component.
//-----------------------------------------------------------------------------

#ifndef PLATFORMPLACEHOLDERITEM_H
#define PLATFORMPLACEHOLDERITEM_H

#include <QGraphicsRectItem>

#include <common/graphicsItems/GraphicsItemTypes.h>

//-----------------------------------------------------------------------------
//! PlatformPlaceholderItem class.
//-----------------------------------------------------------------------------
class PlatformPlaceholderItem : public QGraphicsRectItem
{
public:
    enum { Type = GFX_TYPE_PLATFORM_PLACEHOLDER };

    /*!
     *  Constructor.
     *
     *      @param [in] parent The parent graphics item.
     */
    PlatformPlaceholderItem(QGraphicsItem* parent = 0);

    /*!
     *  Destructor.
     */
    ~PlatformPlaceholderItem();

    /*!
     *  Returns the type id of the placeholder.
     */
    int type() const { return Type; }

private:
    // Disable copying.
    PlatformPlaceholderItem(PlatformPlaceholderItem const& rhs);
    PlatformPlaceholderItem& operator=(PlatformPlaceholderItem const& rhs);

    enum
    {
        WIDTH = 200,
        HEIGHT = 40
    };
};

//-----------------------------------------------------------------------------

#endif // PLATFORMPLACEHOLDERITEM_H
