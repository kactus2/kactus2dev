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

using namespace std;

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
AbstractParameterInterface(validator, expressionParser, expressionFormatter),
parameters_()
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
int ParametersInterface::getItemIndex(string const& itemName) const
{
    if (parameters_)
    {
        for (int i = 0; i < parameters_->size(); ++i)
        {
            if (parameters_->at(i)->name().toStdString() == itemName)
            {
                return i;
            }
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::getIndexedItemName()
//-----------------------------------------------------------------------------
string ParametersInterface::getIndexedItemName(int const& itemIndex) const
{
    string parameterName = "";
    if (parameters_ && (itemIndex >= 0 && itemIndex < parameters_->size()))
    {
        parameterName = parameters_->at(itemIndex)->name().toStdString();
    }

    return parameterName;
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
vector<string> ParametersInterface::getItemNames() const
{
    vector<string> names;
    if (parameters_)
    {
        for (auto parameter : *parameters_)
        {
            names.push_back(parameter->name().toStdString());
        }
    }

    return names;
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::addParameter()
//-----------------------------------------------------------------------------
void ParametersInterface::addParameter(int const& row, string const& newParameterName)
{
    QString parameterName = getUniqueName(newParameterName, "parameter");

    QSharedPointer<Parameter> newParameter(new Parameter());
    newParameter->setName(parameterName);

    parameters_->insert(row, newParameter);
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::removeParameter()
//-----------------------------------------------------------------------------
bool ParametersInterface::removeParameter(string const& parameterName)
{
    QSharedPointer<Parameter> removedParameter = getParameter(parameterName);
    if (!removedParameter)
    {
        return false;
    }

    return parameters_->removeOne(removedParameter);
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::getParameter()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> ParametersInterface::getParameter(string const& parameterName) const
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
