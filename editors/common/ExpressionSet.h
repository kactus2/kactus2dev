//-----------------------------------------------------------------------------
// File: ExpressionSet.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 20.6.2023
//
// Description:
// Utilities for providing classes needed in expression handling in a bundle.
//-----------------------------------------------------------------------------

#ifndef EXPRESSIONSET_H
#define EXPRESSIONSET_H

#include <KactusAPI/include/ParameterFinder.h>
#include <KactusAPI/include/ExpressionParser.h>
#include <KactusAPI/include/ExpressionFormatter.h>

#include <QSharedPointer>

struct ExpressionSet
{
    QSharedPointer<ParameterFinder> finder;
    QSharedPointer<ExpressionParser> parser;
    QSharedPointer<ExpressionFormatter> formatter;
};

//-----------------------------------------------------------------------------

#endif // EXPRESSIONSET_H
