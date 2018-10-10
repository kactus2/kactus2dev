//-----------------------------------------------------------------------------
// File: KactusColors.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 6.6.2012
//
// Description:
// Common colors used in Kactus2.
//-----------------------------------------------------------------------------

#ifndef KACTUSCOLORS_H
#define KACTUSCOLORS_H

#include <QColor>

namespace KactusColors
{
    //! Icon and visual design base colors.
    QColor const KACTUS_BLUE(0x1b, 0x65, 0xaf);
    QColor const KACTUS_GREEN(0x32,0xbc, 0x8f);

    //! Errors.
    QColor const ERROR = Qt::red;

    //! Warnings.
    QColor const WARNING = QColor(255, 102, 45);

    //! Colors used in text.
    QColor const REGULAR_TEXT = Qt::black; // Regular text.
    QColor const DISABLED_TEXT = Qt::darkGray; // Text in disabled or uneditable elements.
    QColor const REGULAR_MESSAGE = Qt::blue; // Regular console messages.

    //! Colors used as field backgrounds.
    QColor const REGULAR_FIELD = Qt::white; // Backgrounds and regular fields.
    QColor const MANDATORY_FIELD = QColor(QStringLiteral("LemonChiffon")); // Must be filled.
    QColor const DISABLED_FIELD = QColor(QStringLiteral("whiteSmoke")); // Has no relevance in current context.
    QColor const INVALID_FIELD = QColor(255, 192, 192); // The value is not valid.
    QColor const STRONG_FIELD = QColor(QStringLiteral("gainsboro")); // Has more weight than others.
    QColor const LOGICAL_PORT_FIELD = QColor(QStringLiteral("aliceblue")); // Relates to logical ports.

    //! Colors for languages and generated texts.
    QColor const TEXT_BODY_HIGHLIGHT = QColor::fromRgb(183,225,252); // Background for text bodies such as logic.
    QColor const FUNCTION_HIGHLIGHT = QColor(QStringLiteral("darkviolet")); // Functions().
    QColor const PREPROCESSOR_HIGHLIGHT = QColor(QStringLiteral("chocolate")); // Preprocessor #macros.
    QColor const STRING_HIGHLIGHT = QColor(Qt::gray); // "Character strings."

    //! Colors used in design diagram interconnections.
    QColor const REGULAR_CONNECTION = Qt::black; // Most connections.
    QColor const BROKEN_CONNECTION = ERROR; // Broken/invalid connections.
    QColor const COM_CONNECTION = QColor(49, 132, 61); // COM connections.
    QColor const CONNECTION_UNDERCROSSING = QColor(160, 160, 160);  // Used for crossing interconnections.
    QColor const UNKNOWN_CONNECTION = QColor(180, 180, 180); // Connections unfitting to other categories.

    //! Colors corresponding each available interface mode.
    QColor const MASTER_INTERFACE(0x32,0xcb,0xcb); 
    QColor const SLAVE_INTERFACE(0x32,0x99,0x64);
    QColor const MIRROREDMASTER_INTERFACE(0xcb,0xfd,0xfd);
    QColor const MIRROREDSLAVE_INTERFACE(0x00,0xfd,00);
    QColor const SYSTEM_INTERFACE(165,74,170);
    QColor const MIRROREDSYSTEM_INTERFACE(201,189,233);
    QColor const MONITOR_INTERFACE(0xfd,0xfd,0xfd);
    QColor const INVALID_INTERFACE = ERROR; // The interface is somehow invalid, eg. lacks a mode or type.
    QColor const ENDPOINT_TYPE_API = QColor(0xcb,0xfd,0xfd); // The interface is for API connections.

    //! Colors used in component instances.
    QColor const MISSING_COMPONENT = QColor(240, 180, 180); // The component of the instance does not exist.
    QColor const DRAFT_COMPONENT = QColor(217, 217, 217); // The instance is a draft for a new component.
    QColor const HW_BUS_COMPONENT = QColor(0xce,0xdf,0xff); // The component of the instance is a bus or a bridge.
    QColor const HW_COMPONENT = QColor(0xa5,0xc3,0xef); // The component of the instance is hardware component.
    QColor const SW_COMPONENT = QColor(0xc5,0xff, 0xab); // The component of the instance is software component.

    //! Other colors used in design.
    QColor const DIAGRAM_SELECTION = Qt::blue; // Selections in design diagrams.
    QColor const DIAGRAM_ALLOWED_INTERFACE = Qt::magenta; // Highlights allowed interface for a connection.
    QColor const DIAGRAM_COLUMN_HEADER = QColor(210, 210, 210); // Background color for column headers.
    QColor const STICKY_NOTE_BACKGROUND = QColor(QStringLiteral("lemonChiffon")); // Sticky note background

    //! Colors used for memory/address sections in the memory designer.
    QColor const MEMORY_BLOCK = QColor(0xd6, 0xba, 0xff);
    QColor const ADDRESS_SEGMENT = QColor(0xc6,0xef,0xd7);
    QColor const ADDRESS_SEGMENT_UNSEGMENTED = QColor(217, 217, 217);

    QColor const MEM_MAP_COLOR =  HW_COMPONENT;
    QColor const ADDR_BLOCK_COLOR = QColor(194, 214, 244);
    QColor const REGISTER_FILE_COLOR = QColor(203, 219, 244);
    QColor const REGISTER_COLOR = QColor(216, 229, 248);
    QColor const FIELD_COLOR = QColor(235, 241, 251);

    //! Colors used in dependency analysis.
    QColor const DEPENDECY_ADDED = QColor(0, 222, 0); // Added dependency.
    QColor const DEPENDECY_COVERAGE = QColor(0, 158, 255); // Used to illustrate the extent of dependencies.
    QColor const ROW_SEPARATOR = QColor(200, 200, 200); // Used to separate rows on a list.
}

//-----------------------------------------------------------------------------

#endif // KACTUSCOLORS_H
