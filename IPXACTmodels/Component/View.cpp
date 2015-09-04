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

/*
bool View::isValid( const QStringList& fileSetNames, 
                    QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
				   QStringList& errorList, 
				   const QString& parentIdentifier ) const {

	bool valid = true;
	const QString thisIdentifier(QObject::tr("view %1").arg(name()));

	if (name().isEmpty()) {
		errorList.append(QObject::tr("No name specified for view within %1").arg(parentIdentifier));
		valid = false;
	}

	if (envIdentifiers_.isEmpty()) {
		errorList.append(QObject::tr("No environment identifier specified for "
			"view %1 within %2").arg(name()).arg(parentIdentifier));
		valid = false;
	}

	// if this is flat view then check the flat elements.
	if (!hierarchyRef_.isValid()) {
		
		foreach (QSharedPointer<FileBuilder> fileBuilder, defaultFileBuilders_) {
			if (!fileBuilder->isValid(errorList, thisIdentifier)) {
				valid = false;
			}
		}

		// make sure the referenced file sets are found
		foreach (QString fileSetRef, fileSetRefs_) {
			if (!fileSetNames.contains(fileSetRef)) {
				errorList.append(QObject::tr("View %1 contained reference to file"
					" set %2 which is not found within %3").arg(
					name()).arg(fileSetRef).arg(parentIdentifier));
				valid = false;
			}
		}

//         ParameterValidator validator;
//         foreach (QSharedPointer<Parameter> param, *parameters_)
//         {
//             errorList.append(validator.findErrorsIn(param.data(), thisIdentifier, componentChoices));
//             if (!validator.validate(param.data(), componentChoices)) 
//             {
//                 valid = false;
//             }
// 		}
	}
	
	return valid;
}
*/
/*
bool View::isValid( const QStringList& fileSetNames, 
    QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices ) const 
{
	if (name().isEmpty()) 
    {
		return false;
	}

	if (envIdentifiers_.isEmpty())
    {
		return false;
	}

	// if this is flat view then check the flat elements.
	if (!hierarchyRef_.isValid()) {

		foreach (QSharedPointer<FileBuilder> fileBuilder, defaultFileBuilders_) {
			if (!fileBuilder->isValid()) {
				return false;
			}
		}

		// make sure the referenced file sets are found
		foreach (QString fileSetRef, fileSetRefs_) {
			if (!fileSetNames.contains(fileSetRef)) {
				return false;
			}
		}

//         ParameterValidator validator;
//         foreach (QSharedPointer<Parameter> param, *parameters_)
//         {
//             if (!validator.validate(param.data(), componentChoices)) 
//             {
//                 return false;
//             }
//         }
	}

	return true;
}
*/

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
const QStringList& View::getEnvIdentifiers() const
{
	return envIdentifiers_;
}

//-----------------------------------------------------------------------------
// Function: View::getEnvIdentifiers()
//-----------------------------------------------------------------------------
QStringList& View::getEnvIdentifiers()
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