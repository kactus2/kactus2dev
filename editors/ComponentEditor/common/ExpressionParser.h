//-----------------------------------------------------------------------------
// File: ExpressionParser.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 04.12.2014
//
// Description:
// Interface for expression parsers.
//-----------------------------------------------------------------------------

#ifndef EXPRESSIONPARSER_H
#define EXPRESSIONPARSER_H

#include <QString>

//-----------------------------------------------------------------------------
//! Interface for expression parsers.
//-----------------------------------------------------------------------------
class ExpressionParser 
{
public:

	//! The destructor.
    virtual ~ExpressionParser() {};

    /*!
     *  Parses an expression to decimal number.
     *
     *      @param [in] expression   The expression to parse.
     *
     *      @return The decimal value of the constant.
     */
    virtual QString parseExpression(QString const& expression) const = 0;

    /*!
     *  Checks if the given expression is not valid for parsing.
     *
     *      @param [in] expression   The expression to check.
     *
     *      @return True, if the expression is not in valid format, otherwise false.
     */
    virtual bool isValidExpression(QString const& expression) const = 0;

};

#endif // EXPRESSIONPARSER_H
