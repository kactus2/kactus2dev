//-----------------------------------------------------------------------------
// File: Utilities.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 09.04.2019
//
// Description:
// Declarations for general purpose functions in expressions.
//-----------------------------------------------------------------------------

#include "utilities.h"

//-----------------------------------------------------------------------------
// Function: ExpressionUtilities::findMatchingEndParenthesis()
//-----------------------------------------------------------------------------
int ExpressionUtilities::findMatchingEndParenthesis(QString const& equation, int parenthesesStart,
    QString commentSyntax)
{
    QRegularExpression parenthesesExpression("[()]");
    QRegularExpression commentExpression(commentSyntax);

    int position = parenthesesStart + 1;
    QRegularExpressionMatch commentMatch = commentExpression.match(equation, position);

    int depth = 1;
    while (depth > 0)
    {
        position = equation.indexOf(parenthesesExpression, position);

        if (commentMatch.hasMatch() && position > commentMatch.capturedStart())
        {
            position = commentMatch.capturedEnd();
            commentMatch = commentExpression.match(equation, position);
            continue;
        }

        if (parenthesesExpression.match(equation.at(position)).captured() == "(")
        {
            depth++;
        }
        else
        {
            depth--;
        }
        position++;
    }

    return position - 1;
}
