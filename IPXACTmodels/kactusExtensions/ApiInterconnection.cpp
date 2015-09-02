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

//-----------------------------------------------------------------------------
// Function: ApiDependency::ApiDependency()
//-----------------------------------------------------------------------------
ApiInterconnection::ApiInterconnection(QDomNode& node) :
Interconnection(),
imported_()
{
    setName(node.firstChildElement("ipxact:name").firstChild().nodeValue());
    setDisplayName(node.firstChildElement("ipxact:displayName").firstChild().nodeValue());
    setDescription(node.firstChildElement("ipxact:description").firstChild().nodeValue());

    QDomElement apiConnectionElement = node.toElement();

    if (!apiConnectionElement.isNull())
    {
        QDomNodeList apiInterfaceNodes = apiConnectionElement.elementsByTagName("kactus2:activeApiInterface");

        if (apiInterfaceNodes.count() == 2)
        {
            QDomNamedNodeMap startInterfaceAttributes = apiInterfaceNodes.at(0).attributes();
            QString startComponentRef = startInterfaceAttributes.namedItem("componentRef").nodeValue();
            QString startApiRef = startInterfaceAttributes.namedItem("apiRef").nodeValue();
            QSharedPointer<ActiveInterface> startInterface (new ActiveInterface(startComponentRef, startApiRef));
            setInterface1(startInterface);

            QDomNamedNodeMap endInterfaceAttributes = apiInterfaceNodes.at(1).attributes();
            QString endComponentRef = endInterfaceAttributes.namedItem("componentRef").nodeValue();
            QString endApiRef = endInterfaceAttributes.namedItem("apiRef").nodeValue();
            QSharedPointer<ActiveInterface> endInterface (new ActiveInterface(endComponentRef, endApiRef));
            setInterface2(endInterface);
        }
    }

    QDomNode routeNode = node.firstChildElement("kactus2:route");
    if (!routeNode.isNull())
    {
        QList<QPointF> route;

        QDomNamedNodeMap routeAttributes = routeNode.attributes();
        if (routeAttributes.namedItem("offPage").nodeValue() == "true")
        {
            setOffPage(true);
        }
        else
        {
            setOffPage(false);
        }

        QDomNodeList routeNodeList = routeNode.childNodes();
        int routeCount = routeNodeList.count();
        for (int routeIndex = 0; routeIndex < routeCount; ++routeIndex)
        {
            QDomNode positionNode = routeNodeList.at(routeIndex);
            if (positionNode.nodeName() == "kactus2:position")
            {
                QDomNamedNodeMap positionAttributes = positionNode.attributes();
                int positionX = positionAttributes.namedItem("x").nodeValue().toInt();
                int positionY = positionAttributes.namedItem("y").nodeValue().toInt();

                QPointF newPosition(positionX, positionY);
                route.append(newPosition);
            }
        }

        setRoute(route);
    }

    QDomNode importedNode = node.firstChildElement();
    imported_ = !importedNode.isNull();
}

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

//-----------------------------------------------------------------------------
// Function: ApiInterconnection::setRoute()
//-----------------------------------------------------------------------------
void ApiInterconnection::setRoute(QList<QPointF> newRoute)
{
    getInterface2()->setRoute(newRoute);
}

//-----------------------------------------------------------------------------
// Function: ApiInterconnection::getRoute()
//-----------------------------------------------------------------------------
QList<QPointF> ApiInterconnection::getRoute() const
{
    return getInterface2()->getRoute();
}