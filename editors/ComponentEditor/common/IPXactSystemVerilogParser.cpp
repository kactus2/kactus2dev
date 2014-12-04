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
    if (isBooleanExpression(expression))
    {
        return expression;
    }

    QString evaluatedExpression = evaluateParameterValuesIn(expression, findParametersInComponent(), 0);
    return SystemVerilogExpressionParser::parseExpression(evaluatedExpression);
}

//-----------------------------------------------------------------------------
// Function: IPXactSystemVerilogParser::isValidExpression()
//-----------------------------------------------------------------------------
bool IPXactSystemVerilogParser::isValidExpression(QString const& expression) const
{
    if (isBooleanExpression(expression))
    {
        return true;
    }

    QString evaluatedExpression = evaluateParameterValuesIn(expression, findParametersInComponent(), 0);
    return SystemVerilogExpressionParser::isValidExpression(evaluatedExpression);
}

//-----------------------------------------------------------------------------
// Function: IPXactSystemVerilogParser::evaluateParameters()
//-----------------------------------------------------------------------------
QString IPXactSystemVerilogParser::evaluateParameterValuesIn(QString const& expression, 
    QList<QSharedPointer<Parameter> > const& componentParameters, int recursionStep) const
{
    if (recursionStep == MAX_RECURSION_STEPS)
    {
        return "x";
    }

    QString evaluated = expression;
    foreach(QSharedPointer<Parameter> parameter, componentParameters)
    {
        if (!parameter->getValueId().isEmpty() && expression.contains(parameter->getValueId()))
        {
            QString parameterValue = 
                evaluateParameterValuesIn(parameter->getValue(), componentParameters, recursionStep + 1);

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

//-----------------------------------------------------------------------------
// Function: IPXactSystemVerilogParser::isBooleanExpression()
//-----------------------------------------------------------------------------
bool IPXactSystemVerilogParser::isBooleanExpression(QString const& expression) const
{
    return expression == "true" || expression == "false";
}