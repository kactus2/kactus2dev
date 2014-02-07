//-----------------------------------------------------------------------------
// File: QuartusPinSyntax.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 19.11.2013
//
// Description:
// Syntax description for Quartus .pin-files.
//-----------------------------------------------------------------------------

#ifndef QUARTUSPINSYNTAX_H
#define QUARTUSPINSYNTAX_H

namespace QuartusPinSyntax
{

    // Delimiter for properties.
    const char DELIMITER = ':';

    //! Whitespace without line end.
    const QString SPACE = "[ \\t]*";    

    // Each pin assignment is described on its own line using the following syntax:
    // Pin Name/Usage : Location : Dir. : I/O Standard : Voltage : I/O Bank  : User Assignment    
    const QRegExp pinName = QRegExp("[^ :]+(?: / [^ :]+)*", Qt::CaseInsensitive);
    const QRegExp location = QRegExp("\\w+", Qt::CaseInsensitive);
    const QRegExp direction = QRegExp("input|output|bidir|power|gnd", Qt::CaseInsensitive);
    const QRegExp ioStandard = QRegExp("[^ :]+(?:" + SPACE + "[^ :]+)*", Qt::CaseInsensitive);
    const QRegExp voltage = QRegExp("\\d+\\.\\d+" + SPACE +"V", Qt::CaseInsensitive);
    const QRegExp ioBank = QRegExp("\\w+", Qt::CaseInsensitive);
    const QRegExp userAssignment = QRegExp("Y|N", Qt::CaseInsensitive);

    // Pin definition line.
    const QRegExp pinDef = QRegExp("(" + pinName.pattern() + ")" + SPACE + DELIMITER + SPACE +        
                                   "(" + location.pattern() + ")" + SPACE + DELIMITER + SPACE +
                                   "(" + direction.pattern() + ")?" + SPACE + DELIMITER + SPACE +
                                   "(" + ioStandard.pattern() + ")?" + SPACE + DELIMITER + SPACE +
                                   "(" + voltage.pattern() + ")?" + SPACE + DELIMITER + SPACE +
                                   "(" + ioBank.pattern() + ")?" + SPACE + DELIMITER + SPACE +
                                   "(" + userAssignment.pattern() + ")?" + SPACE +"$", Qt::CaseInsensitive);

    // Property capture positions.
    enum propertyPos
    {
        PINUSAGE = 1,
        LOCATION,
        DIRECTION,
        IOSTANDARD,
        VOLTAGE,
        IOBANK,
        USERASSIGNMENT,
        PROPERTY_COUNT
    };
}

#endif // QUARTUSPINSYNTAX_H