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

#include "ipxactmodels_global.h"

#include <QXmlStreamWriter>
#include <QDomNode>
#include <QString>
#include <QMap>
#include <QPointF>
#include <QVector2D>
#include <QSharedPointer>

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
    IPXACTMODELS_EXPORT QString removeWhiteSpace(QString const& str);

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

    IPXACTMODELS_EXPORT QPointF parsePoint(QDomNode const& node);

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

    
}

//-----------------------------------------------------------------------------

#endif // XMLUTILS_H
