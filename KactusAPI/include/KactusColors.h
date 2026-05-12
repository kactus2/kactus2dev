//-----------------------------------------------------------------------------
// File: KactusColors.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 6.6.2012
//
// Description:
// Common colors used in Kactus2.
//-----------------------------------------------------------------------------

#ifndef KACTUSCOLORS_H
#define KACTUSCOLORS_H

#include <KactusAPI/KactusAPIGlobal.h>

#include <QColor>
#include <QtGlobal>

class QPalette;

namespace KactusColors
{
    //! Icon and visual design base colors.
    extern KACTUS2_API QColor KACTUS_BLUE;
    extern KACTUS2_API QColor KACTUS_GREEN;

    //! Errors.
    extern KACTUS2_API QColor ERROR_COLOR;

    //! Default window background color assigned by style/theme
    extern KACTUS2_API QColor DEFAULT_WINDOW_BG;

    //! Table view gridline color
    extern KACTUS2_API QColor TABLE_GRIDLINE;

    //! Dock widget
    extern KACTUS2_API QColor DOCK_WIDGET_TITLE_BG;

    //! Warnings.
    extern KACTUS2_API QColor WARNING;

    //! Colors used in text.
    extern KACTUS2_API QColor REGULAR_TEXT; // Regular text.
    extern KACTUS2_API QColor DISABLED_TEXT; // Text in disabled or uneditable elements.
    extern KACTUS2_API QColor REGULAR_MESSAGE; // Regular console messages.

    //! Colors used as field backgrounds.
    extern KACTUS2_API QColor REGULAR_FIELD; // Backgrounds and regular fields.
    extern KACTUS2_API QColor MANDATORY_FIELD; // Must be filled.
    extern KACTUS2_API QColor DISABLED_FIELD; // Has no relevance in current context.
    extern KACTUS2_API QColor INVALID_FIELD; // The value is not valid.
    extern KACTUS2_API QColor STRONG_FIELD; // Has more weight than others.
    extern KACTUS2_API QColor LOGICAL_PORT_FIELD; // Relates to logical ports.

    //! Colors for languages and generated texts.
    extern KACTUS2_API QColor TEXT_BODY_HIGHLIGHT; // Background for text bodies such as logic.
    extern KACTUS2_API QColor FUNCTION_HIGHLIGHT; // Functions().
    extern KACTUS2_API QColor PREPROCESSOR_HIGHLIGHT; // Preprocessor #macros.
    extern KACTUS2_API QColor STRING_HIGHLIGHT; // "Character strings."

    //! Colors used in design diagram interconnections.
    extern KACTUS2_API QColor REGULAR_CONNECTION; // Most connections.
    extern KACTUS2_API QColor BROKEN_CONNECTION; // Broken/invalid connections.
    extern KACTUS2_API QColor COM_CONNECTION; // COM connections.
    extern KACTUS2_API QColor CONNECTION_UNDERCROSSING;  // Used for crossing interconnections.
    extern KACTUS2_API QColor UNKNOWN_CONNECTION; // Connections unfitting to other categories.

    //! Colors corresponding each available interface mode.
    extern KACTUS2_API QColor MASTER_INTERFACE; 
    extern KACTUS2_API QColor SLAVE_INTERFACE;
    extern KACTUS2_API QColor MIRROREDMASTER_INTERFACE;
    extern KACTUS2_API QColor MIRROREDSLAVE_INTERFACE;
    extern KACTUS2_API QColor SYSTEM_INTERFACE;
    extern KACTUS2_API QColor MIRROREDSYSTEM_INTERFACE;
    extern KACTUS2_API QColor MONITOR_INTERFACE;
    extern KACTUS2_API QColor INVALID_INTERFACE; // The interface is somehow invalid, eg. lacks a mode or type.
    extern KACTUS2_API QColor ENDPOINT_TYPE_API; // The interface is for API connections.
    extern KACTUS2_API QColor ADHOC_PORT;

    //! Colors used in component instances.
    extern KACTUS2_API QColor MISSING_COMPONENT; // The component of the instance does not exist.
    extern KACTUS2_API QColor DRAFT_COMPONENT; // The instance is a draft for a new component.
    extern KACTUS2_API QColor HW_BUS_COMPONENT; // The component of the instance is a bus or a bridge.
    extern KACTUS2_API QColor HW_COMPONENT; // The component of the instance is hardware component.
    extern KACTUS2_API QColor SW_COMPONENT; // The component of the instance is software component.

    //! Other colors used in design.
    extern KACTUS2_API QColor DIAGRAM_SELECTION; // Selections in design diagrams.
    extern KACTUS2_API QColor DIAGRAM_ALLOWED_INTERFACE; // Highlights allowed interface for a connection.
    extern KACTUS2_API QColor DIAGRAM_GRID;           // Diagram background dot grid color  
    extern KACTUS2_API QColor DIAGRAM_COLUMN_HEADER; // Background color for column headers.
    extern KACTUS2_API QColor STICKY_NOTE_BACKGROUND; // Sticky note background
    extern KACTUS2_API QColor STICKY_NOTE_TEXT;       // Sticky note text color
    extern KACTUS2_API QColor ASSOCIATION_LINE;     // Association line color

    //! Colors used for memory/address sections in the memory designer.
    extern KACTUS2_API QColor MEMORY_BLOCK;
    extern KACTUS2_API QColor ADDRESS_SEGMENT;
    extern KACTUS2_API QColor ADDRESS_SEGMENT_UNSEGMENTED;

    extern KACTUS2_API QColor MEM_MAP_COLOR;
    extern KACTUS2_API QColor ADDR_BLOCK_COLOR;
    extern KACTUS2_API QColor REGISTER_FILE_COLOR;
    extern KACTUS2_API QColor REGISTER_COLOR;
    extern KACTUS2_API QColor FIELD_COLOR;
    extern KACTUS2_API QColor MEM_ITEM_EXTENSION;
    
    extern KACTUS2_API QColor MEMORY_DESIGNER_TEXT;
    extern KACTUS2_API QColor MEM_DESIGNER_CONNECTION;
    extern KACTUS2_API QColor MEM_DESIGNER_LOCAL_MAP_CONNECTION;
    extern KACTUS2_API QColor MEM_DESIGNER_ADDR_SPACE_CONNECTION;

    //! Memory visualizer
    extern KACTUS2_API QColor MEM_GRAPH_ITEM_BORDER;

    //! Colors used in dependency analysis.
    extern KACTUS2_API QColor DEPENDECY_ADDED; // Added dependency.
    extern KACTUS2_API QColor DEPENDECY_COVERAGE; // Used to illustrate the extent of dependencies.
    extern KACTUS2_API QColor ROW_SEPARATOR; // Used to separate rows on a list.

    // Colors for ribbon
    extern KACTUS2_API QColor RIBBON_GRADIENT_TOP;
    extern KACTUS2_API QColor RIBBON_GRADIENT_BOTTOM;
    extern KACTUS2_API QColor RIBBONGROUP_TITLE_GRADIENT_TOP;
    extern KACTUS2_API QColor RIBBONGROUP_TITLE_GRADIENT_BOTTOM;
    extern KACTUS2_API QColor RIBBONGROUP_TITLE_TEXT;
    
    //! Portmap editor alternate color
    extern KACTUS2_API QColor PORTMAP_ALT_ROW;
}

//-----------------------------------------------------------------------------

#endif // KACTUSCOLORS_H
