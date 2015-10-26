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

#include <QTextStream>
#include <QSharedPointer>

//-----------------------------------------------------------------------------

namespace XmlUtils
{

    //-----------------------------------------------------------------------------
    // Function: removeWhiteSpace()
    //-----------------------------------------------------------------------------
    IPXACTMODELS_EXPORT QString removeWhiteSpace(QString const& str)
    {
        return str.simplified().replace(' ', '_');
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
}