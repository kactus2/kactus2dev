//-----------------------------------------------------------------------------
// File: GraphicsItemTypes.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 3.5.2012
//
// Description:
// Enumeration of custom graphics item types.
//-----------------------------------------------------------------------------

#ifndef GRAPHICSITEMTYPES_H
#define GRAPHICSITEMTYPES_H

#include <QGraphicsItem>

//-----------------------------------------------------------------------------
// Custom graphics item types.
//-----------------------------------------------------------------------------
enum GraphicsItemTypes
{
    GFX_TYPE_DIAGRAM_COMPONENT = QGraphicsItem::UserType + 1,
    GFX_TYPE_DIAGRAM_INTERCONNECTION,
    GFX_TYPE_DIAGRAM_PORT,
    GFX_TYPE_DIAGRAM_INTERFACE,
    GFX_TYPE_DIAGRAM_COLUMN,
    GFX_TYPE_DIAGRAM_ADHOC_PORT,
    GFX_TYPE_DIAGRAM_ADHOC_INTERFACE,
    GFX_TYPE_DIAGRAM_OFF_PAGE_CONNECTOR,

    GFX_TYPE_SYSTEM_COLUMN,
    GFX_TYPE_PROGRAM_ENTITY,
    GFX_TYPE_APP_PLACEHOLDER,
    GFX_TYPE_ENDPOINT_STACK,
    GFX_TYPE_ENDPOINT_CONNECTION,
    GFX_TYPE_ENDPOINT,
    GFX_TYPE_MAPPING_COMPONENT,
    GFX_TYPE_PLATFORM_COMPONENT,
    GFX_TYPE_PLATFORM_PLACEHOLDER,
    GFX_TYPE_COMPONENT_ITEM,
    GFX_TYPE_APPLICATION_ITEM,

    GFX_TYPE_HW_MAPPING_ITEM,
    GFX_TYPE_SW_COMPONENT_ITEM,
    GFX_TYPE_SW_PORT_ITEM,
    GFX_TYPE_SW_INTERFACE_ITEM,
    GFX_TYPE_SW_CONNECTION,

    GFX_TYPE_MEMORY_ITEM,
    GFX_TYPE_ADDRESS_SPACE_ITEM,
    GFX_TYPE_ADDRESS_SECTION_ITEM,
    GFX_TYPE_MEMORY_COLUMN,

	GFX_TYPE_EXPAND_COLLAPSE_ITEM
};

//-----------------------------------------------------------------------------

#endif // GRAPHICSITEMTYPES_H
