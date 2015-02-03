//-----------------------------------------------------------------------------
// File: NullParser.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 03.02.2015
//
// Description:
// Expression parser implementation that does nothing.
//-----------------------------------------------------------------------------

#ifndef NULLPARSER_H
#define NULLPARSER_H

#include <editors/ComponentEditor/common/ExpressionParser.h>

//-----------------------------------------------------------------------------
//! Expression parser implementation that does nothing.
//-----------------------------------------------------------------------------
class NullParser : public ExpressionParser
{
public:

	//! The constructor.
	NullParser();

	//! The destructor.
	virtual ~NullParser();

    /*!
     *  Parses an expression to decimal number.
     *
     *      @param [in] expression   The expression to parse.
     *
     *      @return The decimal value of the constant.
     */
    virtual QString parseExpression(QString const& expression) const;

    /*!
     *  Checks if the given expression is valid for parsing.
     *
     *      @param [in] expression   The expression to check.
     *
     *      @return True, if the expression is in valid format, otherwise false.
     */
    virtual bool isValidExpression(QString const& expression) const;
        
    /*!
     *  Checks if the given expression is a plain value and does not need evaluation.
     *
     *      @param [in] expression   The expression to check.
     *
     *      @return True, if the expression is a plain value, otherwise false.
     */
    virtual bool isPlainValue(QString const& expression) const;
    
    /*!
     *  Finds the common base in the expression.
     *
     *      @param [in] expression   The expression to search in.
     *
     *      @return The common base for the expression.
     */
    virtual int baseForExpression(QString const& expression) const;

private:

	// Disable copying.
	NullParser(NullParser const& rhs);
	NullParser& operator=(NullParser const& rhs);

};

#endif // NULLPARSER_H