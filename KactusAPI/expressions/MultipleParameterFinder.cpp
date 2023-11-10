//-----------------------------------------------------------------------------
// File: MultipleParameterFinder.h
//-----------------------------------------------------------------------------
// Project: Kactus2
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
MultipleParameterFinder::MultipleParameterFinder()
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
// Function: MultipleParameterFinder::removeFinder()
//-----------------------------------------------------------------------------
void MultipleParameterFinder::removeFinder(QSharedPointer<ParameterFinder> finder)
{
    finders_.removeAll(finder);
}

//-----------------------------------------------------------------------------
// Function: MultipleParameterFinder::removeAllFinders()
//-----------------------------------------------------------------------------
void MultipleParameterFinder::removeAllFinders()
{
    finders_.clear();
}

//-----------------------------------------------------------------------------
// Function: MultipleParameterFinder::getParameterWithID()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> MultipleParameterFinder::getParameterWithID(QStringView parameterId) const
{
    for (QSharedPointer<ParameterFinder> finder : finders_)
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
bool MultipleParameterFinder::hasId(QStringView id) const
{
    return std::any_of(finders_.cbegin(), finders_.cend(), [id](const auto& finder) { return finder->hasId(id); });
}

//-----------------------------------------------------------------------------
// Function: MultipleParameterFinder::nameForId()
//-----------------------------------------------------------------------------
QString MultipleParameterFinder::nameForId(QStringView id) const
{
    return getParameterWithID(id)->name();
}

//-----------------------------------------------------------------------------
// Function: MultipleParameterFinder::valueForId()
//-----------------------------------------------------------------------------
QString MultipleParameterFinder::valueForId(QStringView id) const
{
    for (QSharedPointer<ParameterFinder> finder : finders_)
    {
        if (finder->hasId(id))
        {
            return finder->valueForId(id);
        }
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: MultipleParameterFinder::getAllParameterIds()
//-----------------------------------------------------------------------------
QStringList MultipleParameterFinder::getAllParameterIds() const
{
    QStringList allParameterIds;

    for (QSharedPointer<ParameterFinder> finder : finders_)
    {
        QStringList finderParameters = finder->getAllParameterIds();
        allParameterIds.append(finderParameters);
    }
    
    return allParameterIds;
}

//-----------------------------------------------------------------------------
// Function: MultipleParameterFinder::getNumberOfParameters()
//-----------------------------------------------------------------------------
int MultipleParameterFinder::getNumberOfParameters() const noexcept
{
    return getAllParameterIds().size();
}

//-----------------------------------------------------------------------------
// Function: MultipleParameterFinder::registerParameterModel()
//-----------------------------------------------------------------------------
void MultipleParameterFinder::registerParameterModel(QAbstractItemModel const* /*model*/)
{
    // Nothing to do.
}
