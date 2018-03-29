//-----------------------------------------------------------------------------
// File: ExpressionFormatter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 26.01.2015
//
// Description:
// Formatter for parameter expressions consisting of references.
//-----------------------------------------------------------------------------

#include "ExpressionFormatter.h"

#include "ExpressionParser.h"

#include <IPXACTmodels/common/validators/ValueFormatter.h>

#include <QStringList>
#include <QRegularExpression>


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
// Function: ExpressionFormatter::format()
//-----------------------------------------------------------------------------
QString ExpressionFormatter::format(QString const& expression, QSharedPointer<ExpressionParser> parser)
{
    bool isValidExpression = false;
    QString value = parser->parseExpression(expression, &isValidExpression);

    if (isValidExpression)
    {
        ValueFormatter formatter;
        return formatter.format(value, parser->baseForExpression(expression));
    }
    else
    {
        return "n/a";
    }
}

//-----------------------------------------------------------------------------
// Function: ExpressionFormatter::formatExpressionWithReference()
//-----------------------------------------------------------------------------
QString ExpressionFormatter::formatReferringExpression(QString const& expression) const
{
    static const QRegularExpression NON_ALPHA_CHARACTERS("[^a-zA-Z0-9:_.]+");

    QString formattedExpression = expression;

    foreach (QString const& valueID, expression.split(NON_ALPHA_CHARACTERS, QString::SkipEmptyParts))
    {
        if (parameterFinder_->hasId(valueID))
        {
            formattedExpression.replace(valueID, parameterFinder_->nameForId(valueID));
        }
    }

    return formattedExpression;
}
