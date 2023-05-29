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

//-----------------------------------------------------------------------------
// Function: Interconnection::Interconnection()
//-----------------------------------------------------------------------------
Interconnection::Interconnection(std::string const& name, QSharedPointer<ActiveInterface> startInterface,
    std::string const& displayName, std::string const& description) :
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
activeInterfaces_(new QList<QSharedPointer<ActiveInterface> > ()),
hierInterfaces_(new QList<QSharedPointer<HierInterface> > ()),
isPresent_(other.isPresent_)
{
    for (auto const& singleInterface: *other.activeInterfaces_)
    {
        QSharedPointer<ActiveInterface> copy =
            QSharedPointer<ActiveInterface>(new ActiveInterface(*singleInterface));
        activeInterfaces_->append(copy);
    }

    for (auto const& singleInterface : *other.hierInterfaces_)
    {
        QSharedPointer<HierInterface> copy =
            QSharedPointer<HierInterface>(new HierInterface(*singleInterface));
        hierInterfaces_->append(copy);
    }
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
        for (auto const& singleInterface : *other.activeInterfaces_)
        {
            QSharedPointer<ActiveInterface> copy =
                QSharedPointer<ActiveInterface>(new ActiveInterface(*singleInterface));
            activeInterfaces_->append(copy);
        }

        hierInterfaces_->clear();
        for (auto const& singleInterface : *other.hierInterfaces_)
        {
            QSharedPointer<HierInterface> copy =
                QSharedPointer<HierInterface>(new HierInterface(*singleInterface));
            hierInterfaces_->append(copy);
        }
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: Interconnection::isOffPage()
//-----------------------------------------------------------------------------
bool Interconnection::isOffPage() const
{
    return findVendorExtension("kactus2:offPage") != nullptr;
}

//-----------------------------------------------------------------------------
// Function: Interconnection::setOffPage()
//-----------------------------------------------------------------------------
void Interconnection::setOffPage(bool offpage)
{
    setPlaceholderExtension(offpage, "kactus2:offPage");
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
std::string Interconnection::getIsPresent() const
{
    return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: Interconnection::setIsPresent()
//-----------------------------------------------------------------------------
void Interconnection::setIsPresent(std::string const& newIsPresent)
{
    isPresent_ = newIsPresent;
}

//-----------------------------------------------------------------------------
// Function: Interconnection::hasInterfaceReferencing()
//-----------------------------------------------------------------------------
bool Interconnection::hasInterfaceReferencingComponent(std::string const& instanceName,
    std::string const& busInterfaceName) const
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
void Interconnection::changeInterfaceComponentReferences(std::string const& oldComponent, 
    std::string const& newComponent)
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
