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
#include <IPXACTmodels/Component/MemoryArray.h>
#include <IPXACTmodels/Component/FieldReference.h>

//-----------------------------------------------------------------------------
// Function: Field::Field()
//-----------------------------------------------------------------------------
Field::Field(QString const& name /* = QString() */) :
NameGroup(name),
Extendable(),
id_(),
isPresent_(),
bitOffset_(),
resets_(new QList<QSharedPointer<FieldReset> >()),
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
fieldDefinitionRef_(other.fieldDefinitionRef_),
typeDefinitionsRef_(other.typeDefinitionsRef_),
resets_(new QList<QSharedPointer<FieldReset> >()),
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
    Utilities::copyList(enumeratedValues_, other.enumeratedValues_);
    Utilities::copyList(parameters_, other.parameters_);
    Utilities::copyList(resets_, other.resets_);
    Utilities::copyList(fieldAccessPolicies_, other.fieldAccessPolicies_);
    copyWriteValueConstraint(other);
    copyMemoryArray(other);
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
        typeIdentifier_ = other.typeIdentifier_;
        fieldDefinitionRef_ = other.fieldDefinitionRef_;
        typeDefinitionsRef_ = other.typeDefinitionsRef_;
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

        Utilities::copyList(enumeratedValues_, other.enumeratedValues_);
        Utilities::copyList(parameters_, other.parameters_);
        Utilities::copyList(resets_, other.resets_);
        Utilities::copyList(fieldAccessPolicies_, other.fieldAccessPolicies_);
        copyWriteValueConstraint(other);
        copyMemoryArray(other);
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: Field::~Field()
//-----------------------------------------------------------------------------
Field::~Field()
{
    resets_.clear();
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
// Function: Field::getMemoryArray()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryArray> Field::getMemoryArray() const
{
    return memoryArray_;
}

//-----------------------------------------------------------------------------
// Function: Field::setMemoryArray()
//-----------------------------------------------------------------------------
void Field::setMemoryArray(QSharedPointer<MemoryArray> memArray)
{
    memoryArray_ = memArray;
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
// Function: Field::getFieldDefinitionRef()
//-----------------------------------------------------------------------------
QString Field::getFieldDefinitionRef() const
{
    return fieldDefinitionRef_;
}

//-----------------------------------------------------------------------------
// Function: Field::setFieldDefinitionRef()
//-----------------------------------------------------------------------------
void Field::setFieldDefinitionRef(QString const& newFieldDefRef)
{
    fieldDefinitionRef_ = newFieldDefRef;
}

//-----------------------------------------------------------------------------
// Function: Field::getTypeDefinitionsRef()
//-----------------------------------------------------------------------------
QString Field::getTypeDefinitionsRef() const
{
    return typeDefinitionsRef_;
}

//-----------------------------------------------------------------------------
// Function: Field::setTypeDefinitionsRef()
//-----------------------------------------------------------------------------
void Field::setTypeDefinitionsRef(QString const& newTypeDefRef)
{
    typeDefinitionsRef_ = newTypeDefRef;
}

//-----------------------------------------------------------------------------
// Function: Field::getResets()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<FieldReset> > > Field::getResets() const
{
    return resets_;
}

//-----------------------------------------------------------------------------
// Function: Field::setResets()
//-----------------------------------------------------------------------------
void Field::setResets(QSharedPointer<QList<QSharedPointer<FieldReset> > > newResets)
{
    resets_->clear();
    resets_ = newResets;
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
// Function: Field::getFieldReference()
//-----------------------------------------------------------------------------
QSharedPointer<FieldReference> Field::getFieldReference() const
{
    return fieldReference_;
}

//-----------------------------------------------------------------------------
// Function: Field::setFieldReference()
//-----------------------------------------------------------------------------
void Field::setFieldReference(QSharedPointer<FieldReference> fieldRef)
{
    fieldReference_ = fieldRef;
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
// Function: Field::getFieldAccessPolicies()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<FieldAccessPolicy> > > Field::getFieldAccessPolicies() const
{
    return fieldAccessPolicies_;
}

//-----------------------------------------------------------------------------
// Function: Field::setFieldAccessPolicies()
//-----------------------------------------------------------------------------
void Field::setFieldAccessPolicies(QSharedPointer<QList<QSharedPointer<FieldAccessPolicy> > > newFieldAccessPolicies)
{
    fieldAccessPolicies_ = newFieldAccessPolicies;
}

//-----------------------------------------------------------------------------
// Function: Field::copyWriteValueConstraint()
//-----------------------------------------------------------------------------
void Field::copyWriteValueConstraint(const Field& other)
{
    writeValueConstraint_.clear();
    if (other.writeValueConstraint_)
    {
        writeValueConstraint_ =
            QSharedPointer<WriteValueConstraint>(new WriteValueConstraint(*other.writeValueConstraint_));
    }
}

//-----------------------------------------------------------------------------
// Function: Field::copyMemoryArray()
//-----------------------------------------------------------------------------
void Field::copyMemoryArray(Field const& other)
{
    memoryArray_.clear();
    if (other.memoryArray_)
    {
        memoryArray_ = QSharedPointer<MemoryArray>(new MemoryArray(*other.memoryArray_));
    }
}
