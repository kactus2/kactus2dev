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
// Function: FieldReference::setReference()
//-----------------------------------------------------------------------------
void FieldReference::setReference(FieldReference::Type refType, QString const& ref)
{
    switch (refType)
    {
    case FieldReference::Type::ADDRESS_SPACE:
        addressSpaceRef_ = ref;
        break;
    case FieldReference::Type::MEMORY_MAP:
        memoryMapRef_ = ref;
        break;
    case FieldReference::Type::MEMORY_REMAP:
        memoryRemapRef_ = ref;
        break;
    case FieldReference::Type::BANK:
        bankRefs_.append(ref);
        break;
    case FieldReference::Type::ADDRESS_BLOCK:
        addressBlockRef_.reference_ = ref;
        break;
    case FieldReference::Type::REGISTER_FILE:
        registerFileRefs_.append(IndexedReference({ ref, QList<QString>() }));
        break;
    case FieldReference::Type::REGISTER:
        registerRef_.reference_ = ref;
        break;
    case FieldReference::Type::ALTERNATE_REGISTER:
        alternateRegisterRef_ = ref;
        break;
    case FieldReference::Type::FIELD:
        fieldRef_.reference_ = ref;
        break;
    default:
        break;
    }
}

//-----------------------------------------------------------------------------
// Function: FieldReference::getReference()
//-----------------------------------------------------------------------------
QStringList FieldReference::getReference(Type refType) const
{
    switch (refType)
    {
    case FieldReference::Type::ADDRESS_SPACE:
        return QStringList(addressSpaceRef_);
    case FieldReference::Type::MEMORY_MAP:
        return QStringList(memoryMapRef_);
    case FieldReference::Type::MEMORY_REMAP:
        return QStringList(memoryRemapRef_);
    case FieldReference::Type::BANK:
        return QStringList(bankRefs_);
    case FieldReference::Type::ADDRESS_BLOCK:
        return QStringList(addressBlockRef_.reference_);
    case FieldReference::Type::REGISTER_FILE:
    {
        QStringList registerFiles;
        std::for_each(registerFileRefs_.cbegin(), registerFileRefs_.cend(),
            [&registerFiles](IndexedReference const& ref)
            {
                registerFiles << ref.reference_;
            });
        return registerFiles;
    }
    case FieldReference::Type::REGISTER:
        return QStringList(registerRef_.reference_);
    case FieldReference::Type::ALTERNATE_REGISTER:
        return QStringList(alternateRegisterRef_);
    case FieldReference::Type::FIELD:
        return QStringList(fieldRef_.reference_);
    default:
        return QStringList();
    }
}

//-----------------------------------------------------------------------------
// Function: FieldReference::setReferenceIndices()
//-----------------------------------------------------------------------------
void FieldReference::setReferenceIndices(FieldReference::Type refType, QString const& ref, QList<QString> const& indices)
{
    if (refType == FieldReference::Type::ADDRESS_BLOCK)
    {
        addressBlockRef_.indices_ = indices;
    }
    else if (refType == FieldReference::Type::REGISTER_FILE)
    {
        auto matchingRef = std::find_if(registerFileRefs_.begin(), registerFileRefs_.end(),
            [ref](IndexedReference const& e)
            {
                return e.reference_ == ref;
            });

        if (matchingRef != registerFileRefs_.end())
        {
            matchingRef->indices_ = indices;
        }
    }
    else if (refType == FieldReference::Type::REGISTER)
    {
        registerRef_.indices_ = indices;
    }
    else if (refType == FieldReference::Type::FIELD)
    {
        fieldRef_.indices_ = indices;
    }
}

//-----------------------------------------------------------------------------
// Function: FieldReference::getReferenceIndices()
//-----------------------------------------------------------------------------
QStringList FieldReference::getReferenceIndices(Type refType, QString const& ref) const
{
    if (refType == FieldReference::Type::ADDRESS_BLOCK)
    {
        return addressBlockRef_.indices_;
    }
    else if (refType == FieldReference::Type::REGISTER_FILE)
    {
        // There can be multiple register file references, so find the one matching to ref.
        auto matchingRef = std::find_if(registerFileRefs_.begin(), registerFileRefs_.end(),
            [ref](IndexedReference const& e)
            {
                return e.reference_ == ref;
            });

        if (matchingRef != registerFileRefs_.end())
        {
            return matchingRef->indices_;
        }
    }
    else if (refType == FieldReference::Type::REGISTER)
    {
        return registerRef_.indices_;
    }
    else if (refType == FieldReference::Type::FIELD)
    {
        return fieldRef_.indices_;
    }
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
