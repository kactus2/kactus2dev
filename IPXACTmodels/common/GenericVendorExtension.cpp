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
    name_(extensionNode.nodeName()),
    value_(),
    attributes_(),
    children_()
{
    QDomNamedNodeMap attributes = extensionNode.attributes();
    const int attributeCount = attributes.length();
    for (int i = 0; i < attributeCount; ++i)
    {
        QDomNode attribute = attributes.item(i);
        attributes_.append(qMakePair(attribute.nodeName(), attribute.nodeValue()));
    }

    QDomNodeList childNodes = extensionNode.childNodes();
    value_ = extensionNode.firstChild().nodeValue();

    const int childCount = childNodes.count();
    for (int i = 1; i < childCount; ++i)
    {
        QDomNode childNode = childNodes.at(i);
        children_.append(GenericVendorExtension(childNode));
    }
}

//-----------------------------------------------------------------------------
// Function: GenericVendorExtension::clone()
//-----------------------------------------------------------------------------
GenericVendorExtension* GenericVendorExtension::clone() const
{
    return new GenericVendorExtension(*this);
}

//-----------------------------------------------------------------------------
// Function: GenericVendorExtension::type()
//-----------------------------------------------------------------------------
QString GenericVendorExtension::type() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: GenericVendorExtension::write()
//-----------------------------------------------------------------------------
void GenericVendorExtension::write(QXmlStreamWriter& writer) const
{
    writeNode(*this, writer);    
}

//-----------------------------------------------------------------------------
// Function: GenericVendorExtension::name()
//-----------------------------------------------------------------------------
QString GenericVendorExtension::name() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: GenericVendorExtension::setName()
//-----------------------------------------------------------------------------
void GenericVendorExtension::setName(QString const& name)
{
    name_ = name;
}

//-----------------------------------------------------------------------------
// Function: GenericVendorExtension::value()
//-----------------------------------------------------------------------------
QString GenericVendorExtension::value() const
{
    return value_;
}

//-----------------------------------------------------------------------------
// Function: GenericVendorExtension::setValue()
//-----------------------------------------------------------------------------
void GenericVendorExtension::setValue(QString const& value)
{
    value_ = value;
}

//-----------------------------------------------------------------------------
// Function: GenericVendorExtension::attributeValue()
//-----------------------------------------------------------------------------
QString GenericVendorExtension::attributeValue(QString const& attributeName) const
{
    auto target = std::find_if(attributes_.cbegin(), attributes_.cend(), 
        [attributeName](QPair<QString, QString> const& attribute) {return attribute.first == attributeName;});

    if (target == attributes_.cend())
    {
        return QString();
    }

    return target->second;
}

//-----------------------------------------------------------------------------
// Function: GenericVendorExtension::setAttributeValue()
//-----------------------------------------------------------------------------
void GenericVendorExtension::setAttributeValue(QString const& attributeName, QString const& attributeValue)
{
    auto target = std::find_if(attributes_.begin(), attributes_.end(),
        [attributeName](QPair<QString, QString> const& attribute) {return attribute.first == attributeName; });

    if (target == attributes_.end())
    {
        attributes_.append(qMakePair(attributeName, attributeValue));
    }
    else
    {
        target->second = attributeValue;
    }
}

//-----------------------------------------------------------------------------
// Function: GenericVendorExtension::writeNode()
//-----------------------------------------------------------------------------
void GenericVendorExtension::writeNode(GenericVendorExtension const& node, QXmlStreamWriter& writer) const
{
    writer.writeStartElement(node.name());
    writeAttributes(node, writer);
    if (value_.isEmpty() == false)
    {
        writer.writeCharacters(value_);
    }
    writeChildNodes(node, writer);
    writer.writeEndElement();
}

//-----------------------------------------------------------------------------
// Function: GenericVendorExtension::writeAttributes()
//-----------------------------------------------------------------------------
void GenericVendorExtension::writeAttributes(GenericVendorExtension const& node, QXmlStreamWriter& writer) const
{
    for (auto attribute : node.attributes_)
    {      
        writer.writeAttribute(attribute.first, attribute.second);
    }
}

//-----------------------------------------------------------------------------
// Function: GenericVendorExtension::writeChildNodes()
//-----------------------------------------------------------------------------
void GenericVendorExtension::writeChildNodes(GenericVendorExtension const& node, QXmlStreamWriter& writer) const
{
    for (auto child : node.children_)
    {
        writeNode(child, writer);                
    }
}
