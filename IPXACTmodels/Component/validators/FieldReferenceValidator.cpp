//-----------------------------------------------------------------------------
// File: FieldReferenceValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 3.8.2023
//
// Description:
// Validator for field reference group.
//-----------------------------------------------------------------------------

#include "FieldReferenceValidator.h"

//-----------------------------------------------------------------------------
// Function: FieldReferenceValidator::validate()
//-----------------------------------------------------------------------------
bool FieldReferenceValidator::validate(QSharedPointer<FieldReference> fieldReference)
{
    if (fieldReference->getReference(FieldReference::ADDRESS_SPACE) &&
        (fieldReference->getReference(FieldReference::MEMORY_MAP) ||
            fieldReference->getReference(FieldReference::MEMORY_REMAP)))
    {
        return false;
    }

    if (!fieldReference->getReference(FieldReference::FIELD))
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldReferenceValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void FieldReferenceValidator::findErrorsIn(QStringList& errorList, QSharedPointer<FieldReference> fieldReference, QString const& context)
{
    if (fieldReference->getReference(FieldReference::ADDRESS_SPACE) &&
        (fieldReference->getReference(FieldReference::MEMORY_MAP) ||
            fieldReference->getReference(FieldReference::MEMORY_REMAP)))
    {
        errorList.append(QObject::tr("Field reference in %1 cannot contain both an address space reference"
            " and a memory map or memory remap reference.").arg(context));
    }

    if (!fieldReference->getReference(FieldReference::FIELD))
    {
        errorList.append(QObject::tr("Field reference in %1 must contain a reference to a field.").arg(context));
    }
}
