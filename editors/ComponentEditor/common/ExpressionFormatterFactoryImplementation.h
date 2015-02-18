//-----------------------------------------------------------------------------
// File: ExpressionFormatterFactoryImplementation.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 16.02.2015
//
// Description:
// Class for expression formatter factory.
//-----------------------------------------------------------------------------

#ifndef EXPRESSIONFORMATTERFACTORYIMPLEMENTATION_H
#define EXPRESSIONFORMATTERFACTORYIMPLEMENTATION_H

#include "ExpressionFormatterFactory.h"

#include <editors/ComponentEditor/common/ExpressionFormatter.h>

//-----------------------------------------------------------------------------
//! Class for constructing expression formatters.
//-----------------------------------------------------------------------------
class ExpressionFormatterFactoryImplementation : public ExpressionFormatterFactory
{

public:
    /*!
     *  Constructor.
     */
    ExpressionFormatterFactoryImplementation();

    /*!
     *  Destructor.
     */
    ~ExpressionFormatterFactoryImplementation();

    /*!
     *  Create an expression formatter using the given component.
     *
     *      @param [in] component   Pointer to the component, whose parameters are used in the formatter.
     */
    virtual ExpressionFormatter* makeExpressionFormatter(QSharedPointer<Component> component);

private:
    // Disable copying.
    ExpressionFormatterFactoryImplementation(ExpressionFormatterFactoryImplementation const& rhs);
    ExpressionFormatterFactoryImplementation& operator=(ExpressionFormatterFactoryImplementation const& rhs);
};

#endif // DESIGNWIDGETFACTORYIMPLEMENTATION_H
