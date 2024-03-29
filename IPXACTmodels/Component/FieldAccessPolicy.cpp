//-----------------------------------------------------------------------------
// File: FieldAccessPolicy.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 31.7.2023
//
// Description:
// Describes the ipxact:fieldAccessPolicy element.
//-----------------------------------------------------------------------------

#include "FieldAccessPolicy.h"

#include <IPXACTmodels/utilities/Copy.h>

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::AccessRestriction::AccessRestriction()
//-----------------------------------------------------------------------------
FieldAccessPolicy::AccessRestriction::AccessRestriction(AccessRestriction const& other) :
    modeRefs_(new QList<QSharedPointer<ModeReference> >()),
    readAccessMask_(other.readAccessMask_),
    writeAccessMask_(other.writeAccessMask_)
{
    Copy::copyList(other.modeRefs_, modeRefs_);
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::AccessRestriction::operator=()
//-----------------------------------------------------------------------------
FieldAccessPolicy::AccessRestriction& FieldAccessPolicy::AccessRestriction::operator=(FieldAccessPolicy::AccessRestriction const& other)
{
    if (this != &other)
    {
        readAccessMask_ = other.readAccessMask_;
        writeAccessMask_ = other.writeAccessMask_;

        modeRefs_->clear();
        Copy::copyList(other.modeRefs_, modeRefs_);
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::FieldAccessPolicy()
//-----------------------------------------------------------------------------
FieldAccessPolicy::FieldAccessPolicy():
    AccessPolicy()
{

}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::FieldAccessPolicy()
//-----------------------------------------------------------------------------
FieldAccessPolicy::FieldAccessPolicy(FieldAccessPolicy const& other) :
    AccessPolicy(other),
    fieldAccessPolicyDefinitionRef_(other.fieldAccessPolicyDefinitionRef_),
    fieldAccessPolicyTypeDefinitionRef_(other.fieldAccessPolicyTypeDefinitionRef_),
    modifiedWrite_(other.modifiedWrite_),
    modifiedWriteModify_(other.modifiedWriteModify_),
    writeValueConstraint_(),
    readAction_(other.readAction_),
    readActionModify_(other.readActionModify_),
    readResponse_(other.readResponse_),
    broadcasts_(new QList<QSharedPointer<FieldReference> >()),
    accessRestrictions_(new QList<QSharedPointer<AccessRestriction> >()),
    testable_(other.testable_),
    testConstraint_(other.testConstraint_),
    reserved_(other.reserved_)
{
    Copy::copyList(other.broadcasts_, broadcasts_);
    Copy::copyList(other.accessRestrictions_, accessRestrictions_);
    copyWriteValueConstraint(other);
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::operator=()
//-----------------------------------------------------------------------------
FieldAccessPolicy& FieldAccessPolicy::operator=(FieldAccessPolicy const& other)
{
    if (this != &other)
    {
        AccessPolicy::operator=(other);
        fieldAccessPolicyDefinitionRef_ = other.fieldAccessPolicyDefinitionRef_;
        fieldAccessPolicyTypeDefinitionRef_ = other.fieldAccessPolicyTypeDefinitionRef_;
        modifiedWrite_ = other.modifiedWrite_;
        modifiedWriteModify_ = other.modifiedWriteModify_;
        readAction_ = other.readAction_;
        readActionModify_ = other.readActionModify_;
        readResponse_ = other.readResponse_;
        testable_ = other.testable_;
        testConstraint_ = other.testConstraint_;
        reserved_ = other.reserved_;

        broadcasts_->clear();
        Copy::copyList(other.broadcasts_, broadcasts_);

        accessRestrictions_->clear();
        Copy::copyList(other.accessRestrictions_, accessRestrictions_);
        copyWriteValueConstraint(other);
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::getModifiedWrite()
//-----------------------------------------------------------------------------
General::ModifiedWrite FieldAccessPolicy::getModifiedWrite() const
{
    return modifiedWrite_;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::setModifiedWrite()
//-----------------------------------------------------------------------------
void FieldAccessPolicy::setModifiedWrite(General::ModifiedWrite modifiedWrite)
{
    modifiedWrite_ = modifiedWrite;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::getModifiedWriteModify()
//-----------------------------------------------------------------------------
QString FieldAccessPolicy::getModifiedWriteModify() const
{
    return modifiedWriteModify_;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::setModifiedWriteModify()
//-----------------------------------------------------------------------------
void FieldAccessPolicy::setModifiedWriteModify(QString const& modify)
{
    modifiedWriteModify_ = modify;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::getWriteValueConstraint()
//-----------------------------------------------------------------------------
QSharedPointer<WriteValueConstraint> FieldAccessPolicy::getWriteValueConstraint() const
{
    return writeValueConstraint_;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::setWriteValueConstraint()
//-----------------------------------------------------------------------------
void FieldAccessPolicy::setWriteValueConstraint(QSharedPointer<WriteValueConstraint> constraint)
{
    writeValueConstraint_ = constraint;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::getReadAction()
//-----------------------------------------------------------------------------
General::ReadAction FieldAccessPolicy::getReadAction() const
{
    return readAction_;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::setReadAction()
//-----------------------------------------------------------------------------
void FieldAccessPolicy::setReadAction(General::ReadAction readAction)
{
    readAction_ = readAction;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::getReadActionModify()
//-----------------------------------------------------------------------------
QString FieldAccessPolicy::getReadActionModify() const
{
    return readActionModify_;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::setReadActionModify()
//-----------------------------------------------------------------------------
void FieldAccessPolicy::setReadActionModify(QString const& readActionModify)
{
    readActionModify_ = readActionModify;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::getTestable()
//-----------------------------------------------------------------------------
BooleanValue FieldAccessPolicy::getTestable() const
{
    return testable_;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::setTestable()
//-----------------------------------------------------------------------------
void FieldAccessPolicy::setTestable(bool newTestable)
{
    testable_.setValue(newTestable);
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::clearTestable()
//-----------------------------------------------------------------------------
void FieldAccessPolicy::clearTestable()
{
    testable_.setUnspecified();
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::getReadResponse()
//-----------------------------------------------------------------------------
QString FieldAccessPolicy::getReadResponse() const
{
    return readResponse_;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::setReadResponse()
//-----------------------------------------------------------------------------
void FieldAccessPolicy::setReadResponse(QString const& response)
{
    readResponse_ = response;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::getTestConstraint()
//-----------------------------------------------------------------------------
General::TestConstraint FieldAccessPolicy::getTestConstraint() const
{
    return testConstraint_;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::setTestConstraint()
//-----------------------------------------------------------------------------
void FieldAccessPolicy::setTestConstraint(General::TestConstraint constraint)
{
    testConstraint_ = constraint;
}

// Function: FieldAccessPolicy::getReserved()
//-----------------------------------------------------------------------------
QString FieldAccessPolicy::getReserved() const
{
    return reserved_;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::setReserved()
//-----------------------------------------------------------------------------
void FieldAccessPolicy::setReserved(QString const& reserved)
{
    reserved_ = reserved;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::getFieldAccessPolicyDefinitionRef()
//-----------------------------------------------------------------------------
QString FieldAccessPolicy::getFieldAccessPolicyDefinitionRef() const
{
    return fieldAccessPolicyDefinitionRef_;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::setFieldAccessPolicyDefinitionRef()
//-----------------------------------------------------------------------------
void FieldAccessPolicy::setFieldAccessPolicyDefinitionRef(QString const& definitionRef)
{
    fieldAccessPolicyDefinitionRef_ = definitionRef;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::getFieldAccessPolicyTypeDefinitionRef()
//-----------------------------------------------------------------------------
QString FieldAccessPolicy::getFieldAccessPolicyTypeDefinitionRef() const
{
    return fieldAccessPolicyTypeDefinitionRef_;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::setFieldAccessPolicyTypeDefinitionRef()
//-----------------------------------------------------------------------------
void FieldAccessPolicy::setFieldAccessPolicyTypeDefinitionRef(QString const& typeDefRef)
{
    fieldAccessPolicyTypeDefinitionRef_ = typeDefRef;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::getAccessRestrictions()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<FieldAccessPolicy::FieldAccessPolicy::AccessRestriction> > > FieldAccessPolicy::getAccessRestrictions() const
{
    return accessRestrictions_;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::setAccessRestrictions()
//-----------------------------------------------------------------------------
void FieldAccessPolicy::setAccessRestrictions(QSharedPointer<QList<QSharedPointer<AccessRestriction> > > newAccessRestrictions)
{
    accessRestrictions_ = newAccessRestrictions;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::copyWriteValueConstraint()
//-----------------------------------------------------------------------------
void FieldAccessPolicy::copyWriteValueConstraint(FieldAccessPolicy const& other)
{
    writeValueConstraint_.clear();
    if (other.writeValueConstraint_)
    {
        writeValueConstraint_ = QSharedPointer<WriteValueConstraint>(new WriteValueConstraint(*other.writeValueConstraint_));
    }
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::getBroadcasts()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<FieldReference> > > FieldAccessPolicy::getBroadcasts() const
{
    return broadcasts_;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::setBroadcasts()
//-----------------------------------------------------------------------------
void FieldAccessPolicy::setBroadcasts(QSharedPointer<QList<QSharedPointer<FieldReference> > > broadcasts)
{
    broadcasts_ = broadcasts;
}

