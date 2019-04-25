 //-----------------------------------------------------------------------------
 // File: Utilities.h
 //-----------------------------------------------------------------------------
 // Project: Kactus 2
 // Author: Mikko Teuho
 // Date: 09.04.2019
 //
 // Description:
 // Declarations for general purpose functions in expressions.
 //-----------------------------------------------------------------------------

#ifndef EXPRESSIONUTILITIES_H
#define EXPRESSIONUTILITIES_H

#include <QRegularExpression>
#include <QObject>

//-----------------------------------------------------------------------------
//! Declarations for general purpose functions in expressions.
//-----------------------------------------------------------------------------
namespace ExpressionUtilities
{
    /*!
     *  Finds the index of a matching end parenthesis in an equation.
     *
     *      @param [in] equation            The selected equation.
     *      @param [in] parenthesisStart    Index of the start parenthesis.
     *      @param [in] commentSyntax       Syntax for possible comments in the equation.
     *
     *      @return Index of the end parenthesis matching the selected start parenthesis.
     */
    int findMatchingEndParenthesis(QString const& equation, int parenthesesStart,
        QString commentSyntax = "[/]{2}[\\t]*([^\\r\\n]*)(?=\\r?\\n|$)");
}

#endif // EXPRESSIONUTILITIES_H
