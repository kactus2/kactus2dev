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
    //! Color of errors.
    QColor const ERROR = Qt::red;

    //! Color of warnings.
    QColor const WARNING = QColor(255, 102, 45);

    //! Color for regular text
    QColor const REGULAR_TEXT = Qt::black;

    //! Color for text in disabled elements
    QColor const DISABLED_TEXT = Qt::lightGray;

    //! Color of regular console messages.
    QColor const REGULAR_MESSAGE = Qt::blue;

    //! Color used for backgrounds and regular fields.
    QColor const REGULAR_FIELD = Qt::white;

    //! Color used for mandatory fields.
    QColor const MANDATORY_FIELD = QColor(QStringLiteral("LemonChiffon"));

    //! Color used for ineffective fields.
    QColor const DISABLED_FIELD = QColor(QStringLiteral("whiteSmoke"));

    //! Color used for invalid fields.
    QColor const INVALID_FIELD = QColor(255, 192, 192);

    //! Color used for selections in design diagrams.
    QColor const DIAGRAM_SELECTION = Qt::blue;

    //! Color used for diagram connections.
    QColor const REGULAR_CONNECTION = Qt::black;

    //! Color used for broken/invalid connections in design diagrams.
    QColor const BROKEN_CONNECTION = ERROR;

    //! Special color used for COM connections.
    QColor const COM_CONNECTION = QColor(49, 132, 61);

    //! Color for under crossings of the connections.
    QColor const CONNECTION_UNDERCROSSING = QColor(160, 160, 160);

    //! Color for connections unfitting to other categories.
    QColor const UNKNOWN_CONNECTION = QColor(180, 180, 180);

    //! Color used for allowed visualization in design diagrams.
    QColor const DIAGRAM_ALLOWED_INTERFACE = Qt::magenta;

    //! Background color for column headers.
    QColor const DIAGRAM_COLUMN_HEADER = QColor(210, 210, 210);

    //! Color for catalog header.
    QColor const CATALOG_HEADER = QColor(QStringLiteral("gainsboro"));

    //! Color for sticky note background
    QColor const STICKY_NOTE_BACKGROUND = QColor(QStringLiteral("lemonChiffon"));

    //! Color for logical port back ground.
    QColor const LOGICAL_PORT_BACKGROUND = QColor(QStringLiteral("aliceblue"));

    //! Colors used for component.
    QColor const MISSING_COMPONENT = QColor(0xe8, 0xc5, 0xc5);
    QColor const DRAFT_COMPONENT = QColor(217, 217, 217);
    QColor const HW_BUS_COMPONENT = QColor(0xce,0xdf,0xff);
    QColor const HW_COMPONENT = QColor(0xa5,0xc3,0xef);
    QColor const SW_COMPONENT = QColor(0xc5,0xff, 0xab);

    //! Colors used for interfaces in design.
    QColor const INVALID_INTERFACE = ERROR;
    QColor const MASTER_INTERFACE(0x32,0xcb,0xcb);
    QColor const SLAVE_INTERFACE(0x32,0x99,0x64);
    QColor const MIRROREDMASTER_INTERFACE(0xcb,0xfd,0xfd);
    QColor const MIRROREDSLAVE_INTERFACE(0x00,0xfd,00);
    QColor const SYSTEM_INTERFACE(165,74,170);
    QColor const MIRROREDSYSTEM_INTERFACE(201,189,233);
    QColor const MONITOR_INTERFACE(0xfd,0xfd,0xfd);

    QColor const ENDPOINT_TYPE_API = QColor(0xcb,0xfd,0xfd);

    //! Colors used for memory/address sections in the memory designer.
    QColor const MEMORY_BLOCK = QColor(0xd6, 0xba, 0xff);
    QColor const ADDRESS_SEGMENT = QColor(0xc6,0xef,0xd7);
    QColor const ADDRESS_SEGMENT_UNSEGMENTED = QColor(217, 217, 217);

    QColor const MEM_MAP_COLOR =  HW_COMPONENT;
    QColor const ADDR_BLOCK_COLOR = QColor(194, 214, 244);
    QColor const REGISTER_COLOR = QColor(216, 229, 248);
    QColor const FIELD_COLOR = QColor(235, 241, 251);

}

//-----------------------------------------------------------------------------

#endif // KACTUSCOLORS_H
