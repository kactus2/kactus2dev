//-----------------------------------------------------------------------------
// File: Kactus2Group.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 05.06.2014
//
// Description:
// Kactus2 vendor extension for a group of extensions.
//-----------------------------------------------------------------------------

#include "Kactus2Group.h"

#include <IPXACTmodels/VendorExtension.h>

#include <QSharedPointer>

//-----------------------------------------------------------------------------
// Function: Kactus2Flag::Kactus2Flag()
//-----------------------------------------------------------------------------
Kactus2Group::Kactus2Group(QString name):
    name_(name),
    groupExtensions_()
{

}

//-----------------------------------------------------------------------------
// Function: Kactus2Group::Kactus2Group()
//-----------------------------------------------------------------------------
Kactus2Group::Kactus2Group(Kactus2Group const& other):
    name_(other.name_),
    groupExtensions_()
{
    foreach(QSharedPointer<VendorExtension> extension, other.groupExtensions_)
    {
        groupExtensions_.append(QSharedPointer<VendorExtension>(extension->clone()));
    }
}


//-----------------------------------------------------------------------------
// Function: Kactus2Flag::~Kactus2Flag()
//-----------------------------------------------------------------------------
Kactus2Group::~Kactus2Group()
{

}

//-----------------------------------------------------------------------------
// Function: Kactus2Group::clone()
//-----------------------------------------------------------------------------
Kactus2Group* Kactus2Group::clone() const
{
    Kactus2Group* cloned = new Kactus2Group(name_);

    foreach(QSharedPointer<VendorExtension> extension, groupExtensions_)
    {
        cloned->groupExtensions_.append(QSharedPointer<VendorExtension>(extension->clone()));
    }

    return cloned;
}

//-----------------------------------------------------------------------------
// Function: Kactus2Group::type()
//-----------------------------------------------------------------------------
QString Kactus2Group::type() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: Kactus2Group::write()
//-----------------------------------------------------------------------------
void Kactus2Group::write(QXmlStreamWriter& writer) const
{
    writer.writeStartElement(name_);

    foreach(QSharedPointer<VendorExtension> extension, groupExtensions_)
    {
        extension->write(writer);
    }

    writer.writeEndElement();
}

//-----------------------------------------------------------------------------
// Function: Kactus2Group::addExtension()
//-----------------------------------------------------------------------------
void Kactus2Group::addToGroup(QSharedPointer<VendorExtension> extension)
{
    if (!groupExtensions_.contains(extension))
    {
        groupExtensions_.append(extension);
    }
}

//-----------------------------------------------------------------------------
// Function: Kactus2Group::getExtensionForType()
//-----------------------------------------------------------------------------
QList<QSharedPointer<VendorExtension> > Kactus2Group::getByType(QString const& type) const
{
    QList<QSharedPointer<VendorExtension> > typedExtensions;

    foreach(QSharedPointer<VendorExtension> extension, groupExtensions_)
    {
        if (extension->type() == type)
        {
            typedExtensions.append(extension);
        }
    }

    return typedExtensions;
}
