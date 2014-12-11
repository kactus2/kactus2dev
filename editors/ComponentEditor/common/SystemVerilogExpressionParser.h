//-----------------------------------------------------------------------------
// File: SystemVerilogExpressionParser.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
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
     *      @param [in] expression   The expression to parse.
     *
     *      @return The decimal value of the constant.
     */
    virtual QString parseExpression(QString const& expression) const;

    /*!
     *  Checks if the given expression is valid for parsing.
     *
     *      @param [in] expression   The expression to check.
     *
     *      @return True, if the expression is in valid format, otherwise false.
     */
    virtual bool isValidExpression(QString const& expression) const;


private:

	// Disable copying.
	SystemVerilogExpressionParser(SystemVerilogExpressionParser const& rhs);
	SystemVerilogExpressionParser& operator=(SystemVerilogExpressionParser const& rhs);

    /*!
     *  Checks if the given expression is a string.
     *
     *      @param [in] expression   The expression to check.
     *
     *      @return True, if the expression is a string, otherwise false.
     */
    bool isStringLiteral(QString const &expression) const;

    /*!
     *  Splits the given expression to string list with terms and operations as separate items.
     *
     *      @param [in] expression   The expression to split.
     *
     *      @return The separated list.
     */
    QStringList toStringList(QString const& expression) const;

    /*!
     *  Splits the given operand to string list with terms and parentheses as separate items.
     *
     *      @param [in] operand   The operand to split.
     *
     *      @return The separated list.
     */
    QStringList parseLiteralAndParentheses(QString const& operand) const;

    /*!
     *  Solves function calls to math functions in a given equation.
     *
     *      @param [in] equation   The equation to solve.
     *
     *      @return Equation where the function calls have been replaced with the solved results
     *              and without the parentheses.
     */
    QStringList solveMathFuctions(QStringList const& equation) const;
    
    /*!
     *  Solves the SystemVerilog $clog2 function.
     *
     *      @param [in] value   The value for which the function is called.
     *
     *      @return The solved value.
     */
    QString solveClog2(QString const& value) const;

    /*!
     *  Solves expressions in parentheses in a given equation.
     *
     *      @param [in] equation   The equation to solve.
     *
     *      @return Equation where the expressions in parenthesis have been replaced with the solved results
     *              and without the parentheses.
     */
    QStringList solveExpressionsInParentheses(QStringList const& equation) const;

    /*!
     *  Finds the matching end parenthesis in the given equation for the opening parenthesis in given position.
     *
     *      @param [in] equation            The equation to search for ending parenthesis.
     *      @param [in] parenthesesStart    The position of the opening parenthesis.
     *
     *      @return The position of the ending parenthesis.
     */
    int findMatchingEndParenthesis(QStringList const& equation, int parenthesesStart) const;
      
    /*!
     *  Solves power operations in a given equation.
     *
     *      @param [in] equation   The equation to solve.
     *
     *      @return Equation where the terms and operators have been replaced with the result.
     */  
    QStringList solvePower(QStringList const& equation) const;

    /*!
     *  Solves multiply and division operations in a given equation.
     *
     *      @param [in] equation   The equation to solve.
     *
     *      @return Equation where the terms and operators have been replaced with the result.
     */
    QStringList solveMultiplyAndDivide(QStringList const& equation) const;

    /*!
    *  Solves addition and subtraction operations in given equation.
    *
    *      @param [in] equation   The equation to solve.
    *
    *      @return Equation where the terms and operators have been replaced with the result.
    */
    QStringList solveAdditionAndSubtraction(QStringList const& equation) const;

    /*!
    *  Solves binary operations in given equation.
    *
    *      @param [in] equation         The equation to solve.
    *      @param [in] binaryOperator   An expression for finding the operator to solve.
    *
    *      @return Equation where the terms and operators have been replaced with the result.
    */
    QStringList solveBinaryOperationsFromLeftToRight(QStringList const& equation, 
        QRegExp const& binaryOperator) const;

    /*!
    *  Solves a binary operation.
    *
    *      @param [in] firstTerm   The first term of the operation.
    *      @param [in] operation   The operation to solve.
    *      @param [in] secondTerm  The second term of the operation.
    *
    *      @return The result of the operation.
    */
    QString solve(QString const& firstTerm, QString const& operation, QString const& secondTerm) const;
        
    /*!
     *  Parses a constant number to a real number.
     *
     *      @param [in] constantNumber   The constant to parse.
     *
     *      @return The real value of the constant.
     */
    qreal parseConstantToDecimal(QString const& constantNumber) const;
    int getBaseForNumber(QString const& constantNumber) const;

    /*!
     *  Converts the base format to the base number e.g. h to 16.
     *
     *      @param [in] baseFormat   The format to convert.
     *
     *      @return The base number for the format.
     */
    int baseForFormat(QString const& baseFormat) const;
    bool isLiteral(QString const& expression) const;
};

#endif // SYSTEMVERILOGEXPRESSIONPARSER_H
