//-----------------------------------------------------------------------------
// File: AlternateRegister.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.09.2015
//
// Description:
// Describes the ipxact:alternateRegister element.
//-----------------------------------------------------------------------------

#include "AlternateRegister.h"

//-----------------------------------------------------------------------------
// Function: AlternateRegister::AlternateRegister()
//-----------------------------------------------------------------------------
AlternateRegister::AlternateRegister(QString const& name, QString const& alternateGroup):
RegisterDefinition(name),
alternateGroups_(new QStringList())
{
    if (!alternateGroup.isEmpty())
    {
        alternateGroups_->append(alternateGroup);
    }
}

//-----------------------------------------------------------------------------
// Function: AlternateRegister::AlternateRegister()
//-----------------------------------------------------------------------------
AlternateRegister::AlternateRegister(const AlternateRegister& other):
RegisterDefinition(other),
alternateGroups_(new QStringList())
{
    copyAlternateGroups(other);
}

//-----------------------------------------------------------------------------
// Function: AlternateRegister::operator=()
//-----------------------------------------------------------------------------
AlternateRegister& AlternateRegister::operator=(const AlternateRegister& other)
{
    if (this != &other)
    {
        RegisterDefinition::operator=(other);

        alternateGroups_->clear();
        copyAlternateGroups(other);
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: AlternateRegister::~AlternateRegister()
//-----------------------------------------------------------------------------
AlternateRegister::~AlternateRegister()
{
	alternateGroups_.clear();
}

//-----------------------------------------------------------------------------
// Function: AlternateRegister::clone()
//-----------------------------------------------------------------------------
QSharedPointer<RegisterBase> AlternateRegister::clone()
{
    return QSharedPointer<RegisterBase>(new AlternateRegister(*this));
}

/*
//-----------------------------------------------------------------------------
// Function: alternateregister::isValid()
//-----------------------------------------------------------------------------
bool AlternateRegister::isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
    QStringList& errorList, const QString& parentIdentifier ) const {

	bool valid = true;

	if (name().isEmpty())
    {
		errorList.append(QObject::tr("No name specified for alternate register"
			" within %1").arg(parentIdentifier));
		valid = false;
	}

	if (alternateGroups_.isEmpty())
    {
		errorList.append(QObject::tr("At least one alternate group must be "
			"specified for alternate register %1 within %2").arg(name()).arg(
			parentIdentifier));
		valid = false;
	}

	if (alternateRegisterDef_ && !alternateRegisterDef_->isValid(componentChoices, errorList, 
        QObject::tr("alternate register %1").arg(name()))) {
		valid = false;
	}

//     ParameterValidator validator;
//     foreach (QSharedPointer<Parameter> param, parameters_)
//     {
//         errorList.append(validator.findErrorsIn(param.data(), 
//             QObject::tr("alternate register %1").arg(name()), componentChoices));
// 
//         if (!validator.validate(param.data(), componentChoices)) 
//         {
//             valid = false;
//         }
//     }

	return valid;
}*/
/*
//-----------------------------------------------------------------------------
// Function: alternateregister::isValid()
//-----------------------------------------------------------------------------
bool AlternateRegister::isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices) const
{
	if (name().isEmpty()) {
		return false;
	}

	if (alternateGroups_.isEmpty()) {
		return false;
	}

	if (alternateRegisterDef_ && !alternateRegisterDef_->isValid(componentChoices)) {
			return false;
	}

//     ParameterValidator validator;
//     foreach (QSharedPointer<Parameter> param, parameters_)
//     {
//         if (!validator.validate(param.data(), componentChoices))
//         {
//             return false;
//         }
//     }

	return true;
}*/

//-----------------------------------------------------------------------------
// Function: AlternateRegister::getAlternateGroups()
//-----------------------------------------------------------------------------
QSharedPointer<QStringList> AlternateRegister::getAlternateGroups() const
{
    return alternateGroups_;
}

//-----------------------------------------------------------------------------
// Function: AlternateRegister::setAlternateGroups()
//-----------------------------------------------------------------------------
void AlternateRegister::setAlternateGroups(QSharedPointer<QStringList> newAlternateGroups)
{
    alternateGroups_->clear();
    alternateGroups_ = newAlternateGroups;
}

//-----------------------------------------------------------------------------
// Function: AlternateRegister::copyAlternateGroups()
//-----------------------------------------------------------------------------
void AlternateRegister::copyAlternateGroups(const AlternateRegister& other)
{
    foreach (QString group, *other.alternateGroups_)
    {
        alternateGroups_->append(group);
    }
}