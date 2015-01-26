//-----------------------------------------------------------------------------
// File: ExpressionFormatter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 26.01.2015
//
// Description:
// Formatter for parameter expressions consisting of references.
//-----------------------------------------------------------------------------

#include "ExpressionFormatter.h"

//-----------------------------------------------------------------------------
// Function: ExpressionFormatter::ExpressionFormatter()
//-----------------------------------------------------------------------------
ExpressionFormatter::ExpressionFormatter(QSharedPointer<ParameterFinder> parameterFinder):
parameterFinder_(parameterFinder)
{

}

//-----------------------------------------------------------------------------
// Function: ExpressionFormatter::~ExpressionFormatter()
//-----------------------------------------------------------------------------
ExpressionFormatter::~ExpressionFormatter()
{

}

//-----------------------------------------------------------------------------
// Function: ExpressionFormatter::formatExpressionWithReference()
//-----------------------------------------------------------------------------
QString ExpressionFormatter::formatReferringExpression(QString const& expression) const
{
    QStringList parameterIds = parameterFinder_->getAllParameterIds();

    QString formattedExpression = expression;

    foreach (QString valueID, parameterIds)
    {
        if (expression.contains(valueID))
        {
            formattedExpression.replace(valueID, parameterFinder_->nameForId(valueID));
        }
    }

    return formattedExpression;
}
