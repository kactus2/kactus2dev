//-----------------------------------------------------------------------------
// File: SystemVerilogSyntax.h
//-----------------------------------------------------------------------------
// Project: Kactus2
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

    const QString DECIMAL_NUMBER = "(?:" + SIZE + "'" + SIGNED + "[dD]?)?[0-9_]{1,18}";
    const QString HEXADECIMAL_NUMBER = SIZE + "'" + SIGNED + "[hH][0-9a-fA-F_]{1,15}";
    const QString OCTAL_NUMBER = SIZE + "'" + SIGNED + "[oO][0-7_]{1,20}";
    const QString BINARY_NUMBER = SIZE + "'" + SIGNED + "[bB][01_]{1,62}";

    const QString INTEGRAL_NUMBER("[+-]?(?:" + HEXADECIMAL_NUMBER + "|" +
        OCTAL_NUMBER + "|" + BINARY_NUMBER + "|" + DECIMAL_NUMBER + ")");

    const QString STRING_LITERAL("\".*\"");
}
