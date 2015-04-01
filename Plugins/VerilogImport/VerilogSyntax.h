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

#ifndef VERILOGSYNTAX_H
#define VERILOGSYNTAX_H

#include <Plugins/common/HDLmath.h>

#include <QRegularExpression>

namespace VerilogSyntax
{
    const QString TERM = "(?:[`]?\\w+)(?:\\s*(?:" + HDLmath::OPERATOR + ")\\s*(?:[`]?\\w+))*";

    //! Module begins with module <name> #(<parameters>) (<ports>);.
    const QRegularExpression MODULE_BEGIN("module\\s+(\\w+)\\s*(#\\s*[(].*[)])?\\s*(?=([(][^)][)])?\\s*;)?",
        QRegularExpression::DotMatchesEverythingOption);

    //! Module ends with keyword endmodule.
    const QRegularExpression MODULE_END("endmodule");

    //! Pattern for ranges for e.g. port sizes are given as [left:right].
    const QString RANGE("\\[" + TERM + "\\s*[:]\\s*" + TERM + "\\]");

    /*  Identifiers e.g. port names may contain characters a-z, A-Z, numbers, underscores and dollar signs.
     *  Multiple identifiers declared at once must be comma separated.
     */
    const QString NAMES("[a-zA-Z0-9_$]+(?:\\s*[,]\\s*[a-zA-Z0-9_$]+)*");

    //! Pattern for one-line comments.
    const QString COMMENT("//[ \\t]*([^\\r\\n]*)(?=\\r?\\n|$)");

    //! Single-line comments.
    const QRegularExpression COMMENTLINE("(^|\\r?\\n)[ \\t]*" + COMMENT);

    //! Multiline comments.
    const QRegularExpression MULTILINE_COMMENT("/\\*.*\\*/", 
        QRegularExpression::DotMatchesEverythingOption | QRegularExpression::InvertedGreedinessOption);  

    //! An expression that may an operator or an alphanumeric symbol.
    const QString OPERATION_OR_ALPHANUMERIC("([+*\\(\\)\\{\\}/-])|([']?\\{)|(\\w+)|((\\w+)?'\\w+)");
    
    //! Name + value pair, eg name=value.
    const QString NAME_VALUE = "\\w+\\s*=(\\s*(" + OPERATION_OR_ALPHANUMERIC + "))+";  
}

#endif // VERILOGSYNTAX_H