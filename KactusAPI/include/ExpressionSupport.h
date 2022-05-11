//-----------------------------------------------------------------------------
// File: ExpressionSupport.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 04.02.2015
//
// Description:
// Interface for import plugins supporting expressions.
//-----------------------------------------------------------------------------

#ifndef EXPRESSIONSUPPORT_H
#define EXPRESSIONSUPPORT_H

class ExpressionSupport 
{
public:
	//! The destructor.
    virtual ~ExpressionSupport(){};

    /*!
     *  Sets the expression parser to use for parsing expressions.
     *
     *      @param [in] parser   The expression parser to use.
     */
    virtual void setExpressionParser(QSharedPointer<ExpressionParser> parser) = 0;
};

#endif // EXPRESSIONSUPPORT_H
