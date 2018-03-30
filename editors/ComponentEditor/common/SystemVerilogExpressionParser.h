//-----------------------------------------------------------------------------
// File: SystemVerilogExpressionParser.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 25.11.2014
//
// Description:
// Parser for SystemVerilog expressions.
//-----------------------------------------------------------------------------

#ifndef SYSTEMVERILOGEXPRESSIONPARSER_H
#define SYSTEMVERILOGEXPRESSIONPARSER_H

#include "ExpressionParser.h"

#include <QString>
#include <QMap>

//-----------------------------------------------------------------------------
//! Parser for SystemVerilog expressions.
//-----------------------------------------------------------------------------
class SystemVerilogExpressionParser : public ExpressionParser
{
public:

	//! The constructor.
	SystemVerilogExpressionParser();

	//! The destructor.
	virtual ~SystemVerilogExpressionParser();

    /*!
     *  Parses an expression to decimal number.
     *
     *      @param [in]  expression         The expression to parse.
     *      @param [out] validExpression    Set to true, if the parsing was successful, otherwise false.
     *
     *      @return The decimal value of the evaluated expression.
     */
    virtual QString parseExpression(QString const& expression, bool* validExpression = nullptr) const;

    /*!
     *  Check if the given expression is an array.
     *
     *      @param [in] expression   The expression to check.
     *
     *      @return True, if the expression is an array, otherwise false.
     */
    virtual bool isArrayExpression(QString const& expression) const;

    /*!
     *  Checks if the given expression is a plain value and does not need evaluation.
     *
     *      @param [in] expression   The expression to check.
     *
     *      @return True, if the expression is a plain value, otherwise false.
     */
    virtual bool isPlainValue(QString const& expression) const;

    /*!
     *  Finds the common base in the expression.
     *
     *      @param [in] expression   The expression to search in.
     *
     *      @return The common base for the expression.
     */
    virtual int baseForExpression(QString const& expression) const;

protected:
     
    /*!
     *  Parses a constant number to a real number.
     *
     *      @param [in] constantNumber   The constant to parse.
     *
     *      @return The real value of the constant.
     */
    virtual qreal parseConstantToDecimal(QString const& constantNumber) const;
    
    QString parseConstant(QString const& constantNumber) const;

    virtual bool isSymbol(QString const& expression) const;

    virtual QString findSymbolValue(QString const& expression) const;

private:

	// Disable copying.
	SystemVerilogExpressionParser(SystemVerilogExpressionParser const& rhs);
	SystemVerilogExpressionParser& operator=(SystemVerilogExpressionParser const& rhs);

    QStringList toRPN(QString const& expression) const;

    QString solveRPN(QStringList const& rpn, bool* validExpression) const;

    /*!
     *  Checks if the given expression is a string.
     *
     *      @param [in] expression   The expression to check.
     *
     *      @return True, if the expression is a string, otherwise false.
     */
    bool isStringLiteral(QString const &expression) const;

    /*!
     *  Checks if the given expression is a numeric literal.
     *
     *      @param [in] expression   The expression to check.
     *
     *      @return True, if the expression is a literal, otherwise false.
     */
    bool isLiteral(QString const& expression) const;

    /*!
     *  Checks if the given token is a unary operator.
     *
     *      @param [in] expression   The token to check.
     *
     *      @return True, if the token is a unary operator, otherwise false.
     */
    bool isUnaryOperator(QString const& token) const;

    /*!
     *  Checks if the given token is a binary operator.
     *
     *      @param [in] expression   The token to check.
     *
     *      @return True, if the token is a binary operator, otherwise false.
     */
    bool isBinaryOperator(QString const& token) const;

    /*!
     *  Solves a binary operation.
     *
     *      @param [in] operation   The operation to solve.
     *      @param [in] leftTerm    The first term of the operation.
     *      @param [in] rightTerm   The second term of the operation.
     *
     *      @return The result of the operation.
     */
    QString solveBinary(QString const& operation, QString const& leftTerm, QString const& rightTerm) const;

    /*!
     *  Solves a binary operation.
     *
     *      @param [in] operation    The operation to solve.
     *      @param [in] term         The term for the operation.
     *
     *      @return The result of the operation.
     */
    QString solveUnary(QString const& operation, QString const& term) const;

    /*!
     *  Solves the SystemVerilog $clog2 function.
     *
     *      @param [in] value   The value for which the function is called.
     *
     *      @return The solved value.
     */
    QString solveClog2(QString const& value) const;
    /*!
     *  Get the precision used from the terms.
     *
     *      @param [in] firstTerm   The first term of the operation.
     *      @param [in] secondTerm  The second term of the operation.
     *
     *      @return The precision of the decimal used in the terms of the operation.
     */
    int getDecimalPrecision(QString const& firstTerm, QString const& secondTerm) const;

    /*!
     *  Get the base for a given number.
     *
     *      @param [in] constantNumber  The selected number.
     *
     *      @return The base for the selected number. Either 2, 8, 10 or 16.
     */
    int getBaseForNumber(QString const& constantNumber) const;

    /*!
     *  Converts the base format to the base number e.g. h to 16.
     *
     *      @param [in] baseFormat   The format to convert.
     *
     *      @return The base number for the format.
     */
    int baseForFormat(QString const& baseFormat) const;

    QMap<QString, int> op_precedence;
};

#endif // SYSTEMVERILOGEXPRESSIONPARSER_H
