//-----------------------------------------------------------------------------
// File: AccessPolicyReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 8.8.2023
//
// Description:
// Reader for ipxact:accessPolicy.
//-----------------------------------------------------------------------------

#ifndef ACCESSPOLICYREADER_H
#define ACCESSPOLICYREADER_H

#include "AccessPolicy.h"

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QDomNode>

namespace AccessPolicyReader
{

    /*!
     *	Create a field access policy from XML description.
     *  
     *      @param [in] accessPolicyNode     The XML description of the access policy.
     *	    
     * 	    @return The created field access policy.
     */
    IPXACTMODELS_EXPORT QSharedPointer<AccessPolicy> createAccessPolicyFrom(QDomNode const& accessPolicyNode);

    namespace Details
    {
        /*!
         *	Read mode references.
         *  
         *      @param [in] rootNode        The containing element.
         *      @param [in/out] modeRefList     The mode reference list to read to.
         */
        void parseModeRefs(QDomNode const& rootNode, QSharedPointer<QList<QSharedPointer<ModeReference> > > modeRefList);

        /*!
         *	Parse field access policy access value.
         *  
         *      @param [in] accessPolicyNode     The XML description of the field access policy.
         *      @param [in] accessPolicy         The created field access policy.
         */
        void parseAccess(QDomNode const& accessPolicyNode, QSharedPointer<AccessPolicy> accessPolicy);
    }
}

#endif // FIELDACCESSPOLICYREADER_H