//-----------------------------------------------------------------------------
// File: ExpressionFormatterFactory.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 16.02.2015
//
// Description:
// Interface for expression formatter factory.
//-----------------------------------------------------------------------------

#ifndef EXPRESSIONFORMATTERFACTORY_H
#define EXPRESSIONFORMATTERFACTORY_H

#include <editors/ComponentEditor/common/ExpressionFormatter.h>

#include <IPXACTmodels/component.h>

//-----------------------------------------------------------------------------
//! Interface for constructing design widgets.
//-----------------------------------------------------------------------------
class ExpressionFormatterFactory
{

public:

    /*!
     *  Destructor.
     */
    virtual ~ExpressionFormatterFactory() {};

    /*!
     *  Create an expression formatter using the given component.
     *
     *      @param [in] component   Pointer to the component, whose parameters are used in the formatter.
     */
    virtual ExpressionFormatter* makeExpressionFormatter(QSharedPointer<Component> component) = 0;
};

#endif // EXPRESSIONFORMATTERFACTORY_H
