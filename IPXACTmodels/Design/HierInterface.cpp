//-----------------------------------------------------------------------------
// File: HierInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 19.08.2015
//
// Description:
// Describes the ipxact:hierInterface element in an IP-XACT document.
//-----------------------------------------------------------------------------

#include "HierInterface.h"

//-----------------------------------------------------------------------------
// Function: Interface::Interface()
//-----------------------------------------------------------------------------
/*
Interface::Interface(QDomNode &interfaceNode):
componentRef_(""), 
busRef_("")
{
    QDomNamedNodeMap attributes = interfaceNode.attributes();
    
    componentRef_ = attributes.namedItem("spirit:componentRef").nodeValue();
    busRef_ = attributes.namedItem("spirit:busRef").nodeValue();
}*/

//-----------------------------------------------------------------------------
// Function: HierInterface::HierInterface()
//-----------------------------------------------------------------------------
HierInterface::HierInterface(QString busRef /* = QString("") */) :
Extendable(),
busRef_(busRef),
isPresent_(),
description_()
{

}

//-----------------------------------------------------------------------------
// Function: HierInterface::HierInterface()
//-----------------------------------------------------------------------------
HierInterface::HierInterface(const HierInterface& other) :
Extendable(other),
busRef_(other.busRef_),
isPresent_(other.isPresent_),
description_(other.description_)
{

}

//-----------------------------------------------------------------------------
// Function: HierInterface::~HierInterface()
//-----------------------------------------------------------------------------
HierInterface::~HierInterface()
{

}

//-----------------------------------------------------------------------------
// Function: HierInterface::operator=()
//-----------------------------------------------------------------------------
HierInterface& HierInterface::operator=(const HierInterface& other)
{
    if (this != &other)
    {
        Extendable::operator =(other);
        busRef_ = other.busRef_;
        isPresent_ = other.isPresent_;
        description_ = other.description_;
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: HierInterface::operator==()
//-----------------------------------------------------------------------------
bool HierInterface::operator==(const HierInterface& other)
{
    return (busRef_.compare(other.busRef_, Qt::CaseInsensitive) == 0);
}

//-----------------------------------------------------------------------------
// Function: HierInterface::operator!=()
//-----------------------------------------------------------------------------
bool HierInterface::operator!=(const HierInterface& other)
{
    return !operator==(other);
}

//-----------------------------------------------------------------------------
// Function: HierInterface::operator<()
//-----------------------------------------------------------------------------
bool HierInterface::operator<(const HierInterface& other)
{
    int busResult = busRef_.compare(other.busRef_, Qt::CaseInsensitive);

    return busResult < 0;
}

/*
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
*/

//-----------------------------------------------------------------------------
// Function: HierInterface::getBusReference()
//-----------------------------------------------------------------------------
QString HierInterface::getBusReference() const
{
    return busRef_;
}

//-----------------------------------------------------------------------------
// Function: HierInterface::setBusReference()
//-----------------------------------------------------------------------------
void HierInterface::setBusReference(QString const& newBusReference)
{
    busRef_ = newBusReference;
}

//-----------------------------------------------------------------------------
// Function: HierInterface::getIsPresent()
//-----------------------------------------------------------------------------
QString HierInterface::getIsPresent() const
{
    return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: HierInterface::setIsPresent()
//-----------------------------------------------------------------------------
void HierInterface::setIsPresent(QString const& newIsPresent)
{
    isPresent_ = newIsPresent;
}

//-----------------------------------------------------------------------------
// Function: HierInterface::getDescription()
//-----------------------------------------------------------------------------
QString HierInterface::getDescription() const
{
    return description_;
}

//-----------------------------------------------------------------------------
// Function: HierInterface::setDescription()
//-----------------------------------------------------------------------------
void HierInterface::setDescription(QString const& newDescription)
{
    description_ = newDescription;
}