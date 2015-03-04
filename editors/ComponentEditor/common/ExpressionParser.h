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
     *  Checks if the given expression is valid for parsing.
     *
     *      @param [in] expression   The expression to check.
     *
     *      @return True, if the expression is in valid format, otherwise false.
     */
    virtual bool isValidExpression(QString const& expression) const = 0;
    
    /*!
     *  Checks if the given expression is a plain value and does not need evaluation.
     *
     *      @param [in] expression   The expression to check.
     *
     *      @return True, if the expression is a plain value, otherwise false.
     */
    virtual bool isPlainValue(QString const& expression) const = 0;

    /*!
     *  Check if the given expression is an array.
     *
     *      @param [in] expression   The expression to check.
     *
     *      @return True, if the expression is an array, otherwise false.
     */
    virtual bool isArrayExpression(QString const& expression) const = 0;

    /*!
     *  Finds the common base in the expression.
     *
     *      @param [in] expression   The expression to search in.
     *
     *      @return The common base for the expression.
     */
    virtual int baseForExpression(QString const& expression) const = 0;
};

#endif // EXPRESSIONPARSER_H
