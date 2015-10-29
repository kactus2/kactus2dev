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
    // Function: parsePoint()
    //-----------------------------------------------------------------------------
    QPointF parsePoint(QDomNode const& node)
    {
        int x = node.attributes().namedItem("x").nodeValue().toInt();
        int y = node.attributes().namedItem("y").nodeValue().toInt();

        return QPointF(x, y);
    }
}
