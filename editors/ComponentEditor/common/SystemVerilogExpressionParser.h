//-----------------------------------------------------------------------------
// File: SystemVerilogExpressionParser.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.11.2014
//
// Description:
// Parser for SystemVerilog expressions.
//-----------------------------------------------------------------------------

#ifndef SYSTEMVERILOGEXPRESSIONPARSER_H
#define SYSTEMVERILOGEXPRESSIONPARSER_H

#include <QString>

//-----------------------------------------------------------------------------
//! Parser for SystemVerilog expressions.
//-----------------------------------------------------------------------------
class SystemVerilogExpressionParser 
{
public:

	//! The constructor.
	SystemVerilogExpressionParser();

	//! The destructor.
	~SystemVerilogExpressionParser();

    /*!
     *  Parses a constant number to decimal number.
     *
     *      @param [in] constantNumber   The constant to parse.
     *
     *      @return The decimal value of the constant.
     */
    QString parseConstant(QString const& constantNumber);

private:

	// Disable copying.
	SystemVerilogExpressionParser(SystemVerilogExpressionParser const& rhs);
	SystemVerilogExpressionParser& operator=(SystemVerilogExpressionParser const& rhs);

    /*!
     *  Converts the base format to the base number e.g. h to 16.
     *
     *      @param [in] baseFormat   The format to convert.
     *
     *      @return The base number for the format.
     */
    int baseForFormat(QString const& baseFormat);
};

#endif // SYSTEMVERILOGEXPRESSIONPARSER_H
