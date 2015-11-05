//-----------------------------------------------------------------------------
// File: RemapPort.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.09.2015
//
// Description:
// Describes the ipxact:remapPort element.
//-----------------------------------------------------------------------------

#include "RemapPort.h"

/*
RemapPort::RemapPort(QDomNode &remapPortNode): value_(QString()),
portNameRef_(QString()), portIndex_(-1) {

	// get the attributes
	QDomNamedNodeMap attributeMap = remapPortNode.attributes();

	// get the portNameRef attribute and strip the whitespace characters
	portNameRef_ = attributeMap.namedItem(QString("ipxact:portNameRef")).
			childNodes().at(0).nodeValue();
	portNameRef_ = XmlUtils::removeWhiteSpace(portNameRef_);

	QString portIndex = attributeMap.namedItem(QString("ipxact:portIndex")).
			childNodes().at(0).nodeValue();
	if (!portIndex.isNull()) {
		portIndex_ = portIndex.toInt();
	}

	value_ = remapPortNode.childNodes().at(0).nodeValue();
}*/

//-----------------------------------------------------------------------------
// Function: RemapPort::RemapPort()
//-----------------------------------------------------------------------------
RemapPort::RemapPort(QString const& portReference /* = QString() */) :
portRef_(portReference),
portIndex_(),
value_()
{

}

//-----------------------------------------------------------------------------
// Function: RemapPort::RemapPort()
//-----------------------------------------------------------------------------
RemapPort::RemapPort(const RemapPort &other) :
portRef_(other.portRef_),
portIndex_(other.portIndex_),
value_(other.value_)
{

}

//-----------------------------------------------------------------------------
// Function: RemapPort::operator=()
//-----------------------------------------------------------------------------
RemapPort& RemapPort::operator=(const RemapPort& other)
{
    if (this != &other)
    {
        portRef_ = other.portRef_;
        portIndex_ = other.portIndex_;
        value_ = other.value_;
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: RemapPort::~RemapPort()
//-----------------------------------------------------------------------------
RemapPort::~RemapPort()
{

}

/*
bool RemapPort::isValid( const QStringList& portNames, 
						QStringList& errorList, 
						const QString& parentIdentifier ) const {
	bool valid = true;

	if (value_.isEmpty()) {
		errorList.append(QObject::tr("No value set for remap port within %1").arg(
			parentIdentifier));
		valid = false;
	}

	if (portRef_.isEmpty()) {
		errorList.append(QObject::tr("No port reference assigned to remap port"
			" within %1").arg(parentIdentifier));
		valid = false;
	}
	else if (!portNames.contains(portRef_)) {
		errorList.append(QObject::tr("The port %1 referenced in remap port "
			" within %2 is not found in the containing component.").arg(
			portRef_).arg(parentIdentifier));
		valid = false;
	}

	return valid;
}*/
/*
bool RemapPort::isValid( const QStringList& portNames ) const {
	if (value_.isEmpty()) {
		return false;
	}

	if (portRef_.isEmpty()) {
		return false;
	}
	else if (!portNames.contains(portRef_)) {
		return false;
	}

	return true;
}*/

//-----------------------------------------------------------------------------
// Function: RemapPort::getPortNameRef()
//-----------------------------------------------------------------------------
QString RemapPort::getPortNameRef() const
{
    return portRef_;
}

//-----------------------------------------------------------------------------
// Function: RemapPort::setPortNameRef()
//-----------------------------------------------------------------------------
void RemapPort::setPortNameRef(QString const& newPortNameRef)
{
    portRef_ = newPortNameRef;
}

//-----------------------------------------------------------------------------
// Function: RemapPort::getPortIndex()
//-----------------------------------------------------------------------------
QString RemapPort::getPortIndex() const
{
    return portIndex_;
}

//-----------------------------------------------------------------------------
// Function: RemapPort::setPortIndex()
//-----------------------------------------------------------------------------
void RemapPort::setPortIndex(QString const& newPortIndex)
{
    portIndex_ = newPortIndex;
}

//-----------------------------------------------------------------------------
// Function: RemapPort::getValue()
//-----------------------------------------------------------------------------
QString RemapPort::getValue() const
{
    return value_;
}

//-----------------------------------------------------------------------------
// Function: RemapPort::setValue()
//-----------------------------------------------------------------------------
void RemapPort::setValue(QString const& newValue)
{
    value_ = newValue;
}
