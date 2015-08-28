//-----------------------------------------------------------------------------
// File: ComInterconnection.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.08.2015
//
// Description:
// Class encapsulating communication connection data.
//-----------------------------------------------------------------------------

#include "ComInterconnection.h"

//-----------------------------------------------------------------------------
// Function: ComInterconnection::ComInterconnection()
//-----------------------------------------------------------------------------
ComInterconnection::ComInterconnection() :
Interconnection()
{

}

//-----------------------------------------------------------------------------
// Function: ComInterconnection::ComInterconnection()
//-----------------------------------------------------------------------------
ComInterconnection::ComInterconnection(QString const& name, QString const& displayName, QString const& description,
    QSharedPointer<ActiveInterface> ref1, QSharedPointer<ActiveInterface> ref2, QList<QPointF> const& route) :
Interconnection(name, ref1, displayName, description)
{
    setRoute(route);
    setInterface2(ref2);
}

//-----------------------------------------------------------------------------
// Function: ComInterconnection::ComInterconnection()
//-----------------------------------------------------------------------------
ComInterconnection::ComInterconnection(ComInterconnection const& rhs) :
Interconnection(rhs)
{

}
/*
//-----------------------------------------------------------------------------
// Function: ComConnection::ComConnection()
//-----------------------------------------------------------------------------
ComConnection::ComConnection(QDomNode& node) :
NameGroup(),
//name_(), displayName_(), desc_(),
interface1_(new ActiveInterface()),
interface2_(new ActiveInterface()),
route_(),
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
        else if (childNode.nodeName() == "kactus2:activeComInterface")
        {
            if (interface1_->getComponentReference().isEmpty())
            {
                interface1_->setComponentReference(
                    childNode.attributes().namedItem("kactus2:componentRef").nodeValue());
                interface1_->setBusReference(childNode.attributes().namedItem("kactus2:comRef").nodeValue());
            }
            else
            {
                //interface2_.componentRef = childNode.attributes().namedItem("kactus2:componentRef").nodeValue();
                //interface2_.comRef = childNode.attributes().namedItem("kactus2:comRef").nodeValue();
                interface2_->setComponentReference(
                    childNode.attributes().namedItem("kactus2:componentRef").nodeValue());
                interface2_->setBusReference(childNode.attributes().namedItem("kactus2:comRef").nodeValue());
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
}*/

//-----------------------------------------------------------------------------
// Function: ComInterconnection::~ComInterconnection()
//-----------------------------------------------------------------------------
ComInterconnection::~ComInterconnection()
{

}

//-----------------------------------------------------------------------------
// Function: ComInterconnection::clone()
//-----------------------------------------------------------------------------
ComInterconnection* ComInterconnection::clone() const
{
    return new ComInterconnection(*this);
}

//-----------------------------------------------------------------------------
// Function: ComInterconnection::type()
//-----------------------------------------------------------------------------
QString ComInterconnection::type() const
{
    return QString("kactus2:comConnection");
}

//-----------------------------------------------------------------------------
// Function: ComConnection::write()
//-----------------------------------------------------------------------------
void ComInterconnection::write(QXmlStreamWriter& writer) const
{
    writer.writeStartElement("kactus2:comConnection");

    writer.writeTextElement("ipxact:name", name());
    writer.writeTextElement("ipxact:displayName", displayName());
    writer.writeTextElement("ipxact:description", description());

    writer.writeEmptyElement("kactus2:activeComInterface");
    writer.writeAttribute("componentRef", getInterface1()->getComponentReference());
    writer.writeAttribute("comRef", getInterface1()->getBusReference());

    writer.writeEmptyElement("kactus2:activeComInterface");
    writer.writeAttribute("componentRef", getInterface2()->getComponentReference());
    writer.writeAttribute("comRef", getInterface2()->getBusReference());

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

    writer.writeEndElement(); // kactus2:comConnection
}

//-----------------------------------------------------------------------------
// Function: ComConnection::isValid()
//-----------------------------------------------------------------------------
/*
bool ComConnection::isValid(QStringList& errorList, QStringList const& instanceNames,
                            QString const& parentId) const
{
    bool valid = true;
    QString const thisId(QObject::tr("COM connection in %1").arg(parentId));

    if (name().isEmpty())
    {
        errorList.append(QObject::tr("No name specified for COM connection in %1").arg(parentId));
        valid = false;
    }

    // Validate the interface references.
    if (interface1_->getComponentReference().isEmpty())
    {
        errorList.append(QObject::tr("No component reference set for active COM interface in %1").arg(thisId));
        valid = false;
    }
    else if (!instanceNames.contains(interface1_->getComponentReference()))
    {
        errorList.append(QObject::tr("Active COM interface in %1 contains a reference "
                                     "to component instance '%2' that does not exist.").arg(
                                        thisId).arg(interface1_->getComponentReference()));
        valid = false;
    }

    if (interface1_->getBusReference().isEmpty())
    {
        errorList.append(QObject::tr("No COM reference set for active COM interface in %1").arg(thisId));
        valid = false;
    }

    if (interface2_->getComponentReference().isEmpty())
    {
        errorList.append(QObject::tr("No component reference set for active COM interface in %1").arg(thisId));
        valid = false;
    }
    else if (!instanceNames.contains(interface2_->getComponentReference()))
    {
        errorList.append(QObject::tr("Active COM interface in %1 contains a reference "
                                     "to component instance '%2' that does not exist.").arg(
                                        thisId).arg(interface2_->getComponentReference()));
        valid = false;
    }

    if (interface2_->getBusReference().isEmpty())
    {
        errorList.append(QObject::tr("No COM reference set for active COM interface in %1").arg(thisId));
        valid = false;
    }

    return valid;
}
*//*
bool ComConnection::isValid( const QStringList& instanceNames ) const {
	
	if (name().isEmpty()) {
		return false;
	}

	// Validate the interface references.
	if (interface1_->getComponentReference().isEmpty()) {
		return false;
	}
	else if (!instanceNames.contains(interface1_->getComponentReference())) {
		return false;
	}

	if (interface1_->getBusReference().isEmpty()) {
		return false;
	}

	if (interface2_->getComponentReference().isEmpty()) {
		return false;
	}
	else if (!instanceNames.contains(interface2_->getComponentReference())) {
		return false;
	}

	if (interface2_->getBusReference().isEmpty()) {
		return false;
	}

	return true;
}*/

//-----------------------------------------------------------------------------
// Function: ComConnection::setInterface1()
//-----------------------------------------------------------------------------
void ComInterconnection::setInterface1(QSharedPointer<ActiveInterface> ref)
{
    setStartInterface(ref);
}

//-----------------------------------------------------------------------------
// Function: ComConnection::setInterface2()
//-----------------------------------------------------------------------------
void ComInterconnection::setInterface2(QSharedPointer<ActiveInterface> ref)
{
    getActiveInterfaces()->clear();
    getActiveInterfaces()->append(ref);
}

//-----------------------------------------------------------------------------
// Function: ComConnection::getInterface1()
//-----------------------------------------------------------------------------
QSharedPointer<ActiveInterface> ComInterconnection::getInterface1() const
{
    return getStartInterface();
}

//-----------------------------------------------------------------------------
// Function: ComConnection::getInterface2()
//-----------------------------------------------------------------------------
QSharedPointer<ActiveInterface> ComInterconnection::getInterface2() const
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
