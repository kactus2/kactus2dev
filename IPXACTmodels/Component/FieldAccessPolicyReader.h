//-----------------------------------------------------------------------------
// File: FieldAccessPolicyReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 31.7.2023
//
// Description:
// Reader for ipxact:fieldAccessPolicy
//-----------------------------------------------------------------------------

#ifndef FIELDACCESSPOLICYREADER_H
#define FIELDACCESSPOLICYREADER_H

#include "FieldAccessPolicy.h"

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QDomNode>

namespace FieldAccessPolicyReader
{

    /*!
     *	Create a field access policy from XML description.
     *  
     *      @param [in] fieldAccessPolicyNode     The XML description of the field access policy.
     *	    
     * 	    @return The created field access policy.
     */
    IPXACTMODELS_EXPORT QSharedPointer<FieldAccessPolicy> createFieldAccessPolicyFrom(QDomNode const& fieldAccessPolicyNode);

    namespace Details
    {
        /*!
         *	Read mode references.
         *  
         *      @param [in] rootNode        The containing element.
         *      @param [in] accessPolicy    The created field access policy.
         */
        void parseModeRefs(QDomNode const& rootNode, QSharedPointer<FieldAccessPolicy> accessPolicy);

        /*!
         *	Read the access policy definition reference.
         *  
         *      @param [in] accessPolicyNode     The XML description of the field access policy.
         *      @param [in] accessPolicy         The created field access policy.
         */
        void parseDefinitionRef(QDomNode const& accessPolicyNode, QSharedPointer<FieldAccessPolicy> accessPolicy);

        /*!
         *	Parse field access policy access value.
         *  
         *      @param [in] accessPolicyNode     The XML description of the field access policy.
         *      @param [in] accessPolicy         The created field access policy.
         */
        void parseAccess(QDomNode const& accessPolicyNode, QSharedPointer<FieldAccessPolicy> accessPolicy);

        /*!
         *	Read the modified write value of the field access policy.
         *  
         *      @param [in] accessPolicyNode     The XML description of the field access policy.
         *      @param [in] accessPolicy         The created field access policy.
         */
        void parseModifiedWriteValue(QDomNode const& accessPolicyNode, QSharedPointer<FieldAccessPolicy> accessPolicy);

        /*!
         *	Read the write value constraint.
         *  
         *      @param [in] accessPolicyNode     The XML description of the field access policy.
         *      @param [in] accessPolicy         The created field access policy.
         */
        void parseWriteValueConstraint(QDomNode const& accessPolicyNode, QSharedPointer<FieldAccessPolicy> accessPolicy);

        /*!
         *	Read the read action.
         *  
         *      @param [in] accessPolicyNode     The XML description of the field access policy.
         *      @param [in] accessPolicy         The created field access policy.
         */
        void parseReadAction(QDomNode const& accessPolicyNode, QSharedPointer<FieldAccessPolicy> accessPolicy);

        /*!
         *	Read the read response of the field access policy.
         *  
         *      @param [in] accessPolicyNode     The XML description of the field access policy.
         *      @param [in] accessPolicy         The created field access policy.
         */
        void parseReadResponse(QDomNode const& accessPolicyNode, QSharedPointer<FieldAccessPolicy> accessPolicy);

        /*!
         *	Read the broadcasts of the field access policy.
         *  
         *      @param [in] accessPolicyNode     The XML description of the field access policy.
         *      @param [in] accessPolicy         The created field access policy.
         */
        void parseBroadcasts(QDomNode const& accessPolicyNode, QSharedPointer<FieldAccessPolicy> accessPolicy);

        /*!
         *	Read the access restrictions of the field access policy.
         *  
         *      @param [in] accessPolicyNode     The XML description of the field access policy.
         *      @param [in] accessPolicy         The created field access policy.
         */
        void parseAccessRestrictions(QDomNode const& accessPolicyNode, QSharedPointer<FieldAccessPolicy> accessPolicy);

        /*!
         *	Read the testable value of the field access policy.
         *  
         *      @param [in] accessPolicyNode     The XML description of the field access policy.
         *      @param [in] accessPolicy         The created field access policy.
         */
        void parseTestable(QDomNode const& accessPolicyNode, QSharedPointer<FieldAccessPolicy> accessPolicy);
        
        /*!
         *	Read the reserved value of the field access policy.
         *  
         *      @param [in] accessPolicyNode     The XML description of the field access policy.
         *      @param [in] accessPolicy         The created field access policy.
         */
        void parseReserved(QDomNode const& accessPolicyNode, QSharedPointer<FieldAccessPolicy> accessPolicy);
    }
}

#endif // FIELDACCESSPOLICYREADER_H