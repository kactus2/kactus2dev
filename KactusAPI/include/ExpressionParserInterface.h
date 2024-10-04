//-----------------------------------------------------------------------------
// File: ExpressionParserInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 26.09.2024
//
// Description:
// Base class for parser interfaces for creating and handling expression parsers.
//-----------------------------------------------------------------------------

#ifndef EXPRESSIONPARSERINTERFACE_H
#define EXPRESSIONPARSERINTERFACE_H

#include <IPXACTmodels/Component/PortSlice.h>
#include <IPXACTmodels/Component/FieldSlice.h>

#include <KactusAPI/include/ExpressionParser.h>

class ExpressionParserInterface
{
public:

	ExpressionParserInterface() = default;
	virtual ~ExpressionParserInterface() = default;

	ExpressionParserInterface(const ExpressionParserInterface& other) = delete;
	ExpressionParserInterface& operator=(const ExpressionParserInterface& other) = delete;

	/*!
	 *	Create the expression parser.
	 *  
	 * 	    @return The created expression parser.
	 */
	virtual QSharedPointer<ExpressionParser> createParser() = 0;

};

#endif // EXPRESSIONPARSERINTERFACE_H