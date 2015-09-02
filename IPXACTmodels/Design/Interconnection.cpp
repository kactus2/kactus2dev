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
Interconnection::Interconnection(QString const name, QSharedPointer<ActiveInterface> startInterface,
    QString const& displayName /* = QString() */, QString const& description /* = QString () */) :
NameGroup(name, displayName, description),
Extendable(),
startInterface_(startInterface),
activeInterfaces_(new QList<QSharedPointer<ActiveInterface> > ()),
hierInterfaces_(new QList<QSharedPointer<HierInterface> > ()),
isPresent_()
{

}

//-----------------------------------------------------------------------------
// Function: Interconnection::Interconnection()
//-----------------------------------------------------------------------------
Interconnection::Interconnection():
NameGroup(),
Extendable(),
startInterface_(new ActiveInterface()),
activeInterfaces_(new QList<QSharedPointer<ActiveInterface> > ()),
hierInterfaces_(new QList<QSharedPointer<HierInterface> > ()),
isPresent_()
{

}

//-----------------------------------------------------------------------------
// Function: Interconnection::Interconnection()
//-----------------------------------------------------------------------------
Interconnection::Interconnection(const Interconnection& other) :
NameGroup(other),
Extendable(other),
startInterface_(new ActiveInterface(*other.startInterface_.data())),
activeInterfaces_(new QList<QSharedPointer<ActiveInterface> > ()),
hierInterfaces_(new QList<QSharedPointer<HierInterface> > ()),
isPresent_(other.isPresent_)
{
    foreach (QSharedPointer<ActiveInterface> singleInterface, *other.activeInterfaces_)
    {
        if(singleInterface)
        {
            QSharedPointer<ActiveInterface> copy =
                QSharedPointer<ActiveInterface>(new ActiveInterface(*singleInterface.data()));
            activeInterfaces_->append(copy);
        }
    }

    foreach (QSharedPointer<HierInterface> singleInterface, *other.hierInterfaces_)
    {
        if (singleInterface)
        {
            QSharedPointer<HierInterface> copy =
                QSharedPointer<HierInterface>(new HierInterface(*singleInterface.data()));
            hierInterfaces_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Interconnection::~Interconnection()
//-----------------------------------------------------------------------------
Interconnection::~Interconnection()
{
    startInterface_.clear();
    activeInterfaces_.clear();
    hierInterfaces_.clear();
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
        startInterface_ = QSharedPointer<ActiveInterface>(new ActiveInterface(*other.startInterface_.data()));

        activeInterfaces_->clear();
        foreach (QSharedPointer<ActiveInterface> singleInterface, *other.activeInterfaces_)
        {
            if(singleInterface)
            {
                QSharedPointer<ActiveInterface> copy =
                    QSharedPointer<ActiveInterface>(new ActiveInterface(*singleInterface.data()));
                activeInterfaces_->append(copy);
            }
        }

        hierInterfaces_->clear();
        foreach (QSharedPointer<HierInterface> singleInterface, *other.hierInterfaces_)
        {
            if (singleInterface)
            {
                QSharedPointer<HierInterface> copy =
                    QSharedPointer<HierInterface>(new HierInterface(*singleInterface.data()));
                hierInterfaces_->append(copy);
            }
        }
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: Interconnection::isOffPage()
//-----------------------------------------------------------------------------
bool Interconnection::isOffPage() const
{
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type().compare("kactus2:offPage") == 0)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: Interconnection::setOffPage()
//-----------------------------------------------------------------------------
void Interconnection::setOffPage(bool offpage)
{
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type().compare("kactus2:offPage") == 0)
        {
            getVendorExtensions()->removeAll(extension);
            break;
        }
    }

    if (offpage)
    {
        QSharedPointer<Kactus2Placeholder> offPageExtension (new Kactus2Placeholder("kactus2:offPage"));
        getVendorExtensions()->append(offPageExtension);
    }
}

/*
//-----------------------------------------------------------------------------
// Function: Interconnection::isValid()
//-----------------------------------------------------------------------------
bool Interconnection::isValid( const QStringList& instanceNames, 
    QStringList& errorList,
    const QString& parentIdentifier ) const {
        bool valid = true;
        const QString thisIdentifier(QObject::tr("interconnection within %1").arg(
            parentIdentifier));

        if (name().isEmpty()) {
            errorList.append(QObject::tr("No name specified for interconnection"
                " within %1").arg(parentIdentifier));
            valid = false;
        }

        if (!interface1.isValid(instanceNames, errorList, thisIdentifier)) {
            valid = false;
        }

        if (!interface2.isValid(instanceNames, errorList, thisIdentifier)) {
            valid = false;
        }

        return valid;
}*/
/*
//-----------------------------------------------------------------------------
// Function: Interconnection::isValid()
//-----------------------------------------------------------------------------
bool Interconnection::isValid( const QStringList& instanceNames ) const {
    if (name().isEmpty()) {
        return false;
    }

    if (!interface1.isValid(instanceNames)) {
        return false;
    }

    if (!interface2.isValid(instanceNames)) {
        return false;
    }
    return true;
}*/

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
bool Interconnection::hasInterfaceReferencingComponent(
    QString const& instanceName, QString const& busInterfaceName) const
{
    if (startInterface_->getComponentReference() == instanceName &&
        startInterface_->getBusReference() == busInterfaceName)
    {
        return true;
    }

    foreach (QSharedPointer<ActiveInterface> singleInterface, *activeInterfaces_)
    {
        if (singleInterface->getComponentReference() == instanceName &&
            singleInterface->getBusReference() == busInterfaceName)
        {
            return true;
        }
    }

    return false;
}