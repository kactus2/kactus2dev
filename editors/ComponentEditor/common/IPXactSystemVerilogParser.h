//-----------------------------------------------------------------------------
// File: IPXactSystemVerilogParser.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 02.12.2014
//
// Description:
// Parser for SystemVerilog expressions within one IP-XACT component.
//-----------------------------------------------------------------------------

#ifndef IPXACTSYSTEMVERILOGPARSER_H
#define IPXACTSYSTEMVERILOGPARSER_H

#include "SystemVerilogExpressionParser.h"

#include <QSharedPointer>
#include <QString>

class Component;
class Parameter;

//-----------------------------------------------------------------------------
// Parser for SystemVerilog expressions within one IP-XACT component.
//-----------------------------------------------------------------------------
class IPXactSystemVerilogParser : public SystemVerilogExpressionParser
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] component   The component whose parameters are available in the SystemVerilog expressions.
     */
    IPXactSystemVerilogParser(QSharedPointer<Component> component);
    
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
    bool isBooleanExpression(QString const& expression) const;


    /*!
     *  Checks if the given expression is not valid for parsing.
     *
     *      @param [in] expression   The expression to check.
     *
     *      @return True, if the expression is not in valid format, otherwise false.
     */
    virtual bool isValidExpression(QString const& expression) const;

private:

    // Disable copying.
    IPXactSystemVerilogParser(IPXactSystemVerilogParser const& rhs);
    IPXactSystemVerilogParser& operator=(IPXactSystemVerilogParser const& rhs);

    QString evaluateParameterValuesIn(QString const& expression, 
        QList<QSharedPointer<Parameter> > const& componentParameters, int recursionStep) const;

    QList<QSharedPointer<Parameter> > findParametersInComponent() const;

    static const int MAX_RECURSION_STEPS = 32;

    QSharedPointer<Component> component_;
};

#endif // IPXACTSYSTEMVERILOGPARSER_H
