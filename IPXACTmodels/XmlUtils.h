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

    /*! \brief removes white spaces from the given QString
     *
     *	This function removes white spaces from the beginning and end of the string
     *	and also replaces any embedded whitespaces with characted '_'. If you only
     *	want to remove the white spaces from the start and end use
     *	QString::trimmed() instead of this.
     *
     *	IP-Xact types used for this function:
     *	-Name
     *	-NMTOKEN
     *	-PortName
     *	-ID
     *	-IDREF
     *
     * \param str A reference to the QString where white spaces are wanted to
     * be removed from.
     *
     * \return QString containing the string that has been stripped from
     * whitespaces and embedded whitespaces have been replaces with '_'
     */
    QString removeWhiteSpace(QString str);

    /*! Parses the attributes from the root of the given node to a QMap.
     *
     *       @param [in] node       A QDomNode to parse the attributes from.
     *
     *       @return    A QMap with the parsed attributes.
     */
    QMap<QString, QString> parseAttributes(const QDomNode &node);

    /*! Reads the attributes from the QMap and uses writer to write them. The
     *  element to write the attributes to must be opened by the writer before
     *  calling this function.
     *
     *  @param [in] writer      A QXmlStreamWriter instance that has been set up to write a document.
     *  @param [in] attributes  A reference to a QMap that contains the attributes to be written.
     */
    void writeAttributes(QXmlStreamWriter& writer, const QMap<QString, QString>& attributes);

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
