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

#include "KactusAPI/KactusAPIGlobal.h"

#include <QSharedPointer>
#include <QString>
#include <QVector>

class ParameterFinder;

//-----------------------------------------------------------------------------
// Parser for SystemVerilog expressions with parameter references.
//-----------------------------------------------------------------------------
class KACTUS2_API IPXactSystemVerilogParser : public SystemVerilogExpressionParser
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] finder   The finder for parameters available in the SystemVerilog expressions.
     */
    explicit IPXactSystemVerilogParser(QSharedPointer<ParameterFinder> finder);
    
    //! The destructor.
    virtual ~IPXactSystemVerilogParser() = default;

    // Disable copying.
    IPXactSystemVerilogParser(IPXactSystemVerilogParser const& rhs) = delete;
    IPXactSystemVerilogParser& operator=(IPXactSystemVerilogParser const& rhs) = delete;

    /*!
     *  Checks if the given expression is a symbol e.g. reference.
     *
     *      @param [in] expression   The expression to check.
     *
     *      @return True, if the expression is a symbol, otherwise false.
     */
    bool isSymbol(QStringView expression) const override;

    /*!
     *  Finds the value for given symbol.
     *
     *      @param [in] symbol  The symbol whose value to find.
     *
     *      @return The found symbol value.
     */
    QString findSymbolValue(QStringView symbol) const override;

    /*!
     *  Finds the base in the symbol.
     *
     *      @param [in] symbol   The symbol whose base to find.
     *
     *      @return The base for the symbol.
     */
    int getBaseForSymbol(QStringView symbol) const final;

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
    
    //! The finder for parameters available in the SystemVerilog expressions.
    QSharedPointer<ParameterFinder> finder_;

    //! Stack for tracking already evaluated symbols to check for loops in references.
    mutable QVector<QStringView> symbolStack_;
};

#endif // IPXACTSYSTEMVERILOGPARSER_H
