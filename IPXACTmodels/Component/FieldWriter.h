//-----------------------------------------------------------------------------
// File: FieldWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 24.09.2015
//
// Description:
// Writer for IP-XACT field element.
//-----------------------------------------------------------------------------

#ifndef FIELDWRITER_H
#define FIELDWRITER_H

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/CommonItemsWriter.h>
#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/component/Field.h>

#include <QXmlStreamWriter>
#include <QSharedPointer>

class Field;
class WriteValueConstraint;

//-----------------------------------------------------------------------------
//! Writer for IP-XACT field element.
//-----------------------------------------------------------------------------
namespace FieldWriter
{

    /*!
     *  Write a field to an XML file.
     *
     *      @param [in] writer  The used xml writer.
     *      @param [in] field   The field to be written.
     */
    IPXACTMODELS_EXPORT void writeField(QXmlStreamWriter& writer, QSharedPointer<Field> field, Document::Revision docRevision = Document::Revision::Std14);

    namespace Details
    {

        /*!
         *  Write the field id attribute.
         *
         *      @param [in] writer      Used XML writer.
         *      @param [in] fieldID     The field id.
         */
        void writeID(QXmlStreamWriter& writer, QString const& fieldID, Document::Revision docRevision);

        /*!
         *	Write the field memory array.
         *  
     *      @param [in] writer  The used xml writer.
     *      @param [in] field   The field to be written.
         */
        void writeMemoryArray(QXmlStreamWriter& writer, QSharedPointer<Field> field, Document::Revision docRevision);

        void writeFieldDefinitionRef(QXmlStreamWriter& writer, QSharedPointer<Field> field);

        /*!
         *  Write the reset.
         *
         *      @param [in] writer  Used XML writer.
         *      @param [in] field   The selected field item.
         */
        void writeResets(QXmlStreamWriter& writer, QSharedPointer<Field> field);

        /*!
         *  Write the type identifier.
         *
         *      @param [in] writer  Used XML writer.
         *      @param [in] field   The selected field item.
         */
        void writeTypeIdentifier(QXmlStreamWriter& writer, QSharedPointer<Field> field);

        /*!
         *  Write the volatile value.
         *
         *      @param [in] writer  Used XML writer.
         *      @param [in] field   The selected field item.
         */
        void writeVolatile(QXmlStreamWriter& writer, QSharedPointer<Field> field);

        /*!
         *  Write the access value.
         *
         *      @param [in] writer  Used XML writer.
         *      @param [in] field   The selected field item.
         */
        void writeAccess(QXmlStreamWriter& writer, QSharedPointer<Field> field);

        /*!
         *  Write the enumerated values.
         *
         *      @param [in] writer  Used XML writer.
         *      @param [in] field   The selected field item.
         */
        void writeEnumerations(QXmlStreamWriter& writer, QSharedPointer<Field> field);

        /*!
         *  Write the modified write value.
         *
         *      @param [in] writer  Used XML writer.
         *      @param [in] field   The selected field item.
         */
        void writeModifiedWriteValue(QXmlStreamWriter& writer, QSharedPointer<Field> field);

        /*!
         *  Write the write value constraint.
         *
         *      @param [in] writer          Used XML writer.
         *      @param [in] writeConstaint  The field write value constraint.
         */
        void writeWriteValueConstraint(QXmlStreamWriter& writer, QSharedPointer<WriteValueConstraint> writeConstraint)
           ;

        /*!
         *  Write the read action.
         *
         *      @param [in] writer  Used XML writer.
         *      @param [in] field   The selected field item.
         */
        void writeReadAction(QXmlStreamWriter& writer, QSharedPointer<Field> field);

        /*!
         *  Write the testable element.
         *
         *      @param [in] writer  Used XML writer.
         *      @param [in] field   The selected field item.
         */
        void writeTestable(QXmlStreamWriter& writer, QSharedPointer<Field> field);

        /*!
         *  Write the reserved value.
         *
         *      @param [in] writer  Used XML writer.
         *      @param [in] field   The selected field item.
         */
        void writeReserved(QXmlStreamWriter& writer, QSharedPointer<Field> field);

        void writeFieldData2014(QXmlStreamWriter& writer, QSharedPointer<Field> field);

        void writeFieldData2022(QXmlStreamWriter& writer, QSharedPointer<Field> field);

        void writeFieldReference(QXmlStreamWriter& writer, QSharedPointer<Field> field);

        void writeMultipleFieldReference(QXmlStreamWriter& writer, QSharedPointer<FieldReference> fieldReference,
            FieldReference::Type refType);
        
        void writeSingleFieldReference(QXmlStreamWriter& writer, QSharedPointer<FieldReference::IndexedReference> reference,
            FieldReference::Type refType);

        void writeFieldReferenceIndices(QXmlStreamWriter& writer, QSharedPointer<FieldReference::IndexedReference> reference);
    }
};

#endif // FIELDWRITER_H
