//-----------------------------------------------------------------------------
// File: ApiInterconnection.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.08.2015
//
// Description:
// Class encapsulating API connection data.
//-----------------------------------------------------------------------------

#include "ApiInterconnection.h"

//-----------------------------------------------------------------------------
// Function: ApiInterconnection::ApiInterconnection()
//-----------------------------------------------------------------------------
ApiInterconnection::ApiInterconnection() :
Interconnection(),
imported_(false)
{

}

//-----------------------------------------------------------------------------
// Function: ApiInterconnection::ApiInterconnection()
//-----------------------------------------------------------------------------
ApiInterconnection::ApiInterconnection(QString const& name, QString const& displayName, QString const& description,
    QSharedPointer<ActiveInterface> ref1, QSharedPointer<ActiveInterface> ref2, QList<QPointF> const& route,
    bool imported /* = false */) :
Interconnection(name, ref1, displayName, description),
imported_(imported)
{
    setInterface2(ref2);
    setRoute(route);
}

//-----------------------------------------------------------------------------
// Function: ApiInterconnection::ApiInterconnection()
//-----------------------------------------------------------------------------
ApiInterconnection::ApiInterconnection(ApiInterconnection const& other) :
Interconnection(other),
imported_(other.imported_)
{

}

/*
//-----------------------------------------------------------------------------
// Function: ApiDependency::ApiDependency()
//-----------------------------------------------------------------------------
ApiConnection::ApiConnection(QDomNode& node) :
NameGroup(),
//name_(), displayName_(), desc_(),
interface1_(new ActiveInterface()),
interface2_(new ActiveInterface()),
route_(),
imported_(false),
offPage_(false)
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
            //name_ = childNode.childNodes().at(0).nodeValue();
            setName(childNode.childNodes().at(0).nodeValue());
        }
        else if (childNode.nodeName() == "spirit:displayName")
        {
            //displayName_ = childNode.childNodes().at(0).nodeValue();
            setDisplayName(childNode.childNodes().at(0).nodeValue());
        }
        else if (childNode.nodeName() == "spirit:description")
        {
            //desc_ = childNode.childNodes().at(0).nodeValue();
            setDescription(childNode.childNodes().at(0).nodeValue());
        }
        else if (childNode.nodeName() == "kactus2:activeApiInterface")
        {
            //if (interface1_.componentRef.isNull())
            if (interface1_->getComponentReference().isEmpty())
            {
                //interface1_.componentRef = childNode.attributes().namedItem("kactus2:componentRef").nodeValue();
                //interface1_.apiRef = childNode.attributes().namedItem("kactus2:apiRef").nodeValue();
                interface1_->setComponentReference(
                    childNode.attributes().namedItem("kactus2:componentRef").nodeValue());
                interface1_->setBusReference(childNode.attributes().namedItem("kactus2:apiRef").nodeValue());
            }
            else
            {
                //interface2_.componentRef = childNode.attributes().namedItem("kactus2:componentRef").nodeValue();
                //interface2_.apiRef = childNode.attributes().namedItem("kactus2:apiRef").nodeValue();
                interface2_->setComponentReference(
                    childNode.attributes().namedItem("kactus2:componentRef").nodeValue());
                interface2_->setBusReference(childNode.attributes().namedItem("kactus2:apiRef").nodeValue());
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
}*/

//-----------------------------------------------------------------------------
// Function: ApiInterconnection::~ApiInterconnection()
//-----------------------------------------------------------------------------
ApiInterconnection::~ApiInterconnection()
{

}

//-----------------------------------------------------------------------------
// Function: ApiInterconnection::clone()
//-----------------------------------------------------------------------------
ApiInterconnection* ApiInterconnection::clone() const
{
    return new ApiInterconnection(*this);
}

//-----------------------------------------------------------------------------
// Function: ApiInterconnection::type()
//-----------------------------------------------------------------------------
QString ApiInterconnection::type() const
{
    return QString("kactus2:apiConnection");
}

//-----------------------------------------------------------------------------
// Function: ApiInterconnection::write()
//-----------------------------------------------------------------------------
void ApiInterconnection::write(QXmlStreamWriter& writer) const
{
    writer.writeStartElement("kactus2:apiConnection");

    writer.writeTextElement("ipxact:name", name());
    writer.writeTextElement("ipxact:displayName", displayName());
    writer.writeTextElement("ipxact:description", description());

    writer.writeEmptyElement("kactus2:activeApiInterface");
    writer.writeAttribute("componentRef", getInterface1()->getComponentReference());
    writer.writeAttribute("apiRef", getInterface1()->getBusReference());

    writer.writeEmptyElement("kactus2:activeApiInterface");
    writer.writeAttribute("componentRef", getInterface2()->getComponentReference());
    writer.writeAttribute("apiRef", getInterface2()->getBusReference());

    if (!getRoute().isEmpty())
    {
        writer.writeStartElement("kactus2:route");

        if (isOffPage())
        {
            writer.writeAttribute("offPage", "true");
        }
        else
        {
            writer.writeAttribute("offPage", "false");
        }

        foreach (QPointF const& point, getRoute())
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

    writer.writeEndElement(); // kactus2:apiConnection
}

/*
//-----------------------------------------------------------------------------
// Function: ApiConnection::isValid()
//-----------------------------------------------------------------------------
bool ApiConnection::isValid( QStringList& errorList, QStringList const& instanceNames, QString const& parentId ) const {

	bool valid = true;
	QString const thisId(QObject::tr("API dependency in %1").arg(parentId));

	if (name().isEmpty()) {
		errorList.append(QObject::tr("No name specified for API dependency in %1").arg(parentId));
		valid = false;
	}
	// interface 1:

	// if the component instance name is empty
	if (interface1_->getComponentReference().isEmpty()) {
		errorList.append(QObject::tr("No component instance reference set for %1.").arg(thisId));
		valid = false;
	}
	// if the component instance does not exist.
	else if (!instanceNames.contains(interface1_->getComponentReference())) {
		errorList.append(QObject::tr("%1 contains reference to component instance %2 that does not exist.").arg(
			thisId).arg(interface1_->getComponentReference()));
		valid = false;
	}
	// if the API reference is empty
	if (interface1_->getBusReference().isEmpty()) {
		errorList.append(QObject::tr("No API reference set for API dependency in %1").arg(thisId));
		valid = false;
	}

	// interface 2:

	// if the component instance name is empty
	if (interface2_->getComponentReference().isEmpty()) {
		errorList.append(QObject::tr("No component instance reference set for %1.").arg(thisId));
		valid = false;
	}
	// if the component instance does not exist.
	else if (!instanceNames.contains(interface2_->getComponentReference())) {
		errorList.append(QObject::tr("%1 contains reference to component instance %2 that does not exist.").arg(
			thisId).arg(interface2_->getComponentReference()));
		valid = false;
	}
	// if the API reference is empty
	if (interface2_->getBusReference().isEmpty()) {
		errorList.append(QObject::tr("No API reference set for API dependency in %1").arg(thisId));
		valid = false;
	}	

	return valid;
}
*//*
//-----------------------------------------------------------------------------
// Function: ApiConnection::isValid()
//-----------------------------------------------------------------------------
bool ApiConnection::isValid( const QStringList& instanceNames ) const {
	
	if (name().isEmpty()) {
		return false;
	}
	// interface 1:
	
	// if the component instance name is empty
	if (interface1_->getComponentReference().isEmpty()) {
		return false;
	}
	// if the component instance does not exist.
	else if (!instanceNames.contains(interface1_->getComponentReference())) {
		return false;
	}
	// if the API reference is empty
	if (interface1_->getBusReference().isEmpty()) {
		return false;
	}

	// interface 2:

	// if the component instance name is empty
	if (interface2_->getComponentReference().isEmpty()) {
		return false;
	}
	// if the component instance does not exist.
	else if (!instanceNames.contains(interface2_->getComponentReference())) {
		return false;
	}
	// if the API reference is empty
	if (interface2_->getBusReference().isEmpty()) {
		return false;
	}	

	// all ok
	return true;
}*/

//-----------------------------------------------------------------------------
// Function: ApiInterconnection::setInterface1()
//-----------------------------------------------------------------------------
void ApiInterconnection::setInterface1(QSharedPointer<ActiveInterface> ref)
{
    setStartInterface(ref);
}

//-----------------------------------------------------------------------------
// Function: ApiInterconnection::setInterface2()
//-----------------------------------------------------------------------------
void ApiInterconnection::setInterface2(QSharedPointer<ActiveInterface> ref)
{
    getActiveInterfaces()->clear();
    getActiveInterfaces()->append(ref);
}

//-----------------------------------------------------------------------------
// Function: ApiInterconnection::setImported()
//-----------------------------------------------------------------------------
void ApiInterconnection::setImported(bool imported)
{
    imported_ = imported;
}

//-----------------------------------------------------------------------------
// Function: ApiInterconnection::getInterface1()
//-----------------------------------------------------------------------------
QSharedPointer<ActiveInterface> ApiInterconnection::getInterface1() const
{
    return getStartInterface();
}

//-----------------------------------------------------------------------------
// Function: ApiInterconnection::getInterface2()
//-----------------------------------------------------------------------------
QSharedPointer<ActiveInterface> ApiInterconnection::getInterface2() const
{
    if (getActiveInterfaces()->isEmpty())
    {
        return QSharedPointer<ActiveInterface>(new ActiveInterface());
    }
    else
    {
        return getActiveInterfaces()->first();
    }
}

//-----------------------------------------------------------------------------
// Function: ApiInterconnection::isImported()
//-----------------------------------------------------------------------------
bool ApiInterconnection::isImported() const
{
    return imported_;
}

//-----------------------------------------------------------------------------
// Function: ApiInterconnection::operator=()
//-----------------------------------------------------------------------------
ApiInterconnection& ApiInterconnection::operator=(ApiInterconnection const& other)
{
    if (&other != this)
    {
        Interconnection::operator=(other);
        imported_ = other.imported_;
    }

    return *this;
}
