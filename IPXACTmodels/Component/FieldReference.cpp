//-----------------------------------------------------------------------------
// File: FieldReference.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 26.7.2023
//
// Description:
// Reference to a field in the encapsulating component.
//-----------------------------------------------------------------------------

#include "FieldReference.h"

const QString FieldReference::TYPES_AS_STRINGS[] = {
    QLatin1String("addressSpaceRef"),
    QLatin1String("memoryMapRef"),
    QLatin1String("memoryRemapRef"),
    QLatin1String("bankRef"),
    QLatin1String("addressBlockRef"),
    QLatin1String("registerFileRef"),
    QLatin1String("registerRef"),
    QLatin1String("alternateRegisterRef"),
    QLatin1String("fieldRef"),
    QLatin1String("")
};

//-----------------------------------------------------------------------------
// Function: FieldReference::FieldReference()
//-----------------------------------------------------------------------------
FieldReference::FieldReference()
{

}

//-----------------------------------------------------------------------------
// Function: FieldReference::FieldReference()
//-----------------------------------------------------------------------------
FieldReference::FieldReference(FieldReference const& other) :
    addressSpaceRef_(other.addressSpaceRef_),
    memoryMapRef_(other.memoryMapRef_),
    memoryRemapRef_(other.memoryRemapRef_),
    bankRefs_(other.bankRefs_),
    addressBlockRef_(other.addressBlockRef_),
    registerFileRefs_(other.registerFileRefs_),
    registerRef_(other.registerRef_),
    alternateRegisterRef_(other.alternateRegisterRef_),
    fieldRef_(other.fieldRef_)
{

}

//-----------------------------------------------------------------------------
// Function: FieldReference::setReference()
//-----------------------------------------------------------------------------
void FieldReference::setReference(QSharedPointer<IndexedReference> reference, FieldReference::Type refType)
{
    switch (refType)
    {
    case FieldReference::Type::ADDRESS_SPACE:
        addressSpaceRef_ = reference;
        break;
    case FieldReference::Type::MEMORY_MAP:
        memoryMapRef_ = reference;
        break;
    case FieldReference::Type::MEMORY_REMAP:
        memoryRemapRef_ = reference;
        break;
    case FieldReference::Type::BANK:
        if (!bankRefs_)
        {
            bankRefs_ = QSharedPointer<QList<QSharedPointer<IndexedReference> > >(
                new QList<QSharedPointer<IndexedReference> >());
        }
        bankRefs_->append(reference);
        break;
    case FieldReference::Type::ADDRESS_BLOCK:
        addressBlockRef_ = reference;
        break;
    case FieldReference::Type::REGISTER_FILE:
        if (!registerFileRefs_)
        {
            registerFileRefs_ = QSharedPointer<QList<QSharedPointer<IndexedReference> > >(
                new QList<QSharedPointer<IndexedReference> >());
        }
        registerFileRefs_->append(reference);
        break;
    case FieldReference::Type::REGISTER:
        registerRef_ = reference;
        break;
    case FieldReference::Type::ALTERNATE_REGISTER:
        alternateRegisterRef_ = reference;
        break;
    case FieldReference::Type::FIELD:
        fieldRef_ = reference;
        break;
    default:
        break;
    }
}

//-----------------------------------------------------------------------------
// Function: FieldReference::getReference()
//-----------------------------------------------------------------------------
QSharedPointer<FieldReference::IndexedReference> FieldReference::getReference(Type refType) const
{
    switch (refType)
    {
    case FieldReference::Type::ADDRESS_SPACE:
        return addressSpaceRef_;
    case FieldReference::Type::MEMORY_MAP:
        return memoryMapRef_;
    case FieldReference::Type::MEMORY_REMAP:
        return memoryRemapRef_;
    case FieldReference::Type::ADDRESS_BLOCK:
        return addressBlockRef_;
    case FieldReference::Type::REGISTER:
        return registerRef_;
    case FieldReference::Type::ALTERNATE_REGISTER:
        return alternateRegisterRef_;
    case FieldReference::Type::FIELD:
        return fieldRef_;
    default:
        return nullptr;
    }
}

//-----------------------------------------------------------------------------
// Function: FieldReference::getMultipleReference()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<FieldReference::IndexedReference> > > FieldReference::getMultipleReference(Type refType) const
{
    QSharedPointer<QList<QSharedPointer<FieldReference::IndexedReference> > > referenceList(
        new QList<QSharedPointer<FieldReference::IndexedReference> >());

    switch (refType)
    {
    case FieldReference::Type::ADDRESS_SPACE:
        referenceList->append(addressSpaceRef_);
        break;
    case FieldReference::Type::MEMORY_MAP:
        referenceList->append(memoryMapRef_);
        break;
    case FieldReference::Type::MEMORY_REMAP:
        referenceList->append(memoryRemapRef_);
        break;
    case FieldReference::Type::BANK:
        return bankRefs_;
    case FieldReference::Type::ADDRESS_BLOCK:
        referenceList->append(addressBlockRef_);
        break;
    case FieldReference::Type::REGISTER_FILE:
        return registerFileRefs_;
    case FieldReference::Type::REGISTER:
        referenceList->append(registerRef_);
        break;
    case FieldReference::Type::ALTERNATE_REGISTER:
        referenceList->append(alternateRegisterRef_);
        break;
    case FieldReference::Type::FIELD:
        referenceList->append(fieldRef_);
        break;
    default:
        break;
    }

    return referenceList;
}

//-----------------------------------------------------------------------------
// Function: FieldReference::str2Type()
//-----------------------------------------------------------------------------
FieldReference::Type FieldReference::str2Type(QString const& refTypeStr)
{
    for (unsigned int i = 0; i < FieldReference::Type::REFERENCE_TYPE_COUNT; ++i)
    {
        if (refTypeStr.compare(TYPES_AS_STRINGS[i]) == 0)
        {
            return static_cast<FieldReference::Type>(i);
        }
    }

    return FieldReference::Type::REFERENCE_TYPE_COUNT;
}

//-----------------------------------------------------------------------------
// Function: FieldReference::type2Str()
//-----------------------------------------------------------------------------
QString FieldReference::type2Str(FieldReference::Type refType)
{
    return TYPES_AS_STRINGS[refType];
}
