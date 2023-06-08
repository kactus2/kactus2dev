//-----------------------------------------------------------------------------
// File: ViewConfiguration.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 05.08.2015
//
// Description:
// Matches the ipxact:viewConfiguration element.
//-----------------------------------------------------------------------------

#include "ViewConfiguration.h"

//-----------------------------------------------------------------------------
// Function: ViewConfiguration::ViewConfiguration()
//-----------------------------------------------------------------------------
ViewConfiguration::ViewConfiguration():
Extendable()
{

}

//-----------------------------------------------------------------------------
// Function: ViewConfiguration::ViewConfiguration()
//-----------------------------------------------------------------------------
ViewConfiguration::ViewConfiguration(QString const& instanceName):
instanceName_(instanceName)
{

}

//-----------------------------------------------------------------------------
// Function: ViewConfiguration::ViewConfiguration()
//-----------------------------------------------------------------------------
ViewConfiguration::ViewConfiguration(const ViewConfiguration& other):
    Extendable(other),
instanceName_(other.instanceName_),
isPresent_(other.isPresent_),
viewRef_(other.viewRef_),
viewConfigurables_(new QList<QSharedPointer<ConfigurableElementValue> > )
{
    for (QSharedPointer<ConfigurableElementValue> configurable : *other.viewConfigurables_)
    {
        auto copy = QSharedPointer<ConfigurableElementValue>(
            new ConfigurableElementValue(configurable->getConfigurableValue(), configurable->getReferenceId()));
        viewConfigurables_->append(copy);
    }
}

//-----------------------------------------------------------------------------
// Function: ViewConfiguration::getInstanceName()
//-----------------------------------------------------------------------------
QString ViewConfiguration::getInstanceName() const
{
    return instanceName_;
}

//-----------------------------------------------------------------------------
// Function: ViewConfiguration::setInstanceName()
//-----------------------------------------------------------------------------
void ViewConfiguration::setInstanceName(QString const& newInstanceName)
{
    instanceName_ = newInstanceName;
}

//-----------------------------------------------------------------------------
// Function: ViewConfiguration::getIsPresent()
//-----------------------------------------------------------------------------
QString ViewConfiguration::getIsPresent() const
{
    return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: ViewConfiguration::setIsPresent()
//-----------------------------------------------------------------------------
void ViewConfiguration::setIsPresent(QString const& newIsPresent)
{
    isPresent_ = newIsPresent;
}

//-----------------------------------------------------------------------------
// Function: ViewConfiguration::getViewReference()
//-----------------------------------------------------------------------------
QString ViewConfiguration::getViewReference() const
{
    return viewRef_;
}

//-----------------------------------------------------------------------------
// Function: ViewConfiguration::setViewReference()
//-----------------------------------------------------------------------------
void ViewConfiguration::setViewReference(QString const& newViewReference)
{
    viewRef_ = newViewReference;
}

//-----------------------------------------------------------------------------
// Function: ViewConfiguration::getViewConfigurableElements()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > ViewConfiguration::getViewConfigurableElements()
    const
{
    return viewConfigurables_;
}

//-----------------------------------------------------------------------------
// Function: ViewConfiguration::setViewConfigurableElements()
//-----------------------------------------------------------------------------
void ViewConfiguration::setViewConfigurableElements(
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > newConfigurableElements)
{
    viewConfigurables_ = newConfigurableElements;
}
