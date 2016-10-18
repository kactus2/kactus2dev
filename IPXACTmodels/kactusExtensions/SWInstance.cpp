//-----------------------------------------------------------------------------
// File: SWInstance.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 10.4.2012
//
// Description:
// SW instance class for extending IP-XACT designs.
//-----------------------------------------------------------------------------

#include "SWInstance.h"

//-----------------------------------------------------------------------------
// Function: SWInstance::SWInstance()
//-----------------------------------------------------------------------------
SWInstance::SWInstance() :
ComponentInstance(),
fileSetRef_(),
hwRef_(),
comInterfaces_()
{
}

//-----------------------------------------------------------------------------
// Function: SWInstance::SWInstance()
//-----------------------------------------------------------------------------
SWInstance::SWInstance(SWInstance const& rhs) :
ComponentInstance(rhs),
fileSetRef_(rhs.fileSetRef_),
hwRef_(rhs.hwRef_),
comInterfaces_()
{
    foreach (QSharedPointer<ComInterface> comIf, rhs.comInterfaces_)
    {
        comInterfaces_.append(QSharedPointer<ComInterface>(new ComInterface(*comIf.data())));
    }
}

//-----------------------------------------------------------------------------
// Function: SWInstance::SWInstance()
//-----------------------------------------------------------------------------
SWInstance::SWInstance(QDomNode& node) :
ComponentInstance(),
fileSetRef_(),
hwRef_(),
comInterfaces_()
{
    for (int i = 0; i < node.childNodes().count(); ++i)
    {
        QDomNode childNode = node.childNodes().at(i);

        if (childNode.isComment())
        {
            continue;
        }

        if (childNode.nodeName() == QLatin1String("kactus2:instanceName"))
        {
            setInstanceName(childNode.firstChild().nodeValue());
        }
        else if (childNode.nodeName() == QLatin1String("kactus2:displayName"))
        {
            setDisplayName(childNode.firstChild().nodeValue());
        }
        else if (childNode.nodeName() == QLatin1String("kactus2:description"))
        {
            setDescription(childNode.firstChild().nodeValue());
        }
        else if (childNode.nodeName() == QLatin1String("kactus2:componentRef"))
        {
            setComponentRef(createComponentReference(childNode));
        }
        else if (childNode.nodeName() == QLatin1String("kactus2:fileSetRef"))
        {
            fileSetRef_ = childNode.firstChild().nodeValue();
        }
        else if (childNode.nodeName() == QLatin1String("kactus2:mapping"))
        {
            hwRef_ = childNode.attributes().namedItem(QStringLiteral("hwRef")).nodeValue();
        }
        else if (childNode.nodeName() == QLatin1String("kactus2:position"))
        {
            int positionX = childNode.attributes().namedItem(QStringLiteral("x")).nodeValue().toInt();
            int positionY = childNode.attributes().namedItem(QStringLiteral("y")).nodeValue().toInt();
            setPosition(QPointF(positionX, positionY));
        }
        else if (childNode.nodeName() == QLatin1String("kactus2:imported"))
        {
            setImportRef(childNode.attributes().namedItem(QStringLiteral("importRef")).nodeValue());
        }
        else if (childNode.nodeName() == QLatin1String("kactus2:comInterfaces"))
        {
            parseComInterfaces(childNode);
        }
        else if (childNode.nodeName() == QLatin1String("kactus2:propertyValues"))
        {
            parsePropertyValues(childNode);
        }
        else if (childNode.nodeName() == QLatin1String("kactus2:apiInterfacePositions"))
        {
            QMap<QString, QPointF> apiInterfacePositions = createMappedPositions(childNode, 
                QStringLiteral("apiRef"));
            QMapIterator<QString, QPointF> apiInterfaceIterator(apiInterfacePositions);

            while (apiInterfaceIterator.hasNext())
            {
                apiInterfaceIterator.next();

                updateApiInterfacePosition(apiInterfaceIterator.key(), apiInterfaceIterator.value());
            }

        }
        else if (childNode.nodeName() == QLatin1String("kactus2:comInterfacePositions"))
        {
            QMap<QString, QPointF> comInterfacePositions = createMappedPositions(childNode, 
                QStringLiteral("comRef"));
            QMapIterator<QString, QPointF> apiInterfaceIterator(comInterfacePositions);

            while (apiInterfaceIterator.hasNext())
            {
                apiInterfaceIterator.next();

                updateComInterfacePosition(apiInterfaceIterator.key(), apiInterfaceIterator.value());
            }

        }
        else if (childNode.nodeName() == QLatin1String("kactus2:draft"))
        {
            setDraft(true);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SWInstance::~SWInstance()
//-----------------------------------------------------------------------------
SWInstance::~SWInstance()
{
}

//-----------------------------------------------------------------------------
// Function: SWInstance::clone()
//-----------------------------------------------------------------------------
SWInstance* SWInstance::clone() const
{
    return new SWInstance(*this);
}

//-----------------------------------------------------------------------------
// Function: SWInstance::type()
//-----------------------------------------------------------------------------
QString SWInstance::type() const
{
    return QStringLiteral("kactus2:swInstance");
}

//-----------------------------------------------------------------------------
// Function: SWInstance::write()
//-----------------------------------------------------------------------------
void SWInstance::write(QXmlStreamWriter& writer) const
{
    writer.writeStartElement(QStringLiteral("kactus2:swInstance"));

    // Write general data.
    writer.writeTextElement(QStringLiteral("kactus2:instanceName"), getInstanceName());
    if (!getDisplayName().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("kactus2:displayName"), getDisplayName());
    }
    if (!getDescription().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("kactus2:description"), getDescription());
    }

    if (!getComponentRef()->isEmpty())
    {
        writer.writeEmptyElement(QStringLiteral("kactus2:componentRef"));
        writeVLNVasAttributes(writer, *getComponentRef());
    }
    
    if (!fileSetRef_.isEmpty())
    {
        writer.writeTextElement(QStringLiteral("kactus2:fileSetRef"), fileSetRef_);
    }
    
    writer.writeEmptyElement(QStringLiteral("kactus2:mapping"));
    writer.writeAttribute(QStringLiteral("hwRef"), hwRef_);

    writer.writeEmptyElement(QStringLiteral("kactus2:position"));
    writer.writeAttribute(QStringLiteral("x"), QString::number(int(getPosition().x())));
    writer.writeAttribute(QStringLiteral("y"), QString::number(int(getPosition().y())));

    if (isImported())
    {
        writer.writeEmptyElement(QStringLiteral("kactus2:imported"));
        writer.writeAttribute(QStringLiteral("importRef"), getImportRef());
    }

    if (isDraft())
    {
        writer.writeEmptyElement(QStringLiteral("kactus2:draft"));
    }

    // Write communication interfaces.
    if (!comInterfaces_.empty())
    {
        writer.writeStartElement(QStringLiteral("kactus2:comInterfaces"));

        foreach (QSharedPointer<ComInterface> comIf, comInterfaces_)
        {
            comIf->write(writer);
        }

        writer.writeEndElement(); // kactus2:comInterfaces
    }

    // Write property values.
    if (!getPropertyValues().isEmpty())
    {
        writer.writeStartElement(QStringLiteral("kactus2:propertyValues"));

        QMapIterator<QString, QString> iter(getPropertyValues());

        while (iter.hasNext())
        {
            iter.next();

            writer.writeEmptyElement(QStringLiteral("kactus2:propertyValue"));
            writer.writeAttribute(QStringLiteral("name"), iter.key());
            writer.writeAttribute(QStringLiteral("value"), iter.value());
        }

        writer.writeEndElement(); // kactus2:propertyValues
    }

    // Write API and COM interface positions.
    writeMappedPosition(writer, getApiInterfacePositions(), QStringLiteral("kactus2:apiInterfacePosition"), 
        QStringLiteral("apiRef"));
    writeMappedPosition(writer, getComInterfacePositions(), QStringLiteral("kactus2:comInterfacePosition"),
        QStringLiteral("comRef"));

    writer.writeEndElement(); // kactus2:swInstance
}

//-----------------------------------------------------------------------------
// Function: SWInstance::isValid()
//-----------------------------------------------------------------------------
/*
bool SWInstance::isValid(QStringList& errorList, QStringList const& hwUUIDs,
                         QString const& parentId) const
{
    bool valid = true;
    const QString thisId(QObject::tr("SW instance '%1'").arg(instanceName_));

    if (instanceName_.isEmpty())
    {
        errorList.append(QObject::tr("No name specified for SW instance in %1").arg(parentId));
        valid = false;
    }

    if (!draft_ && !componentRef_.isValid() && fileSetRef_.isEmpty())
    {
        errorList.append(QObject::tr("No valid VLNV reference or a file set reference "
                                     "specified for %1").arg(thisId));
        valid = false;
    }

    // Check whether the mapping is valid or not.
    if (!hwRef_.isEmpty() && !hwUUIDs.contains(hwRef_))
    {
        errorList.append(QObject::tr("Component instance '%1' referenced "
                                     "by %2 not found in %3").arg(hwRef_, thisId, parentId));
        valid = false;
    }

    // Validate COM interfaces.
    QStringList interfaceNames;

    foreach (QSharedPointer<ComInterface> comIf, comInterfaces_)
    {
        if (interfaceNames.contains(comIf->name()))
        {
            errorList.append(QObject::tr("%1 contains several COM interfaces with"
                                         " name '%2'").arg(thisId, comIf->name()));
            valid = false;
        }
        else
        {
            interfaceNames.append(comIf->name());
        }

        if (!comIf->isValid(errorList, thisId))
        {
            valid = false;
        }
    }

    return valid;
}
*/
/*
bool SWInstance::isValid( const QStringList& hwUUIDs ) const {

	if (instanceName_.isEmpty()) {
		return false;
	}

	if (!draft_ && !componentRef_.isValid() && fileSetRef_.isEmpty()) {
		return false;
	}

	// Check whether the mapping is valid or not.
	if (!hwRef_.isEmpty() && !hwUUIDs.contains(hwRef_)) {
		return false;
	}

	// Validate COM interfaces.
	QStringList interfaceNames;

	foreach (QSharedPointer<ComInterface> comIf, comInterfaces_)
	{
		if (interfaceNames.contains(comIf->name())) {
			return false;
		}
		else {
			interfaceNames.append(comIf->name());
		}

		if (!comIf->isValid()) {
			return false;
		}
	}

	return true;
}
*/
//-----------------------------------------------------------------------------
// Function: SWInstance::setFileSetRef()
//-----------------------------------------------------------------------------
void SWInstance::setFileSetRef(QString const& fileSetName)
{
    fileSetRef_ = fileSetName;
}


//-----------------------------------------------------------------------------
// Function: SWInstance::setMapping()
//-----------------------------------------------------------------------------
void SWInstance::setMapping(QString const& hwRef)
{
    hwRef_ = hwRef;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::getFileSetRef()
//-----------------------------------------------------------------------------
QString const& SWInstance::getFileSetRef() const
{
    return fileSetRef_;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::getMapping()
//-----------------------------------------------------------------------------
QString const& SWInstance::getMapping() const
{
    return hwRef_;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::operator=()
//-----------------------------------------------------------------------------
SWInstance& SWInstance::operator=(SWInstance const& rhs)
{
    if (&rhs != this)
    {
        ComponentInstance::operator=(rhs);
        fileSetRef_ = rhs.fileSetRef_;
        hwRef_ = rhs.hwRef_;

        comInterfaces_.clear();

        foreach (QSharedPointer<ComInterface> comIf, rhs.comInterfaces_)
        {
            comInterfaces_.append(QSharedPointer<ComInterface>(new ComInterface(*comIf.data())));
        }
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::parseComInterfaces()
//-----------------------------------------------------------------------------
void SWInstance::parseComInterfaces(QDomNode& node)
{
    for (int i = 0; i < node.childNodes().count(); ++i)
    {
        QDomNode comIfNode = node.childNodes().at(i);

        if (comIfNode.nodeName() == QLatin1String("kactus2:comInterface"))
        {
            QSharedPointer<ComInterface> comIf(new ComInterface(comIfNode));
            comInterfaces_.append(comIf);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SWInstance::parsePropertyValues()
//-----------------------------------------------------------------------------
void SWInstance::parsePropertyValues(QDomNode& node)
{
    QMap<QString, QString> newPropertyValues;

    for (int i = 0; i < node.childNodes().count(); ++i)
    {
        QDomNode propNode = node.childNodes().at(i);

        if (propNode.nodeName() == QLatin1String("kactus2:propertyValue"))
        {
            QString name = propNode.attributes().namedItem(QStringLiteral("name")).nodeValue();
            QString value = propNode.attributes().namedItem(QStringLiteral("value")).nodeValue();

            newPropertyValues.insert(name, value);
        }
    }

    setPropertyValues(newPropertyValues);
}

//-----------------------------------------------------------------------------
// Function: SWInstance::writeVLNVasAttributes()
//-----------------------------------------------------------------------------
void SWInstance::writeVLNVasAttributes(QXmlStreamWriter& writer, VLNV const& targetVLNV) const
{
    writer.writeAttribute(QStringLiteral("vendor"), targetVLNV.getVendor());
    writer.writeAttribute(QStringLiteral("library"), targetVLNV.getLibrary());
    writer.writeAttribute(QStringLiteral("name"), targetVLNV.getName());
    writer.writeAttribute(QStringLiteral("version"), targetVLNV.getVersion());
}

//-----------------------------------------------------------------------------
// Function: SWInstance::writeMappedPosition()
//-----------------------------------------------------------------------------
void SWInstance::writeMappedPosition(QXmlStreamWriter& writer, QMap<QString, QPointF> const& positions,
    QString const& identifier, QString const& refIdentifier) const
{
    if (!positions.isEmpty())
    {
        QMapIterator<QString, QPointF> itrPortPos(positions);
        writer.writeStartElement(identifier + QStringLiteral("s"));

        while (itrPortPos.hasNext())
        {
            itrPortPos.next();

            writer.writeStartElement(identifier);
            writer.writeAttribute(refIdentifier, itrPortPos.key());
            writer.writeAttribute(QStringLiteral("x"), QString::number(int(itrPortPos.value().x())));
            writer.writeAttribute(QStringLiteral("y"), QString::number(int(itrPortPos.value().y())));


            writer.writeEndElement();
        }

        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: SWInstance::parseSinglePoint()
//-----------------------------------------------------------------------------
QPointF SWInstance::parseSinglePoint(QDomNode const& node) 
{
    int x = node.attributes().namedItem(QStringLiteral("x")).nodeValue().toInt();
    int y = node.attributes().namedItem(QStringLiteral("y")).nodeValue().toInt();

    return QPointF(x, y);
}

//-----------------------------------------------------------------------------
// Function: SWInstance::createComponentReference()
//-----------------------------------------------------------------------------
QSharedPointer<ConfigurableVLNVReference> SWInstance::createComponentReference(const QDomNode& node) const
{
    QDomNamedNodeMap attributeMap = node.attributes();

    QString vendor = attributeMap.namedItem(QStringLiteral("vendor")).nodeValue();
    QString library = attributeMap.namedItem(QStringLiteral("library")).nodeValue();
    QString name = attributeMap.namedItem(QStringLiteral("name")).nodeValue();
    QString version = attributeMap.namedItem(QStringLiteral("version")).nodeValue();

    QSharedPointer<ConfigurableVLNVReference> vlnvReference(
        new ConfigurableVLNVReference(VLNV::COMPONENT, vendor, library, name, version));

    QDomNode configurableElementsNode = node.firstChildElement(QStringLiteral("ipxact:configurableElementValues"));

    QDomNodeList configurableElementNodeList = configurableElementsNode.childNodes();
    for (int i = 0; i < configurableElementNodeList.size(); ++i)
    {
        QSharedPointer<ConfigurableElementValue> newConfigurableElementValue =
            parseConfigurableElement(configurableElementNodeList.at(i));

        vlnvReference->getConfigurableElementValues()->append(newConfigurableElementValue);
    }

    return vlnvReference;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::parseConfigurableElement()
//-----------------------------------------------------------------------------
QSharedPointer<ConfigurableElementValue> SWInstance::parseConfigurableElement
    (QDomNode const& configurableElementNode) const
{
    QSharedPointer<ConfigurableElementValue> newConfigurableElementValue (new ConfigurableElementValue());

    newConfigurableElementValue->setConfigurableValue(configurableElementNode.firstChild().nodeValue());

    QDomNamedNodeMap attributeMap = configurableElementNode.attributes();

    for (int i = 0; i < attributeMap.size(); ++i)
    {
        QDomNode attributeItem = attributeMap.item(i);
        newConfigurableElementValue->insertAttribute(
            attributeItem.nodeName(), attributeItem.firstChild().nodeValue());
    }

    return newConfigurableElementValue;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::createMappedPositions()
//-----------------------------------------------------------------------------
QMap<QString, QPointF> SWInstance::createMappedPositions(QDomNode& mapNode, QString const& referenceIdentifier)
    const
{
    QDomNodeList apiInterfaceNodes = mapNode.childNodes();

    QMap<QString, QPointF> interfacePositions;

    for (int i = 0; i < apiInterfaceNodes.count(); ++i)
    {
        QDomNode interfaceNode = apiInterfaceNodes.at(i);
        QDomNamedNodeMap interfaceAttributes = interfaceNode.attributes();

        QString interfaceName = interfaceAttributes.namedItem(referenceIdentifier).nodeValue();
        int positionX = interfaceAttributes.namedItem(QStringLiteral("x")).nodeValue().toInt();
        int positionY = interfaceAttributes.namedItem(QStringLiteral("y")).nodeValue().toInt();

        interfacePositions.insert(interfaceName, QPointF(positionX, positionY));
    }

    return interfacePositions;
}