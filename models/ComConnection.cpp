//-----------------------------------------------------------------------------
// File: ComConnection.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 11.4.2012
//
// Description:
// Class encapsulating communication connection data.
//-----------------------------------------------------------------------------

#include "ComConnection.h"

#include <QStringList>

//-----------------------------------------------------------------------------
// Function: ComConnection::ComConnection()
//-----------------------------------------------------------------------------
ComConnection::ComConnection() : name_(), displayName_(), desc_(),
                                 interface1_(), interface2_(), offPage_(false)
{
}

//-----------------------------------------------------------------------------
// Function: ComConnection::ComConnection()
//-----------------------------------------------------------------------------
ComConnection::ComConnection(QString const& name, QString const& displayName,
                             QString const& description, ComInterfaceRef const& ref1,
                             ComInterfaceRef const& ref2, QList<QPointF> const& route)
    : name_(name),
      displayName_(displayName),
      desc_(description),
      interface1_(ref1),
      interface2_(ref2),
      route_(route),
      offPage_(false)
{
}


//-----------------------------------------------------------------------------
// Function: ComConnection::ComConnection()
//-----------------------------------------------------------------------------
ComConnection::ComConnection(ComConnection const& rhs) : name_(rhs.name_),
                                                         displayName_(rhs.displayName_),
                                                         desc_(rhs.desc_),
                                                         interface1_(rhs.interface1_),
                                                         interface2_(rhs.interface2_),
                                                         route_(rhs.route_),
                                                         offPage_(rhs.offPage_)
{
}

//-----------------------------------------------------------------------------
// Function: ComConnection::ComConnection()
//-----------------------------------------------------------------------------
ComConnection::ComConnection(QDomNode& node) : name_(), displayName_(), desc_(),
                                               interface1_(), interface2_(),
                                               route_(), offPage_(false)
{
    for (int i = 0; i < node.childNodes().count(); ++i)
    {
        QDomNode childNode = node.childNodes().at(i);

        if (childNode.isComment())
        {
            continue;
        }

        if (childNode.nodeName() == "spirit:name")
        {
            name_ = childNode.childNodes().at(0).nodeValue();
        }
        else if (childNode.nodeName() == "spirit:displayName")
        {
            displayName_ = childNode.childNodes().at(0).nodeValue();
        }
        else if (childNode.nodeName() == "spirit:description")
        {
            desc_ = childNode.childNodes().at(0).nodeValue();
        }
        else if (childNode.nodeName() == "kactus2:activeComInterface")
        {
            if (interface1_.componentRef.isNull())
            {
                interface1_.componentRef = childNode.attributes().namedItem("kactus2:componentRef").nodeValue();
                interface1_.comRef = childNode.attributes().namedItem("kactus2:comRef").nodeValue();
            }
            else
            {
                interface2_.componentRef = childNode.attributes().namedItem("kactus2:componentRef").nodeValue();
                interface2_.comRef = childNode.attributes().namedItem("kactus2:comRef").nodeValue();
            }
        }
        else if (childNode.nodeName() == "kactus2:route")
        {
            offPage_ = childNode.attributes().namedItem("kactus2:offPage").nodeValue() == "true";

            for (int i = 0; i < childNode.childNodes().size(); ++i)
            {
                QDomNode posNode = childNode.childNodes().at(i);
                QPointF pos;

                if (posNode.nodeName() == "kactus2:position")
                {
                    pos.setX(posNode.attributes().namedItem("x").nodeValue().toInt());
                    pos.setY(posNode.attributes().namedItem("y").nodeValue().toInt());
                    route_.append(pos);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComConnection::~ComConnection()
//-----------------------------------------------------------------------------
ComConnection::~ComConnection()
{
}

//-----------------------------------------------------------------------------
// Function: ComConnection::write()
//-----------------------------------------------------------------------------
void ComConnection::write(QXmlStreamWriter& writer) const
{
    writer.writeStartElement("kactus2:comConnection");

    writer.writeTextElement("spirit:name", name_);
    writer.writeTextElement("spirit:displayName", displayName_);
    writer.writeTextElement("spirit:description", desc_);

    writer.writeEmptyElement("kactus2:activeComInterface");
    writer.writeAttribute("kactus2:componentRef", interface1_.componentRef);
    writer.writeAttribute("kactus2:comRef", interface1_.comRef);

    writer.writeEmptyElement("kactus2:activeComInterface");
    writer.writeAttribute("kactus2:componentRef", interface2_.componentRef);
    writer.writeAttribute("kactus2:comRef", interface2_.comRef);

    if (!route_.isEmpty())
    {
        writer.writeStartElement("kactus2:route");

        if (offPage_)
        {
            writer.writeAttribute("kactus2:offPage", "true");
        }
        else
        {
            writer.writeAttribute("kactus2:offPage", "false");
        }

        foreach (QPointF const& point, route_)
        {
            writer.writeEmptyElement("kactus2:position");
            writer.writeAttribute("x", QString::number(int(point.x())));
            writer.writeAttribute("y", QString::number(int(point.y())));
        }

        writer.writeEndElement();
    }

    writer.writeEndElement(); // kactus2:comConnection
}

//-----------------------------------------------------------------------------
// Function: ComConnection::isValid()
//-----------------------------------------------------------------------------
bool ComConnection::isValid(QStringList& errorList, QStringList const& instanceNames,
                            QString const& parentId) const
{
    bool valid = true;
    QString const thisId(QObject::tr("COM connection in %1").arg(parentId));

    if (name_.isEmpty())
    {
        errorList.append(QObject::tr("No name specified for COM connection in %1").arg(parentId));
        valid = false;
    }

    // Validate the interface references.
    if (interface1_.componentRef.isEmpty())
    {
        errorList.append(QObject::tr("No component reference set for active COM interface in %1").arg(thisId));
        valid = false;
    }
    else if (!instanceNames.contains(interface1_.componentRef))
    {
        errorList.append(QObject::tr("Active COM interface in %1 contains a reference "
                                     "to component instance '%2' that does not exist.").arg(
                                        thisId).arg(interface1_.componentRef));
        valid = false;
    }

    if (interface1_.comRef.isEmpty())
    {
        errorList.append(QObject::tr("No COM reference set for active COM interface in %1").arg(thisId));
        valid = false;
    }

    if (interface2_.componentRef.isEmpty())
    {
        errorList.append(QObject::tr("No component reference set for active COM interface in %1").arg(thisId));
        valid = false;
    }
    else if (!instanceNames.contains(interface2_.componentRef))
    {
        errorList.append(QObject::tr("Active COM interface in %1 contains a reference "
                                     "to component instance '%2' that does not exist.").arg(
                                        thisId).arg(interface2_.componentRef));
        valid = false;
    }

    if (interface2_.comRef.isEmpty())
    {
        errorList.append(QObject::tr("No COM reference set for active COM interface in %1").arg(thisId));
        valid = false;
    }

    return valid;
}

//-----------------------------------------------------------------------------
// Function: ComConnection::setName()
//-----------------------------------------------------------------------------
void ComConnection::setName(QString const& name)
{
    name_ = name;
}

//-----------------------------------------------------------------------------
// Function: ComConnection::setDisplayName()
//-----------------------------------------------------------------------------
void ComConnection::setDisplayName(QString const& displayName)
{
    displayName_ = displayName;
}

//-----------------------------------------------------------------------------
// Function: ComConnection::setDescription()
//-----------------------------------------------------------------------------
void ComConnection::setDescription(QString const& description)
{
    desc_ = description;
}

//-----------------------------------------------------------------------------
// Function: ComConnection::setInterface1()
//-----------------------------------------------------------------------------
void ComConnection::setInterface1(ComInterfaceRef const& ref)
{
    interface1_ = ref;
}

//-----------------------------------------------------------------------------
// Function: ComConnection::setInterface2()
//-----------------------------------------------------------------------------
void ComConnection::setInterface2(ComInterfaceRef const& ref)
{
    interface2_ = ref;
}

//-----------------------------------------------------------------------------
// Function: ComConnection::setOffPage()
//-----------------------------------------------------------------------------
void ComConnection::setOffPage(bool offPage)
{
    offPage_ = offPage;
}

//-----------------------------------------------------------------------------
// Function: ComConnection::getName()
//-----------------------------------------------------------------------------
QString const& ComConnection::getName() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: ComConnection::getDisplayName()
//-----------------------------------------------------------------------------
QString const& ComConnection::getDisplayName() const
{
    return displayName_;
}

//-----------------------------------------------------------------------------
// Function: ComConnection::getDescription()
//-----------------------------------------------------------------------------
QString const& ComConnection::getDescription() const
{
    return desc_;
}

//-----------------------------------------------------------------------------
// Function: ComConnection::getInterface1()
//-----------------------------------------------------------------------------
ComInterfaceRef const& ComConnection::getInterface1() const
{
    return interface1_;
}

//-----------------------------------------------------------------------------
// Function: ComConnection::getInterface2()
//-----------------------------------------------------------------------------
ComInterfaceRef const& ComConnection::getInterface2() const
{
    return interface2_;
}

//-----------------------------------------------------------------------------
// Function: ComConnection::getRoute()
//-----------------------------------------------------------------------------
QList<QPointF> const& ComConnection::getRoute() const
{
    return route_;
}

//-----------------------------------------------------------------------------
// Function: ComConnection::isOffPage()
//-----------------------------------------------------------------------------
bool ComConnection::isOffPage() const
{
    return offPage_;
}

//-----------------------------------------------------------------------------
// Function: ComConnection::operator=()
//-----------------------------------------------------------------------------
ComConnection& ComConnection::operator=(ComConnection const& rhs)
{
    if (&rhs != this)
    {
        name_ = rhs.name_;
        displayName_ = rhs.displayName_;
        desc_ = rhs.desc_;
        interface1_ = rhs.interface1_;
        interface2_ = rhs.interface2_;
        offPage_ = rhs.offPage_;
    }

    return *this;
}
