//-----------------------------------------------------------------------------
// File: Field.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 24.09.2015
//
// Description:
// Describes the ipxact:field element.
//-----------------------------------------------------------------------------

#include "Field.h"

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/Component/EnumeratedValue.h>
#include <IPXACTmodels/Component/WriteValueConstraint.h>

//-----------------------------------------------------------------------------
// Function: Field::Field()
//-----------------------------------------------------------------------------
Field::Field(QString const& name /* = QString() */) :
NameGroup(name),
Extendable(),
id_(),
isPresent_(),
bitOffset_(),
resetTypeReference_(),
resetValue_(),
resetMask_(),
typeIdentifier_(),
bitWidth_(),
volatile_(),
access_(AccessTypes::ACCESS_COUNT),
enumeratedValues_(new QList<QSharedPointer<EnumeratedValue> > ()),
modifiedWrite_(General::MODIFIED_WRITE_COUNT),
modifiedWriteModify_(),
writeValueConstraint_(),
readAction_(General::READ_ACTION_COUNT),
readActionModify_(),
testable_(),
testConstraint_(General::TESTCONSTRAINT_COUNT),
reserved_(),
parameters_(new QList<QSharedPointer<Parameter> > ())
{

}

//-----------------------------------------------------------------------------
// Function: Field::Field()
//-----------------------------------------------------------------------------
Field::Field(const Field& other) :
NameGroup(other),
Extendable(other),
id_(other.id_),
isPresent_(other.isPresent_),
bitOffset_(other.bitOffset_),
resetTypeReference_(other.resetTypeReference_),
resetValue_(other.resetValue_),
resetMask_(other.resetMask_),
typeIdentifier_(other.typeIdentifier_),
bitWidth_(other.bitWidth_),
volatile_(other.volatile_),
access_(other.access_),
enumeratedValues_(new QList<QSharedPointer<EnumeratedValue> > ()),
modifiedWrite_(other.modifiedWrite_),
modifiedWriteModify_(other.modifiedWriteModify_),
writeValueConstraint_(),
readAction_(other.readAction_),
readActionModify_(other.readActionModify_),
testable_(other.testable_),
testConstraint_(other.testConstraint_),
reserved_(other.reserved_),
parameters_(new QList<QSharedPointer<Parameter> > ())
{
    copyEnumeratedValues(other);
    copyParameters(other);
    copyWriteValueConstraint(other);
}

//-----------------------------------------------------------------------------
// Function: Field::operator=()
//-----------------------------------------------------------------------------
Field& Field::operator=( const Field& other )
{
    if (this != &other)
    {
        NameGroup::operator=(other);
        Extendable::operator=(other);
        id_ = other.id_;
        isPresent_ = other.isPresent_;
        bitOffset_ = other.bitOffset_;
        resetTypeReference_ = other.resetTypeReference_;
        resetValue_ = other.resetValue_;
        resetMask_ = other.resetMask_;
        typeIdentifier_ = other.typeIdentifier_;
        bitWidth_ = other.bitWidth_;
        volatile_ = other.volatile_;
        access_ = other.access_;
        modifiedWrite_ = other.modifiedWrite_;
        modifiedWriteModify_ = other.modifiedWriteModify_;
        readAction_ = other.readAction_;
        readActionModify_ = other.readActionModify_;
        testable_ = other.testable_;
        testConstraint_ = other.testConstraint_;
        reserved_ = other.reserved_;

        enumeratedValues_->clear();
        copyEnumeratedValues(other);
        writeValueConstraint_.clear();
        copyWriteValueConstraint(other);
        parameters_->clear();
        copyParameters(other);
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: Field::~Field()
//-----------------------------------------------------------------------------
Field::~Field()
{
    enumeratedValues_.clear();
    parameters_.clear();
    writeValueConstraint_.clear();
}

//-----------------------------------------------------------------------------
// Function: Field::getId()
//-----------------------------------------------------------------------------
QString Field::getId() const
{
    return id_;
}

//-----------------------------------------------------------------------------
// Function: Field::setId()
//-----------------------------------------------------------------------------
void Field::setId(QString const& id)
{
    id_ = id;
}

//-----------------------------------------------------------------------------
// Function: Field::getIsPresent()
//-----------------------------------------------------------------------------
QString Field::getIsPresent() const
{
    return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: Field::setIsPresent()
//-----------------------------------------------------------------------------
void Field::setIsPresent(QString const& newIsPresent)
{
    isPresent_ = newIsPresent;
}

//-----------------------------------------------------------------------------
// Function: Field::getBitOffset()
//-----------------------------------------------------------------------------
QString Field::getBitOffset() const
{
    return bitOffset_;
}

//-----------------------------------------------------------------------------
// Function: Field::setBitOffset()
//-----------------------------------------------------------------------------
void Field::setBitOffset(QString const& newBitOffset)
{
    bitOffset_ = newBitOffset;
}

//-----------------------------------------------------------------------------
// Function: Field::getResetTypeReference()
//-----------------------------------------------------------------------------
QString Field::getResetTypeReference() const
{
    return resetTypeReference_;
}

//-----------------------------------------------------------------------------
// Function: Field::setResetTypeReference()
//-----------------------------------------------------------------------------
void Field::setResetTypeReference(QString const& newResetTypeReference)
{
    resetTypeReference_ = newResetTypeReference;
}

//-----------------------------------------------------------------------------
// Function: field::getResetValue()
//-----------------------------------------------------------------------------
QString Field::getResetValue() const
{
    return resetValue_;
}

//-----------------------------------------------------------------------------
// Function: field::setResetValue()
//-----------------------------------------------------------------------------
void Field::setResetValue(QString const& newResetValue)
{
    resetValue_ = newResetValue;
}

//-----------------------------------------------------------------------------
// Function: field::getResetMask()
//-----------------------------------------------------------------------------
QString Field::getResetMask() const
{
    return resetMask_;
}

//-----------------------------------------------------------------------------
// Function: field::setResetMask()
//-----------------------------------------------------------------------------
void Field::setResetMask(QString const& newResetMask)
{
    resetMask_ = newResetMask;
}

//-----------------------------------------------------------------------------
// Function: Field::getTypeIdentifier()
//-----------------------------------------------------------------------------
QString Field::getTypeIdentifier() const
{
    return typeIdentifier_;
}

//-----------------------------------------------------------------------------
// Function: Field::setTypeIdentifier()
//-----------------------------------------------------------------------------
void Field::setTypeIdentifier(const QString& typeIdentifier)
{
    typeIdentifier_ = typeIdentifier;
}

//-----------------------------------------------------------------------------
// Function: Field::getBitWidth()
//-----------------------------------------------------------------------------
QString Field::getBitWidth() const
{
    return bitWidth_;
}

//-----------------------------------------------------------------------------
// Function: Field::setBitWidth()
//-----------------------------------------------------------------------------
void Field::setBitWidth(QString const& newBitWidth)
{
    bitWidth_ = newBitWidth;
}

//-----------------------------------------------------------------------------
// Function: Field::getVolatile()
//-----------------------------------------------------------------------------
BooleanValue Field::getVolatile() const
{
    return volatile_;
}

//-----------------------------------------------------------------------------
// Function: Field::setVolatile()
//-----------------------------------------------------------------------------
void Field::setVolatile( bool volatileValue )
{
    volatile_.setValue(volatileValue);
}

//-----------------------------------------------------------------------------
// Function: Field::clearVolatile()
//-----------------------------------------------------------------------------
void Field::clearVolatile()
{
    volatile_.setUnspecified();
}

//-----------------------------------------------------------------------------
// Function: Field::getAccess()
//-----------------------------------------------------------------------------
AccessTypes::Access Field::getAccess() const
{
    return access_;
}

//-----------------------------------------------------------------------------
// Function: Field::setAccess()
//-----------------------------------------------------------------------------
void Field::setAccess( AccessTypes::Access access )
{
    access_ = access;
}

//-----------------------------------------------------------------------------
// Function: Field::getEnumeratedValues()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<EnumeratedValue> > > Field::getEnumeratedValues() const
{
    return enumeratedValues_;
}

//-----------------------------------------------------------------------------
// Function: Field::setEnumeratedValues()
//-----------------------------------------------------------------------------
void Field::setEnumeratedValues(QSharedPointer<QList<QSharedPointer<EnumeratedValue> > > newEnumeratedValues)
{
        enumeratedValues_ = newEnumeratedValues;
}

//-----------------------------------------------------------------------------
// Function: Field::getModifiedWrite()
//-----------------------------------------------------------------------------
General::ModifiedWrite Field::getModifiedWrite() const
{
    return modifiedWrite_;
}

//-----------------------------------------------------------------------------
// Function: Field::setModifiedWrite()
//-----------------------------------------------------------------------------
void Field::setModifiedWrite(General::ModifiedWrite const& newModifiedWriteValue)
{
    modifiedWrite_ = newModifiedWriteValue;
}

//-----------------------------------------------------------------------------
// Function: Field::getModifiedWriteModify()
//-----------------------------------------------------------------------------
QString Field::getModifiedWriteModify() const
{
    return modifiedWriteModify_;
}

//-----------------------------------------------------------------------------
// Function: Field::setModifiedWriteModify()
//-----------------------------------------------------------------------------
void Field::setModifiedWriteModify(QString const& newModify)
{
    modifiedWriteModify_ = newModify;
}

//-----------------------------------------------------------------------------
// Function: Field::getWriteConstraint()
//-----------------------------------------------------------------------------
QSharedPointer<WriteValueConstraint> Field::getWriteConstraint() const
{
    return writeValueConstraint_;
}

//-----------------------------------------------------------------------------
// Function: Field::setWriteConstraint()
//-----------------------------------------------------------------------------
void Field::setWriteConstraint(QSharedPointer<WriteValueConstraint> newWriteValueConstraint)
{
    writeValueConstraint_ = newWriteValueConstraint;
}

//-----------------------------------------------------------------------------
// Function: Field::getReadAction()
//-----------------------------------------------------------------------------
General::ReadAction Field::getReadAction() const
{
    return readAction_;
}

//-----------------------------------------------------------------------------
// Function: Field::setReadAction()
//-----------------------------------------------------------------------------
void Field::setReadAction(General::ReadAction const& readAction)
{
    readAction_ = readAction;
}

//-----------------------------------------------------------------------------
// Function: Field::getReadActionModify()
//-----------------------------------------------------------------------------
QString Field::getReadActionModify() const
{
    return readActionModify_;
}

//-----------------------------------------------------------------------------
// Function: Field::setReadActionModify()
//-----------------------------------------------------------------------------
void Field::setReadActionModify(QString const& newModify)
{
    readActionModify_ = newModify;
}

//-----------------------------------------------------------------------------
// Function: Field::getTestable()
//-----------------------------------------------------------------------------
BooleanValue Field::getTestable() const
{
    return testable_;
}

//-----------------------------------------------------------------------------
// Function: Field::setTestable()
//-----------------------------------------------------------------------------
void Field::setTestable(bool newTestable)
{
    testable_.setValue(newTestable);
}

//-----------------------------------------------------------------------------
// Function: Field::clearTestable()
//-----------------------------------------------------------------------------
void Field::clearTestable()
{
    testable_.setUnspecified();
}

//-----------------------------------------------------------------------------
// Function: Field::getTestConstraint()
//-----------------------------------------------------------------------------
General::TestConstraint Field::getTestConstraint() const
{
    return testConstraint_;
}

//-----------------------------------------------------------------------------
// Function: Field::setTestConstraint()
//-----------------------------------------------------------------------------
void Field::setTestConstraint(General::TestConstraint const& newTestContraint)
{
    testConstraint_ = newTestContraint;
}

//-----------------------------------------------------------------------------
// Function: Field::getReserved()
//-----------------------------------------------------------------------------
QString Field::getReserved() const
{
    return reserved_;
}

//-----------------------------------------------------------------------------
// Function: Field::setReserved()
//-----------------------------------------------------------------------------
void Field::setReserved(QString const& newReserved)
{
    reserved_ = newReserved;
}

//-----------------------------------------------------------------------------
// Function: Field::getParameters()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Parameter> > > Field::getParameters() const
{
    return parameters_;
}

//-----------------------------------------------------------------------------
// Function: Field::setParameters()
//-----------------------------------------------------------------------------
void Field::setParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters)
{
    parameters_->clear();
    parameters_ = newParameters;
}

//-----------------------------------------------------------------------------
// Function: Field::copyEnumeratedValues()
//-----------------------------------------------------------------------------
void Field::copyEnumeratedValues(const Field& other)
{
    foreach (QSharedPointer<EnumeratedValue> enumValue, *other.enumeratedValues_)
    {
        if (enumValue)
        {
            QSharedPointer<EnumeratedValue> copy =
                QSharedPointer<EnumeratedValue>(new EnumeratedValue(*enumValue.data()));
            enumeratedValues_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Field::copyParameters()
//-----------------------------------------------------------------------------
void Field::copyParameters(const Field& other)
{
    foreach (QSharedPointer<Parameter> param, *other.parameters_)
    {
        if (param)
        {
            QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(new Parameter(*param.data()));
            parameters_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Field::copyWriteValueConstraint()
//-----------------------------------------------------------------------------
void Field::copyWriteValueConstraint(const Field& other)
{
    if (other.writeValueConstraint_)
    {
        writeValueConstraint_ =
            QSharedPointer<WriteValueConstraint>(new WriteValueConstraint(*other.writeValueConstraint_.data()));
    }
}
