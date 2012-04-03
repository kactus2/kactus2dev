//-----------------------------------------------------------------------------
// File: ComDefinition.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 2.4.2012
//
// Description:
// Class which encapsulates the handling of a custom communication definition
// object.
//-----------------------------------------------------------------------------

#include "ComDefinition.h"

#include "ComProperty.h"

//-----------------------------------------------------------------------------
// Function: ComDefinition::ComDefinition()
//-----------------------------------------------------------------------------
ComDefinition::ComDefinition(VLNV const& vlnv) : LibraryComponent(vlnv),
                                                 dataTypes_(),
                                                 properties_()
{
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::ComDefinition()
//-----------------------------------------------------------------------------
ComDefinition::ComDefinition(ComDefinition const& rhs) : LibraryComponent(rhs),
                                                         dataTypes_(rhs.dataTypes_),
                                                         properties_()
{
    // Make deep copies of the properties.
    foreach (QSharedPointer<ComProperty> property, rhs.properties_)
    {
        properties_.append(QSharedPointer<ComProperty>(new ComProperty(*property.data())));
    }
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::ComDefinition()
//-----------------------------------------------------------------------------
ComDefinition::ComDefinition(QDomDocument& doc) : LibraryComponent(doc),
                                                  dataTypes_(),
                                                  properties_()
{
    LibraryComponent::vlnv_->setType(VLNV::COMDEFINITION);

    // Find the IP-Xact root element.
    QDomNodeList nodeList = doc.childNodes();

    int i = 0;

    while (!nodeList.at(i).hasChildNodes())
    {
        ++i;
    }

    QDomNode comNode = doc.childNodes().item(i);

    // Parse child nodes.
    for (int i = 0; i < comNode.childNodes().count(); ++i)
    {
        QDomNode childNode = comNode.childNodes().at(i);

        if (childNode.isComment())
        {
            continue;
        }

        if (childNode.nodeName() == "kactus2:dataTypes")
        {
            parseDataTypes(childNode);
        }
        else if (childNode.nodeName() == "kactus2:properties")
        {
            parseProperties(childNode);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::~ComDefinition()
//-----------------------------------------------------------------------------
ComDefinition::~ComDefinition()
{
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::write()
//-----------------------------------------------------------------------------
void ComDefinition::write(QFile& file)
{
    QXmlStreamWriter writer(&file);

    // Set the writer to use auto formatting for a clearer output.
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(-1);

    // Write basic information.
    LibraryComponent::write(writer);
    LibraryComponent::writeVLNV(writer);

    if (!LibraryComponent::description_.isEmpty())
    {
        writer.writeTextElement("spirit:description", description_);
    }

    // Write data types.
    writer.writeStartElement("kactus2:dataTypes");

    foreach (QString const& type, dataTypes_)
    {
        writer.writeEmptyElement("kactus2:dataType");
        writer.writeAttribute("kactus2:name", type);
    }

    writer.writeEndElement(); // kactus2:dataTypes

    // Write properties.
    writer.writeStartElement("kactus2:properties");

    foreach (QSharedPointer<ComProperty> prop, properties_)
    {
        prop->write(writer);
    }

    writer.writeEndElement(); // kactus2:properties
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::addDataType()
//-----------------------------------------------------------------------------
void ComDefinition::addDataType(QString const& type)
{
    // Check for duplicates
    if (!dataTypes_.contains(type))
    {
        dataTypes_.append(type);
    }
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::removeDataType()
//-----------------------------------------------------------------------------
void ComDefinition::removeDataType(QString const& type)
{
    dataTypes_.removeAll(type);
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::setDataTypes()
//-----------------------------------------------------------------------------
void ComDefinition::setDataTypes(QStringList const& types)
{
    dataTypes_ = types;
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::addProperty()
//-----------------------------------------------------------------------------
void ComDefinition::addProperty(QSharedPointer<ComProperty> prop)
{
    properties_.append(prop);
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::removeProperty()
//-----------------------------------------------------------------------------
void ComDefinition::removeProperty(QString const& name)
{
    foreach (QSharedPointer<ComProperty> property, properties_)
    {
        if (property->getName() == name)
        {
            properties_.removeOne(property);
            break;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::setProperties()
//-----------------------------------------------------------------------------
void ComDefinition::setProperties(QList< QSharedPointer<ComProperty> > const& properties)
{
    properties_ = properties;
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::getDataTypes()
//-----------------------------------------------------------------------------
QStringList const& ComDefinition::getDataTypes() const
{
    return dataTypes_;
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::getProperties()
//-----------------------------------------------------------------------------
QList< QSharedPointer<ComProperty> > ComDefinition::getProperties() const
{
    return properties_;
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::parseDataTypes()
//-----------------------------------------------------------------------------
void ComDefinition::parseDataTypes(QDomNode& node)
{
    for (int i = 0; i < node.childNodes().count(); ++i)
    {
        QDomNode typeNode = node.childNodes().at(i);

        if (typeNode.nodeName() == "kactus2:dataType")
        {
            QString name = typeNode.attributes().namedItem("kactus2:name").nodeValue();
            dataTypes_.append(name);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::parseProperties()
//-----------------------------------------------------------------------------
void ComDefinition::parseProperties(QDomNode& node)
{
    for (int i = 0; i < node.childNodes().count(); ++i)
    {
        QDomNode propNode = node.childNodes().at(i);

        if (propNode.nodeName() == "kactus2:property")
        {
            QSharedPointer<ComProperty> prop(new ComProperty(propNode));
            properties_.append(prop);
        }
    }
}