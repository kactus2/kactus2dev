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

#include <QObject>
#include <QSharedPointer>
#include <QDomNode>

//-----------------------------------------------------------------------------
//! Reader class for IP-XACT port element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT PortReader : public CommonItemsReader
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this reader.
     */
    PortReader(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~PortReader();

    /*!
     *  Creates a new port from XML description.
     *
     *      @param [in] portNode    XML description of the port.
     *
     *      @return A new port created from the XML description.
     */
    QSharedPointer<Port> createPortFrom(QDomNode const& portNode) const;

private:

    //! No copying allowed.
    PortReader(PortReader const& rhs);
    PortReader& operator=(PortReader const& rhs);

    /*!
     *  Parses a wire port.
     *
     *      @param [in] wireElement     XML description of the wire.
     *      @param [in] newPort         The port containing the wire.
     */
    void parseWire(QDomElement const& wireElement, QSharedPointer<Port> newPort) const;

    /*!
     *  Parse vectors.
     *
     *      @param [in] vectorsElement  XML description of the vectors.
     *      @param [in] newWire         The containing wire item.
     */
    void parseWireVectors(QDomElement const& vectorsElement, QSharedPointer<Wire> newWire) const;

    /*!
     *  Parse the wire type definitions.
     *
     *      @param [in] typeDefinitionsElement  XML description of the type definitions.
     *      @param [in] elementName             Name of the type definition element.
     *      @param [in] attributeName           Name of the attribute containing the constraint.
     *
     *      @return Pointer to a list containing the created type definitions.
     */
    QSharedPointer<QList<QSharedPointer<WireTypeDef> > >parseWireTypeDefinitions
        (QDomElement const& typeDefinitionsElement, QString const& elementName, QString const& attributeName) const;

    /*!
     *  Parse the type definitions.
     *
     *      @param [in] wireTypeDefinitionElement   XML description of the wire type definition.
     *      @param [in] newWireTypeDefinition       The containing wire type definition.
     */
    void parseTypeDefinitions(QDomElement const& wireTypeDefinitionElement,
        QSharedPointer<WireTypeDef> newWireTypeDefinition) const;

    /*!
     *  Parse the view references.
     *
     *      @param [in] wireTypeDefinitionElement   XML description of the wire type definition.
     *      @param [in] newWireTypeDefinition       The containing wire type definition.
     */
    void parseViewReferences(QDomElement const& wireTypeDefinitionElement,
        QSharedPointer<WireTypeDef> newWireTypeDefinition) const;

    /*!
     *  Parse the wire driver default value.
     *
     *      @param [in] wireElement     XML description of the wire.
     *      @param [in] newWire         The containing wire item.
     */
    void parseWireDefaultValue(QDomElement const& wireElement, QSharedPointer<Wire> newWire) const;

    /*!
     *  Parse a transactional port.
     *
     *      @param [in] transactionalElement    XML description of the transactional.
     *      @param [in] newPort                 The containing port.
     */
    void parseTransactional(QDomElement const& transactionalElement, QSharedPointer<Port> newPort) const;

    /*!
     *  Parse the kind of the transactional port.
     *
     *      @param [in] kindElement     XML description of the kind.
     *      @param [in] transactional   The containing transactional port.
     */
    void parseTransactionalKind(QDomElement const& kindElement, QSharedPointer<Transactional> transactional) const;

    /*!
     *  Parse the transactional protocol.
     *
     *      @param [in] transactionalNode   XML description of the transactional.
     *      @param [in] transactional       The containing transactional port.
     */
    void parseTransactionalProtocol(QDomNode const& transactionalNode, QSharedPointer<Transactional> transactional)
        const;

    /*!
     *  Parse the maximum and minimum connections allowed to a transactional port.
     *
     *      @param [in] transactionalElement    XML description of the transactional.
     *      @param [in] transactional           The containing transactional port.
     */
    void parseTransactionalConnectionsMinMax(QDomElement const& transactionalElement,
        QSharedPointer<Transactional> transactional) const;

    /*!
     *  Parse the port arrays.
     *
     *      @param [in] portNode    XML description of the port.
     *      @param [in] newPort     The containing port item.
     */
    void parseArrays(QDomNode const& portNode, QSharedPointer<Port> newPort) const;

    /*!
     *  Parse the port vendor extensions.
     *
     *      @param [in] portNode    XML description of the port.
     *      @param [in] newPort     The containing port item.
     */
    void parsePortExtensions(QDomNode const& portNode, QSharedPointer<Port> newPort) const;

    /*!
     *  Parse the port position.
     *
     *      @param [in] positionElement     XML description of the port position.
     *      @param [in] newPort             The contianing port item.
     */
    void parsePosition(QDomElement const& positionElement, QSharedPointer<Port> newPort) const;
};

#endif // PORTREADER_H