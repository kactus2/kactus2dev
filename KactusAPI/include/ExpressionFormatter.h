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

#ifndef EXPRESSIONFORMATTER_H
#define EXPRESSIONFORMATTER_H

#include "ParameterFinder.h"

#include "KactusAPI/KactusAPIGlobal.h"

#include <QSharedPointer>

class ExpressionParser;

//-----------------------------------------------------------------------------
//! Expression formatter.
//-----------------------------------------------------------------------------
class KACTUS2_API ExpressionFormatter
{

public:

    /*!
     *  Constructor.
     *
     *    @param [in] parameterFinder     Parameter finder used to search the components parameters.
     */
    ExpressionFormatter(QSharedPointer<ParameterFinder> parameterFinder);

    /*!
     *  Destructor.
     */
    ~ExpressionFormatter();

    /*!
     *  Format an expression containing a reference to another parameter.
     *
     *    @param [in] expression  The expression being worked on.
     *
     *    @return Formatted expression.
     */
    QString formatReferringExpression(QString const& expression) const;

    static QString format(QString const& expression, QSharedPointer<ExpressionParser> parser);

private:

	//! No copying
    ExpressionFormatter(const ExpressionFormatter& other);
	//! No assignment
    ExpressionFormatter& operator=(const ExpressionFormatter& other);

    //! Used to search the components parameters.
    QSharedPointer<ParameterFinder> parameterFinder_;
};

#endif // EXPRESSIONFORMATTER_H
