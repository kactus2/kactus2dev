//-----------------------------------------------------------------------------
// File: PortWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 11.09.2015
//
// Description:
// Writer class for IP-XACT port element.
//-----------------------------------------------------------------------------

#ifndef PORTWRITER_H
#define PORTWRITER_H

#include "Port.h"

#include <IPXACTmodels/common/CommonItemsWriter.h>
#include <IPXACTmodels/ipxactmodels_global.h>

#include <QXmlStreamWriter>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT port element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT PortWriter : public CommonItemsWriter
{
public:

    /*!
     *  Write a component port to an XML file.
     *
     *      @param [in] writer  The used XML writer.
     *      @param [in] port    The port to be written.
     */
    void writePort(QXmlStreamWriter& writer, QSharedPointer<Port> port, Document::Revision docRevision) const;

private:

    /*!
     *  Write a wire port.
     *
     *      @param [in] writer  Used XML writer.
     *      @param [in] wire    The wire to be written.
     */
    void writeWire(QXmlStreamWriter& writer, QSharedPointer<Wire> wire, Document::Revision docRevision) const;

    /*!
     *  Write a transactional port.
     *
     *      @param [in] writer          Used XML writer.
     *      @param [in] transactional   The transactional to be written.
     */
    void writeTransactional(QXmlStreamWriter& writer, QSharedPointer<Transactional> transactional) const;

    /*!
     *  Write the port arrays.
     *
     *      @param [in] writer  Used XML writer.
     *      @param [in] arrays  The arrays to be written.
     */
    void writeArrays(QXmlStreamWriter& writer, QSharedPointer<QList<QSharedPointer<Array> > > arrays) const;

    /*!
     *  Write a vector.
     *
     *      @param [in] writer  Used XML writer.
     *      @param [in] vector  The vector to be written.
     */
    void writeVector(QXmlStreamWriter& writer, QSharedPointer<Vector> vector, Document::Revision docRevision) const;

    /*!
     *  Write the wire type definitions.
     *
     *      @param [in] writer              Used XML writer.
     *      @param [in] typeDefinitions     A list of the wire type definitions to be written.
     */
    void writeWireTypeDefinitions(QXmlStreamWriter& writer,
        QSharedPointer<QList<QSharedPointer<WireTypeDef> > > typeDefinitions) const;

    /*!
     *  Check if the wire type definitions are empty.
     *
     *      @param [in] typeDefinitions     A list of the wire type definitions to be written.
     *
     *      @return True, if the type definitions are empty, false otherwise.
     */
    bool wireTypeDefinitionsAreEmpty(QSharedPointer<QList<QSharedPointer<WireTypeDef> > > typeDefinitions) const;

    /*!
     *  Check a single wire type definition is empty.
     *
     *      @param [in] typeDefinition  The selected wire type definition.
     *
     *      @return True, if the type definition is empty, false otherwise.
     */
    bool singleWireTypeDefintionIsEmpty(QSharedPointer<WireTypeDef> typeDefinition) const;

    /*!
     *  Write a single type definition.
     *
     *      @param [in] writer          Used XML writer.
     *      @param [in] typeDefinition  The type definition to be written.
     *      @param [in] attributeName   The type definition constraining attribute name.
     */
    void writeSingleTypeDefinition(QXmlStreamWriter& writer, QSharedPointer<WireTypeDef> typeDefinition,
        QString const& attributeName) const;

    /*!
     *  Write the wire driver.
     *
     *      @param [in] writer          Used XML writer.
     *      @param [in] driver          The driver to be written.
     */
    void writeWireDriver(QXmlStreamWriter& writer, QSharedPointer<Driver> driver) const;

    /*!
     *  Write the transactional kind.
     *
     *      @param [in] writer  Used XML writer.
     *      @param [in] kind    The kind to be written.
     */
    void writeTransactionalKind(QXmlStreamWriter& writer, QString const& kind) const;

    /*!
     *  Write the transactional bus width.
     *
     *      @param [in] writer      Used XML writer.
     *      @param [in] busWidth    The bus width to be written.
     */
    void writeTransactionalBusWidth(QXmlStreamWriter& writer, QString const& busWidth) const;

    /*!
     *  Write the transactional type definitions.
     *
     *      @param [in] writer          Used XML writer.
     *      @param [in] transactional   The transactional containing the type definitions.
     */
    void writeTransactionalTypeDefinitions(QXmlStreamWriter& writer, QSharedPointer<Transactional> transactional)
        const;

    /*!
     *  Write the transactional minimum and maximum connections.
     *
     *      @param [in] writer          Used XML writer.
     *      @param [in] transactional   The transactional containing the minimum and maximum connections.
     */
    void writeTransactionalMinMaxConnections(QXmlStreamWriter& writer, QSharedPointer<Transactional> transactional)
        const;
};

#endif // INSTANTIATIONSWRITER_H