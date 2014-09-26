//-----------------------------------------------------------------------------
// File: VerilogSyntax.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 26.09.2014
//
// Description:
// Common elements of the Verilog language.
//-----------------------------------------------------------------------------

namespace VerilogSyntax
{
    //! Module begins with module <name> #(<parameters>) ( followed by ports.
    const QRegExp MODULE_BEGIN("module\\s+\\w+\\s*(#\\s*[(][^)]*[)])?\\s*[(]", Qt::CaseSensitive);

    //! Module ends with keyword endmodule.
    const QRegExp MODULE_END("endmodule", Qt::CaseSensitive);

    //! Pattern for ranges for e.g. port sizes are given as [left:right].
    const QString RANGE("[\\[]\\w+\\s*[:]\\s*\\w+[\\]]");

    /*  Identifiers e.g. port names may contain characters a-z, A-Z, numbers, underscores and dollar signs.
     *  Multiple identifiers declared at once must be comma separated.
     */
    const QString NAMES("[a-zA-Z0-9_$]+(?:\\s*[,]\\s*[a-zA-Z0-9_$]+)*");

    //! Pattern for single-line comments.
    const QString COMMENT("//[ \\t]*([^\\r\\n]*)(?=\\r?\\n|$)");

    //! Pattern for multiline comments.
    const QRegExp MULTILINE_COMMENT("/\\*.*\\*/");    
}
