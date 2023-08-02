//-----------------------------------------------------------------------------
// File: FieldReferenceWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 1.8.2023
//
// Description:
// Writer for the fieldReferenceGroup group.
//-----------------------------------------------------------------------------

#ifndef FIELDREFERENCEWRITER_H
#define FIELDREFERENCEWRITER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/Component/FieldReference.h>

#include <QXmlStreamWriter>

namespace FieldReferenceWriter
{
    
    /*!
     *	Writes a given field reference to XML.
     *  
     *      @param [in] writer              The XML writer to use.
     *      @param [in] fieldReference      The field reference to write.
     */
    IPXACTMODELS_EXPORT void writeFieldReference(QXmlStreamWriter& writer, QSharedPointer<FieldReference> fieldReference);

    namespace Details
    {
        /*!
         *	Write a given reference element of specified type.
         *  
         *      @param [in] writer               The XML writer to use.
         *      @param [in] referenceElement     The field reference element to write.
         *      @param [in] refType              The reference type to write.
         */
        void writeSingleFieldReference(QXmlStreamWriter& writer,
            QSharedPointer<FieldReference::IndexedReference> referenceElement, FieldReference::Type refType);

        /*!
         *	Write a list of similar field reference elements, e.g. bank refrences or register file references.
         *  
         *      @param [in] writer             The XML writer to use.
         *      @param [in] fieldReference     The field reference to write.
         *      @param [in] refType            The reference type to write.
         */
        void writeMultipleFieldReference(QXmlStreamWriter& writer, QSharedPointer<FieldReference> fieldReference,
            FieldReference::Type refType);

        /*!
         *	Write the indices of a reference element.
         *  
         *      @param [in] writer               The XML writer to use.
         *      @param [in] referenceElement     The field reference element to write.
         */
        void writeFieldReferenceIndices(QXmlStreamWriter& writer, 
            QSharedPointer<FieldReference::IndexedReference> referenceElement);
    }
}

#endif // FIELDREFERENCEWRITER_H

