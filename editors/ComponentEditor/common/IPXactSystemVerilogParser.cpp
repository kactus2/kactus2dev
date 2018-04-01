//-----------------------------------------------------------------------------
// File: IPXactSystemVerilogParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 02.12.2014
//
// Description:
// Parser for SystemVerilog expressions with parameter references.
//-----------------------------------------------------------------------------

#include "IPXactSystemVerilogParser.h"

#include <editors/ComponentEditor/common/ParameterFinder.h>

#include <QRegularExpression>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: IPXactSystemVerilogParser::IPXactSystemVerilogParser()
//-----------------------------------------------------------------------------
IPXactSystemVerilogParser::IPXactSystemVerilogParser(QSharedPointer<ParameterFinder> finder):
SystemVerilogExpressionParser(), finder_(finder), symbolStack_(new QStringList())
{

}

//-----------------------------------------------------------------------------
// Function: IPXactSystemVerilogParser::~IPXactSystemVerilogParser()
//-----------------------------------------------------------------------------
IPXactSystemVerilogParser::~IPXactSystemVerilogParser()
{
    delete symbolStack_;
}

//-----------------------------------------------------------------------------
// Function: IPXactSystemVerilogParser::isSymbol()
//-----------------------------------------------------------------------------
bool IPXactSystemVerilogParser::isSymbol(QString const& expression) const
{
    return finder_->hasId(expression);
}

//-----------------------------------------------------------------------------
// Function: IPXactSystemVerilogParser::findSymbolValue()
//-----------------------------------------------------------------------------
QString IPXactSystemVerilogParser::findSymbolValue(QString const& expression) const
{
    // Check for ring references.
    if (symbolStack_->contains(expression))
    {
        symbolStack_->clear();
        return QStringLiteral("x");
    }

    symbolStack_->append(expression);
    QString value = parseExpression(finder_->valueForId(expression));
    symbolStack_->removeOne(expression);

    return value;
}

//-----------------------------------------------------------------------------
// Function: IPXactSystemVerilogParser::findSymbolValue()
//-----------------------------------------------------------------------------
int IPXactSystemVerilogParser::getBaseForSymbol(QString const& symbol) const
{
    return baseForExpression(finder_->valueForId(symbol));
}
