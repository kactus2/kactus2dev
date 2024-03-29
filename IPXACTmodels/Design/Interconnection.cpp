//-----------------------------------------------------------------------------
// File: Interconnection.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 19.08.2015
//
// Description:
// Describes the ipxact:interconnection element in an IP-XACT document.
//-----------------------------------------------------------------------------

#include "Interconnection.h"

#include <IPXACTmodels/kactusExtensions/Kactus2Placeholder.h>

#include <IPXACTmodels/utilities/Copy.h>

//-----------------------------------------------------------------------------
// Function: Interconnection::Interconnection()
//-----------------------------------------------------------------------------
Interconnection::Interconnection(QString const name, QSharedPointer<ActiveInterface> startInterface,
    QString const& displayName, QString const& description) :
NameGroup(name, displayName, description),
Extendable(),
startInterface_(startInterface)
{

}

//-----------------------------------------------------------------------------
// Function: Interconnection::Interconnection()
//-----------------------------------------------------------------------------
Interconnection::Interconnection():
NameGroup(),
Extendable()
{

}

//-----------------------------------------------------------------------------
// Function: Interconnection::Interconnection()
//-----------------------------------------------------------------------------
Interconnection::Interconnection(const Interconnection& other) :
NameGroup(other),
Extendable(other),
startInterface_(new ActiveInterface(*other.startInterface_)),
isPresent_(other.isPresent_)
{
    Copy::copyList(other.activeInterfaces_, activeInterfaces_);

    Copy::copyList(other.hierInterfaces_, hierInterfaces_);
}

//-----------------------------------------------------------------------------
// Function: Interconnection::operator=()
//-----------------------------------------------------------------------------
Interconnection& Interconnection::operator=( const Interconnection& other)
{
    if (this != &other)
    {
        NameGroup::operator=(other);
        Extendable::operator=(other);

        isPresent_ = other.isPresent_;

        startInterface_.clear();
        startInterface_ = QSharedPointer<ActiveInterface>(new ActiveInterface(*other.startInterface_));

        activeInterfaces_->clear();
        Copy::copyList(other.activeInterfaces_, activeInterfaces_);

        hierInterfaces_->clear();

        Copy::copyList(other.hierInterfaces_, hierInterfaces_);
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: Interconnection::isOffPage()
//-----------------------------------------------------------------------------
bool Interconnection::isOffPage() const
{
    return findVendorExtension(QStringLiteral("kactus2:offPage")) != nullptr;
}

//-----------------------------------------------------------------------------
// Function: Interconnection::setOffPage()
//-----------------------------------------------------------------------------
void Interconnection::setOffPage(bool offpage)
{
    setPlaceholderExtension(offpage, QStringLiteral("kactus2:offPage"));
}

//-----------------------------------------------------------------------------
// Function: Interconnection::getStartInterface()
//-----------------------------------------------------------------------------
QSharedPointer<ActiveInterface> Interconnection::getStartInterface() const
{
    return startInterface_;
}

//-----------------------------------------------------------------------------
// Function: Interconnection::setStartInterface()
//-----------------------------------------------------------------------------
void Interconnection::setStartInterface(QSharedPointer<ActiveInterface> newStartInterface)
{
    startInterface_ = newStartInterface;
}

//-----------------------------------------------------------------------------
// Function: Interconnection::getActiveInterfaces()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<ActiveInterface> > > Interconnection::getActiveInterfaces() const
{
    return activeInterfaces_;
}

//-----------------------------------------------------------------------------
// Function: Interconnection::getHierInterfaces()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<HierInterface> > > Interconnection::getHierInterfaces() const
{
    return hierInterfaces_;
}

//-----------------------------------------------------------------------------
// Function: Interconnection::getIsPresent()
//-----------------------------------------------------------------------------
QString Interconnection::getIsPresent() const
{
    return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: Interconnection::setIsPresent()
//-----------------------------------------------------------------------------
void Interconnection::setIsPresent(QString const& newIsPresent)
{
    isPresent_ = newIsPresent;
}

//-----------------------------------------------------------------------------
// Function: Interconnection::hasInterfaceReferencing()
//-----------------------------------------------------------------------------
bool Interconnection::hasInterfaceReferencingComponent(QString const& instanceName, 
    QString const& busInterfaceName) const
{
    if (startInterface_->getComponentReference() == instanceName &&
        startInterface_->getBusReference() == busInterfaceName)
    {
        return true;
    }

    return std::any_of(activeInterfaces_->cbegin(), activeInterfaces_->cend(),
        [&instanceName, &busInterfaceName](auto const& singleInterface)
        { return singleInterface->getComponentReference() == instanceName &&
          singleInterface->getBusReference() == busInterfaceName; });
}

//-----------------------------------------------------------------------------
// Function: Interconnection::changeInterfaceComponentReferences()
//-----------------------------------------------------------------------------
void Interconnection::changeInterfaceComponentReferences(QString const& oldComponent, QString const& newComponent)
{
    if (startInterface_ && startInterface_->getComponentReference() == oldComponent)
    {
        startInterface_->setComponentReference(newComponent);
    }

    for (auto currentInterface : *activeInterfaces_)
    {
        if (currentInterface->getComponentReference().compare(oldComponent) == 0)
        {
            currentInterface->setComponentReference(newComponent);
        }
    }
}
