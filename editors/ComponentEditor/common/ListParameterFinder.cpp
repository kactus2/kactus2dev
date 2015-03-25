//-----------------------------------------------------------------------------
// File: ListParameterFinder.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
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
parameterList_(new QList<QSharedPointer<Parameter> > ())
{

}

//-----------------------------------------------------------------------------
// Function: ListParameterFinder::~ListParameterFinder()
//-----------------------------------------------------------------------------
ListParameterFinder::~ListParameterFinder()
{

}

//-----------------------------------------------------------------------------
// Function: ListParameterFinder::getParameterWithID()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> ListParameterFinder::getParameterWithID(QString const& parameterId) const
{
    foreach (QSharedPointer<Parameter> parameter, *parameterList_)
    {
        if (parameter->getValueId() == parameterId)
        {
            return parameter;
        }
    }

    return QSharedPointer<Parameter> (new Parameter());
}

//-----------------------------------------------------------------------------
// Function: ListParameterFinder::hasId()
//-----------------------------------------------------------------------------
bool ListParameterFinder::hasId(QString const& id) const
{
    foreach (QSharedPointer<Parameter> parameter, *parameterList_)
    {
        if (parameter->getValueId() == id)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ListParameterFinder::nameForId()
//-----------------------------------------------------------------------------
QString ListParameterFinder::nameForId(QString const& id) const
{
    QSharedPointer<Parameter> targetParameter = getParameterWithID(id);

    return targetParameter->getName();
}

//-----------------------------------------------------------------------------
// Function: ListParameterFinder::valueForId()
//-----------------------------------------------------------------------------
QString ListParameterFinder::valueForId(QString const& id) const
{
    QSharedPointer<Parameter> targetParameter = getParameterWithID(id);

    return targetParameter->getValue();
}

//-----------------------------------------------------------------------------
// Function: ListParameterFinder::getAllParameterIds()
//-----------------------------------------------------------------------------
QStringList ListParameterFinder::getAllParameterIds() const
{
    QStringList allParameterIds;

    foreach (QSharedPointer<Parameter> parameter, *parameterList_)
    {
        allParameterIds.append(parameter->getValueId());
    }

    allParameterIds.removeAll("");
    return allParameterIds;
}

//-----------------------------------------------------------------------------
// Function: ListParameterFinder::getNumberOfParameters()
//-----------------------------------------------------------------------------
int ListParameterFinder::getNumberOfParameters() const
{
    return parameterList_->size();
}

//-----------------------------------------------------------------------------
// Function: ListParameterFinder::setParameterList()
//-----------------------------------------------------------------------------
void ListParameterFinder::setParameterList(QSharedPointer<QList<QSharedPointer<Parameter> > > parameterList)
{
    parameterList_ = parameterList;
}
