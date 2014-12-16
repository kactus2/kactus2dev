//-----------------------------------------------------------------------------
// File: IPXactSystemVerilogParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 02.12.2014
//
// Description:
// Parser for SystemVerilog expressions within one IP-XACT component.
//-----------------------------------------------------------------------------

#include "IPXactSystemVerilogParser.h"

#include <IPXACTmodels/addressblock.h>
#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/memorymap.h>
#include <IPXACTmodels/memorymapitem.h>
#include <IPXACTmodels/parameter.h>
#include <IPXACTmodels/register.h>
#include <IPXACTmodels/registermodel.h>


//-----------------------------------------------------------------------------
// Function: IPXactSystemVerilogParser::IPXactSystemVerilogParser()
//-----------------------------------------------------------------------------
IPXactSystemVerilogParser::IPXactSystemVerilogParser(QSharedPointer<Component> component)
    : SystemVerilogExpressionParser(), component_(component)
{

}

//-----------------------------------------------------------------------------
// Function: IPXactSystemVerilogParser::~IPXactSystemVerilogParser()
//-----------------------------------------------------------------------------
IPXactSystemVerilogParser::~IPXactSystemVerilogParser()
{

}

//-----------------------------------------------------------------------------
// Function: IPXactSystemVerilogParser::parseExpression()
//-----------------------------------------------------------------------------
QString IPXactSystemVerilogParser::parseExpression(QString const& expression) const
{
    if (SystemVerilogExpressionParser::isValidExpression(expression))
    {
        return SystemVerilogExpressionParser::parseExpression(expression);
    }

    QString evaluatedExpression = evaluateReferencesIn(expression, findParametersInComponent(), 0);
    if (SystemVerilogExpressionParser::isValidExpression(evaluatedExpression))
    {
        return SystemVerilogExpressionParser::parseExpression(evaluatedExpression);
    }
  
    return evaluatedExpression;
}

//-----------------------------------------------------------------------------
// Function: IPXactSystemVerilogParser::isValidExpression()
//-----------------------------------------------------------------------------
bool IPXactSystemVerilogParser::isValidExpression(QString const& expression) const
{
    if (SystemVerilogExpressionParser::isValidExpression(expression))
    {
        return true;
    }

    QString evaluatedExpression = replaceReferencesIn(expression, findParametersInComponent(), 0);
    return SystemVerilogExpressionParser::isValidExpression(evaluatedExpression);
}

//-----------------------------------------------------------------------------
// Function: IPXactSystemVerilogParser::baseForExpression()
//-----------------------------------------------------------------------------
int IPXactSystemVerilogParser::baseForExpression(QString const& expression) const
{
    if (SystemVerilogExpressionParser::isValidExpression(expression))
    {
        return SystemVerilogExpressionParser::baseForExpression(expression);
    }

    QString evaluatedExpression = replaceReferencesIn(expression, findParametersInComponent(), 0);

    return SystemVerilogExpressionParser::baseForExpression(evaluatedExpression);
}

//-----------------------------------------------------------------------------
// Function: IPXactSystemVerilogParser::evaluateReferencesIn()
//-----------------------------------------------------------------------------
QString IPXactSystemVerilogParser::evaluateReferencesIn(QString const& expression, 
    QList<QSharedPointer<Parameter> > const& componentParameters, int recursionStep) const
{
    if (shouldTerminateRecursion(recursionStep))
    {
        return "x";
    }

    if (SystemVerilogExpressionParser::isValidExpression(expression))
    {
        return expression;
    }

    QString evaluated = expression;
    foreach(QSharedPointer<Parameter> parameter, componentParameters)
    {
        if (!parameter->getValueId().isEmpty() && expression.contains(parameter->getValueId()))
        {
            QString parameterValue = 
                evaluateReferencesIn(parameter->getValue(), componentParameters, recursionStep + 1);

            if (SystemVerilogExpressionParser::isValidExpression(parameterValue))
            {
                parameterValue = SystemVerilogExpressionParser::parseExpression(parameterValue);
            }

            evaluated.replace(parameter->getValueId(), parameterValue);
        }
    }

    return evaluated;
}

//-----------------------------------------------------------------------------
// Function: IPXactSystemVerilogParser::replaceReferencesIn()
//-----------------------------------------------------------------------------
QString IPXactSystemVerilogParser::replaceReferencesIn(QString const& expression, 
    QList<QSharedPointer<Parameter> > const& componentParameters, int recursionStep) const
{
    if (shouldTerminateRecursion(recursionStep))
    {
        return "x";
    }

    if (SystemVerilogExpressionParser::isValidExpression(expression))
    {
        return expression;
    }

    QString evaluated = expression;
    foreach(QSharedPointer<Parameter> parameter, componentParameters)
    {
        if (!parameter->getValueId().isEmpty() && expression.contains(parameter->getValueId()))
        {
            QString parameterValue = 
                replaceReferencesIn(parameter->getValue(), componentParameters, recursionStep + 1);

            evaluated.replace(parameter->getValueId(), parameterValue);
        }
    }

    return evaluated;
}

//-----------------------------------------------------------------------------
// Function: IPXactSystemVerilogParser::shouldTerminateRecursion()
//-----------------------------------------------------------------------------
bool IPXactSystemVerilogParser::shouldTerminateRecursion(int recursionStep) const
{
    return recursionStep == MAX_RECURSION_STEPS;
}

//-----------------------------------------------------------------------------
// Function: IPXactSystemVerilogParser::findParametersInComponent()
//-----------------------------------------------------------------------------
QList<QSharedPointer<Parameter> > IPXactSystemVerilogParser::findParametersInComponent() const
{
    QList<QSharedPointer<Parameter> > componentParameters = component_->getParameters();

    foreach(QSharedPointer<ModelParameter> parameter, component_->getModelParameters())
    {
        componentParameters.append(parameter);
    }

    foreach(QSharedPointer<View> view, component_->getViews())
    {
        componentParameters.append(view->getParameters());
    }

    foreach(QSharedPointer<BusInterface> busInterface, component_->getBusInterfaces())
    {
        componentParameters.append(busInterface->getParameters());
    }

    foreach(QSharedPointer<MemoryMap> memoryMap, component_->getMemoryMaps())
    {
        foreach(QSharedPointer<MemoryMapItem> addressBlockItem, memoryMap->getItems())
        {
            QSharedPointer<AddressBlock> addressBlock = addressBlockItem.dynamicCast<AddressBlock>();

            if (addressBlock)
            {
                foreach(QSharedPointer<RegisterModel> registerItem, addressBlock->getRegisterData())
                {
                    componentParameters.append(registerItem->getParameters());
                }
            }
        }
    }

    return componentParameters;
}
