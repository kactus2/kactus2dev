//-----------------------------------------------------------------------------
// File: View.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 03.09.2015
//
// Description:
// Describes the ipxact:view element of a component.
//-----------------------------------------------------------------------------

#include "View.h"

//-----------------------------------------------------------------------------
// Function: View::View()
//-----------------------------------------------------------------------------
View::View(QString const& name /* = QString() */, QString const& displayName /* = QString() */,
    QString const& description /* = QString() */) :
NameGroup(name, displayName, description),
isPresent_(),
envIdentifiers_(),
componentInstantiationRef_(),
designInstantiationRef_(),
designConfigurationInstantiationRef_()
{

}

//-----------------------------------------------------------------------------
// Function: View::View()
//-----------------------------------------------------------------------------
View::View(const View &other):
NameGroup(other),
isPresent_(other.isPresent_),
envIdentifiers_(other.envIdentifiers_),
componentInstantiationRef_(other.componentInstantiationRef_),
designInstantiationRef_(other.designInstantiationRef_),
designConfigurationInstantiationRef_(other.designConfigurationInstantiationRef_)
{

}

//-----------------------------------------------------------------------------
// Function: View::operator=()
//-----------------------------------------------------------------------------
View& View::operator=(const View &other)
{
    if (this != &other)
    {
        NameGroup::operator=(other);
        isPresent_ = other.isPresent_;
        envIdentifiers_ = other.envIdentifiers_;
        componentInstantiationRef_ = other.componentInstantiationRef_;
        designInstantiationRef_ = other.designInstantiationRef_;
        designConfigurationInstantiationRef_ = other.designConfigurationInstantiationRef_;
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: View::~View()
//-----------------------------------------------------------------------------
View::~View()
{

}

//-----------------------------------------------------------------------------
// Function: View::setEnvIdentifiers()
//-----------------------------------------------------------------------------
void View::setEnvIdentifiers( const QStringList& envIdentifiers )
{
	envIdentifiers_ = envIdentifiers;
}

//-----------------------------------------------------------------------------
// Function: View::getEnvIdentifiers()
//-----------------------------------------------------------------------------
QStringList View::getEnvIdentifiers() const
{
	return envIdentifiers_;
}

//-----------------------------------------------------------------------------
// Function: View::addEnvIdentifier()
//-----------------------------------------------------------------------------
void View::addEnvIdentifier(QString const& envIdentifier)
{
	envIdentifiers_.append(envIdentifier);
}

//-----------------------------------------------------------------------------
// Function: View::getIsPresent()
//-----------------------------------------------------------------------------
QString View::getIsPresent() const
{
    return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: View::setIsPresent()
//-----------------------------------------------------------------------------
void View::setIsPresent(QString const& newIsPresent)
{
    isPresent_ = newIsPresent;
}

//-----------------------------------------------------------------------------
// Function: View::getComponentInstantiationRef()
//-----------------------------------------------------------------------------
QString View::getComponentInstantiationRef() const
{
    return componentInstantiationRef_;
}

//-----------------------------------------------------------------------------
// Function: View::setComponentInstantiationRef()
//-----------------------------------------------------------------------------
void View::setComponentInstantiationRef(QString const& newInstantiationRef)
{
    componentInstantiationRef_ = newInstantiationRef;
}

//-----------------------------------------------------------------------------
// Function: View::getDesignInstantiationRef()
//-----------------------------------------------------------------------------
QString View::getDesignInstantiationRef() const
{
    return designInstantiationRef_;
}

//-----------------------------------------------------------------------------
// Function: View::setDesignInstantiationRef()
//-----------------------------------------------------------------------------
void View::setDesignInstantiationRef(QString const& newInstantiationRef)
{
    designInstantiationRef_ = newInstantiationRef;
}

//-----------------------------------------------------------------------------
// Function: View::getDesignConfigurationInstantiationRef()
//-----------------------------------------------------------------------------
QString View::getDesignConfigurationInstantiationRef() const
{
    return designConfigurationInstantiationRef_;
}

//-----------------------------------------------------------------------------
// Function: View::setDesignConfiguraitonInstantiationRef()
//-----------------------------------------------------------------------------
void View::setDesignConfigurationInstantiationRef(QString const& newInstantiationRef)
{
    designConfigurationInstantiationRef_ = newInstantiationRef;
}

//-----------------------------------------------------------------------------
// Function: View::isHierarchical()
//-----------------------------------------------------------------------------
bool View::isHierarchical() const
{
    return !designInstantiationRef_.isEmpty() || !designConfigurationInstantiationRef_.isEmpty();
}