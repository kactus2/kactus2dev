//-----------------------------------------------------------------------------
// File: ListParameterFinder.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.03.2015
//
// Description:
// The implementation for finding parameters from a list with the correct ID.
//-----------------------------------------------------------------------------

#include "ListParameterFinder.h"

//-----------------------------------------------------------------------------
// Function: ListParameterFinder::ListParameterFinder()
//-----------------------------------------------------------------------------
ListParameterFinder::ListParameterFinder():
ParameterFinder()
{

}

//-----------------------------------------------------------------------------
// Function: ListParameterFinder::getParameterWithID()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> ListParameterFinder::getParameterWithID(QStringView parameterId) const
{
    if (parameterList_)
    {
        for (QSharedPointer<Parameter> parameter : *parameterList_)
        {
            if (parameter->getValueId().compare(parameterId) == 0)
            {
                return parameter;
            }
        }
    }

    return QSharedPointer<Parameter>();
}

//-----------------------------------------------------------------------------
// Function: ListParameterFinder::hasId()
//-----------------------------------------------------------------------------
bool ListParameterFinder::hasId(QStringView id) const
{
    if (parameterList_ == nullptr)
    {
        return false;
    }

    return std::any_of(parameterList_->cbegin(), parameterList_->cend(), 
        [id](auto const& parameter) { return parameter->getValueId().compare(id) == 0; });
}

//-----------------------------------------------------------------------------
// Function: ListParameterFinder::nameForId()
//-----------------------------------------------------------------------------
QString ListParameterFinder::nameForId(QStringView id) const
{
    if (QSharedPointer<Parameter> targetParameter = getParameterWithID(id); 
        targetParameter)
    {
        return targetParameter->name();
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: ListParameterFinder::valueForId()
//-----------------------------------------------------------------------------
QString ListParameterFinder::valueForId(QStringView id) const
{
    
    if (QSharedPointer<Parameter> targetParameter = getParameterWithID(id); 
        targetParameter)
    {
        return targetParameter->getValue();
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: ListParameterFinder::getAllParameterIds()
//-----------------------------------------------------------------------------
QStringList ListParameterFinder::getAllParameterIds() const
{
    QStringList allParameterIds;

    if (parameterList_)
    {
        for (QSharedPointer<Parameter> parameter : *parameterList_)
        {
            allParameterIds.append(parameter->getValueId());
        }
    }

    allParameterIds.removeAll(QString());
    return allParameterIds;
}

//-----------------------------------------------------------------------------
// Function: ListParameterFinder::getNumberOfParameters()
//-----------------------------------------------------------------------------
int ListParameterFinder::getNumberOfParameters() const noexcept
{
    return parameterList_->size();
}

//-----------------------------------------------------------------------------
// Function: ListParameterFinder::setParameterList()
//-----------------------------------------------------------------------------
void ListParameterFinder::setParameterList(QSharedPointer<QList<QSharedPointer<Parameter> > > parameterList) noexcept
{
    parameterList_ = parameterList;
}

//-----------------------------------------------------------------------------
// Function: ListParameterFinder::registerParameterModel()
//-----------------------------------------------------------------------------
void ListParameterFinder::registerParameterModel(QAbstractItemModel const* /*model*/)
{
    // Nothing to do.
}
