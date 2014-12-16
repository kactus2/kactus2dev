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
     *  Checks if the given value is a hexadecimal number.
     *
     *      @param [in] value  The value to check.
     *
     *      @return True, if the expression is a hexadecimal number, otherwise false.
     */
    bool isHexadecimal(QString const& value) const;

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

