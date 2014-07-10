//-----------------------------------------------------------------------------
// File: Interconnection.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 5.6.2014
//
// Description:
// Describes the spirit:interconnection element in an IP-XACT document.
//-----------------------------------------------------------------------------

#include "Interconnection.h"

#include "Interface.h"
#include "XmlUtils.h"

#include <QDomNode>
#include <QString>
#include <QStringList>
#include <QPair>

//-----------------------------------------------------------------------------
// Function: Interconnection::Interconnection()
//-----------------------------------------------------------------------------
Interconnection::Interconnection(QDomNode& interconnectionNode): 
    nameGroup_(), 
    interface1(QString(), QString()), 
    interface2(QString(), QString()), 
    route_(), 
    offPage_(false)
{
    QDomNodeList nodes = interconnectionNode.childNodes();

    for (int i = 0; i < nodes.size(); i++) {
        QDomNode node = nodes.at(i);

        if (node.nodeName() == "spirit:name") 
        {
            nameGroup_.setName(node.firstChild().nodeValue());
        } 
        else if (node.nodeName() == "spirit:displayName")
        {
            nameGroup_.setDisplayName(node.firstChild().nodeValue());
        } 
        else if (node.nodeName() == "spirit:description")
        {
            nameGroup_.setDescription(node.firstChild().nodeValue());
        }
        else if (node.nodeName() == "spirit:activeInterface")
        {
            if (interface1.getComponentRef().isNull())
            {
                interface1 = Interface(node);
            }
            else
            {
                interface2 = Interface(node);
            }
        } // TODO: spirit:monitoredActiveInterface spirit:monitorInterface
    }
}

//-----------------------------------------------------------------------------
// Function: Interconnection::Interconnection()
//-----------------------------------------------------------------------------
Interconnection::Interconnection(QString name,
    Interface interface1,
    Interface interface2,
    QList<QPointF> const& route,
    bool offPage,
    QString displayName,
    QString description): 
    nameGroup_(name, displayName, description),
    interface1(interface1), 
    interface2(interface2), 
    route_(route), 
    offPage_(offPage)
{

}

//-----------------------------------------------------------------------------
// Function: Interconnection::Interconnection()
//-----------------------------------------------------------------------------
Interconnection::Interconnection( const Interconnection& other ):
nameGroup_(other.nameGroup_),
    interface1(other.interface1),
    interface2(other.interface2),
    route_(other.route_),
    offPage_(other.offPage_)
{
}

//-----------------------------------------------------------------------------
// Function: Interconnection::~Interconnection()
//-----------------------------------------------------------------------------
Interconnection::~Interconnection()
{

}

//-----------------------------------------------------------------------------
// Function: Interconnection::operator=()
//-----------------------------------------------------------------------------
Interconnection& Interconnection::operator=( const Interconnection& other ) {
    if (this != &other) {
        nameGroup_ = other.nameGroup_;
        interface1 = other.interface1;
        interface2 = other.interface2;
        route_ = other.route_;
        offPage_ = other.offPage_;
    }
    return *this;
}

//-----------------------------------------------------------------------------
// Function: Interconnection::name()
//-----------------------------------------------------------------------------
QString Interconnection::name() const
{
    return nameGroup_.name();
}

//-----------------------------------------------------------------------------
// Function: Interconnection::displayName()
//-----------------------------------------------------------------------------
QString Interconnection::displayName() const
{
    return nameGroup_.displayName();
}

//-----------------------------------------------------------------------------
// Function: Interconnection::description()
//-----------------------------------------------------------------------------
QString Interconnection::description() const
{
    return nameGroup_.description();
}

//-----------------------------------------------------------------------------
// Function: Interconnection::isOffPage()
//-----------------------------------------------------------------------------
bool Interconnection::isOffPage() const
{
    return offPage_;
}

//-----------------------------------------------------------------------------
// Function: Interconnection::setOffPage()
//-----------------------------------------------------------------------------
void Interconnection::setOffPage(bool offpage)
{
    offPage_ = offpage;
}

//-----------------------------------------------------------------------------
// Function: Interconnection::getRoute()
//-----------------------------------------------------------------------------
QList<QPointF> Interconnection::getRoute() const
{
    return route_;
}

//-----------------------------------------------------------------------------
// Function: Interconnection::setRoute()
//-----------------------------------------------------------------------------
void Interconnection::setRoute(QList<QPointF> route)
{
    route_ = route;
}

//-----------------------------------------------------------------------------
// Function: Interconnection::getInterfaces()
//-----------------------------------------------------------------------------
QPair<Interface, Interface> Interconnection::getInterfaces() const
{
    return qMakePair(interface1, interface2);
}

//-----------------------------------------------------------------------------
// Function: Interconnection::isValid()
//-----------------------------------------------------------------------------
bool Interconnection::isValid( const QStringList& instanceNames, 
    QStringList& errorList,
    const QString& parentIdentifier ) const {
        bool valid = true;
        const QString thisIdentifier(QObject::tr("interconnection within %1").arg(
            parentIdentifier));

        if (nameGroup_.name().isEmpty()) {
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
}

//-----------------------------------------------------------------------------
// Function: Interconnection::isValid()
//-----------------------------------------------------------------------------
bool Interconnection::isValid( const QStringList& instanceNames ) const {
    if (nameGroup_.name().isEmpty()) {
        return false;
    }

    if (!interface1.isValid(instanceNames)) {
        return false;
    }

    if (!interface2.isValid(instanceNames)) {
        return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
// Function: Interconnection::write()
//-----------------------------------------------------------------------------
void Interconnection::write(QXmlStreamWriter& writer)
{
    writer.writeStartElement("spirit:interconnection");

    nameGroup_.write(writer);    

    // Write interface manually, since interface.write() writes element with name spirit:interface.
    writer.writeEmptyElement("spirit:activeInterface");
    writer.writeAttribute("spirit:componentRef", interface1.getComponentRef());
    writer.writeAttribute("spirit:busRef", interface1.getBusRef());

    writer.writeEmptyElement("spirit:activeInterface");
    writer.writeAttribute("spirit:componentRef", interface2.getComponentRef());
    writer.writeAttribute("spirit:busRef", interface2.getBusRef());

    writer.writeEndElement();
}
