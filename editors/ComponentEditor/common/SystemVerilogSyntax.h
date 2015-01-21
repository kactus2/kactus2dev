//-----------------------------------------------------------------------------
// File: SystemVerilogSyntax.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 11.12.2014
//
// Description:
// Common elements of SystemVerilog.
//-----------------------------------------------------------------------------

#include <QString>

namespace SystemVerilogSyntax
{
    const QString SIZE = "[1-9]?[0-9]*";
    const QString SIGNED = "[sS]?";

    const QString DECIMAL_NUMBER = "(?:" + SIZE + "'" + SIGNED + "[dD]?)?[0-9_]+";
    const QString HEXADECIMAL_NUMBER = SIZE + "'" + SIGNED + "[hH][0-9a-fA-F_]+";
    const QString OCTAL_NUMBER = SIZE + "'" + SIGNED + "[oO][0-7_]+";
    const QString BINARY_NUMBER = SIZE + "'" + SIGNED + "[bB][01_]+";

    const QString INTEGRAL_NUMBER("[+-]?(?:" + HEXADECIMAL_NUMBER + "|" +
        OCTAL_NUMBER + "|" + BINARY_NUMBER + "|" + DECIMAL_NUMBER + ")");

    const QString STRING_LITERAL("\".*\"");
}
