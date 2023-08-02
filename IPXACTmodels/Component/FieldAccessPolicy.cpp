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

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::AccessRestriction::AccessRestriction()
//-----------------------------------------------------------------------------
FieldAccessPolicy::AccessRestriction::AccessRestriction(AccessRestriction const& other) :
    modeRefs_(new QList<QSharedPointer<ModeReference> >()),
    readAccessMask_(other.readAccessMask_),
    writeAccessMask_(other.writeAccessMask_)
{
    for (auto const& modeRef : *other.modeRefs_)
    {
        if (modeRef)
        {
            auto copy = QSharedPointer<ModeReference>(new ModeReference(*modeRef));
            modeRefs_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::FieldAccessPolicy()
//-----------------------------------------------------------------------------
FieldAccessPolicy::FieldAccessPolicy():
    Extendable()
{

}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::FieldAccessPolicy()
//-----------------------------------------------------------------------------
FieldAccessPolicy::FieldAccessPolicy(FieldAccessPolicy const& other) :
    Extendable(other),
    modeReferences_(new QList<QSharedPointer<ModeReference> >()),
    fieldAccessPolicyDefinitionRef_(other.fieldAccessPolicyDefinitionRef_),
    fieldAccessPolicyTypeDefinitionRef_(other.fieldAccessPolicyTypeDefinitionRef_),
    access_(other.access_),
    modifiedWrite_(other.modifiedWrite_),
    modifiedWriteModify_(other.modifiedWriteModify_),
    writeValueConstraint_(new WriteValueConstraint(*other.writeValueConstraint_)),
    readAction_(other.readAction_),
    readActionModify_(other.readActionModify_),
    readResponse_(other.readResponse_),
    broadcasts_(new QList<QSharedPointer<FieldReference> >()),
    accessRestrictions_(new QList<QSharedPointer<AccessRestriction> >()),
    testable_(other.testable_),
    testConstraint_(other.testConstraint_),
    reserved_(other.reserved_)
{
    for (auto const& modeRef : *other.modeReferences_)
    {
        if (modeRef)
        {
            auto copy = QSharedPointer<ModeReference>(new ModeReference(*modeRef));
            modeReferences_->append(copy);
        }
    }

    for (auto const& broadcast : *other.broadcasts_)
    {
        if (broadcast)
        {
            auto copy = QSharedPointer<FieldReference>(new FieldReference(*broadcast));
            broadcasts_->append(copy);
        }
    }

    for (auto const& restriction : *other.getAccessRestrictions())
    {
        if (restriction)
        {
            auto copy = QSharedPointer<AccessRestriction>(new AccessRestriction(*restriction));
            accessRestrictions_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::getModeReferences()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<ModeReference> > > FieldAccessPolicy::getModeReferences() const
{
    return modeReferences_;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::setModeReferences()
//-----------------------------------------------------------------------------
void FieldAccessPolicy::setModeReferences(QSharedPointer<QList<QSharedPointer<ModeReference> > > modeRefs)
{
    modeReferences_ = modeRefs;
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
// Function: FieldAccessPolicy::getAccess()
//-----------------------------------------------------------------------------
AccessTypes::Access FieldAccessPolicy::getAccess() const
{
    return access_;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicy::setAccess()
//-----------------------------------------------------------------------------
void FieldAccessPolicy::setAccess(AccessTypes::Access newAccess)
{
    access_ = newAccess;
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

