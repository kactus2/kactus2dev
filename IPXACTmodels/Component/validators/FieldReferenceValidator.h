//-----------------------------------------------------------------------------
// File: FieldReferenceValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 3.8.2023
//
// Description:
// Validator for field reference group.
//-----------------------------------------------------------------------------

#ifndef FIELDREFERENCEVALIDATOR_H
#define FIELDREFERENCEVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/Component/FieldReference.h>

#include <QObject>

namespace FieldReferenceValidator
{
    /*!
     *  Validates the given field reference.
     *
     *      @param [in] fieldReference   The field reference to validate.
     *
     *      @return True, if the field is valid IP-XACT, otherwise false.
     */
    IPXACTMODELS_EXPORT bool validate(QSharedPointer<FieldReference> fieldReference);

    /*!
     *  Locate errors within a field reference.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] fieldReference      The selected field reference.
     *      @param [in] context             Context to help locate the error.
     */
    IPXACTMODELS_EXPORT void findErrorsIn(QStringList& errorList, QSharedPointer<FieldReference> fieldReference, QString const& context);

}

#endif // FIELDREFERENCEVALIDATOR_H
