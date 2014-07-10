//-----------------------------------------------------------------------------
// File: Utils.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 9.5.2012
//
// Description:
// Common utilities for writing/parsing IP-XACT documents.
//-----------------------------------------------------------------------------

#include "XmlUtils.h"
#include "VendorExtension.h"

#include <QTextStream>
#include <QSharedPointer>

//-----------------------------------------------------------------------------

namespace XmlUtils
{

    //-----------------------------------------------------------------------------
    // Function: removeWhiteSpace()
    //-----------------------------------------------------------------------------
    QString removeWhiteSpace(QString str)
    {
        QTextStream stream(&str);
        QString resultStr;

        // remove white spaces from the start and the end
        str = str.trimmed();

        // keep parsing until the end is reached
        while (!stream.atEnd()) {
            QString temp;

            // strip the whitespace if any is found
            stream.skipWhiteSpace();
            stream >> temp;
            // if there is still characters left in the stream
            if (!stream.atEnd()) {
                // replace the skipped whitespace with '_'
                temp.append("_");
            }
            // add the parsed string to the total string
            resultStr += temp;
        }
        // return the string that has been stripped from white spaces
        return resultStr;
    }

    //-----------------------------------------------------------------------------
    // Function: parseAttributes()
    //-----------------------------------------------------------------------------
    QMap<QString, QString> parseAttributes(const QDomNode &node)
    {
        QMap<QString, QString> map;

            // parse all the attributes
            QDomNamedNodeMap attributeMap = node.attributes();
            for (int j = 0; j < attributeMap.size(); ++j) 
            {
                QString name = attributeMap.item(j).nodeName();
                QString value = attributeMap.item(j).nodeValue();

                map.insert(name, value);
            }            
        return map;
    }

    //-----------------------------------------------------------------------------
    // Function: writeAttributes()
    //-----------------------------------------------------------------------------
    void writeAttributes(QXmlStreamWriter& writer, const QMap<QString, QString>& attributes)
    {
        // go through all attributes in the map
        for (QMap<QString, QString>::const_iterator i = attributes.begin(); i != attributes.end(); ++i)
        {        
                writer.writeAttribute(i.key(), i.value());
        }
        return;
    }

    //-----------------------------------------------------------------------------
    // Function: parsePositionsMap()
    //-----------------------------------------------------------------------------
    void parsePositionsMap(QDomNode& node, QString const& identifier,
                            QString const& refIdentifier,
                            QMap<QString, QPointF>& positions)
    {
        for (int i = 0; i < node.childNodes().size(); ++i)
        {
            QDomNode childNode = node.childNodes().at(i);

            if (childNode.nodeName() == identifier)
            {
                QString name = childNode.attributes().namedItem(refIdentifier).nodeValue();
                QPointF pos;

                if (childNode.childNodes().size() > 0 &&
                    childNode.childNodes().at(0).nodeName() == "kactus2:position")
                {
                    QDomNode posNode = childNode.childNodes().at(0);
                    pos = parsePoint(posNode);
                }

                positions[name] = pos;
            }
        }
    }

    //-----------------------------------------------------------------------------
    // Function: parseAdHocVisibilities()
    //-----------------------------------------------------------------------------
    void parseAdHocVisibilities(QDomNode& node, QMap<QString, bool>& portAdHocVisibilities,
                                QMap<QString, QPointF>& adHocPortPositions)
    {
        for (int i = 0; i < node.childNodes().size(); ++i)
        {
            QDomNode childNode = node.childNodes().at(i);

            if (childNode.nodeName() == "kactus2:adHocVisible")
            {
                QString name = childNode.attributes().namedItem("portName").nodeValue();
                portAdHocVisibilities[name] = true;

                QPointF pos = parsePoint(childNode);
                adHocPortPositions[name] = pos;
            }
        }
    }

    //-----------------------------------------------------------------------------
    // Function: parsePosition()
    //-----------------------------------------------------------------------------
    QPointF parsePoint(QDomNode const& node)
    {
        int x = node.attributes().namedItem("x").nodeValue().toInt();
        int y = node.attributes().namedItem("y").nodeValue().toInt();

        return QPointF(x, y);
    }

    //-----------------------------------------------------------------------------
    // Function: writePosition()
    //-----------------------------------------------------------------------------
    void writePosition(QXmlStreamWriter& xmlWriter, QPointF const& pos)
    {
        xmlWriter.writeEmptyElement("kactus2:position");
        xmlWriter.writeAttribute("x", QString::number(int(pos.x())));
        xmlWriter.writeAttribute("y", QString::number(int(pos.y())));
    }

    //-----------------------------------------------------------------------------
    // Function: writeDirection()
    //-----------------------------------------------------------------------------
    void writeDirection(QXmlStreamWriter& xmlWriter, QVector2D const& dir)
    {
        xmlWriter.writeEmptyElement("kactus2:direction");
        xmlWriter.writeAttribute("x", QString::number(int(dir.x())));
        xmlWriter.writeAttribute("y", QString::number(int(dir.y())));
    }

    //-----------------------------------------------------------------------------
    // Function: writeAdHocVisibilities()
    //-----------------------------------------------------------------------------
    void writeAdHocVisibilities(QXmlStreamWriter& xmlWriter,
                                QMap<QString, bool> const& adHocVisibilities,
                                QMap<QString, QPointF> const& adHocPortPositions)
    {
        QMapIterator<QString, bool> itrAdHoc(adHocVisibilities);

        if (!adHocVisibilities.isEmpty())
        {
            xmlWriter.writeStartElement("kactus2:adHocVisibilities");

            while (itrAdHoc.hasNext())
            {
                itrAdHoc.next();

                if (itrAdHoc.value())
                {
                    xmlWriter.writeStartElement("kactus2:adHocVisible");
                    xmlWriter.writeAttribute("portName", itrAdHoc.key());
                    xmlWriter.writeAttribute("x", QString::number(int(adHocPortPositions.value(itrAdHoc.key()).x())));
                    xmlWriter.writeAttribute("y", QString::number(int(adHocPortPositions.value(itrAdHoc.key()).y())));
                    xmlWriter.writeEndElement();
                }
            }

            xmlWriter.writeEndElement(); // kactus2:adHocVisibilities
        }
    }

    //-----------------------------------------------------------------------------
    // Function: writePositionsMap()
    //-----------------------------------------------------------------------------
    void writePositionsMap(QXmlStreamWriter& writer, QMap<QString, QPointF> const& positions,
                           QString const& identifier, QString const& refIdentifier)
    {
        if (!positions.isEmpty())
        {
            QMapIterator<QString, QPointF> itrPortPos(positions);
            writer.writeStartElement(identifier + "s");

            while (itrPortPos.hasNext())
            {
                itrPortPos.next();

                writer.writeStartElement(identifier);
                writer.writeAttribute(refIdentifier, itrPortPos.key());
                writePosition(writer, itrPortPos.value());
                writer.writeEndElement();
            }

            writer.writeEndElement();
        }
    }

    //-----------------------------------------------------------------------------
    // Function: writeVendorExtensions()
    //-----------------------------------------------------------------------------
    void writeVendorExtensions(QXmlStreamWriter& writer, 
        QList<QSharedPointer<VendorExtension> > const& vendorExtensions)
    {
        foreach(QSharedPointer<VendorExtension> extension, vendorExtensions)
        {
            extension->write(writer);
        }
    }
}