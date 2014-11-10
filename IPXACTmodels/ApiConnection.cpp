//-----------------------------------------------------------------------------
// File: ApiConnection.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 11.4.2012
//
// Description:
// Class encapsulating API dependency connection data.
//-----------------------------------------------------------------------------

#include "ApiConnection.h"

//-----------------------------------------------------------------------------
// Function: ApiConnection::ApiConnection()
//-----------------------------------------------------------------------------
ApiConnection::ApiConnection() : name_(), displayName_(), desc_(),
                                 interface1_(), interface2_(),
                                 route_(), imported_(false), offPage_(false)
{
}

//-----------------------------------------------------------------------------
// Function: ApiConnection::ApiConnection()
//-----------------------------------------------------------------------------
ApiConnection::ApiConnection(QString const& name, QString const& displayName,
                             QString const& description, ApiInterfaceRef const& ref1,
                             ApiInterfaceRef const& ref2, QList<QPointF> const& route,
                             bool imported)
    : name_(name),
      displayName_(displayName),
      desc_(description),
      interface1_(ref1),
      interface2_(ref2),
      route_(route),
      imported_(imported),
      offPage_(false)
{

}

//-----------------------------------------------------------------------------
// Function: ApiDependency::ApiDependency()
//-----------------------------------------------------------------------------
ApiConnection::ApiConnection(ApiConnection const& rhs) : name_(rhs.name_),
                                                         displayName_(rhs.displayName_),
                                                         desc_(rhs.desc_),
                                                         interface1_(rhs.interface1_),
                                                         interface2_(rhs.interface2_),
                                                         route_(rhs.route_),
                                                         imported_(rhs.imported_),
                                                         offPage_(rhs.offPage_)
{
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::ApiDependency()
//-----------------------------------------------------------------------------
ApiConnection::ApiConnection(QDomNode& node) : name_(), displayName_(), desc_(),
                                               interface1_(), interface2_(),
                                               route_(), imported_(false), offPage_(false)
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
        else if (childNode.nodeName() == "kactus2:imported")
        {
            imported_ = true;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::~ApiDependency()
//-----------------------------------------------------------------------------
ApiConnection::~ApiConnection()
{
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::write()
//-----------------------------------------------------------------------------
void ApiConnection::write(QXmlStreamWriter& writer) const
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

    if (imported_)
    {
        writer.writeEmptyElement("kactus2:imported");
    }

    writer.writeEndElement(); // kactus2:apiDependency
}

bool ApiConnection::isValid( QStringList& errorList, QStringList const& instanceNames, QString const& parentId ) const {

	bool valid = true;
	QString const thisId(QObject::tr("API dependency in %1").arg(parentId));

	if (name_.isEmpty()) {
		errorList.append(QObject::tr("No name specified for API dependency in %1").arg(parentId));
		valid = false;
	}
	// interface 1:

	// if the component instance name is empty
	if (interface1_.componentRef.isEmpty()) {
		errorList.append(QObject::tr("No component instance reference set for %1.").arg(thisId));
		valid = false;
	}
	// if the component instance does not exist.
	else if (!instanceNames.contains(interface1_.componentRef)) {
		errorList.append(QObject::tr("%1 contains reference to component instance %2 that does not exist.").arg(
			thisId).arg(interface1_.componentRef));
		valid = false;
	}
	// if the API reference is empty
	if (interface1_.apiRef.isEmpty()) {
		errorList.append(QObject::tr("No API reference set for API dependency in %1").arg(thisId));
		valid = false;
	}

	// interface 2:

	// if the component instance name is empty
	if (interface2_.componentRef.isEmpty()) {
		errorList.append(QObject::tr("No component instance reference set for %1.").arg(thisId));
		valid = false;
	}
	// if the component instance does not exist.
	else if (!instanceNames.contains(interface2_.componentRef)) {
		errorList.append(QObject::tr("%1 contains reference to component instance %2 that does not exist.").arg(
			thisId).arg(interface2_.componentRef));
		valid = false;
	}
	// if the API reference is empty
	if (interface2_.apiRef.isEmpty()) {
		errorList.append(QObject::tr("No API reference set for API dependency in %1").arg(thisId));
		valid = false;
	}	

	return valid;
}

bool ApiConnection::isValid( const QStringList& instanceNames ) const {
	
	if (name_.isEmpty()) {
		return false;
	}
	// interface 1:
	
	// if the component instance name is empty
	if (interface1_.componentRef.isEmpty()) {
		return false;
	}
	// if the component instance does not exist.
	else if (!instanceNames.contains(interface1_.componentRef)) {
		return false;
	}
	// if the API reference is empty
	if (interface1_.apiRef.isEmpty()) {
		return false;
	}

	// interface 2:

	// if the component instance name is empty
	if (interface2_.componentRef.isEmpty()) {
		return false;
	}
	// if the component instance does not exist.
	else if (!instanceNames.contains(interface2_.componentRef)) {
		return false;
	}
	// if the API reference is empty
	if (interface2_.apiRef.isEmpty()) {
		return false;
	}	

	// all ok
	return true;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::setName()
//-----------------------------------------------------------------------------
void ApiConnection::setName(QString const& name)
{
    name_ = name;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::setDisplayName()
//-----------------------------------------------------------------------------
void ApiConnection::setDisplayName(QString const& displayName)
{
    displayName_ = displayName;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::setDescription()
//-----------------------------------------------------------------------------
void ApiConnection::setDescription(QString const& description)
{
    desc_ = description;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::setInterface1()
//-----------------------------------------------------------------------------
void ApiConnection::setInterface1(ApiInterfaceRef const& ref)
{
    interface1_ = ref;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::setInterface2()
//-----------------------------------------------------------------------------
void ApiConnection::setInterface2(ApiInterfaceRef const& ref)
{
    interface2_ = ref;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::setRoute()
//-----------------------------------------------------------------------------
void ApiConnection::setRoute(QList<QPointF> const& route)
{
    route_ = route;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::setImported()
//-----------------------------------------------------------------------------
void ApiConnection::setImported(bool imported)
{
    imported_ = imported;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::setOffPage()
//-----------------------------------------------------------------------------
void ApiConnection::setOffPage(bool offPage)
{
    offPage_ = offPage;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::getName()
//-----------------------------------------------------------------------------
QString const& ApiConnection::getName() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::getDisplayName()
//-----------------------------------------------------------------------------
QString const& ApiConnection::getDisplayName() const
{
    return displayName_;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::getDescription()
//-----------------------------------------------------------------------------
QString const& ApiConnection::getDescription() const
{
    return desc_;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::getInterface1()
//-----------------------------------------------------------------------------
ApiInterfaceRef const& ApiConnection::getInterface1() const
{
    return interface1_;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::getInterface2()
//-----------------------------------------------------------------------------
ApiInterfaceRef const& ApiConnection::getInterface2() const
{
    return interface2_;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::getRoute()
//-----------------------------------------------------------------------------
QList<QPointF> const& ApiConnection::getRoute() const
{
    return route_;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::isImported()
//-----------------------------------------------------------------------------
bool ApiConnection::isImported() const
{
    return imported_;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::isOffPage()
//-----------------------------------------------------------------------------
bool ApiConnection::isOffPage() const
{
    return offPage_;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::operator=()
//-----------------------------------------------------------------------------
ApiConnection& ApiConnection::operator=(ApiConnection const& rhs)
{
    if (&rhs != this)
    {
        name_ = rhs.name_;
        displayName_ = rhs.displayName_;
        desc_ = rhs.desc_;
        interface1_ = rhs.interface1_;
        interface2_ = rhs.interface2_;
        route_ = rhs.route_;
        imported_ = rhs.imported_;
        offPage_ = rhs.offPage_;
    }

    return *this;
}
