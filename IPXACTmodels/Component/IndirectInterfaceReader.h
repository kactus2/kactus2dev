//-----------------------------------------------------------------------------
// File: IndirectInterfaceReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 02.08.2017
//
// Description:
// XML reader class for IP-XACT Indirect Interface element.
//-----------------------------------------------------------------------------

#ifndef INDIRECTINTERFACEREADER_H
#define INDIRECTINTERFACEREADER_H

#include <QSharedPointer>
#include <QDomNode>

#include <IPXACTmodels/common/CommonItemsReader.h>

#include <IPXACTmodels/ipxactmodels_global.h>

class IndirectInterface;

namespace  IndirectInterfaceReader
{
    /*!
     *  Creates an indirect interface definition from an XML description.
     *
     *    @param [in] interfaceNode   The XML node describing the interface.
     *
     *    @return The created indirect interface.
     */
    IPXACTMODELS_EXPORT QSharedPointer<IndirectInterface> createIndirectInterfaceFrom(QDomNode const& interfaceNode,
        Document::Revision docRevision);
    
    namespace Details
    {
        /*!
          *  Reads the attributes from XML to an indirect interface.
          *
          *    @param [in] interfaceNode           The XML description of the indirect interface.
          *    @param [in/out] indirectInterface   The indirect interface to insert the attributes into.
          */
        void parseAttributes(QDomNode const& interfaceNode, QSharedPointer<IndirectInterface> indirectInterface);

        /*!
          *  Reads the indirect address and data from XML to an indirect interface.
          *
          *    @param [in] interfaceNode           The XML description of the indirect interface.
          *    @param [in/out] indirectInterface   The indirect interface to insert the address and data into.
          */
        void parseIndirectAddressAndData(QDomNode const& interfaceNode,
            QSharedPointer<IndirectInterface> indirectInterface);

        /*!
          *  Reads the memory map reference from XML to an indirect interface.
          *
          *    @param [in] interfaceNode           The XML description of the indirect interface.
          *    @param [in/out] indirectInterface   The indirect interface to insert the memory map reference into.
          */
        void parseMemoryMapRef(QDomNode const& interfaceNode, QSharedPointer<IndirectInterface> indirectInterface);

        /*!
          *  Reads the transparent bridges from XML to an indirect interface.
          *
          *    @param [in] interfaceNode           The XML description of the indirect interface.
          *    @param [in/out] indirectInterface   The indirect interface to insert the transparent bridges into.
          */
        void parseTransparentBridges(QDomNode const& interfaceNode,
            QSharedPointer<IndirectInterface> indirectInterface, Document::Revision docRevision);

        /*!
          *  Reads the endianness from XML to an indirect interface.
          *
          *    @param [in] interfaceNode           The XML description of the indirect interface.
          *    @param [in/out] indirectInterface   The indirect interface to insert the endianness into.
          */
        void parseEndianness(QDomNode const& interfaceNode, QSharedPointer<IndirectInterface> indirectInterface);

        /*!
          *  Reads the bits in lau element from XML to an indirect interface.
          *
          *    @param [in] interfaceNode           The XML description of the indirect interface.
          *    @param [in/out] indirectInterface   The indirect interface to insert the bits in lau info into.
          */
        void parseBitsInLau(QDomNode const& interfaceNode, QSharedPointer<IndirectInterface> indirectInterface);

        /*!
          *  Reads the parameters from XML to an indirect interface.
          *
          *    @param [in] interfaceNode           The XML description of the indirect interface.
          *    @param [in/out] indirectInterface   The indirect interface to insert the parameters into.
          */
        void parseParameters(QDomNode const& interfaceNode, QSharedPointer<IndirectInterface> indirectInterface,
            Document::Revision docRevision);
    }
};

#endif // INDIRECTINTERFACEREADER_H
