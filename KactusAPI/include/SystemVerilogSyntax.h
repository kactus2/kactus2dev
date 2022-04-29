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
    const QString SIZE = QStringLiteral("[1-9]?[0-9]*");
    const QString SIGNED = QStringLiteral("[sS]?");

    const QString BOOLEAN_VALUE = QStringLiteral("(?i)true|(?i)false");
    const QString REAL_NUMBER = QStringLiteral("[-+]?[0-9_]+[.][0-9_]+");
    const QString DECIMAL_NUMBER = QStringLiteral("(?:") + SIZE + QStringLiteral("'") + SIGNED + QStringLiteral("[dD]?)?[0-9_]{1,18}");
    const QString HEXADECIMAL_NUMBER = SIZE + QStringLiteral("'") + SIGNED + QStringLiteral("[hH][0-9a-fA-F_]{1,15}");
    const QString OCTAL_NUMBER = SIZE + QStringLiteral("'") + SIGNED + QStringLiteral("[oO][0-7_]{1,20}");
    const QString BINARY_NUMBER = SIZE + QStringLiteral("'") + SIGNED + QStringLiteral("[bB][01_]{1,62}");

    const QString INTEGRAL_NUMBER = QStringLiteral("[+-]?(?:") + HEXADECIMAL_NUMBER + QStringLiteral("|") +
        OCTAL_NUMBER + QStringLiteral("|") + BINARY_NUMBER + QStringLiteral("|") + DECIMAL_NUMBER + QStringLiteral(")");

    const QString STRING_LITERAL = QStringLiteral("\"[^\"]*\"");
}
