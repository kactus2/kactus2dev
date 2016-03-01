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
                                       QSharedPointer<HierInterface> ref1, QSharedPointer<HierInterface> ref2,
                                       bool imported):
Interconnection(),
imported_(imported)
{
    setName(name);
    setDisplayName(displayName);
    setDescription(description);

    setInterface(ref1);
    setInterface(ref2);
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

        for (int connectionIndex = 0; connectionIndex < apiInterfaceNodes.count(); ++connectionIndex)
        {
            QDomNode activeInterfaceNode = apiInterfaceNodes.at(connectionIndex);
            QDomNamedNodeMap activeAttributes = activeInterfaceNode.attributes();
            QString componentReference = activeAttributes.namedItem("componentRef").nodeValue();
            QString apiReference = activeAttributes.namedItem("apiRef").nodeValue();

            QSharedPointer<ActiveInterface> apiInterface(new ActiveInterface(componentReference, apiReference));
            setInterface(apiInterface);
        }

        QDomElement hierInterfaceElement = apiConnectionElement.firstChildElement("kactus2:hierApiInterface");
        if (!hierInterfaceElement.isNull())
        {
            QString apiReference = hierInterfaceElement.attribute("apiRef");
            QSharedPointer<HierInterface> hierarchicalInterface (new HierInterface(apiReference));
            setInterface(hierarchicalInterface);
        }
    }

    QDomNode importedNode = node.firstChildElement("kactus2:imported");
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
    if (!displayName().isEmpty())
    {
        writer.writeTextElement("ipxact:displayName", displayName());
    }
    if (!description().isEmpty())
    {
        writer.writeTextElement("ipxact:description", description());
    }

    writer.writeEmptyElement("kactus2:activeApiInterface");
    writer.writeAttribute("componentRef", getStartInterface()->getComponentReference());
    writer.writeAttribute("apiRef", getStartInterface()->getBusReference());

    writeEndInterface(writer);

    if (imported_)
    {
        writer.writeEmptyElement("kactus2:imported");
    }

    writer.writeEndElement(); // kactus2:apiConnection
}

//-----------------------------------------------------------------------------
// Function: ApiInterconnection::writeEndInterface()
//-----------------------------------------------------------------------------
void ApiInterconnection::writeEndInterface(QXmlStreamWriter& writer) const
{
    QSharedPointer<ActiveInterface> activeEndInterface = getEndInterface().dynamicCast<ActiveInterface>();
    if (activeEndInterface)
    {
        writer.writeEmptyElement("kactus2:activeApiInterface");
        writer.writeAttribute("componentRef", activeEndInterface->getComponentReference());
    }
    else
    {
        writer.writeEmptyElement("kactus2:hierApiInterface");
    }

    writer.writeAttribute("apiRef", getEndInterface()->getBusReference());
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
// Function: ApiInterconnection::setInterface()
//-----------------------------------------------------------------------------
void ApiInterconnection::setInterface(QSharedPointer<HierInterface> interfaceReference)
{
    QSharedPointer<ActiveInterface> activeInterface = interfaceReference.dynamicCast<ActiveInterface>();
    if (activeInterface)
    {
        if (!getStartInterface())
        {
            setStartInterface(activeInterface);
        }
        else
        {
            if (!getActiveInterfaces()->isEmpty())
            {
                getActiveInterfaces()->clear();
            }

            getActiveInterfaces()->append(activeInterface);
        }
    }
    else
    {
        if (!getHierInterfaces()->isEmpty())
        {
            getHierInterfaces()->clear();
        }

        getHierInterfaces()->append(interfaceReference);
    }
}

//-----------------------------------------------------------------------------
// Function: ApiInterconnection::setImported()
//-----------------------------------------------------------------------------
void ApiInterconnection::setImported(bool imported)
{
    imported_ = imported;
}

//-----------------------------------------------------------------------------
// Function: ApiInterconnection::getEndInterface()
//-----------------------------------------------------------------------------
QSharedPointer<HierInterface> ApiInterconnection::getEndInterface() const
{
    if (!getActiveInterfaces()->isEmpty())
    {
        return getActiveInterfaces()->first();
    }
    else if (!getHierInterfaces()->isEmpty())
    {
        return getHierInterfaces()->first();
    }
    else
    {
        return QSharedPointer<ActiveInterface>(new ActiveInterface());
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
