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

#include <IPXACTmodels/common/GenericVendorExtension.h>

#include <IPXACTmodels/kactusExtensions/ComProperty.h>

//-----------------------------------------------------------------------------
// Function: ComDefinition::ComDefinition()
//-----------------------------------------------------------------------------
ComDefinition::ComDefinition(VLNV const& vlnv) : Document(vlnv),
    transferTypes_(),
    properties_()
{
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::ComDefinition()
//-----------------------------------------------------------------------------
ComDefinition::ComDefinition(ComDefinition const& rhs) : Document(rhs),
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
ComDefinition::ComDefinition(QDomDocument& doc) : Document(),
                                                  transferTypes_(),
                                                  properties_()
{
	QString vendor = doc.firstChildElement("ipxact:vendor").firstChild().nodeValue();
	QString library = doc.firstChildElement("ipxact:library").firstChild().nodeValue();
	QString name = doc.firstChildElement("ipxact:name").firstChild().nodeValue();
	QString version = doc.firstChildElement("ipxact:version").firstChild().nodeValue();

	VLNV itemVLNV;
	itemVLNV.setType(VLNV::COMDEFINITION);
	itemVLNV.setVendor(vendor);
	itemVLNV.setLibrary(library);
	itemVLNV.setName(name);
	itemVLNV.setVersion(version);

	QDomNodeList extensionNodes = doc.firstChildElement("ipxact:vendorExtensions").childNodes();

	int extensionCount = extensionNodes.count();
	for (int i = 0; i < extensionCount; i++)
	{
		QDomNode extensionNode = extensionNodes.at(i);

		if (!extensionNode.nodeName().startsWith("kactus2:"))
		{
			QSharedPointer<VendorExtension> extension(new GenericVendorExtension(extensionNode));
			getVendorExtensions()->append(extension);
		}
	}

    QDomNode comNode = doc.childNodes().item(0);

    // Parse child nodes.
    for (int i = 0; i < comNode.childNodes().count(); ++i)
    {
        QDomNode childNode = comNode.childNodes().at(i);

        if (childNode.isComment())
        {
            continue;
        }

		if (childNode.nodeName() ==QString("ipxact:description"))
		{
			setDescription( childNode.childNodes().at(0).nodeValue() );
		}
		else if (childNode.nodeName() == "kactus2:transferTypes")
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
QSharedPointer<Document> ComDefinition::clone() const
{
    return QSharedPointer<Document>(new ComDefinition(*this));
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::write()
//-----------------------------------------------------------------------------
void ComDefinition::write(QXmlStreamWriter& writer)
{
    // Set the writer to use auto formatting for a clearer output.
    writer.setAutoFormatting(true);
	writer.setAutoFormattingIndent(-1);

	// write the element that specifies the type of the document.
	writer.writeStartElement(getVlnv().getTypestr());

	// Write basic information.
	writer.writeTextElement("ipxact:vendor", getVlnv().getVendor());
	writer.writeTextElement("ipxact:library", getVlnv().getLibrary());
	writer.writeTextElement("ipxact:name", getVlnv().getName());
	writer.writeTextElement("ipxact:version", getVlnv().getVersion());

	if (!Document::getDescription().isEmpty())
	{
		writer.writeTextElement("ipxact:description", Document::getDescription());
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

	if (getVendorExtensions()->isEmpty())
	{
		writer.writeStartElement("ipxact:vendorExtensions");
		foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
		{
			extension->write(writer);
		}
		writer.writeEndElement(); // ipxact:vendorExtensions
	}

    writer.writeEndElement(); // kactus2:comDefinition
    writer.writeEndDocument();
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::findErrors()
//-----------------------------------------------------------------------------
void ComDefinition::findErrors(QVector<QString>& errorList) const
{
    QString thisIdentifier(QObject::tr("the containing COM definition"));

    if (getVlnv().isValid(errorList, thisIdentifier))
    {
        thisIdentifier = QObject::tr("COM definition '%1'").arg(getVlnv().toString());
    }

    // Check for multiple definitions of same data type.
    QStringList dataTypeNames;
    foreach (QString const& dataType, transferTypes_)
    {
        if (dataTypeNames.contains(dataType))
        {
            errorList.append(QObject::tr("Data type '%1' defined multiple times in '%2'").arg(dataType, 
                thisIdentifier));
        }
        else
        {
            dataTypeNames.push_back(dataType);
        }
    }

    // Validate the properties.
    QStringList propertyNames;
    foreach (QSharedPointer<ComProperty> comProperty, properties_)
    {
        if (propertyNames.contains(comProperty->name()))
        {
            errorList.append(QObject::tr("Property '%1' defined multiple times in %2").arg(comProperty->name(),
                thisIdentifier));
        }
        else
        {
            propertyNames.append(comProperty->name());
        }

        comProperty->findErrors(errorList, thisIdentifier);
    }
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::validate()
//-----------------------------------------------------------------------------
bool ComDefinition::validate() const
{
    if (!getVlnv().isValid())
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
        if (propertyNames.contains(prop->name()))
        {
            return false;
        }
        else
        {
            propertyNames.append(prop->name());
        }

        if (!prop->validate())
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::getDependentFiles()
//-----------------------------------------------------------------------------
QStringList ComDefinition::getDependentFiles() const
{
    return QStringList();
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::getDependentVLNVs()
//-----------------------------------------------------------------------------
QList<VLNV> ComDefinition::getDependentVLNVs() const
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
        if (property->name() == name)
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
