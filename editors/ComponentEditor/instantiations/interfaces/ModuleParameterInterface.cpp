//-----------------------------------------------------------------------------
// File: ModuleParameterInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 22.09.2020
//
// Description:
// Interface for editing module parameters.
//-----------------------------------------------------------------------------

#include "ModuleParameterInterface.h"

#include <IPXACTmodels/common/ModuleParameter.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>

#include <editors/ComponentEditor/common/ComponentInstantiationParameterFinder.h>

//-----------------------------------------------------------------------------
// Function: ModuleParameterInterface::ModuleParameterInterface()
//-----------------------------------------------------------------------------
ModuleParameterInterface::ModuleParameterInterface(QSharedPointer<ParameterValidator> validator,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ComponentInstantiationParameterFinder> parameterFinder):
AbstractParameterInterface(validator, expressionParser, expressionFormatter),
moduleParameters_(),
parameterFinder_(parameterFinder)
{

}

//-----------------------------------------------------------------------------
// Function: ModuleParameterInterface::setModuleParameters()
//-----------------------------------------------------------------------------
void ModuleParameterInterface::setModuleParameters(QSharedPointer<ComponentInstantiation> instantiation)
{
    moduleParameters_ = instantiation->getModuleParameters();

    parameterFinder_->setComponentInstantiation(instantiation);
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterInterface::getItemIndex()
//-----------------------------------------------------------------------------
int ModuleParameterInterface::getItemIndex(std::string const& itemName) const
{
    if (moduleParameters_)
    {
        for (int i = 0; i < moduleParameters_->size(); ++i)
        {
            if (moduleParameters_->at(i)->name().toStdString() == itemName)
            {
                return i;
            }
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterInterface::getIndexedItemName()
//-----------------------------------------------------------------------------
std::string ModuleParameterInterface::getIndexedItemName(int const& itemIndex) const
{
    std::string parameterName = "";
    if (moduleParameters_ && (itemIndex >= 0 && itemIndex < moduleParameters_->size()))
    {
        parameterName = moduleParameters_->at(itemIndex)->name().toStdString();
    }

    return parameterName;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterInterface::itemCount()
//-----------------------------------------------------------------------------
int ModuleParameterInterface::itemCount() const
{
    if (moduleParameters_)
    {
        return moduleParameters_->count();
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterInterface::getItemNames()
//-----------------------------------------------------------------------------
std::vector<std::string> ModuleParameterInterface::getItemNames() const
{
    std::vector<std::string> names;
    if (moduleParameters_)
    {
        for (auto parameter : *moduleParameters_)
        {
            names.push_back(parameter->name().toStdString());
        }
    }

    return names;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterInterface::addModuleParameter()
//-----------------------------------------------------------------------------
void ModuleParameterInterface::addModuleParameter(int const& row, std::string const& newParameterName)
{
    QString parameterName = getUniqueName(newParameterName, "parameter");

    QSharedPointer<ModuleParameter> newParameter(new ModuleParameter());
    newParameter->setName(parameterName);

    moduleParameters_->insert(row, newParameter);
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterInterface::removeModuleParameter()
//-----------------------------------------------------------------------------
bool ModuleParameterInterface::removeModuleParameter(std::string const& parameterName)
{
    QSharedPointer<ModuleParameter> removedParameter = getModuleParameter(parameterName);
    if (!removedParameter)
    {
        return false;
    }

    return moduleParameters_->removeOne(removedParameter);
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterInterface::getParameter()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> ModuleParameterInterface::getParameter(std::string const& parameterName) const
{
    return getModuleParameter(parameterName);
}


//-----------------------------------------------------------------------------
// Function: ModuleParameterInterface::isModuleParameter()
//-----------------------------------------------------------------------------
QSharedPointer<ModuleParameter> ModuleParameterInterface::getModuleParameter(std::string const& parameterName)
const
{
    if (moduleParameters_)
    {
        for (auto parameter : *moduleParameters_)
        {
            if (parameter->name().toStdString() == parameterName)
            {
                return parameter;
            }
        }
    }

    return QSharedPointer<ModuleParameter>();
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterInterface::getDataType()
//-----------------------------------------------------------------------------
std::string ModuleParameterInterface::getDataType(std::string const& parameterName) const
{
    QSharedPointer<ModuleParameter> moduleParameter = getModuleParameter(parameterName);
    if (moduleParameter)
    {
        return moduleParameter->getDataType().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterInterface::setDataType()
//-----------------------------------------------------------------------------
bool ModuleParameterInterface::setDataType(std::string const& parameterName, std::string const& newDataType)
{
    QSharedPointer<ModuleParameter> moduleParameter = getModuleParameter(parameterName);
    if (!moduleParameter)
    {
        return false;
    }

    moduleParameter->setDataType(QString::fromStdString(newDataType));
    return true;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterInterface::getUsageType()
//-----------------------------------------------------------------------------
std::string ModuleParameterInterface::getUsageType(std::string const& parameterName) const
{
    QSharedPointer<ModuleParameter> moduleParameter = getModuleParameter(parameterName);
    if (moduleParameter)
    {
        return moduleParameter->getUsageType().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterInterface::setUsageType()
//-----------------------------------------------------------------------------
bool ModuleParameterInterface::setUsageType(std::string const& parameterName, std::string const& newUsageType)
{
    QSharedPointer<ModuleParameter> moduleParameter = getModuleParameter(parameterName);
    if (!moduleParameter)
    {
        return false;
    }

    moduleParameter->setUsageType(QString::fromStdString(newUsageType));
    return true;
}
