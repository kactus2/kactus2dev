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

#ifndef UTILS_H
#define UTILS_H

#include <QXmlStreamWriter>
#include <QDomNode>
#include <QString>
#include <QMap>
#include <QPointF>
#include <QVector2D>

// TODO: Complete commentation.

//-----------------------------------------------------------------------------

namespace XmlUtils
{
    /*!
     *  Parses positions from the given XML node.
     */
    void parsePositionsMap(QDomNode& node, QString const& identifier,
                           QString const& refIdentifier, QMap<QString, QPointF>& positions);

    /*!
     *  Parses ad-hoc visibilities (and positions) from the given XML node.
     */
    void parseAdHocVisibilities(QDomNode& node, QMap<QString, bool>& portAdHocVisibilities,
                                QMap<QString, QPointF>& adHocPortPositions);

    /*!
     *  Writes a kactus2:position to the XML stream.
     */
    void writePosition(QXmlStreamWriter& xmlWriter, QPointF const& pos);

    /*!
     *  Writes a kactus2:direction to the XML stream.
     */
    void writeDirection(QXmlStreamWriter& xmlWriter, QVector2D const& dir);

    /*!
     *  Writes the given port ad-hoc visibilities along with the port positions to
     *  a specific XML format.
     */
    void writeAdHocVisibilities(QXmlStreamWriter& xmlWriter,
                                QMap<QString, bool> const& adHocVisibilities,
                                QMap<QString, QPointF> const& adHocPortPositions);

    /*!
     *  Writes the given positions map to the XML stream, using the given identifier
     *  as the XML node name for the child items and reference identifier for the key.
     */
    void writePositionsMap(QXmlStreamWriter& writer, QMap<QString, QPointF> const& positions,
                           QString const& identifier, QString const& refIdentifier);
}

//-----------------------------------------------------------------------------

#endif // UTILS_H
