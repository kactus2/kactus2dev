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
#include <QStringList>

//! Description element name.
namespace
{
    const QString DESCRIPTION = QStringLiteral("Description");
};

//-----------------------------------------------------------------------------
// Function: GenericVendorExtension::GenericVendorExtension()
//-----------------------------------------------------------------------------
GenericVendorExtension::GenericVendorExtension(QDomNode const& extensionNode, GenericVendorExtension* parent):
nameSpace_(),
name_(),
value_(),
attributes_(),
children_(),
parent_(parent)
{
    QStringList combiName = extensionNode.nodeName().split(QLatin1Char(':'));
    if (combiName.size() == 2)
    {
        nameSpace_ = combiName.first();
    }
    name_ = combiName.last();

    QDomNamedNodeMap attributes = extensionNode.attributes();
    const int attributeCount = attributes.length();
    for (int i = 0; i < attributeCount; ++i)
    {
        QDomNode attribute = attributes.item(i);
        attributes_.append(qMakePair(attribute.nodeName(), attribute.nodeValue()));
    }

    QDomNode firstChildNode = extensionNode.firstChild();
    if (!firstChildNode.isNull())
    {
        int startIndex = 0;
        if (firstChildNode.isText())
        {
            value_ = firstChildNode.nodeValue();
            startIndex = 1;
        }

        QDomNodeList childNodes = extensionNode.childNodes();

        const int childCount = childNodes.count();
        for (int i = startIndex; i < childCount; ++i)
        {
            QDomNode childNode = childNodes.at(i);
            children_.append(new GenericVendorExtension(childNode, this));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: GenericVendorExtension::~GenericVendorExtension()
//-----------------------------------------------------------------------------
GenericVendorExtension::~GenericVendorExtension()
{
    for (int i = children_.size() - 1; i >= 0; --i)
    {
        GenericVendorExtension* child = children_.at(i);
        delete child;
        child = nullptr;
    }

    children_.clear();
}

//-----------------------------------------------------------------------------
// Function: GenericVendorExtension::GenericVendorExtension()
//-----------------------------------------------------------------------------
GenericVendorExtension::GenericVendorExtension(GenericVendorExtension const& other)
{
    nameSpace_ = other.nameSpace_;
    name_ = other.name_;
    value_ = other.value_;
    attributes_ = other.attributes_;
    children_.clear();
    parent_ = nullptr;

    children_.clear();

    for (auto otherChild : other.children_)
    {
        GenericVendorExtension* newChild = new GenericVendorExtension(*otherChild);
        newChild->setParent(this);
        children_.append(newChild);
    }
}

//-----------------------------------------------------------------------------
// Function: GenericVendorExtension::operator=()
//-----------------------------------------------------------------------------
GenericVendorExtension& GenericVendorExtension::operator=(const GenericVendorExtension& other)
{
    if (this != &other)
    {
        nameSpace_ = other.nameSpace_;
        name_ = other.name_;
        value_ = other.value_;
        attributes_ = other.attributes_;
        children_ = other.children_;
        parent_ = other.parent_;
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: GenericVendorExtension::isSame()
//-----------------------------------------------------------------------------
bool GenericVendorExtension::isSame(const GenericVendorExtension& otherItem) const
{
    return this == &otherItem;
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
// Function: GenericVendorExtension::nameSpace()
//-----------------------------------------------------------------------------
QString GenericVendorExtension::nameSpace() const
{
    return nameSpace_;
}

//-----------------------------------------------------------------------------
// Function: GenericVendorExtension::setNameSpace()
//-----------------------------------------------------------------------------
void GenericVendorExtension::setNameSpace(QString const& newNameSpace)
{
    nameSpace_ = newNameSpace;

    if (hasDescription())
    {
        setDescription(getDescription());
    }
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
// Function: GenericVendorExtension::description()
//-----------------------------------------------------------------------------
QString GenericVendorExtension::getDescription() const
{
    for (auto childItem : children_)
    {
        if (childItem->name().endsWith(DESCRIPTION))
        {
            return childItem->value();
        }
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: GenericVendorExtension::setDescription()
//-----------------------------------------------------------------------------
void GenericVendorExtension::setDescription(QString const& newDescription)
{
    for (int i = 0; i < children_.size(); ++i)
    {
        GenericVendorExtension* childItem = children_[i];
        if (childItem->name().endsWith(DESCRIPTION))
        {
            if (newDescription.isEmpty())
            {
                delete childItem;
                childItem = nullptr;
                children_.remove(i);
            }
            else
            {
                childItem->setValue(newDescription);
                childItem->setNameSpace(nameSpace());
            }

            return;
        }
    }

    if (!newDescription.isEmpty())
    {
        GenericVendorExtension* newDescriptionChild(new GenericVendorExtension());
        newDescriptionChild->setNameSpace(nameSpace());
        newDescriptionChild->setName(DESCRIPTION);
        newDescriptionChild->setValue(newDescription);
        newDescriptionChild->setParent(this);

        children_.append(newDescriptionChild);
    }
}

//-----------------------------------------------------------------------------
// Function: GenericVendorExtension::hasDescription()
//-----------------------------------------------------------------------------
bool GenericVendorExtension::hasDescription() const
{
    for (auto child : children_)
    {
        if (child->name().endsWith(DESCRIPTION))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: GenericVendorExtension::getDescriptionIndex()
//-----------------------------------------------------------------------------
int GenericVendorExtension::getDescriptionIndex() const
{
    for (int i = 0; i < children_.count(); ++i)
    {
        if (children_.at(i)->name().endsWith(DESCRIPTION))
        {
            return i;
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Function: GenericVendorExtension::writeNode()
//-----------------------------------------------------------------------------
void GenericVendorExtension::writeNode(GenericVendorExtension const& node, QXmlStreamWriter& writer) const
{
    if (node.nameSpace().isEmpty() && node.name().isEmpty() && node.value().isEmpty() &&
        node.attributes_.isEmpty() && node.children_.isEmpty())
    {
        return;
    }

    QString combinedName = node.nameSpace() + QLatin1Char(':') + node.name();
    writer.writeStartElement(combinedName);
    writeAttributes(node, writer);
    if (node.value().isEmpty() == false)
    {
        writer.writeCharacters(node.value());
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
        writeNode(*child, writer);                
    }
}

//-----------------------------------------------------------------------------
// Function: GenericVendorExtension::getChildExtensions()
//-----------------------------------------------------------------------------
QVector<GenericVendorExtension*>& GenericVendorExtension::getChildExtensions()
{
    return children_;
}

//-----------------------------------------------------------------------------
// Function: GenericVendorExtension::removeIndexedChildExtensions()
//-----------------------------------------------------------------------------
void GenericVendorExtension::removeIndexedChildExtensions(int const& startIndex, int const& endIndex)
{
    for (int i = endIndex; i >= startIndex; --i)
    {
        GenericVendorExtension* child = children_.at(i);
        children_.removeAt(i);
        delete child;
        child = nullptr;
    }
}

//-----------------------------------------------------------------------------
// Function: GenericVendorExtension::getParent()
//-----------------------------------------------------------------------------
GenericVendorExtension* GenericVendorExtension::getParent() const
{
    return parent_;
}

//-----------------------------------------------------------------------------
// Function: GenericVendorExtension::setParent()
//-----------------------------------------------------------------------------
void GenericVendorExtension::setParent(GenericVendorExtension* newParent)
{
    parent_ = newParent;
}

//-----------------------------------------------------------------------------
// Function: GenericVendorExtension::getAttributes()
//-----------------------------------------------------------------------------
QVector<QPair<QString, QString> > GenericVendorExtension::getAttributes() const
{
    return attributes_;
}

//-----------------------------------------------------------------------------
// Function: GenericVendorExtension::setNewAttributes()
//-----------------------------------------------------------------------------
void GenericVendorExtension::setNewAttributes(QVector<QPair<QString, QString> > newAttributes)
{
    attributes_ = newAttributes;
}
