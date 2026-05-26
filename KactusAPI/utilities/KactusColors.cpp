//-----------------------------------------------------------------------------
// File: KactusColors.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: 
// Date: 12.02.2026
//
// Description:
// 
//-----------------------------------------------------------------------------

#include "KactusColors.h"

#include <QPalette>

//! Icon and visual design base colors.
QColor KactusColors::KACTUS_BLUE = QColor(0x1b, 0x65, 0xaf);
QColor KactusColors::KACTUS_GREEN = QColor(0x32, 0xbc, 0x8f);

//! Errors.
QColor KactusColors::ERROR_COLOR = Qt::red;

//! Warnings.
QColor KactusColors::WARNING = QColor(255, 102, 45);

QColor KactusColors::DEFAULT_WINDOW_BG = QColor(255, 255, 255);

QColor KactusColors::TABLE_GRIDLINE = Qt::lightGray; // Gridline color used in table editors

QColor KactusColors::DOCK_WIDGET_TITLE_BG = QColor(137, 182, 226); // Default kactus2 blue

//! Colors used in text.
QColor KactusColors::REGULAR_TEXT = Qt::black; // Regular text.
QColor KactusColors::DISABLED_TEXT = Qt::darkGray; // Text in disabled or uneditable elements.
QColor KactusColors::REGULAR_MESSAGE = REGULAR_TEXT; // Regular console messages.

//! Colors used as field backgrounds.
QColor KactusColors::REGULAR_FIELD = Qt::white; // Backgrounds and regular fields.
QColor KactusColors::MANDATORY_FIELD = QColor(QStringLiteral("LemonChiffon")); // Must be filled.
QColor KactusColors::DISABLED_FIELD = QColor(QStringLiteral("whiteSmoke")); // Has no relevance in current context.
QColor KactusColors::INVALID_FIELD = QColor(255, 192, 192); // The value is not valid.
QColor KactusColors::STRONG_FIELD = QColor(QStringLiteral("gainsboro")); // Has more weight than others.
QColor KactusColors::LOGICAL_PORT_FIELD = QColor(QStringLiteral("aliceblue")); // Relates to logical ports.

//! Colors for languages and generated texts.
QColor KactusColors::TEXT_BODY_HIGHLIGHT = QColor::fromRgb(183, 225, 252); // Background for text bodies such as logic.
QColor KactusColors::FUNCTION_HIGHLIGHT = QColor(QStringLiteral("darkviolet")); // Functions().
QColor KactusColors::PREPROCESSOR_HIGHLIGHT = QColor(QStringLiteral("chocolate")); // Preprocessor #macros.
QColor KactusColors::STRING_HIGHLIGHT = QColor(Qt::gray); // "Character strings."

//! Colors used in design diagram interconnections.
QColor KactusColors::REGULAR_CONNECTION = Qt::black; // Most connections.
QColor KactusColors::BROKEN_CONNECTION = ERROR_COLOR; // Broken/invalid connections.
QColor KactusColors::COM_CONNECTION = QColor(49, 132, 61); // COM connections.
QColor KactusColors::CONNECTION_UNDERCROSSING = QColor(160, 160, 160);  // Used for crossing interconnections.
QColor KactusColors::UNKNOWN_CONNECTION = QColor(180, 180, 180); // Connections unfitting to other categories.

//! Colors corresponding each available interface mode.
QColor KactusColors::MASTER_INTERFACE = QColor(0x32, 0xcb, 0xcb);
QColor KactusColors::SLAVE_INTERFACE = QColor(0x32, 0x99, 0x64);
QColor KactusColors::MIRROREDMASTER_INTERFACE = QColor(0xcb, 0xfd, 0xfd);
QColor KactusColors::MIRROREDSLAVE_INTERFACE = QColor(0x00, 0xfd, 00);
QColor KactusColors::SYSTEM_INTERFACE = QColor(165, 74, 170);
QColor KactusColors::MIRROREDSYSTEM_INTERFACE = QColor(201, 189, 233);
QColor KactusColors::MONITOR_INTERFACE = QColor(0xfd, 0xfd, 0xfd);
QColor KactusColors::INVALID_INTERFACE = ERROR_COLOR; // The interface is somehow invalid, eg. lacks a mode or type.
QColor KactusColors::ENDPOINT_TYPE_API = QColor(0xcb, 0xfd, 0xfd); // The interface is for API connections.
QColor KactusColors::ADHOC_PORT = QColor(Qt::black);

//! Colors used in component instances.
QColor KactusColors::MISSING_COMPONENT = QColor(240, 180, 180); // The component of the instance does not exist.
QColor KactusColors::DRAFT_COMPONENT = QColor(217, 217, 217); // The instance is a draft for a new component.
QColor KactusColors::HW_BUS_COMPONENT = QColor(0xce, 0xdf, 0xff); // The component of the instance is a bus or a bridge.
QColor KactusColors::HW_COMPONENT = QColor(0xa5, 0xc3, 0xef); // The component of the instance is hardware component.
QColor KactusColors::SW_COMPONENT = QColor(0xc5, 0xff, 0xab); // The component of the instance is software component.

//! Other colors used in design.
QColor KactusColors::DIAGRAM_SELECTION = Qt::blue; // Selections in design diagrams.
QColor KactusColors::DIAGRAM_ALLOWED_INTERFACE = Qt::magenta; // Highlights allowed interface for a connection.
QColor KactusColors::DIAGRAM_GRID = Qt::gray;
QColor KactusColors::DIAGRAM_COLUMN_HEADER = QColor(160, 193, 226); // Background color for column headers.
QColor KactusColors::STICKY_NOTE_BACKGROUND = QColor(QStringLiteral("lemonChiffon")); // Sticky note background
QColor KactusColors::STICKY_NOTE_TEXT = Qt::black;
QColor KactusColors::ASSOCIATION_LINE = Qt::black;

//! Colors used for memory/address sections in the memory designer.
QColor KactusColors::MEMORY_BLOCK = QColor(0xd6, 0xba, 0xff);
QColor KactusColors::ADDRESS_SEGMENT = QColor(0xc6, 0xef, 0xd7);
QColor KactusColors::ADDRESS_SEGMENT_UNSEGMENTED = QColor(217, 217, 217);

QColor KactusColors::MEM_MAP_COLOR = HW_COMPONENT;
QColor KactusColors::ADDR_BLOCK_COLOR = QColor(194, 214, 244);
QColor KactusColors::REGISTER_FILE_COLOR = QColor(203, 219, 244);
QColor KactusColors::REGISTER_COLOR = QColor(216, 229, 248);
QColor KactusColors::FIELD_COLOR = QColor(235, 241, 251);
QColor KactusColors::SUBSPACE_MAP_COLOR = KactusColors::MIRROREDSYSTEM_INTERFACE;
QColor KactusColors::MEM_ITEM_EXTENSION= QColor("whiteSmoke");

QColor KactusColors::MEMORY_DESIGNER_TEXT = Qt::black;
QColor KactusColors::MEM_DESIGNER_CONNECTION = Qt::black;
QColor KactusColors::MEM_DESIGNER_LOCAL_MAP_CONNECTION = QColor(0, 128, 255);
QColor KactusColors::MEM_DESIGNER_ADDR_SPACE_CONNECTION = QColor(60, 153, 60);

//! Memory visualizer
QColor KactusColors::MEM_GRAPH_ITEM_BORDER = Qt::gray;

//! Colors used in dependency analysis.
QColor KactusColors::DEPENDECY_ADDED = QColor(0, 222, 0); // Added dependency.
QColor KactusColors::DEPENDECY_COVERAGE = QColor(0, 158, 255); // Used to illustrate the extent of dependencies.
QColor KactusColors::ROW_SEPARATOR = QColor(200, 200, 200); // Used to separate rows on a list.

QColor KactusColors::RIBBON_GRADIENT_TOP = QColor(218, 225, 233);
QColor KactusColors::RIBBON_GRADIENT_BOTTOM = QColor(160, 193, 226);
QColor KactusColors::RIBBONGROUP_TITLE_GRADIENT_TOP = QColor(33, 135, 237);
QColor KactusColors::RIBBONGROUP_TITLE_GRADIENT_BOTTOM = QColor(17, 127, 237);
QColor KactusColors::RIBBONGROUP_TITLE_TEXT = Qt::white;

//! Portmap editor alt row color
QColor KactusColors::PORTMAP_ALT_ROW = KactusColors::FIELD_COLOR;

//! Importer plugin colors
QColor KactusColors::Importer::PORT = KactusColors::SW_COMPONENT;
QColor KactusColors::Importer::MODELPARAMETER = KactusColors::HW_BUS_COMPONENT;
QColor KactusColors::Importer::PARAMETER = KactusColors::MEMORY_BLOCK;
QColor KactusColors::Importer::VIEWNAME = QColor("Moccasin");
QColor KactusColors::Importer::INSTANCECOLOR = QColor(201, 82, 251);
