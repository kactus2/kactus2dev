//-----------------------------------------------------------------------------
// File: ApiDependency.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 11.4.2012
//
// Description:
// Class encapsulating API dependency connection data.
//-----------------------------------------------------------------------------

#include "ApiDependency.h"

//-----------------------------------------------------------------------------
// Function: ApiDependency::ApiDependency()
//-----------------------------------------------------------------------------
ApiDependency::ApiDependency() : name_(), displayName_(), desc_(),
                                 interface1_(), interface2_(),
                                 route_()
{
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::ApiDependency()
//-----------------------------------------------------------------------------
ApiDependency::ApiDependency(QString const& name, QString const& displayName,
                             QString const& description, ApiInterfaceRef const& ref1,
                             ApiInterfaceRef const& ref2, QList<QPointF> const& route)
    : name_(name),
      displayName_(displayName),
      desc_(description),
      interface1_(ref1),
      interface2_(ref2),
      route_(route)
{

}

//-----------------------------------------------------------------------------
// Function: ApiDependency::ApiDependency()
//-----------------------------------------------------------------------------
ApiDependency::ApiDependency(ApiDependency const& rhs) : name_(rhs.name_),
                                                         displayName_(rhs.displayName_),
                                                         desc_(rhs.desc_),
                                                         interface1_(rhs.interface1_),
                                                         interface2_(rhs.interface2_),
                                                         route_(rhs.route_)
{
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::ApiDependency()
//-----------------------------------------------------------------------------
ApiDependency::ApiDependency(QDomNode& node) : name_(), displayName_(), desc_(),
                                               interface1_(), interface2_(),
                                               route_()
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
        else if (childNode.nodeName() == "kactus2:activeApiInterface")
        {
            if (interface1_.componentRef.isNull())
            {
                interface1_.componentRef = childNode.attributes().namedItem("kactus2:componentRef").nodeValue();
                interface1_.apiRef = childNode.attributes().namedItem("kactus2:apiRef").nodeValue();
            }
            else
            {
                interface2_.componentRef = childNode.attributes().namedItem("kactus2:componentRef").nodeValue();
                interface2_.apiRef = childNode.attributes().namedItem("kactus2:apiRef").nodeValue();
            }
        }
        else if (childNode.nodeName() == "kactus2:route")
        {
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
// Function: ApiDependency::~ApiDependency()
//-----------------------------------------------------------------------------
ApiDependency::~ApiDependency()
{
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::write()
//-----------------------------------------------------------------------------
void ApiDependency::write(QXmlStreamWriter& writer) const
{
    writer.writeStartElement("kactus2:apiDependency");

    writer.writeTextElement("spirit:name", name_);
    writer.writeTextElement("spirit:displayName", displayName_);
    writer.writeTextElement("spirit:description", desc_);

    writer.writeEmptyElement("kactus2:activeApiInterface");
    writer.writeAttribute("kactus2:componentRef", interface1_.componentRef);
    writer.writeAttribute("kactus2:apiRef", interface1_.apiRef);

    writer.writeEmptyElement("kactus2:activeApiInterface");
    writer.writeAttribute("kactus2:componentRef", interface2_.componentRef);
    writer.writeAttribute("kactus2:apiRef", interface2_.apiRef);

    if (!route_.isEmpty())
    {
        writer.writeStartElement("kactus2:route");

        foreach (QPointF const& point, route_)
        {
            writer.writeEmptyElement("kactus2:position");
            writer.writeAttribute("x", QString::number(int(point.x())));
            writer.writeAttribute("y", QString::number(int(point.y())));
        }

        writer.writeEndElement();
    }

    writer.writeEndElement(); // kactus2:apiDependency
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::setName()
//-----------------------------------------------------------------------------
void ApiDependency::setName(QString const& name)
{
    name_ = name;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::setDisplayName()
//-----------------------------------------------------------------------------
void ApiDependency::setDisplayName(QString const& displayName)
{
    displayName_ = displayName;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::setDescription()
//-----------------------------------------------------------------------------
void ApiDependency::setDescription(QString const& description)
{
    desc_ = description;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::setInterface1()
//-----------------------------------------------------------------------------
void ApiDependency::setInterface1(ApiInterfaceRef const& ref)
{
    interface1_ = ref;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::setInterface2()
//-----------------------------------------------------------------------------
void ApiDependency::setInterface2(ApiInterfaceRef const& ref)
{
    interface2_ = ref;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::getName()
//-----------------------------------------------------------------------------
QString const& ApiDependency::getName() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::getDisplayName()
//-----------------------------------------------------------------------------
QString const& ApiDependency::getDisplayName() const
{
    return displayName_;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::getDescription()
//-----------------------------------------------------------------------------
QString const& ApiDependency::getDescription() const
{
    return desc_;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::getInterface1()
//-----------------------------------------------------------------------------
ApiInterfaceRef const& ApiDependency::getInterface1() const
{
    return interface1_;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::getInterface2()
//-----------------------------------------------------------------------------
ApiInterfaceRef const& ApiDependency::getInterface2() const
{
    return interface2_;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::getRoute()
//-----------------------------------------------------------------------------
QList<QPointF> const& ApiDependency::getRoute() const
{
    return route_;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::operator=()
//-----------------------------------------------------------------------------
ApiDependency& ApiDependency::operator=(ApiDependency const& rhs)
{
    if (&rhs != this)
    {
        name_ = rhs.name_;
        displayName_ = rhs.displayName_;
        desc_ = rhs.desc_;
        interface1_ = rhs.interface1_;
        interface2_ = rhs.interface2_;
        route_ = rhs.route_;
    }

    return *this;
}
