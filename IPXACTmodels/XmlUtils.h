//-----------------------------------------------------------------------------
// File: Utils.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 9.5.2012
//
// Description:
// Common utilities for writing/parsing IP-XACT documents.
//-----------------------------------------------------------------------------

#ifndef XMLUTILS_H
#define XMLUTILS_H

#include <QXmlStreamWriter>
#include <QDomNode>
#include <QString>
#include <QMap>
#include <QPointF>
#include <QVector2D>
#include <QSharedPointer>

class VendorExtension;

//-----------------------------------------------------------------------------

namespace XmlUtils
{
    /*!
     *  Parses named positions from the given XML node.
     *
     *      @param [in]  node           The XML node containing the positions.
     *      @param [in]  identifier     The name of the node children.
     *      @param [in]  refIdentifier  The name of the attribute containing the name reference.
     *      @param [out] positions      The read positions.
     */
    void parsePositionsMap(QDomNode& node, QString const& identifier,
                           QString const& refIdentifier, QMap<QString, QPointF>& positions);

    /*!
     *  Parses ad-hoc visibilities (and positions) from the given XML node.
     *
     *      @param [in]  node                   The XML node containing the ad-hoc information.
     *      @param [out] portAdHocVisibilities  The read ad-hoc visibilities.
     *      @param [out] adHocPortPositions     The read ad-hoc port positions.
     */
    void parseAdHocVisibilities(QDomNode& node, QMap<QString, bool>& portAdHocVisibilities,
                                QMap<QString, QPointF>& adHocPortPositions);

    QPointF parsePoint(QDomNode const& node);

    /*!
     *  Writes a kactus2:position to the XML stream.
     *
     *      @param [in] xmlWriter  The XML writer.
     *      @param [in] pos        The position to write.
     */
    void writePosition(QXmlStreamWriter& xmlWriter, QPointF const& pos);

    /*!
     *  Writes a kactus2:direction to the XML stream.
     *
     *      @param [in] xmlWriter  The XML writer.
     *      @param [in] dir        The direction to write.
     */
    void writeDirection(QXmlStreamWriter& xmlWriter, QVector2D const& dir);

    /*!
     *  Writes the given port ad-hoc visibilities along with the port positions to
     *  a specific XML format.
     *
     *      @param [in] xmlWriter           The XML writer.
     *      @param [in] adHocVisibilities   The ad-hoc visibilities to write.
     *      @param [in] adHocPortPositions  The ad-hoc port positions to write.
     */
    void writeAdHocVisibilities(QXmlStreamWriter& xmlWriter,
                                QMap<QString, bool> const& adHocVisibilities,
                                QMap<QString, QPointF> const& adHocPortPositions);

    /*!
     *  Writes the given positions map to the XML stream, using the given identifier
     *  as the XML node name for the child items and reference identifier for the key.
     *
     *      @param [in] writer        The XML writer.
     *      @param [in] positions     The name-positions pairs to write.
     *      @param [in] identifier    The identifier for the position nodes.
     *      @param [in] refIdentifier The attribute name used for the name reference.
     */
    void writePositionsMap(QXmlStreamWriter& writer, QMap<QString, QPointF> const& positions,
                           QString const& identifier, QString const& refIdentifier);

    /*!
     *  Creates a vendor extension from an XML node.
     *
     *      @param [in] node   The node to create from.
     *
     *      @return The created vendor extension.
     */
    QSharedPointer<VendorExtension> createVendorExtensionFromNode(QDomNode const& node);

    /*!
     *  Writes the vendor extensions to an XML using the given XML writer.
     *
     *      @param [in] writer              The writer to use.
     *      @param [in] vendorExtensions    The vendor extensions to write.
     */
    void writeVendorExtensions(QXmlStreamWriter& writer, 
                               QList<QSharedPointer<VendorExtension> > const& vendorExtensions);
    
}

//-----------------------------------------------------------------------------

#endif // XMLUTILS_H
