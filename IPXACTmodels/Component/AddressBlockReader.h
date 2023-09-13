//-----------------------------------------------------------------------------
// File: AddressBlockReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 30.09.2015
//
// Description:
// Reader for ipxact:addressBlock element.
//-----------------------------------------------------------------------------

#ifndef ADDRESSBLOCKREADER_H
#define ADDRESSBLOCKREADER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/Component/MemoryBlockBaseReader.h>

#include <QSharedPointer>
#include <QDomNode>

class AddressBlock;

//-----------------------------------------------------------------------------
//! Reader for ipxact:addressBlock element.
//-----------------------------------------------------------------------------
namespace AddressBlockReader
{

    /*!
     *  Creates a new address block from a given address block node.
     *
     *      @param [in] addressBlockNode    XML description of the address block.
     *
     *      @return The created address block.
     */
    IPXACTMODELS_EXPORT QSharedPointer<AddressBlock> createAddressBlockFrom(QDomNode const& addressBlockNode, Document::Revision docRevision);
    
    namespace Details
    {

        /*!
         *  Reads the type identifier.
         *
         *      @param [in] addressBlockNode    XML description of the address block.
         *      @param [in] newAddressBlock     The new address block.
         */
        void parseTypeIdentifier(QDomNode const& addressBlockNode, QSharedPointer<AddressBlock> newAddressBlock);

        /*!
         *  Reads the range.
         *
         *      @param [in] addressBlockNode    XML description of the address block.
         *      @param [in] newAddressBlock     The new address block.
         */
        void parseRange(QDomNode const& addressBlockNode, QSharedPointer<AddressBlock> newAddressBlock);

        /*!
         *  Reads the width.
         *
         *      @param [in] addressBlockNode    XML description of the address block.
         *      @param [in] newAddressBlock     The new address block.
         */
        void parseWidth(QDomNode const& addressBlockNode, QSharedPointer<AddressBlock> newAddressBlock);

        /*!
         *  Reads the usage.
         *
         *      @param [in] addressBlockNode    XML description of the address block.
         *      @param [in] newAddressBlock     The new address block.
         */
        void parseUsage(QDomNode const& addressBlockNode, QSharedPointer<AddressBlock> newAddressBlock);

        /*!
         *  Reads the volatile value.
         *
         *      @param [in] addressBlockNode    XML description of the address block.
         *      @param [in] newAddressBlock     The new address block.
         */
        void parseVolatile(QDomNode const& addressBlockNode, QSharedPointer<AddressBlock> newAddressBlock);

        /*!
         *  Reads the access value.
         *
         *      @param [in] addressBlockNode    XML description of the address block.
         *      @param [in] newAddressBlock     The new address block.
         */
        void parseAccess(QDomNode const& addressBlockNode, QSharedPointer<AddressBlock> newAddressBlock);

        /*!
         *  Reads the register data.
         *
         *      @param [in] addressBlockNode    XML description of the address block.
         *      @param [in] newAddressBlock     The new address block.
         */
        void parseRegisterData(QDomNode const& addressBlockNode, QSharedPointer<AddressBlock> newAddressBlock, Document::Revision docRevision);

        /*!
         *	Reads the memory array of the address block.
         *  
         *      @param [in] addressBlockNode    XML description of the address block.
         *      @param [in] newAddressBlock     The new address block.
         */
        void parseMemoryArray(QDomNode const& addressBlockNode, QSharedPointer<AddressBlock> newAddressBlock);

        /*!
         *	Reads the misalignment allowed attribute of the address block.
         *  
         *      @param [in] addressBlockNode    XML description of the address block.
         *      @param [in] newAddressBlock     The new address block.
         */
        void parseMisalignmentAllowed(QDomNode const& addressBlockNode, QSharedPointer<AddressBlock> newAddressBlock);

        /*!
         *	Reads the address block definition reference.
         *  
         *      @param [in] addressBlockNode    XML description of the address block.
         *      @param [in] newAddressBlock     The new address block.
         */
        void parseAddressBlockDefinitionRef(QDomNode const& addressBlockNode, QSharedPointer<AddressBlock> newAddressBlock);

        /*!
         *	Reads the access policies of the address block.
         *  
         *      @param [in] addressBlockNode    XML description of the address block.
         *      @param [in] newAddressBlock     The new address block.
         */
        void parseAccessPolicies(QDomNode const& addressBlockNode, QSharedPointer<AddressBlock> newAddressBlock);
    }
}

#endif // ADDRESSBLOCKREADER_H