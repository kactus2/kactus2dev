//-----------------------------------------------------------------------------
// File: NullParser.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 03.02.2015
//
// Description:
// Expression parser implementation that does nothing.
//-----------------------------------------------------------------------------

#ifndef NULLPARSER_H
#define NULLPARSER_H

#include <KactusAPI/include/ExpressionParser.h>

#include "KactusAPI/KactusAPIGlobal.h"

//-----------------------------------------------------------------------------
//! Expression parser implementation that does nothing.
//-----------------------------------------------------------------------------
class KACTUS2_API NullParser : public ExpressionParser
{
public:

	//! The constructor.
	NullParser();

	//! The destructor.
	~NullParser() final = default;

    // Disable copying.
    NullParser(NullParser const& rhs) = delete;
    NullParser& operator=(NullParser const& rhs) = delete;

    /*!
     *  Parses an expression to decimal number.
     *
     *    @param [in] expression   The expression to parse.
     *
     *    @return The decimal value of the constant.
     */
    QString parseExpression(QStringView expression, bool* isValidExpression) const final;

    /*!
     *  Checks if the given expression is a plain value and does not need evaluation.
     *
     *    @param [in] expression   The expression to check.
     *
     *    @return True, if the expression is a plain value, otherwise false.
     */
    bool isPlainValue(QStringView expression) const final;
    
    /*!
     *  Finds the common base in the expression.
     *
     *    @param [in] expression   The expression to search in.
     *
     *    @return The common base for the expression.
     */
    int baseForExpression(QStringView expression) const final;

};

#endif // NULLPARSER_H