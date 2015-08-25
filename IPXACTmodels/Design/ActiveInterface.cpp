//-----------------------------------------------------------------------------
// File: ActiveInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 21.08.2015
//
// Description:
// Describes the ipxact:activeInterface element in an IP-XACT document.
//-----------------------------------------------------------------------------

#include "ActiveInterface.h"

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
// Function: ActiveInterface::ActiveInterface()
//-----------------------------------------------------------------------------
ActiveInterface::ActiveInterface(QString componentRef /* = QString("") */, QString busRef /* = QString("") */):
HierInterface(busRef),
componentRef_(componentRef),
excludePorts_(new QStringList)
{

}

//-----------------------------------------------------------------------------
// Function: ActiveInterface::ActiveInterface()
//-----------------------------------------------------------------------------
ActiveInterface::ActiveInterface(const ActiveInterface& other) :
HierInterface(other),
componentRef_(other.componentRef_),
excludePorts_(new QStringList())
{
    foreach (QString portName, *other.excludePorts_)
    {
        excludePorts_->append(portName);
    }
}

//-----------------------------------------------------------------------------
// Function: ActiveInterface::~ActiveInterface()
//-----------------------------------------------------------------------------
ActiveInterface::~ActiveInterface()
{
    excludePorts_.clear();
}

//-----------------------------------------------------------------------------
// Function: ActiveInterface::operator=()
//-----------------------------------------------------------------------------
ActiveInterface& ActiveInterface::operator=(const ActiveInterface& other)
{
    if (this != &other)
    {
        HierInterface::operator=(other);
        componentRef_ = other.componentRef_;

        foreach (QString portName, *other.excludePorts_)
        {
            excludePorts_->append(portName);
        }
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: ActiveInterface::operator==()
//-----------------------------------------------------------------------------
bool ActiveInterface::operator==(const ActiveInterface& other)
{
    return (HierInterface::operator==(other)) &&
        (componentRef_.compare(other.componentRef_, Qt::CaseInsensitive) == 0);
}

//-----------------------------------------------------------------------------
// Function: ActiveInterface::operator!=()
//-----------------------------------------------------------------------------
bool ActiveInterface::operator!=(const ActiveInterface& other)
{
    return !operator==(other);
}

//-----------------------------------------------------------------------------
// Function: ActiveInterface::operator<()
//-----------------------------------------------------------------------------
bool ActiveInterface::operator<(const ActiveInterface& other)
{
    int busRefeferencecomparison = HierInterface::operator<(other);
    if (busRefeferencecomparison == 0)
    {
        return componentRef_.compare(other.componentRef_, Qt::CaseInsensitive) < 0;
    }
    else
    {
        return busRefeferencecomparison < 0;
    }
}

//-----------------------------------------------------------------------------
// Function: ActiveInterface::references()
//-----------------------------------------------------------------------------
bool ActiveInterface::references(QString const& instanceName, QString const& busInterfaceName)
{
    return instanceName == componentRef_ && busInterfaceName == getBusReference();
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
// Function: ActiveInterface::getComponentReference()
//-----------------------------------------------------------------------------
QString ActiveInterface::getComponentReference() const
{
    return componentRef_;
}

//-----------------------------------------------------------------------------
// Function: ActiveInterface::setComponentReference()
//-----------------------------------------------------------------------------
void ActiveInterface::setComponentReference(QString const& newComponentReference)
{
    componentRef_ = newComponentReference;
}

//-----------------------------------------------------------------------------
// Function: ActiveInterface::getExcludePorts()
//-----------------------------------------------------------------------------
QSharedPointer<QStringList> ActiveInterface::getExcludePorts() const
{
    return excludePorts_;
}

//-----------------------------------------------------------------------------
// Function: ActiveInterface::setExcludePorts()
//-----------------------------------------------------------------------------
void ActiveInterface::setExcludePorts(QSharedPointer<QStringList> newExcludePorts)
{
    excludePorts_ = newExcludePorts;
}