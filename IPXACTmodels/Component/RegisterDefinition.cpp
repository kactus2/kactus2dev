//-----------------------------------------------------------------------------
// File: RegisterDefinition.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.09.2015
//
// Description:
// Describes the base definition for ipxact:register and ipxact:alternateRegister.
//-----------------------------------------------------------------------------

#include "RegisterDefinition.h"
#include "Field.h"

//-----------------------------------------------------------------------------
// Function: RegisterDefinition::RegisterDefinition()
//-----------------------------------------------------------------------------
RegisterDefinition::RegisterDefinition(QString const& name /* = QString() */) :
RegisterBase(name),
volatile_(),
access_(General::ACCESS_COUNT),
fields_(new QList<QSharedPointer<Field> > ())
{

}

//-----------------------------------------------------------------------------
// Function: RegisterDefinition::RegisterDefinition()
//-----------------------------------------------------------------------------
RegisterDefinition::RegisterDefinition(const RegisterDefinition& other):
RegisterBase(other),
volatile_(other.volatile_),
access_(other.access_),
fields_(new QList<QSharedPointer<Field> > ())
{
    copyFields(other);
}

//-----------------------------------------------------------------------------
// Function: RegisterDefinition::operator=()
//-----------------------------------------------------------------------------
RegisterDefinition& RegisterDefinition::operator=( const RegisterDefinition& other)
{
    if (this != &other)
    {
        RegisterBase::operator=(other);

        volatile_ = other.volatile_;
        access_ = other.access_;

        fields_->clear();
        copyFields(other);
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: RegisterDefinition::~RegisterDefinition()
//-----------------------------------------------------------------------------
RegisterDefinition::~RegisterDefinition()
{

}

/*
bool RegisterDefinition::isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
    QStringList& errorList, 
								 const QString& parentIdentifier ) const {

	bool valid = true;

	if (size_ == 0) {
		errorList.append(QObject::tr("No size specified for %1").arg(parentIdentifier));
		valid = false;
	}

	if (reset_ && !reset_->isValid(errorList, parentIdentifier)) {
		valid = false;
	}

	foreach (QSharedPointer<Field> field, fields_) {
		if (!field->isValid(size_, componentChoices, errorList, parentIdentifier))
        {
			valid = false;
		}
	}

	return valid;
}*/
/*
bool RegisterDefinition::isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices) const
{
	if (size_ == 0)
    {
		return false;
	}

	if (reset_ && !reset_->isValid())
    {
		return false;
	}

	foreach (QSharedPointer<Field> field, fields_)
    {
		if (!field->isValid(size_, componentChoices))
        {
			return false;
		}
	}

	return true;
}*/

//-----------------------------------------------------------------------------
// Function: RegisterDefinition::getVolatile()
//-----------------------------------------------------------------------------
QString RegisterDefinition::getVolatile() const
{
    return volatile_.toString();
}

//-----------------------------------------------------------------------------
// Function: RegisterDefinition::setVolatile()
//-----------------------------------------------------------------------------
void RegisterDefinition::setVolatile(bool newVolatile)
{
    volatile_.setValue(newVolatile);
}

//-----------------------------------------------------------------------------
// Function: RegisterDefinition::clearVolatile()
//-----------------------------------------------------------------------------
void RegisterDefinition::clearVolatile()
{
    volatile_.setUnspecified();
}

//-----------------------------------------------------------------------------
// Function: RegisterDefinition::getAccess()
//-----------------------------------------------------------------------------
General::Access RegisterDefinition::getAccess() const
{
    return access_;
}

//-----------------------------------------------------------------------------
// Function: RegisterDefinition::setAccess()
//-----------------------------------------------------------------------------
void RegisterDefinition::setAccess(General::Access newAccessValue)
{
    access_ = newAccessValue;
}

//-----------------------------------------------------------------------------
// Function: RegisterDefinition::getFields()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Field> > > RegisterDefinition::getFields() const
{
    return fields_;
}

//-----------------------------------------------------------------------------
// Function: RegisterDefinition::setFields()
//-----------------------------------------------------------------------------
void RegisterDefinition::setFields(QSharedPointer<QList<QSharedPointer<Field> > > newFields)
{
    fields_->clear();
    fields_ = newFields;
}

//-----------------------------------------------------------------------------
// Function: RegisterDefinition::copyFields()
//-----------------------------------------------------------------------------
void RegisterDefinition::copyFields(const RegisterDefinition& other)
{
    foreach (QSharedPointer<Field> field, *other.fields_)
    {
        if (field)
        {
            QSharedPointer<Field> copy = QSharedPointer<Field>(new Field(*field.data()));
            fields_->append(copy);
        }
    }
}