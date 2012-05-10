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

//-----------------------------------------------------------------------------

namespace XmlUtils
{
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
                    pos.setX(posNode.attributes().namedItem("x").nodeValue().toInt());
                    pos.setY(posNode.attributes().namedItem("y").nodeValue().toInt());
                }

                positions[name] = pos;
            }
        }
    }

    //-----------------------------------------------------------------------------
    // Function: Design::ComponentInstance::parseAdHocVisibilities()
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

                QPointF pos;
                pos.setX(childNode.attributes().namedItem("x").nodeValue().toInt());
                pos.setY(childNode.attributes().namedItem("y").nodeValue().toInt());

                adHocPortPositions[name] = pos;
            }
        }
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
}