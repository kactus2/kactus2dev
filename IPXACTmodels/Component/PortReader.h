//-----------------------------------------------------------------------------
// File: PortReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 14.09.2015
//
// Description:
// Reader class for IP-XACT port element.
//-----------------------------------------------------------------------------

#ifndef PORTREADER_H
#define PORTREADER_H

#include "Port.h"

#include <IPXACTmodels/common/CommonItemsReader.h>
#include <IPXACTmodels/common/Document.h>

#include <QSharedPointer>
#include <QDomNode>

//-----------------------------------------------------------------------------
//! Reader class for IP-XACT port element.
//-----------------------------------------------------------------------------
namespace  PortReader 
{


    /*!
     *  Creates a new port from XML description.
     *
     *      @param [in] portNode    XML description of the port.
     *
     *      @return A new port created from the XML description.
     */
    IPXACTMODELS_EXPORT QSharedPointer<Port> createPortFrom(QDomNode const& portNode,
        Document::Revision docRevision);

    namespace Details
    {

        /*!
         *  Parses a wire port.
         *
         *      @param [in] wireElement     XML description of the wire.
         *      @param [in] docRevision     The applied IP-XACT revision.
         */
        QSharedPointer<Wire> createWireFrom(QDomElement const& wireElement, 
            Document::Revision docRevision);

        /*!
         *  Parse vectors.
         *
         *      @param [in] wireElement     XML description of the wire.
         *      @param [in] newWire         The containing wire item.
         */
        void parseWireVectors(QDomElement const& wireElement, QSharedPointer<Wire> newWire,
            Document::Revision docRevision);

        /*!
         *  Parse the vectors in the given element.
         *
         *      @param [in] parentElement   The element containing vectors.
         *      @param [in] docRevision     The applied IP-XACT revision.
         *
         *      @return The parsed vectors.
         */
        QList<Vector> parseVectors(QDomElement const& parentElement, Document::Revision docRevision);

        /*!
         *  Parse a single vector.
         *
         *      @param [in] vectorNode  The xml presentation of the vector.
         *      @param [in] docRevision The applied IP-XACT revision.
         *
         *      @return The parsed vector.
         */
        Vector parseVector(QDomNode const& vectorNode, Document::Revision docRevision);

        /*!
         *  Parse the wire type definitions.
         *
         *      @param [in] typeDefinitionsElement  XML description of the type definitions.
         *      @param [in] elementName             Name of the type definition element.
         *      @param [in] attributeName           Name of the attribute containing the constraint.
         *
         *      @return Pointer to a list containing the created type definitions.
         */
        QSharedPointer<QList<QSharedPointer<WireTypeDef> > >parseWireTypeDefinitions(
            QDomElement const& typeDefinitionsElement, QString const& elementName, QString const& attributeName);

        /*!
         *  Parse the type definitions.
         *
         *      @param [in] wireTypeDefinitionElement   XML description of the wire type definition.
         *      @param [in] newWireTypeDefinition       The containing wire type definition.
         */
        void parseTypeDefinitions(QDomElement const& wireTypeDefinitionElement,
            QSharedPointer<WireTypeDef> newWireTypeDefinition);

        /*!
         *  Parse the view references.
         *
         *      @param [in] wireTypeDefinitionElement   XML description of the wire type definition.
         *      @param [in] newWireTypeDefinition       The containing wire type definition.
         */
        void parseViewReferences(QDomElement const& wireTypeDefinitionElement,
            QSharedPointer<WireTypeDef> newWireTypeDefinition);

        /*!
         *  Parse the wire driver default value.
         *
         *      @param [in] wireElement     XML description of the wire.
         *      @param [in] newWire         The containing wire item.
         */
        void parseWireDefaultValue(QDomElement const& wireElement, QSharedPointer<Wire> newWire);

        /*!
         *  Parse a transactional port.
         *
         *      @param [in] transactionalElement    XML description of the transactional.
         *      @param [in] newPort                 The containing port.
         */
        void parseTransactional(QDomElement const& transactionalElement, QSharedPointer<Port> newPort);

        /*!
         *  Parse the kind of the transactional port.
         *
         *      @param [in] kindElement     XML description of the kind.
         *      @param [in] transactional   The containing transactional port.
         */
        void parseTransactionalKind(QDomElement const& kindElement, QSharedPointer<Transactional> transactional);

        /*!
         *  Parse the transactional protocol.
         *
         *      @param [in] transactionalNode   XML description of the transactional.
         *      @param [in] transactional       The containing transactional port.
         */
        void parseTransactionalProtocol(QDomNode const& transactionalNode, 
            QSharedPointer<Transactional> transactional);

        /*!
         *  Parse the maximum and minimum connections allowed to a transactional port.
         *
         *      @param [in] transactionalElement    XML description of the transactional.
         *      @param [in] transactional           The containing transactional port.
         */
        void parseTransactionalConnectionsMinMax(QDomElement const& transactionalElement,
            QSharedPointer<Transactional> transactional);

        /*!
         *  Parse a structured port.
         *
         *      @param [in] structuredElement   The XML description of the structured port.
         *      @param [in] newPort             The containing port item.
         *
         *      @return 
         */
        QSharedPointer<Structured> createStructuredFrom(QDomElement const& structuredElement);

        /*!
         *  Parse the structured port type.
         *
         *      @param [in] structuredElementt   The XML description of the structured port.
         *      @param [in] newStructured        The containing structured port.
         */
        void parseStructuredType(QDomElement const& structuredElement, QSharedPointer<Structured> newStructured);

        /*!
         *  Parse the vectors in the structured port.
         *
         *      @param [in] structuredElementt   The XML description of the structured port.
         *      @param [in] newStructured        The containing structured port.
         */
        void parseStructuredVectors(QDomElement const& structuredElement, QSharedPointer<Structured> newStructured);

        /*!
         *  Parse the sub-ports in the structured port.
         *
         *      @param [in] structuredElementt   The XML description of the structured port.
         *      @param [in] newStructured        The containing structured port.
         */
        void parseSubPorts(QDomElement const& structuredElement, QSharedPointer<Structured> newStructured);

        /*!
         *  Parse the port arrays.
         *
         *      @param [in] parentNode  XML node .
         *      @param [in] newPort     The containing port item.
         */
        QList<Array> createArrays(QDomNode const& parentNode);

        /*!
         *  Parse the port vendor extensions.
         *
         *      @param [in] portNode    XML description of the port.
         *      @param [in] newPort     The containing port item.
         */
        void parsePortExtensions(QDomNode const& portNode, QSharedPointer<Port> newPort);

        /*!
         *  Parse the port position.
         *
         *      @param [in] positionElement     XML description of the port position.
         *      @param [in] newPort             The contianing port item.
         */
        void parsePosition(QDomElement const& positionElement, QSharedPointer<Port> newPort) ;
    }
};

#endif // PORTREADER_H
