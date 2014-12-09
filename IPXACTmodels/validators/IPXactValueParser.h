//-----------------------------------------------------------------------------
// File: IPXactValueParser.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 04.12.2014
//
// Description:
// Parser for IP-XACT values.
//-----------------------------------------------------------------------------

#ifndef IPXACTVALUEPARSER_H
#define IPXACTVALUEPARSER_H

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <IPXACTmodels/ipxactmodels_global.h>

//-----------------------------------------------------------------------------
//! Parser for IP-XACT values.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT IPXactValueParser : public ExpressionParser
{
public:

    //! The constructor.
    IPXactValueParser();

    //! The destructor.
    virtual ~IPXactValueParser();
    
    /*!
     *  Parses an expression to decimal number.
     *
     *      @param [in] expression   The expression to parse.
     *
     *      @return The decimal value of the constant.
     */
    virtual QString parseExpression(QString const& expression) const;

    qreal parseConstant(QString const& expression, QString const& format) const;

    /*!
     *  Checks if the given expression is valid for parsing.
     *
     *      @param [in] expression   The expression to check.
     *
     *      @return True, if the expression is in valid format, otherwise false.
     */
    virtual bool isValidExpression(QString const& expression) const;

    /*!
     *  Checks if the given expression is valid for parsing.
     *
     *      @param [in] expression  The expression to check.
     *      @param [in] format      The format to check against.
     *
     *      @return True, if the expression is valid for the format, otherwise false.
     */
    virtual bool isValidExpression(QString const& expression, QString const& format) const;

    /*!
     *  Gets the matching format for the given expression.
     *
     *      @param [in] expression   The expression whose format to get.
     *
     *      @return The best matching format fot the expression.
     */
    QString getFormatForExpression(QString const& expression) const;

private:

    // Disable copying.
    IPXactValueParser(IPXactValueParser const& rhs);
    IPXactValueParser& operator=(IPXactValueParser const& rhs);

    /*!
     *  Evaluates the value of the given value using format long.
     *
     *      @param [in] value   The value to evaluate.
     *
     *      @return The evaluated value.
     */
    qreal longValueOf(QString const& value) const;
       
    /*!
     *  Evaluates the value of the given value using format float.
     *
     *      @param [in] value   The value to evaluate.
     *
     *      @return The evaluated value.
     */
    qreal floatValueOf(QString const& value) const;

};

#endif // IPXACTVALUEPARSER_H

