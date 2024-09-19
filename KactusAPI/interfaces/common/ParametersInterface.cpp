//-----------------------------------------------------------------------------
// File: ParametersInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 13.03.2020
//
// Description:
// Interface for editing parameters.
//-----------------------------------------------------------------------------

#include "ParametersInterface.h"

#include <IPXACTmodels/common/Parameter.h>

#include <KactusAPI/include/ListHelper.h>

namespace
{
    QString const ARRAY_LEFT_ATTRIBTE = QLatin1String("kactus2:arrayLeft");
    QString const ARRAY_RIGHT_ATTRIBUTE = QLatin1String("kactus2:arrayRight");
};

//-----------------------------------------------------------------------------
// Function: ParametersInterface::ParametersInterface()
//-----------------------------------------------------------------------------
ParametersInterface::ParametersInterface(QSharedPointer<ParameterValidator> validator,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ExpressionFormatter> expressionFormatter):
AbstractParameterInterface(validator, expressionParser, expressionFormatter)
{

}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::setParameters()
//-----------------------------------------------------------------------------
void ParametersInterface::setParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters)
{
    parameters_ = newParameters;
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::getItemIndex()
//-----------------------------------------------------------------------------
int ParametersInterface::getItemIndex(std::string const& itemName) const
{
    return ListHelper::itemIndex(itemName, parameters_);
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::getIndexedItemName()
//-----------------------------------------------------------------------------
std::string ParametersInterface::getIndexedItemName(int itemIndex) const
{
    if (parameters_ && (itemIndex >= 0 && itemIndex < parameters_->size()))
    {
        return parameters_->at(itemIndex)->name().toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::itemCount()
//-----------------------------------------------------------------------------
int ParametersInterface::itemCount() const
{
    if (parameters_)
    {
        return parameters_->count();
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::getItemNames()
//-----------------------------------------------------------------------------
std::vector<std::string> ParametersInterface::getItemNames() const
{
    return ListHelper::listNames(parameters_);
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::addParameter()
//-----------------------------------------------------------------------------
void ParametersInterface::addParameter(int const& row, std::string const& newParameterName)
{
    QString parameterName = getUniqueName(newParameterName, "parameter");

    QSharedPointer<Parameter> newParameter(new Parameter());
    newParameter->setName(parameterName);

    parameters_->insert(row, newParameter);
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::removeParameter()
//-----------------------------------------------------------------------------
bool ParametersInterface::removeParameter(std::string const& parameterName)
{
    QSharedPointer<Parameter> removedParameter = getParameter(parameterName);
    if (!removedParameter)
    {
        return false;
    }

    return parameters_->removeOne(removedParameter);
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::getItem()
//-----------------------------------------------------------------------------
QSharedPointer<NameGroup> ParametersInterface::getItem(std::string const& parameterName) const
{
    return getParameter(parameterName);
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::getParameter()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> ParametersInterface::getParameter(std::string const& parameterName) const
{
    if (parameters_)
    {
        for (auto parameter : *parameters_)
        {
            if (parameter->name().toStdString() == parameterName)
            {
                return parameter;
            }
        }
    }

    return QSharedPointer<Parameter>();
}
