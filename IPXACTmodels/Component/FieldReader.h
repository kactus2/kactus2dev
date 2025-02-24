//-----------------------------------------------------------------------------
// File: FieldReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 25.09.2015
//
// Description:
// Reader for IP-XACT field element.
//-----------------------------------------------------------------------------

#ifndef FIELDREADER_H
#define FIELDREADER_H

#include "FieldReference.h"

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/CommonItemsReader.h>


#include <QSharedPointer>
#include <QDomNode>

class Field;
class FieldReset;

//-----------------------------------------------------------------------------
//! Reader for IP-XACT field element.
//-----------------------------------------------------------------------------
namespace FieldReader
{

    /*!
     *  Creates a new field from a given field node.
     *
     *    @param [in] fieldNode   XML description of the field.
     *
     *    @return The created field.
     */
    IPXACTMODELS_EXPORT QSharedPointer<Field> createFieldFrom(QDomNode const& fieldNode, Document::Revision docRevision);

    namespace Details
    {

        /*!
         *  Reads the field ID.
         *
         *    @param [in] fieldElement    XML description of the field.
         *    @param [in] newField        The new field item.
         */
        void parseID(QDomElement const& fieldElement, QSharedPointer<Field> newField);

        /*!
         *  Reads the is present value.
         *
         *    @param [in] fieldElement    XML description of the field.
         *    @param [in] newField        The new field item.
         */
        void parsePresence(QDomElement const& fieldElement, QSharedPointer<Field> newField);

        /*!
         *	Reads the field memory array.
         *  
         *    @param [in] fieldElement    XML description of the field.
         *    @param [in] newField        The new field item.
         */
        void parseMemoryArray(QDomElement const& fieldElement, QSharedPointer<Field> newField);

        /*!
         *  Reads the bit offset.
         *
         *    @param [in] fieldElement    XML description of the field.
         *    @param [in] newField        The new field item.
         */
        void parseBitOffset(QDomElement const& fieldElement, QSharedPointer<Field> newField);

        /*!
         *	Reads the std2022 field definition reference.
         *  
         *    @param [in] fieldElement     XML description of the field.
         *    @param [in] newField         The new field item.
         */
        void parseFieldDefinitionRef(QDomElement const& fieldElement, QSharedPointer<Field> newField);

        /*!
         *  Reads the reset.
         *
         *    @param [in] fieldElement    XML description of the field.
         *    @param [in] newField        The new field item.
         */
        void parseResets(QDomElement const& fieldElement, QSharedPointer<Field> newField);

        /*!
         *  Reads the reset type reference.
         *
         *    @param [in] resetElement    XML description of the reset.
         *    @param [in] fldReset        The Reset Structure to parse into.
         */
        void parseResetTypeRef(QDomElement const& resetElement, QSharedPointer<FieldReset> fieldReset);

        /*!
         *  Reads the reset value.
         *
         *    @param [in] resetElement    XML description of the reset.
         *    @param [in] fldReset        The Reset Structure to parse into.
         */
        void parseResetValue(QDomElement const& resetElement, QSharedPointer<FieldReset> fieldReset);

        /*!
         *  Reads the reset mask.
         *
         *    @param [in] resetElement    XML description of the reset.
         *    @param [in] fldReset        The Reset Structure to parse into.
         */
        void parseResetMask(QDomElement const& resetElement, QSharedPointer<FieldReset> fieldReset);

        /*!
         *  Reads the type identifier.
         *
         *    @param [in] fieldElement    XML description of the field.
         *    @param [in] newField        The new field item.
         */
        void parseTypeIdentifier(QDomElement const& fieldElement, QSharedPointer<Field> newField);

        /*!
         *  Reads the bit width.
         *
         *    @param [in] fieldElement    XML description of the field.
         *    @param [in] newField        The new field item.
         */
        void parseBitWidth(QDomElement const& fieldElement, QSharedPointer<Field> newField);

        /*!
         *  Reads the volatile value.
         *
         *    @param [in] fieldElement    XML description of the field.
         *    @param [in] newField        The new field item.
         */
        void parseVolatile(QDomElement const& fieldElement, QSharedPointer<Field> newField);
        
        /*!
        *  Reads the field reference (ipxact:aliasOf).
        *
        *    @param [in] fieldElement    XML description of the field.
        *    @param [in] newField        The new field item.
        */
        void parseFieldReference(QDomElement const& fieldElement, QSharedPointer<Field> newField);

        /*!
         *  Reads the access value.
         *
         *    @param [in] fieldElement    XML description of the field.
         *    @param [in] newField        The new field item.
         */
        void parseAccess(QDomElement const& fieldElement, QSharedPointer<Field> newField);

        /*!
         *  Reads the enumerated values.
         *
         *    @param [in] fieldElement    XML description of the field.
         *    @param [in] newField        The new field item.
         */
        void parseEnumeratedValues(QDomElement const& fieldElement, QSharedPointer<Field> newField);

        /*!
         *  Reads the modified write value.
         *
         *    @param [in] fieldElement    XML description of the field.
         *    @param [in] newField        The new field item.
         */
        void parseModifiedWriteValue(QDomElement const& fieldElement, QSharedPointer<Field> newField);

        /*!
         *  Reads the write value constraint.
         *
         *    @param [in] fieldElement    XML description of the field.
         *    @param [in] newField        The new field item.
         */
        void parseWriteValueConstraint(QDomElement const& fieldElement, QSharedPointer<Field> newField);

        /*!
         *  Reads the read action value.
         *
         *    @param [in] fieldElement    XML description of the field.
         *    @param [in] newField        The new field item.
         */
        void parseReadAction(QDomElement const& fieldElement, QSharedPointer<Field> newField);

        /*!
         *  Reads the testable value.
         *
         *    @param [in] fieldElement    XML description of the field.
         *    @param [in] newField        The new field item.
         */
        void parseTestable(QDomElement const& fieldElement, QSharedPointer<Field> newField);

        /*!
         *  Reads the reserved value.
         *
         *    @param [in] fieldElement    XML description of the field.
         *    @param [in] newField        The new field item.
         */
        void parseReserved(QDomElement const& fieldElement, QSharedPointer<Field> newField);

        /*!
         *  Reads the parameters.
         *
         *    @param [in] fieldElement    XML description of the field.
         *    @param [in] newField        The new field item.
         */
        void parseParameters(QDomElement const& fieldElement, QSharedPointer<Field> newField);

        /*!
         *	Parse 2022 std field access policies.
         *  
         *    @param [in] fieldElement    XML description of the field.
         *    @param [in] newField        The new field item.
         */
        void parseFieldAccessPolicies(QDomElement const& fieldElement, QSharedPointer<Field> newField);
    }
};

#endif // FIELDREADER_H
