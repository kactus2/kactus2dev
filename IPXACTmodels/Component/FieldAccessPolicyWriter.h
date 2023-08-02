//-----------------------------------------------------------------------------
// File: FieldAccessPolicyWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 2.8.2023
//
// Description:
// Writer for ipxact:fieldAccessPolicy.
//-----------------------------------------------------------------------------

#ifndef FIELDACCESSPOLICYWRITER_H
#define FIELDACCESSPOLICYWRITER_H

#include "FieldAccessPolicy.h"

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QXmlStreamWriter>

namespace FieldAccessPolicyWriter
{

    /*!
     *	Write a given field access policy to XML.
     *  
     *      @param [in] writer                The XML writer to use.
     *      @param [in] fieldAccessPolicy     The field access policy to write.
     */
    IPXACTMODELS_EXPORT void writeFieldAccessPolicy(QXmlStreamWriter& writer, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy);

    namespace Details
    {
        void writeModeRefs(QXmlStreamWriter& writer, QSharedPointer<QList<QSharedPointer<ModeReference> > > modeRefs);

        void writeFieldAccessPolicyDefinitionRef(QXmlStreamWriter& writer, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy);

        void writeAccess(QXmlStreamWriter& writer, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy);

        void writeModifiedWriteValue(QXmlStreamWriter& writer, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy);

        void writeValueConstraint(QXmlStreamWriter& writer, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy);

        void writeReadAction(QXmlStreamWriter& writer, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy);

        void writeReadResponse(QXmlStreamWriter& writer, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy);

        void writeBroadCasts(QXmlStreamWriter& writer, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy);

        void writeAccessRestrictions(QXmlStreamWriter& writer, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy);

        void writeTestable(QXmlStreamWriter& writer, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy);

        void writeReserved(QXmlStreamWriter& writer, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy);
    }
}

#endif // FIELDACCESSPOLICYWRITER_H
