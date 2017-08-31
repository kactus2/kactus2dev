//-----------------------------------------------------------------------------
// File: ConfigurableElementFinder.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 30.08.2017
//
// Description:
// The implementation for finding configurable elements from a list with the correct ID.
//-----------------------------------------------------------------------------

#include "ConfigurableElementFinder.h"

#include <designEditors/common/ComponentInstanceEditor/EditorConfigurableElement.h>

//-----------------------------------------------------------------------------
// Function: ConfigurableElementFinder::ConfigurableElementFinder()
//-----------------------------------------------------------------------------
ConfigurableElementFinder::ConfigurableElementFinder():
elementList_()
{

}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementFinder::~ConfigurableElementFinder()
//-----------------------------------------------------------------------------
ConfigurableElementFinder::~ConfigurableElementFinder()
{

}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementFinder::getParameterWithID()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> ConfigurableElementFinder::getParameterWithID(QString const& parameterId) const
{
    QSharedPointer<EditorConfigurableElement> element = getElementForID(parameterId);
    return element->getReferencedParameter();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementFinder::hasId()
//-----------------------------------------------------------------------------
bool ConfigurableElementFinder::hasId(QString const& id) const
{
    foreach (QSharedPointer<EditorConfigurableElement> element, elementList_)
    {
        QSharedPointer<Parameter> referencedParameter = element->getReferencedParameter();
        if (referencedParameter && referencedParameter->getValueId() == id)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementFinder::getElementForID()
//-----------------------------------------------------------------------------
QSharedPointer<EditorConfigurableElement> ConfigurableElementFinder::getElementForID(QString const& id) const
{
    foreach (QSharedPointer<EditorConfigurableElement> element, elementList_)
    {
        QSharedPointer<Parameter> referencedParameter = element->getReferencedParameter();
        if (referencedParameter && referencedParameter->getValueId() == id)
        {
            return element;
        }
    }

    return QSharedPointer<EditorConfigurableElement> ();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementFinder::nameForId()
//-----------------------------------------------------------------------------
QString ConfigurableElementFinder::nameForId(QString const& id) const
{
    QSharedPointer<EditorConfigurableElement> element = getElementForID(id);
    if (element && element->getReferencedParameter())
    {
        return element->getReferencedParameter()->name();
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementFinder::valueForId()
//-----------------------------------------------------------------------------
QString ConfigurableElementFinder::valueForId(QString const& id) const
{
    QSharedPointer<EditorConfigurableElement> element = getElementForID(id);
    if (element && element->getReferencedParameter())
    {
        return element->getConfiguratedValue();
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementFinder::getAllParameterIds()
//-----------------------------------------------------------------------------
QStringList ConfigurableElementFinder::getAllParameterIds() const
{
    QStringList allElementIDs;

    foreach (QSharedPointer<EditorConfigurableElement> element, elementList_)
    {
        if (element->getReferencedParameter())
        {
            allElementIDs.append(element->getReferencedParameter()->getValueId());
        }
    }

    allElementIDs.removeAll("");
    return allElementIDs;
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementFinder::getNumberOfParameters()
//-----------------------------------------------------------------------------
int ConfigurableElementFinder::getNumberOfParameters() const
{
    return elementList_.size();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementFinder::setParameterList()
//-----------------------------------------------------------------------------
void ConfigurableElementFinder::setConfigurableElementList(
    QList<QSharedPointer<EditorConfigurableElement> > newElementList)
{
    elementList_ = newElementList;
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementFinder::registerParameterModel()
//-----------------------------------------------------------------------------
void ConfigurableElementFinder::registerParameterModel(QAbstractItemModel const* /*model*/)
{
    // Nothing to do.
}
