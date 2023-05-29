//-----------------------------------------------------------------------------
// File: Extendable.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 11.08.2015
//
// Description:
// Base class for all IP-XACT implementations with vendor extensions.
//-----------------------------------------------------------------------------

#include "Extendable.h"

#include <IPXACTmodels/common/VendorExtension.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Group.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Placeholder.h>

//-----------------------------------------------------------------------------
// Function: Extendable::getVendorExtensions()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<VendorExtension> > > Extendable::getVendorExtensions() const
{
    return vendorExtensions_;
}

//-----------------------------------------------------------------------------
// Function: Extendable::Extendable()
//-----------------------------------------------------------------------------
Extendable::Extendable() :
vendorExtensions_(new QList<QSharedPointer<VendorExtension> >())
{

}

//-----------------------------------------------------------------------------
// Function: Extendable::Extendable()
//-----------------------------------------------------------------------------
Extendable::Extendable(Extendable const& other) : 
vendorExtensions_(new QList<QSharedPointer<VendorExtension> >())
{
    copyVendorExtensions(other);
}

//-----------------------------------------------------------------------------
// Function: Extendable::operator=()
//-----------------------------------------------------------------------------
Extendable& Extendable::operator=(Extendable const& other)
{
    if (this != &other)
    {
        vendorExtensions_->clear();
        copyVendorExtensions(other);
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: Extendable::copyVendorExtensions()
//-----------------------------------------------------------------------------
void Extendable::copyVendorExtensions(Extendable const& other)
{
    for (QSharedPointer<VendorExtension> extension : *other.vendorExtensions_)
    {
        vendorExtensions_->append(QSharedPointer<VendorExtension>(extension->clone()));
    }
}

//-----------------------------------------------------------------------------
// Function: Extendable::getGroupedExtensionsByType()
//-----------------------------------------------------------------------------
QList<QSharedPointer<VendorExtension> > Extendable::getGroupedExtensionsByType(std::string const& groupName,
    std::string const& extensionType) const
{
    auto extension = findVendorExtension(groupName);

    if (extension != nullptr)
    {
        QSharedPointer<Kactus2Group> extensionGroup = extension.dynamicCast<Kactus2Group>();
        return extensionGroup->getByType(extensionType);
    }

    return QList<QSharedPointer<VendorExtension> >();
}

//-----------------------------------------------------------------------------
// Function: Extendable::setPlaceholderExtension()
//-----------------------------------------------------------------------------
void Extendable::setPlaceholderExtension(bool value, std::string const& extensionType)
{
    auto extension = findVendorExtension(extensionType).dynamicCast<Kactus2Placeholder>();

    if (value)
    {
        if (extension == nullptr)
        {
            extension = QSharedPointer<Kactus2Placeholder>(new Kactus2Placeholder(extensionType));
            getVendorExtensions()->append(extension);
        }
    }
    else
    {
        getVendorExtensions()->removeAll(extension);
    }
}

//-----------------------------------------------------------------------------
// Function: Extendable::findVendorExtension()
//-----------------------------------------------------------------------------
QSharedPointer<VendorExtension> Extendable::findVendorExtension(std::string const& type) const
{
    auto it = std::find_if(vendorExtensions_->cbegin(), vendorExtensions_->cend(), 
        [type](auto& extension) {return extension->type() == type;  });

    if (it == vendorExtensions_->cend())
    {
        return nullptr;
    }

    return *it;
}
