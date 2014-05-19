//-----------------------------------------------------------------------------
// File: GenericVendorExtension.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 8.5.2014
//
// Description:
// Generic implementation for any vendor extension.
//-----------------------------------------------------------------------------

#include "GenericVendorExtension.h"

#include <QDomNode>
#include <QObject>

//-----------------------------------------------------------------------------
// Function: GenericVendorExtension::GenericVendorExtension()
//-----------------------------------------------------------------------------
GenericVendorExtension::GenericVendorExtension(QDomNode const& extensionNode):
    vendorExtension_(extensionNode)
{

}

//-----------------------------------------------------------------------------
// Function: GenericVendorExtension::GenericVendorExtension()
//-----------------------------------------------------------------------------
GenericVendorExtension::GenericVendorExtension(GenericVendorExtension const& other):
    vendorExtension_(other.vendorExtension_)
{

}

//-----------------------------------------------------------------------------
// Function: GenericVendorExtension::~GenericVendorExtension()
//-----------------------------------------------------------------------------
GenericVendorExtension::~GenericVendorExtension()
{

}

//-----------------------------------------------------------------------------
// Function: GenericVendorExtension::write()
//-----------------------------------------------------------------------------
void GenericVendorExtension::write(QXmlStreamWriter& writer) const
{
    writeNode(vendorExtension_, writer);    
}

//-----------------------------------------------------------------------------
// Function: GenericVendorExtension::writeDomNode()
//-----------------------------------------------------------------------------
void GenericVendorExtension::writeNode(QDomNode const& node, QXmlStreamWriter& writer) const
{
    writer.writeStartElement(node.nodeName());
    writeAttributes(node, writer);
    writeChildNodes(node, writer);
    writer.writeEndElement();
}

//-----------------------------------------------------------------------------
// Function: GenericVendorExtension::writeAttributes()
//-----------------------------------------------------------------------------
void GenericVendorExtension::writeAttributes(QDomNode const& node, QXmlStreamWriter &writer) const
{
    QDomNamedNodeMap attributes = node.attributes();

    int attributeCount = node.attributes().length();
    for (int i = 0; i < attributeCount; i++)
    {
        QDomNode attribute = attributes.item(i);
        writer.writeAttribute(attribute.nodeName(), attribute.nodeValue());
    }
}

//-----------------------------------------------------------------------------
// Function: GenericVendorExtension::writeChildNodes()
//-----------------------------------------------------------------------------
void GenericVendorExtension::writeChildNodes(QDomNode const& node, QXmlStreamWriter& writer) const
{
    int childCount = node.childNodes().count();    
    for (int i = 0; i < childCount; i++)
    {
        QDomNode childNode = node.childNodes().at(i);
        if (childNode.isText())
        {
            writer.writeCharacters(childNode.nodeValue());
        }
        else
        {
            writeNode(childNode, writer);
        }
    }
}
