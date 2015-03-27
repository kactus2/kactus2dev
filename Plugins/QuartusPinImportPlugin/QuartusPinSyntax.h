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

#include <QString>
#include <QRegularExpression>

namespace QuartusPinSyntax
{
    // Delimiter for properties.
    const char DELIMITER = ':';

    //! Whitespace without line end.
    const QString SPACE = "[ \\t]*";    

    // Each pin assignment is described on its own line using the following syntax:
    // Pin Name/Usage : Location : Dir. : I/O Standard : Voltage : I/O Bank  : User Assignment    
    const QString pinName = "[^ :]+(?: / [^ :]+)*";
    const QString location = "\\w+";
    const QString direction = "input|output|bidir|power|gnd";
    const QString ioStandard = "[^ :]+(?:" + SPACE + "[^ :]+)*";
    const QString voltage = "\\d+\\.\\d+" + SPACE +"V";
    const QString ioBank = "\\w+";
    const QString userAssignment = "Y|N";

    // Pin definition line.
    const QRegularExpression pinDefinition = QRegularExpression("\\b(" + pinName + ")" + SPACE + DELIMITER + SPACE +        
                                   "(" + location + ")" + SPACE + DELIMITER + SPACE +
                                   "(" + direction + ")?" + SPACE + DELIMITER + SPACE +
                                   "(" + ioStandard + ")?" + SPACE + DELIMITER + SPACE +
                                   "(" + voltage + ")?" + SPACE + DELIMITER + SPACE +
                                   "(" + ioBank + ")?" + SPACE + DELIMITER + SPACE +
                                   "(" + userAssignment + ")?(?=\\s*$)", 
                                   QRegularExpression::CaseInsensitiveOption);

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