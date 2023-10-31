//-----------------------------------------------------------------------------
// File: ExpressionParser.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 04.12.2014
//
// Description:
// Interface for expression parsers.
//-----------------------------------------------------------------------------

#ifndef EXPRESSIONPARSER_H
#define EXPRESSIONPARSER_H

#include <QString>
#include <QPair>

//-----------------------------------------------------------------------------
//! Interface for expression parsers.
//-----------------------------------------------------------------------------
class ExpressionParser 
{
public:

	//! The destructor.
    virtual ~ExpressionParser() = default;

    /*!
     *  Parses an expression to decimal number.
     *
     *      @param [in]  expression         The expression to parse.
     *      @param [out] validExpression    Set to true, if the parsing was successful, otherwise false.
     *
     *      @return The decimal value of the evaluated expression.
     */
    virtual QString parseExpression(QStringView expression, bool* validExpression = nullptr) const = 0;

    /*!
     *  Checks if the given expression is a plain value and does not need evaluation.
     *
     *      @param [in] expression   The expression to check.
     *
     *      @return True, if the expression is a plain value, otherwise false.
     */
    virtual bool isPlainValue(QStringView expression) const = 0;

    /*!
     *  Finds the common base in the expression.
     *
     *      @param [in] expression   The expression to search in.
     *
     *      @return The common base for the expression.
     */
    virtual int baseForExpression(QStringView expression) const = 0;
};

#endif // EXPRESSIONPARSER_H
