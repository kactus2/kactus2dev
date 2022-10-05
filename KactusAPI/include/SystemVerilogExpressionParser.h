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

#include "KactusAPI/KactusAPIGlobal.h"

#include <QString>
#include <QMap>

//-----------------------------------------------------------------------------
//! Parser for SystemVerilog expressions.
//-----------------------------------------------------------------------------
class KACTUS2_API SystemVerilogExpressionParser : public ExpressionParser
{
public:

	//! The constructor.
	SystemVerilogExpressionParser() = default;

	//! The destructor.
    ~SystemVerilogExpressionParser() override = default;

    // Disable copying.
    SystemVerilogExpressionParser(SystemVerilogExpressionParser const& rhs) = delete;
    SystemVerilogExpressionParser& operator=(SystemVerilogExpressionParser const& rhs) = delete;

    /*!
     *  Parses an expression to decimal number.
     *
     *      @param [in]  expression         The expression to parse.
     *      @param [out] validExpression    Set to true, if the parsing was successful, otherwise false.
     *
     *      @return The decimal value of the evaluated expression.
     */
    virtual QString parseExpression(QString const& expression, bool* validExpression = nullptr) const override;

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
    virtual bool isPlainValue(QString const& expression) const override;

    /*!
     *  Finds the common base in the expression.
     *
     *      @param [in] expression   The expression to search in.
     *
     *      @return The common base for the expression.
     */
    virtual int baseForExpression(QString const& expression) const override;

protected:
     
    /*!
     *  Parses a token to a decimal number or string.
     *
     *      @param [in] token   The constant to parse.
     *
     *      @return The decimal value of the constant or the given string.
     */
    QString parseConstant(QString token) const;

    /*!
     *  Checks if the given expression is a symbol e.g. reference.
     *
     *      @param [in] expression   The expression to check.
     *
     *      @return True, if the expression is a symbol, otherwise false.
     */
    virtual bool isSymbol(QString const& expression) const;

    /*!
     *  Finds the value for given symbol.
     *
     *      @param [in] symbol  The symbol whose value to find.
     *
     *      @return The found symbol value.
     */
    virtual QString findSymbolValue(QString const& symbol) const;

    /*!
     *  Finds the base in the symbol.
     *
     *      @param [in] symbol   The symbol whose base to find.
     *
     *      @return The base for the symbol.
     */
    virtual int getBaseForSymbol(QString const& symbol) const;

    /*!
    *  Get the operator precedence value for the given operator.
    *
    *      @param [in] oper  The operator whose precedence to get.
    *
    *      @return The precedence value where bigger value has higher precedence.
    */
    unsigned int operatorPrecedence(QString const& oper) const;

private:

    /*!
     *  Converts the given expression to Reverse Polish Notation (RPN) format.
     *  RPN is used to ensure the operations are calculated in the correct precedence order.
     *
     *      @param [in] expression   The expression to convert.
     *
     *      @return The conversion result.
     */
    QVector<QString> convertToRPN(QString const& expression) const;

    /*!
     *  Solves the given RPN expression.
     *
     *      @param [in]     rpn                The expression to solve.
     *      @param [out]    validExpression    Set to true, if the parsing was successful, otherwise false.
     *
     *      @return The solved result.
     */
    QString solveRPN(QVector<QString> const& rpn, bool* validExpression) const;

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
     *  Checks if the given token is a ternary operator.
     *
     *      @param [in] expression   The token to check.
     *
     *      @return True, if the token is a ternary operator, otherwise false.
     */
    bool isTernaryOperator(QString const& token) const;

    /*!
     *  Solves a Ternary operation.
     *
     *      @param [in] condition   The selecting condition of the ternary operation.
     *      @param [in] trueCase    The value for true condition.
     *      @param [in] falseCase   The value for false condition.
     *
     *      @return The result of the operation.
     */
    QString solveTernary( QString const& condition, QString const& trueCase, QString const& falseCase) const;

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
     *  Solves the SystemVerilog $sqrt function.
     *
     *      @param [in] value   The value for which the function is called.
     *
     *      @return The solved value.
     */
    QString solveSqrt(QString const& value) const;

    /*!
     *  Get the precision used from the term.
     *
     *      @param [in] term   The first term of the operation.
     *
     *      @return The precision of the decimal used in the given term.
     */
    int precisionOf(QString const& term) const;

    /*!
     *  Get the base for a given number.
     *
     *      @param [in] constantNumber  The selected number.
     *
     *      @return The base for the selected number. Either 2, 8, 10 or 16.
     */
    int baseOf(QString const& constantNumber) const;
};

#endif // SYSTEMVERILOGEXPRESSIONPARSER_H
