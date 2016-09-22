//-----------------------------------------------------------------------------
// File: IPXactSystemVerilogParser.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 02.12.2014
//
// Description:
// Parser for SystemVerilog expressions with parameter references.
//-----------------------------------------------------------------------------

#ifndef IPXACTSYSTEMVERILOGPARSER_H
#define IPXACTSYSTEMVERILOGPARSER_H

#include "SystemVerilogExpressionParser.h"

#include <QSharedPointer>
#include <QString>

class ParameterFinder;

//-----------------------------------------------------------------------------
// Parser for SystemVerilog expressions with parameter references.
//-----------------------------------------------------------------------------
class IPXactSystemVerilogParser : public SystemVerilogExpressionParser
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] finder   The finder for parameters available in the SystemVerilog expressions.
     */
    IPXactSystemVerilogParser(QSharedPointer<ParameterFinder> finder);
    
    //! The destructor.
    virtual ~IPXactSystemVerilogParser();
    
    /*!
     *  Parses an expression to decimal number.
     *
     *      @param [in] expression   The expression to parse.
     *
     *      @return The decimal value of the constant.
     */
    virtual QString parseExpression(QString const& expression) const;

    /*!
     *  Checks if the given expression is not valid for parsing.
     *
     *      @param [in] expression   The expression to check.
     *
     *      @return True, if the expression is not in valid format, otherwise false.
     */
    virtual bool isValidExpression(QString const& expression) const;

    /*!
     *  Finds the common base in the expression.
     *
     *      @param [in] expression   The expression to search in.
     *
     *      @return The common base for the expression.
     */
    virtual int baseForExpression(QString const& expression) const;

protected:

   //virtual qreal parseConstantToDecimal(QString const& constantNumber) const;


private:

    // Disable copying.
    IPXactSystemVerilogParser(IPXactSystemVerilogParser const& rhs);
    IPXactSystemVerilogParser& operator=(IPXactSystemVerilogParser const& rhs);

    /*!
     *  Evaluates the values of referenced parameters in the given expression recursively.
     *
     *      @param [in] expression              The expression to evaluate.
     *      @param [in] recursionStep           The current depth in recursion.
     *
     *      @return The expression where the references have been replaced with the evaluated values.
     */
    QString parseReferencesIn(QString const& expression, unsigned int recursionStep = 0) const;

    /*!
     *  Recursively replaces all references with their values without evaluating the expressions.
     *
     *      @param [in] expression      The expression to replace references in.
     *      @param [in] recursionStep   The current depth in recursion.
     *
     *      @return The expression where the references have been replaced with their values.
     */
    QString replaceReferencesWithValues(QString const& expression, unsigned int recursionStep = 0) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Maximum number of steps in evaluating a value for a parameter.
    static const int MAX_EVALUATION_STEPS = 25;
    
    //! The finder for parameters available in the SystemVerilog expressions.
    QSharedPointer<ParameterFinder> finder_;
};

#endif // IPXACTSYSTEMVERILOGPARSER_H
