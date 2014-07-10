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

#include "ComInterface.h"
#include "XmlUtils.h"

//-----------------------------------------------------------------------------
// Function: SWInstance::SWInstance()
//-----------------------------------------------------------------------------
SWInstance::SWInstance() : instanceName_(), displayName_(), desc_(),
                           componentRef_(), fileSetRef_(), hwRef_(), pos_(),
                           imported_(false), importRef_(),
                           comInterfaces_(), propertyValues_(),
                           apiInterfacePositions_(), comInterfacePositions_()
{
}

//-----------------------------------------------------------------------------
// Function: SWInstance::SWInstance()
//-----------------------------------------------------------------------------
SWInstance::SWInstance(SWInstance const& rhs)
    : instanceName_(rhs.instanceName_),
      displayName_(rhs.displayName_),
      desc_(rhs.desc_),
      componentRef_(rhs.componentRef_),
      fileSetRef_(rhs.fileSetRef_),
      hwRef_(rhs.hwRef_),
      pos_(rhs.pos_),
      imported_(rhs.imported_),
      importRef_(rhs.importRef_),
      comInterfaces_(),
      propertyValues_(rhs.propertyValues_),
      apiInterfacePositions_(rhs.apiInterfacePositions_),
      comInterfacePositions_(rhs.comInterfacePositions_)
{
    foreach (QSharedPointer<ComInterface> comIf, rhs.comInterfaces_)
    {
        comInterfaces_.append(QSharedPointer<ComInterface>(new ComInterface(*comIf.data())));
    }
}

//-----------------------------------------------------------------------------
// Function: SWInstance::SWInstance()
//-----------------------------------------------------------------------------
SWInstance::SWInstance(QDomNode& node)
    : instanceName_(),
      displayName_(),
      desc_(),
      componentRef_(),
      fileSetRef_(),
      hwRef_(),
      pos_(),
      imported_(false),
      importRef_(),
      comInterfaces_(),
      propertyValues_(),
      apiInterfacePositions_(),
      comInterfacePositions_()
{
    for (int i = 0; i < node.childNodes().count(); ++i)
    {
        QDomNode childNode = node.childNodes().at(i);

        if (childNode.isComment())
        {
            continue;
        }

        if (childNode.nodeName() == "spirit:instanceName")
        {
            instanceName_ = XmlUtils::removeWhiteSpace(childNode.childNodes().at(0).nodeValue());
        }
        else if (childNode.nodeName() == "spirit:displayName")
        {
            displayName_ = childNode.childNodes().at(0).nodeValue();
        }
        else if (childNode.nodeName() == "spirit:description")
        {
            desc_ = childNode.childNodes().at(0).nodeValue();
        }
        else if (childNode.nodeName() == "kactus2:componentRef")
        {
            componentRef_ = VLNV::createVLNV(childNode, VLNV::COMPONENT);
        }
        else if (childNode.nodeName() == "kactus2:fileSetRef")
        {
            fileSetRef_ = childNode.childNodes().at(0).nodeValue();
        }
        else if (childNode.nodeName() == "kactus2:mapping")
        {
            hwRef_ = childNode.attributes().namedItem("kactus2:hwRef").nodeValue();
        }
        else if (childNode.nodeName() == "kactus2:position")
        {
            pos_.setX(childNode.attributes().namedItem("x").nodeValue().toInt());
            pos_.setY(childNode.attributes().namedItem("y").nodeValue().toInt());
        }
        else if (childNode.nodeName() == "kactus2:imported")
        {
            imported_ = true;
            importRef_ = childNode.attributes().namedItem("kactus2:importRef").nodeValue();
        }
        else if (childNode.nodeName() == "kactus2:comInterfaces")
        {
            parseComInterfaces(childNode);
        }
        else if (childNode.nodeName() == "kactus2:propertyValues")
        {
            parsePropertyValues(childNode);
        }
        else if (childNode.nodeName() == "kactus2:apiInterfacePositions")
        {
            XmlUtils::parsePositionsMap(childNode, "kactus2:apiInterfacePosition",
                                        "kactus2:apiRef", apiInterfacePositions_);
        }
        else if (childNode.nodeName() == "kactus2:comInterfacePositions")
        {
            XmlUtils::parsePositionsMap(childNode, "kactus2:comInterfacePosition",
                                        "kactus2:comRef", comInterfacePositions_);
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
// Function: SWInstance::write()
//-----------------------------------------------------------------------------
void SWInstance::write(QXmlStreamWriter& writer) const
{
    writer.writeStartElement("kactus2:swInstance");

    // Write general data.
    writer.writeTextElement("spirit:instanceName", instanceName_);
    writer.writeTextElement("spirit:displayName", displayName_);
    writer.writeTextElement("spirit:description", desc_);

    if (!componentRef_.isEmpty())
    {
        writer.writeEmptyElement("kactus2:componentRef");
        componentRef_.writeAsAttributes(writer);
    }
    
    if (!fileSetRef_.isEmpty())
    {
        writer.writeTextElement("kactus2:fileSetRef", fileSetRef_);
    }
    
    writer.writeEmptyElement("kactus2:mapping");
    writer.writeAttribute("kactus2:hwRef", hwRef_);

    writer.writeEmptyElement("kactus2:position");
    writer.writeAttribute("x", QString::number(int(pos_.x())));
    writer.writeAttribute("y", QString::number(int(pos_.y())));

    if (imported_)
    {
        writer.writeEmptyElement("kactus2:imported");
        writer.writeAttribute("kactus2:importRef", importRef_);
    }

    // Write communication interfaces.
    if (!comInterfaces_.empty())
    {
        writer.writeStartElement("kactus2:comInterfaces");

        foreach (QSharedPointer<ComInterface> comIf, comInterfaces_)
        {
            comIf->write(writer);
        }

        writer.writeEndElement(); // kactus2:comInterfaces
    }

    // Write property values.
    if (!propertyValues_.empty())
    {
        writer.writeStartElement("kactus2:propertyValues");

        QMapIterator<QString, QString> iter(propertyValues_);

        while (iter.hasNext())
        {
            iter.next();

            writer.writeEmptyElement("kactus2:propertyValue");
            writer.writeAttribute("kactus2:name", iter.key());
            writer.writeAttribute("kactus2:value", iter.value());
        }

        writer.writeEndElement(); // kactus2:propertyValues
    }

    // Write API and COM interface positions.
    XmlUtils::writePositionsMap(writer, apiInterfacePositions_, "kactus2:apiInterfacePosition",
                                "kactus2:apiRef");

    XmlUtils::writePositionsMap(writer, comInterfacePositions_, "kactus2:comInterfacePosition",
                                "kactus2:comRef");

    writer.writeEndElement(); // kactus2:swInstance
}

//-----------------------------------------------------------------------------
// Function: SWInstance::isValid()
//-----------------------------------------------------------------------------
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

    if (!componentRef_.isValid() && fileSetRef_.isEmpty())
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
        if (interfaceNames.contains(comIf->getName()))
        {
            errorList.append(QObject::tr("%1 contains several COM interfaces with"
                                         " name '%2'").arg(thisId, comIf->getName()));
            valid = false;
        }
        else
        {
            interfaceNames.append(comIf->getName());
        }

        if (!comIf->isValid(errorList, thisId))
        {
            valid = false;
        }
    }

    return valid;
}

bool SWInstance::isValid( const QStringList& hwUUIDs ) const {

	if (instanceName_.isEmpty()) {
		return false;
	}

	if (!componentRef_.isValid() && fileSetRef_.isEmpty()) {
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
		if (interfaceNames.contains(comIf->getName())) {
			return false;
		}
		else {
			interfaceNames.append(comIf->getName());
		}

		if (!comIf->isValid()) {
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::setInstanceName()
//-----------------------------------------------------------------------------
void SWInstance::setInstanceName(QString const& name)
{
    instanceName_ = name;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::setDisplayName()
//-----------------------------------------------------------------------------
void SWInstance::setDisplayName(QString const& displayName)
{
    displayName_ = displayName;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::setDescription()
//-----------------------------------------------------------------------------
void SWInstance::setDescription(QString const& description)
{
    desc_ = description;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::setComponentRef()
//-----------------------------------------------------------------------------
void SWInstance::setComponentRef(VLNV const& vlnv)
{
    componentRef_ = vlnv;
}

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
// Function: SWInstance::setPosition()
//-----------------------------------------------------------------------------
void SWInstance::setPosition(QPointF const& pos)
{
    pos_ = pos;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::setImported()
//-----------------------------------------------------------------------------
void SWInstance::setImported(bool imported)
{
    imported_ = imported;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::setImportRef()
//-----------------------------------------------------------------------------
void SWInstance::setImportRef(QString const& nameRef)
{
    importRef_ = nameRef;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::updateApiInterfacePosition()
//-----------------------------------------------------------------------------
void SWInstance::updateApiInterfacePosition(QString const& name, QPointF const& pos)
{
    apiInterfacePositions_[name] = pos;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::updateComInterfacePosition()
//-----------------------------------------------------------------------------
void SWInstance::updateComInterfacePosition(QString const& name, QPointF const& pos)
{
    comInterfacePositions_[name] = pos;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::getInstanceName()
//-----------------------------------------------------------------------------
QString const& SWInstance::getInstanceName() const
{
    return instanceName_;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::getDisplayName()
//-----------------------------------------------------------------------------
QString const& SWInstance::getDisplayName() const
{
    return displayName_;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::getDescription()
//-----------------------------------------------------------------------------
QString const& SWInstance::getDescription() const
{
    return desc_;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::getComponentRef()
//-----------------------------------------------------------------------------
VLNV const& SWInstance::getComponentRef() const
{
    return componentRef_;
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
// Function: SWInstance::getPosition()
//-----------------------------------------------------------------------------
QPointF const& SWInstance::getPosition() const
{
    return pos_;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::isImported()
//-----------------------------------------------------------------------------
bool SWInstance::isImported() const
{
    return imported_;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::getImportRef()
//-----------------------------------------------------------------------------
QString const& SWInstance::getImportRef() const
{
    return importRef_;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::operator=()
//-----------------------------------------------------------------------------
SWInstance& SWInstance::operator=(SWInstance const& rhs)
{
    if (&rhs != this)
    {
        instanceName_ = rhs.instanceName_;
        displayName_ = rhs.displayName_;
        desc_ = rhs.desc_;
        componentRef_ = rhs.componentRef_;
        fileSetRef_ = rhs.fileSetRef_;
        hwRef_ = rhs.hwRef_;
        pos_ = rhs.pos_;
        imported_ = rhs.imported_;
        importRef_ = rhs.importRef_;

        comInterfaces_.clear();

        foreach (QSharedPointer<ComInterface> comIf, rhs.comInterfaces_)
        {
            comInterfaces_.append(QSharedPointer<ComInterface>(new ComInterface(*comIf.data())));
        }

        propertyValues_ = rhs.propertyValues_;
        apiInterfacePositions_ = rhs.apiInterfacePositions_;
        comInterfacePositions_ = rhs.comInterfacePositions_;
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

        if (comIfNode.nodeName() == "kactus2:comInterface")
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
    for (int i = 0; i < node.childNodes().count(); ++i)
    {
        QDomNode propNode = node.childNodes().at(i);

        if (propNode.nodeName() == "kactus2:propertyValue")
        {
            QString name = propNode.attributes().namedItem("kactus2:name").nodeValue();
            QString value = propNode.attributes().namedItem("kactus2:value").nodeValue();

            propertyValues_.insert(name, value);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SWInstance::getApiInterfacePositions()
//-----------------------------------------------------------------------------
QMap<QString, QPointF> const& SWInstance::getApiInterfacePositions() const
{
    return apiInterfacePositions_;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::getComInterfacePositions()
//-----------------------------------------------------------------------------
QMap<QString, QPointF> const& SWInstance::getComInterfacePositions() const
{
    return comInterfacePositions_;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::setPropertyValues()
//-----------------------------------------------------------------------------
void SWInstance::setPropertyValues(QMap<QString, QString> const& values)
{
    propertyValues_ = values;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::getPropertyValues()
//-----------------------------------------------------------------------------
QMap<QString, QString> const& SWInstance::getPropertyValues() const
{
    return propertyValues_;
}

