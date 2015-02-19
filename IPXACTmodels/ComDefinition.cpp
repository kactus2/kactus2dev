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
                                                 transferTypes_(),
                                                 properties_()
{
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::ComDefinition()
//-----------------------------------------------------------------------------
ComDefinition::ComDefinition(ComDefinition const& rhs) : LibraryComponent(rhs),
                                                         transferTypes_(rhs.transferTypes_),
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
                                                  transferTypes_(),
                                                  properties_()
{
    LibraryComponent::vlnv_->setType(VLNV::COMDEFINITION);

    // Find the IP-Xact root element.
    QDomNodeList nodeList = doc.childNodes();

    int i = 0;

    while (!nodeList.at(i).hasChildNodes())
    {
		// if the node is for a header comment
		if (nodeList.at(i).isComment()) {
			topComments_.append(nodeList.at(i).nodeValue());
		}
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

        if (childNode.nodeName() == "kactus2:transferTypes")
        {
            parseTransferTypes(childNode);
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
// Function: ComDefinition::clone()
//-----------------------------------------------------------------------------
QSharedPointer<LibraryComponent> ComDefinition::clone() const
{
    return QSharedPointer<LibraryComponent>(new ComDefinition(*this));
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
    writer.writeStartElement("kactus2:transferTypes");

    foreach (QString const& type, transferTypes_)
    {
        writer.writeEmptyElement("kactus2:transferType");
        writer.writeAttribute("kactus2:name", type);
    }

    writer.writeEndElement(); // kactus2:transferTypes

    // Write properties.
    writer.writeStartElement("kactus2:properties");

    foreach (QSharedPointer<ComProperty> prop, properties_)
    {
        prop->write(writer);
    }

    writer.writeEndElement(); // kactus2:properties

    if (!vendorExtensions_.isEmpty())
    {
        writer.writeStartElement("spirit:vendorExtensions");
        writeVendorExtensions(writer);
        writer.writeEndElement(); // spirit:vendorExtensions
    }

    writer.writeEndElement(); // kactus2:comDefinition
    writer.writeEndDocument();
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::isValid()
//-----------------------------------------------------------------------------
bool ComDefinition::isValid(QStringList& errorList) const
{
    QString thisIdentifier(QObject::tr("the containing COM definition"));
    bool valid = true;

    if (!vlnv_)
    {
        errorList.append(QObject::tr("No vlnv specified for the COM definition."));
        valid = false;
    }
    else if (!vlnv_->isValid(errorList, thisIdentifier))
    {
        valid = false;
    }
    else
    {
        thisIdentifier = QObject::tr("COM definition '%1'").arg(vlnv_->toString());
    }

    // Check for multiple definitions of same data type.
    QStringList dataTypeNames;

    foreach (QString const& dataType, transferTypes_)
    {
        if (dataTypeNames.contains(dataType))
        {
            errorList.append(QObject::tr("Data type '%1' defined multiple times"
                                         "in '%2'").arg(dataType, thisIdentifier));
            valid = false;
        }
        else
        {
            dataTypeNames.push_back(dataType);
        }
    }

    // Validate the properties.
    QStringList propertyNames;

    foreach (QSharedPointer<ComProperty> prop, properties_)
    {
        if (propertyNames.contains(prop->getName()))
        {
            errorList.append(QObject::tr("Property '%1' defined multiple times"
                                         "in %2").arg(prop->getName(), thisIdentifier));
            valid = false;
        }
        else
        {
            propertyNames.append(prop->getName());
        }

        if (!prop->isValid(errorList, thisIdentifier))
        {
            valid = false;
        }
    }

    return valid;
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::isValid()
//-----------------------------------------------------------------------------
bool ComDefinition::isValid() const
{
    if (!vlnv_ || !vlnv_->isValid())
    {
        return false;
    }
    
    // Check for multiple definitions of same transfer type.
    QStringList transferTypeNames;

    foreach (QString const& transferType, transferTypes_)
    {
        if (transferTypeNames.contains(transferType))
        {
            return false;
        }
        else
        {
            transferTypeNames.push_back(transferType);
        }
    }

    // Validate the properties.
    QStringList propertyNames;

    foreach (QSharedPointer<ComProperty> prop, properties_)
    {
        if (propertyNames.contains(prop->getName()))
        {
            return false;
        }
        else
        {
            propertyNames.append(prop->getName());
        }

        if (!prop->isValid())
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::getDependentFiles()
//-----------------------------------------------------------------------------
QStringList const ComDefinition::getDependentFiles() const
{
    return QStringList();
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::getDependentVLNVs()
//-----------------------------------------------------------------------------
QList<VLNV> const ComDefinition::getDependentVLNVs() const
{
    return QList<VLNV>();
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::addTransferType()
//-----------------------------------------------------------------------------
void ComDefinition::addTransferType(QString const& type)
{
    // Check for duplicates.
    if (!transferTypes_.contains(type))
    {
        transferTypes_.append(type);
    }
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::removeTransferType()
//-----------------------------------------------------------------------------
void ComDefinition::removeTransferType(QString const& type)
{
    transferTypes_.removeAll(type);
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::setTransferTypes()
//-----------------------------------------------------------------------------
void ComDefinition::setTransferTypes(QStringList const& types)
{
    transferTypes_ = types;
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
// Function: ComDefinition::getTransferTypes()
//-----------------------------------------------------------------------------
QStringList const& ComDefinition::getTransferTypes() const
{
    return transferTypes_;
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::getProperties()
//-----------------------------------------------------------------------------
QList< QSharedPointer<ComProperty> > const& ComDefinition::getProperties() const
{
    return properties_;
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::parseTransferTypes()
//-----------------------------------------------------------------------------
void ComDefinition::parseTransferTypes(QDomNode& node)
{
    for (int i = 0; i < node.childNodes().count(); ++i)
    {
        QDomNode typeNode = node.childNodes().at(i);

        if (typeNode.nodeName() == "kactus2:transferType")
        {
            QString name = typeNode.attributes().namedItem("kactus2:name").nodeValue();
            transferTypes_.append(name);
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
