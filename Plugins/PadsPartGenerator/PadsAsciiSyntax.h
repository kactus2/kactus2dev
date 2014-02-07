//-----------------------------------------------------------------------------
// File: PadsAsciiSyntax.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 6.11.2013
//
// Description:
// Syntax description for PADS ascii files.
//-----------------------------------------------------------------------------

#ifndef PADSASCIISYNTAX_H
#define PADSASCIISYNTAX_H

#include <QRegExp>
#include <QString>

namespace PadsAsciiSyntax {
    
    // All ascii files end with the following expression.
    const QString PADS_FILE_END = "*END*";

    // General separator in ascii files is a space.
    const char SEPARATOR = ' ';

    //-----------------------------------------------------------------------------
    // PADS Part file.
    //-----------------------------------------------------------------------------
    
    // PADS Part file header.
    const QString PART_FILE_BEGIN = "*PADS-LIBRARY-PART-TYPES-V9*";    
    
    // Supported units.
    const QString IMPERIAL = "I";
    const QString METRIC = "M";

    // First header line in PADS part.    
    const QString PART_HEADER_1ST_LINE = "name pcbdecals u logfam attrs gates sigpins pinmap flag";
    const QRegExp PART_HEADER_1ST_EXP = 
        QRegExp("(\\w{1,40}) (\\w{1,40}(?:[:]\\w{1,40}){0,15}) (\\w) (\\w{3}) \\d+ \\d+ \\d+ \\d+ \\d{1,3}");

    // Positions of header fields.
    enum partHeaderPos
    {
        NAME = 0,
        DECALS,
        UNITS,
        LOGFAMILY,
        NUM_ATTRS,
        NUM_GATES,
        NUM_SIGNPINS,
        NUM_PINMAPS,
        FLAG
    };

    // Second header line in PADS part.
    const QString PART_HEADER_2ND_LINE = "TIMESTAMP year.month.day.hour.minute.second";
    const QRegExp PART_HEADER_2ND_EXP = 
        QRegExp("TIMESTAMP \\d{4}[.]\\d{1,2}[.]\\d{1,2}[.]\\d{1,2}[.]\\d{1,2}[.]\\d{1,2}", Qt::CaseInsensitive);

    // Part attribute line.
    const QString ATTRIBUTE_DECLARATION = "name value";
    const QRegExp ATTRIBUTE_EXP = QRegExp("\"[^\"]{1,255}\"(([^\\r\\n])*)");

    // Positions of fields in attributes.
    enum attrPos
    {
        ATTRNAME = 0,
        VALUE
    };

    // Gate declaration for normal parts.
    const QString PART_GATE_DECLARATION = "GATE decals pins gateswap";
    const QRegExp PART_GATE_EXP = QRegExp("GATE (\\d) (\\d+) (\\d)", Qt::CaseInsensitive);    
    const QRegExp PART_GATE_NAME_EXP = QRegExp("(\\w{1,40}([:]\\w{1,40}){0,3})", Qt::CaseInsensitive);

    // Positions of gate fields in normal part.
    enum gatePos
    {        
        NUM_DECALS = 1,
        NUM_PINS,
        GATESWAP        
    };

    // Pin declaration for normal part gates.
    const QString PART_GATE_PIN = "pinnumber pinswap pintype pinname";
    const QRegExp PART_GATE_PIN_EXP = QRegExp("(\\w{1,7}) (\\d) ([S|B|C|O|T|L|Z|P|G|U])( \\w{1,40})?", 
        Qt::CaseInsensitive);

    // Positions of gate pin fields.
    enum pinPos
    {        
        PINNUMBER = 0,        
        PINSWAP,
        PINTYPE,
        PINNAME
    };
 
    //-----------------------------------------------------------------------------
    // PADS CAE decal file.
    //-----------------------------------------------------------------------------

    // PADS CAE decal header.
    const QString CAE_FILE_BEGIN = "*PADS-LIBRARY-SCH-DECALS-V9*";

    // First line of CAE header.
    const QString CAE_HEADER_1ST_LINE = "name x y pnhgt pnwid pnmhgt pnmwid labels pieces txt terminals vis";

    // Positions of CAE header fields.
    enum caeHeaderPos
    {        
        CAENAME = 0,        
        CAE_POS_X,
        CAE_POS_Y,
        PINNUMBER_HEIGHT,
        PINNUMBER_WIDTH,
        PINNAME_HEIGHT,
        PINNAME_WIDTH,
        NUM_LABELS,
        NUM_PIECES,
        NUM_TXT,
        NUM_TERMINALS,
        VISIBILITY
    };

    // Default font definition.
    const QString DEFAULT_FONT = "\"Default Font\"";

    // Drawing symbol for simple rectangle.
    const QString CAE_RECT_PIECE = "CLOSED 5 10 0 -1";
    
    // Text label definition.
    const QString CAE_LABEL = "x y rotation justification height width fontinfo";

    // Position of text label fields.
    enum caeLabelPos
    {                
        LABEL_POS_X = 0,
        LABEL_POS_Y,
        LABEL_ROTATION,
        JUSTIFICATION,
        LABEL_TEXT_HEIGHT,
        LABEL_TEXT_WIDTH,
        LABEL_FONTINFO
    };

    // Free text item defintion.
    const QString TEXT_ITEM = "x y rotation layer height width mirror just drwnum field fontinfo";

    // Positions of free text fields.
    enum textItemPos
    {                
        TEXT_POS_X = 0,
        TEXT_POS_Y,
        TEXT_ROTATION,
        LAYER,        
        TEXT_HEIGHT,
        TEXT_WIDTH,
        TEXT_MIRRORING,
        TEXT_JUSTIFICATION,
        PCB_DRAW_NUMBER,
        FIELD_FLAG,
        TEXT_FONTINFO
    };

    // CAE terminal definition. Preceded by 'T' without space.
    const QString CAE_TERMINAL = "x y rtn xym pnx pny pnrtn pnjust pnmx pnmy pnmrtn pnmjust pindecal";
    
    // Positions of CAE terminal fields.
    enum terminalPos
    {                
        TERMINAL_POS_X = 0,
        TERMINAL_POS_Y,
        TERMINAL_ROTATION,
        TERMINAL_MIRROR,
        TERMINAL_PINNUM_X,
        TERMINAL_PINNUM_Y,
        TERMINAL_PINNUM_ROTATION,
        TERMINAL_PINNUM_JUSTIFICATION,
        TERMINAL_PINNAME_X,
        TERMINAL_PINNAME_Y,
        TERMINAL_PINNAME_ROTATION,
        TERMINAL_PINNAME_JUSTIFICATION,
        TERMINAL_PINNAME
    };

    // CAE terminal pin property label definition. Preceded by 'P' without space.
    const QString CAE_PIN_PROPERTY = "plx ply plrtn pljust nlx nly nlrtn nljust pflags";

    // Positions of pin property fields.
    enum pinPropertyPos
    {                
        PINPROPERTY_POS_X = 0,
        PINPROPERTY_POS_Y,
        PINPROPERTY_ROTATION,
        PINPROPERTY_JUSTIFICATION,
        NETNAME_POS_X,
        NETNAME_POS_Y,
        NETNAME_ROTATION,
        NETNAME_JUSTIFICATION,
        PINPROPERTY_FLAGS
    };

    //-----------------------------------------------------------------------------
    // PADS PCB decal file.
    //-----------------------------------------------------------------------------

    // PADS PCB decal header.
    // const QString PCB_DECAL_HEADER = "name u x y attrs labels pieces txt terminals stacks maxlayers"
    const QRegExp PCB_DECAL_HEADER_EXP = QRegExp("(\\w+) \\w -?\\d+ -?\\d+ \\d+ \\d+ \\d+ \\d+ \\d+ \\d+ \\d");

}

#endif // PADSASCIISYNTAX_H