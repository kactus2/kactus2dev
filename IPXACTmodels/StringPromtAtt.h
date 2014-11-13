//-----------------------------------------------------------------------------
// File: StringPromtAtt.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 12.11.2014
//
// Description:
// Common elements of the IP-XACT element spirit:string.promt.att.
//-----------------------------------------------------------------------------

#ifndef STRINGPROMPTATT_H
#define STRINGPROMPTATT_H

#include <QString>

//-----------------------------------------------------------------------------
//! The common elements of spirit:string.promt.att. element in IP-XACT specification.
//-----------------------------------------------------------------------------
namespace StringPromptAtt
{
    const QString VALID_BOOL_VALUE("true|false");

    const QString VALID_LONG_VALUE("[+-]?(0x|0X|#)?[0-9a-fA-F]+[kmgtKMGT]?");

    const QString VALID_BITSTRING_VALUE("(\")?[01]+\\1");

    const QString VALID_FLOAT_VALUE("[+-]?\\d+([.eE][+-]?\\d+)?");
};

#endif // STRINGPROMPTATT_H
