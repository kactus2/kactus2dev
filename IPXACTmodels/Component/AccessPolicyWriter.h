//-----------------------------------------------------------------------------
// File: AccessPolicyWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 8.8.2023
//
// Description:
// Writer for ipxact:accessPolicy.
//-----------------------------------------------------------------------------

#ifndef ACCESSPOLICYWRITER_H
#define ACCESSPOLICYWRITER_H

#include "AccessPolicy.h"

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QXmlStreamWriter>

namespace AccessPolicyWriter
{

    /*!
     *	Write a given access policy to XML.
     *  
     *      @param [in] writer           The XML writer to use.
     *      @param [in] accessPolicy     The field access policy to write.
     */
    IPXACTMODELS_EXPORT void writeAccessPolicy(QXmlStreamWriter& writer, QSharedPointer<AccessPolicy> accessPolicy);

    namespace Details
    {
        void writeModeRefs(QXmlStreamWriter& writer, QSharedPointer<QList<QSharedPointer<ModeReference> > > modeRefs);

        void writeAccess(QXmlStreamWriter& writer, QSharedPointer<AccessPolicy> accessPolicy);
    }
}

#endif // ACCESSPOLICYWRITER_H
