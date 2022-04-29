//-----------------------------------------------------------------------------
// File: ExpressionFormatterFactory.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 16.02.2015
//
// Description:
// Interface for expression formatter factory.
//-----------------------------------------------------------------------------

#ifndef EXPRESSIONFORMATTERFACTORY_H
#define EXPRESSIONFORMATTERFACTORY_H

#include <KactusAPI/include/ExpressionFormatter.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/Design.h>

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

    /*!
     *  Create an expression formatter for a component instance within a design.
     *
     *      @param [in] component   The component of the selected component instance.
     *      @param [in] design      Design containing the component instance.
     */
    virtual ExpressionFormatter* createDesignInstanceFormatter(QSharedPointer<Component> component,
        QSharedPointer<Design> design) = 0;
};

#endif // EXPRESSIONFORMATTERFACTORY_H
