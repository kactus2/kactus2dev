//-----------------------------------------------------------------------------
// File: FieldReferenceReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 1.8.2023
//
// Description:
// Reads a field reference group from IP-XACT XML.
//-----------------------------------------------------------------------------

#ifndef FIELDREFERENCEREADER_H
#define FIELDREFERENCEREADER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/Component/FieldReference.h>

#include <QDomNode>

namespace FieldReferenceReader
{

    /*!
     *	Create a field reference from a given parent document node.
     *  
     *      @param [in] groupNode     The containing element of the field reference group.
     *	    
     * 	    @return The created field reference.
     */
    IPXACTMODELS_EXPORT QSharedPointer<FieldReference> createFieldReferenceFrom(QDomNode const& groupNode);

    namespace Details
    {
        /*!
         *	Reads a single refrence element of the field reference.
         *  
         *      @param [in] currentNode            The current XML document node.
         *      @param [in] newFieldReference      The created field reference.
         */
        void readSingleReferenceElement(QDomNode const& currentNode, 
            QSharedPointer<FieldReference> newFieldReference);

        /*!
         *	Read the indices of a field reference element.
         *  
         *      @param [in] referenceNode           The XML document node of the field reference element.
         *      @param [in] newFieldRefElement      The created field reference.
         */
        void readReferenceIndices(QDomNode const& referenceNode, 
            QSharedPointer<FieldReference::IndexedReference> newFieldRefElement);
    }
}


#endif // FIELDREFERENCEREADER_H
