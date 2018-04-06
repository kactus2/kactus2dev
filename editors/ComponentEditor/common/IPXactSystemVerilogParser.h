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
#include <QStringList>

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
     *  Checks if the given expression is a symbol e.g. reference.
     *
     *      @param [in] expression   The expression to check.
     *
     *      @return True, if the expression is a symbol, otherwise false.
     */
    virtual bool isSymbol(QString const& expression) const override final;

    /*!
     *  Finds the value for given symbol.
     *
     *      @param [in] symbol  The symbol whose value to find.
     *
     *      @return The found symbol value.
     */
    virtual QString findSymbolValue(QString const& symbol) const override final;

    /*!
     *  Finds the base in the symbol.
     *
     *      @param [in] symbol   The symbol whose base to find.
     *
     *      @return The base for the symbol.
     */
    virtual int getBaseForSymbol(QString const& symbol) const override final;

private:

    // Disable copying.
    IPXactSystemVerilogParser(IPXactSystemVerilogParser const& rhs);
    IPXactSystemVerilogParser& operator=(IPXactSystemVerilogParser const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
    
    //! The finder for parameters available in the SystemVerilog expressions.
    QSharedPointer<ParameterFinder> finder_;

    //! Stack for tracking already evaluated symbols to check for loops in references.
    mutable QStringList symbolStack_;
};

#endif // IPXACTSYSTEMVERILOGPARSER_H
