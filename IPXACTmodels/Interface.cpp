//-----------------------------------------------------------------------------
// File: Interface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 5.6.2014
//
// Description:
// Describes the spirit:interface type in an IP-XACT document
//-----------------------------------------------------------------------------

#include "Interface.h"

#include <QDomNode>
#include <QString>
#include <QStringList>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
// Function: Interface::Interface()
//-----------------------------------------------------------------------------
Interface::Interface(QDomNode &interfaceNode):
componentRef_(""), 
busRef_("")
{
    QDomNamedNodeMap attributes = interfaceNode.attributes();
    
    componentRef_ = attributes.namedItem("spirit:componentRef").nodeValue();
    busRef_ = attributes.namedItem("spirit:busRef").nodeValue();
}

//-----------------------------------------------------------------------------
// Function: Interface::Interface()
//-----------------------------------------------------------------------------
Interface::Interface(QString componentRef_, QString busRef_)
    : componentRef_(componentRef_), busRef_(busRef_)
{
}

//-----------------------------------------------------------------------------
// Function: Interface::Interface()
//-----------------------------------------------------------------------------
Interface::Interface( const Interface& other ):
componentRef_(other.componentRef_),
busRef_(other.busRef_) {
}

//-----------------------------------------------------------------------------
// Function: Interface::~Interface()
//-----------------------------------------------------------------------------
Interface::~Interface()
{

}

//-----------------------------------------------------------------------------
// Function: Interface::operator=()
//-----------------------------------------------------------------------------
Interface& Interface::operator=( const Interface& other )
{
	if (this != &other) 
{
		componentRef_ = other.componentRef_;
		busRef_ = other.busRef_;
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: Interface::operator==()
//-----------------------------------------------------------------------------
bool Interface::operator==( const Interface& other ) 
{
	return (componentRef_.compare(other.componentRef_, Qt::CaseInsensitive) == 0 &&
		busRef_.compare(other.busRef_, Qt::CaseInsensitive) == 0);
}

//-----------------------------------------------------------------------------
// Function: Interface::operator!=()
//-----------------------------------------------------------------------------
bool Interface::operator!=( const Interface& other ) {

    return !operator==(other);
}

//-----------------------------------------------------------------------------
// Function: Interface::operator<()
//-----------------------------------------------------------------------------
bool Interface::operator<( const Interface& other ) {
	int compResult = componentRef_.compare(other.componentRef_, Qt::CaseInsensitive);
	if (compResult == 0) 
    {
		return busRef_.compare(other.busRef_, Qt::CaseInsensitive) < 0;
	}
	else 
    {
		return compResult < 0;
	}
}

//-----------------------------------------------------------------------------
// Function: Interface::getComponentRef()
//-----------------------------------------------------------------------------
QString Interface::getComponentRef() const
{
    return componentRef_;
}

//-----------------------------------------------------------------------------
// Function: Interface::getBusRef()
//-----------------------------------------------------------------------------
QString Interface::getBusRef() const
{
    return busRef_;
}

//-----------------------------------------------------------------------------
// Function: Interface::matches()
//-----------------------------------------------------------------------------
bool Interface::references(QString const& instanceName, QString const& busInterfaceName)
{
    return instanceName == componentRef_ && busInterfaceName == busRef_; 
}

//-----------------------------------------------------------------------------
// Function: Interface::isValid()
//-----------------------------------------------------------------------------
bool Interface::isValid( const QStringList& instanceNames,
								QStringList& errorList,
								const QString& parentIdentifier ) const {
	bool valid = true;
	
	if (componentRef_.isEmpty()) 
    {
		errorList.append(QObject::tr("No component reference set for active"
			" interface within %1").arg(parentIdentifier));
		valid = false;
	}
	else if (!instanceNames.contains(componentRef_))
    {
		errorList.append(QObject::tr("Active interface within %1 contains "
			"reference to component instance %2 that does not exist.").arg(
			parentIdentifier).arg(componentRef_));
		valid = false;
	}

	if (busRef_.isEmpty())
    {
		errorList.append(QObject::tr("No bus reference set for active interface within %1").arg(parentIdentifier));
		valid = false;
	}

	return valid;
}

//-----------------------------------------------------------------------------
// Function: Interface::isValid()
//-----------------------------------------------------------------------------
bool Interface::isValid( const QStringList& instanceNames ) const 
{
	return !componentRef_.isEmpty() && instanceNames.contains(componentRef_) && !busRef_.isEmpty();
}

//-----------------------------------------------------------------------------
// Function: Interface::write()
//-----------------------------------------------------------------------------
void Interface::write(QXmlStreamWriter& writer) const
{
    writer.writeEmptyElement("spirit:activeInterface");

    writer.writeAttribute("spirit:componentRef", componentRef_);
    writer.writeAttribute("spirit:busRef", busRef_);
}
