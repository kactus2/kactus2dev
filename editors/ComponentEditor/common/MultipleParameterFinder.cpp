//-----------------------------------------------------------------------------
// File: MultipleParameterFinder.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 29.01.2015
//
// Description:
// Finds parameters from multiple components.
//-----------------------------------------------------------------------------

#include "MultipleParameterFinder.h"

//-----------------------------------------------------------------------------
// Function: MultipleParameterFinder::MultipleParameterFinder()
//-----------------------------------------------------------------------------
MultipleParameterFinder::MultipleParameterFinder(): finders_()
{

}

//-----------------------------------------------------------------------------
// Function: MultipleParameterFinder::MultipleParameterFinder()
//-----------------------------------------------------------------------------
MultipleParameterFinder::~MultipleParameterFinder()
{

}

//-----------------------------------------------------------------------------
// Function: MultipleParameterFinder::addFinder()
//-----------------------------------------------------------------------------
void MultipleParameterFinder::addFinder(QSharedPointer<ParameterFinder> finder)
{
    if (!finders_.contains(finder))
    {
        finders_.append(finder);
    }
}

//-----------------------------------------------------------------------------
// Function: MultipleParameterFinder::getParameterWithID()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> MultipleParameterFinder::getParameterWithID(QString const& parameterId) const
{
    foreach(QSharedPointer<ParameterFinder> finder, finders_)
    {
        if (finder->hasId(parameterId))
        {
            return finder->getParameterWithID(parameterId);
        }
    }

    return QSharedPointer<Parameter>(new Parameter);
}

//-----------------------------------------------------------------------------
// Function: MultipleParameterFinder::hasId()
//-----------------------------------------------------------------------------
bool MultipleParameterFinder::hasId(QString const& id) const
{
    foreach(QSharedPointer<ParameterFinder> finder, finders_)
    {
        if (finder->hasId(id))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MultipleParameterFinder::nameForId()
//-----------------------------------------------------------------------------
QString MultipleParameterFinder::nameForId(QString const& id) const
{
    return getParameterWithID(id)->getName();
}

//-----------------------------------------------------------------------------
// Function: MultipleParameterFinder::valueForId()
//-----------------------------------------------------------------------------
QString MultipleParameterFinder::valueForId(QString const& id) const
{
    return getParameterWithID(id)->getValue();
}

//-----------------------------------------------------------------------------
// Function: MultipleParameterFinder::getAllParameterIds()
//-----------------------------------------------------------------------------
QStringList MultipleParameterFinder::getAllParameterIds() const
{
    QStringList allParameterIds;

    foreach(QSharedPointer<ParameterFinder> finder, finders_)
    {
        QStringList finderParameters = finder->getAllParameterIds();
        allParameterIds.append(finderParameters);
    }
    
    return allParameterIds;
}

//-----------------------------------------------------------------------------
// Function: MultipleParameterFinder::getNumberOfParameters()
//-----------------------------------------------------------------------------
int MultipleParameterFinder::getNumberOfParameters() const
{
    return getAllParameterIds().size();
}
