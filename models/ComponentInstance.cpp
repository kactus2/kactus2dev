
// File: ComponentInstance.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 12.6.2012
//
// Description:
// IP-XACT component instance class.
//-----------------------------------------------------------------------------

#include "ComponentInstance.h"

#include "XmlUtils.h"
#include "generaldeclarations.h"

#include <common/validators/vhdlNameValidator/vhdlnamevalidator.h>

//-----------------------------------------------------------------------------
// Function: ComponentInstance::ComponentInstance()
//-----------------------------------------------------------------------------
ComponentInstance::ComponentInstance(QString instanceName, QString displayName,
                                     QString description, VLNV const& componentRef,
                                     QPointF const& position)
    : instanceName_(instanceName),
      displayName_(displayName),
      desc_(description),
      componentRef_(componentRef),
      configurableElementValues_(),
      pos_(position),
      imported_(false),
      importRef_(),
      portPositions_(),
      adHocPortPositions_(),
      apiInterfacePositions_(),
      comInterfacePositions_(),
      portAdHocVisibilities_(),
      swPropertyValues_()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::ComponentInstance()
//-----------------------------------------------------------------------------
ComponentInstance::ComponentInstance(ComponentInstance const& other)
    : instanceName_(other.instanceName_),
      displayName_(other.displayName_),
      desc_(other.desc_),
      componentRef_(other.componentRef_),
      configurableElementValues_(other.configurableElementValues_),
      pos_(other.pos_),
      imported_(other.imported_),
      importRef_(other.importRef_),
      portPositions_(other.portPositions_),
      adHocPortPositions_(other.adHocPortPositions_),
      apiInterfacePositions_(other.apiInterfacePositions_),
      comInterfacePositions_(other.comInterfacePositions_),
      portAdHocVisibilities_(other.portAdHocVisibilities_),
      swPropertyValues_(other.swPropertyValues_)
{
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::ComponentInstance()
//-----------------------------------------------------------------------------
ComponentInstance::ComponentInstance(QDomNode& node)
    : instanceName_(),
      displayName_(),
      desc_(),
      componentRef_(),
      configurableElementValues_(),
      pos_(),
      imported_(false),
      importRef_(),
      portPositions_(),
      adHocPortPositions_(),
      apiInterfacePositions_(),
      comInterfacePositions_(),
      portAdHocVisibilities_(),
      swPropertyValues_()
{
    QDomNodeList nodes = node.childNodes();

    for (int i = 0; i < nodes.size(); i++)
    {
        QDomNode node = nodes.at(i);

        if (node.nodeName() == "spirit:instanceName")
        {
            instanceName_ = node.firstChild().nodeValue();
        }
        else if (node.nodeName() == "spirit:displayName")
        {
            displayName_ = node.firstChild().nodeValue();
        }
        else if (node.nodeName() == "spirit:description")
        {
            desc_ = node.firstChild().nodeValue();
        }
        else if (node.nodeName() == "spirit:componentRef")
        {
            componentRef_ = General::createVLNV(node, VLNV::COMPONENT);
        }
        else if (node.nodeName() == "spirit:configurableElementValues")
        {
            for (int i = 0; i < node.childNodes().size(); i++)
            {
                QDomNode confNode = node.childNodes().at(i);
                QString value = confNode.firstChild().nodeValue();

                QDomNamedNodeMap attributes = confNode.attributes();
                QString reference = attributes.namedItem("spirit:referenceId").nodeValue();
                configurableElementValues_.insert(reference, value);
            }
        }
        else if (node.nodeName() == "spirit:vendorExtensions")
        {
            for (int i = 0; i < node.childNodes().size(); ++i)
            {
                QDomNode childNode = node.childNodes().at(i);

                if (childNode.nodeName() == "kactus2:position")
                {
                    QDomNamedNodeMap attributeMap = childNode.attributes();
                    pos_.setX(attributeMap.namedItem("x").nodeValue().toInt());
                    pos_.setY(attributeMap.namedItem("y").nodeValue().toInt());
                }
                else if (childNode.nodeName() == "kactus2:portPositions")
                {
                    XmlUtils::parsePositionsMap(childNode, "kactus2:portPosition",
                                                "kactus2:busRef", portPositions_);
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
                else if (childNode.nodeName() == "kactus2:adHocVisibilities")
                {
                    XmlUtils::parseAdHocVisibilities(childNode, portAdHocVisibilities_, adHocPortPositions_);
                }
                else if (childNode.nodeName() == "kactus2:imported")
                {
                    imported_ = true;
                    importRef_ = childNode.attributes().namedItem("kactus2:importRef").nodeValue();
                }
                else if (childNode.nodeName() == "kactus2:propertyValues")
                {
                    parsePropertyValues(childNode);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::write()
//-----------------------------------------------------------------------------
void ComponentInstance::write(QXmlStreamWriter& writer) const
{
    writer.writeStartElement("spirit:componentInstance");

    writer.writeTextElement("spirit:instanceName", instanceName_);
    writer.writeTextElement("spirit:displayName", displayName_);
    writer.writeTextElement("spirit:description", desc_);

    writer.writeEmptyElement("spirit:componentRef");
    General::writeVLNVAttributes(writer, &componentRef_);

    writer.writeStartElement("spirit:configurableElementValues");

    QMapIterator<QString, QString> i(configurableElementValues_);
    
    while (i.hasNext())
    {
        i.next();

        writer.writeStartElement("spirit:configurableElementValue");
        writer.writeAttribute("spirit:referenceId", i.key());
        writer.writeCharacters(i.value());
        writer.writeEndElement();
    }

    writer.writeEndElement(); // spirit:configurableElementValues

    // Write custom data to vendor extensions.
    writer.writeStartElement("spirit:vendorExtensions");

    // Write the component position.
    XmlUtils::writePosition(writer, pos_);

    // Write the bus, API and COM interface positions.
    XmlUtils::writePositionsMap(writer, portPositions_, "kactus2:portPosition", "kactus2:busRef");
    XmlUtils::writePositionsMap(writer, apiInterfacePositions_, "kactus2:apiInterfacePosition", "kactus2:apiRef");
    XmlUtils::writePositionsMap(writer, apiInterfacePositions_, "kactus2:comInterfacePosition", "kactus2:comRef");            

    // Write the port ad-hoc visibilities.
    XmlUtils::writeAdHocVisibilities(writer, portAdHocVisibilities_, adHocPortPositions_);

    if (imported_)
    {
        writer.writeEmptyElement("kactus2:imported");
        writer.writeAttribute("kactus2:importRef", importRef_);
    }

    writer.writeStartElement("kactus2:propertyValues");

    // Write property values.
    QMapIterator<QString, QString> iter(swPropertyValues_);

    while (iter.hasNext())
    {
        iter.next();

        writer.writeEmptyElement("kactus2:propertyValue");
        writer.writeAttribute("kactus2:name", iter.key());
        writer.writeAttribute("kactus2:value", iter.value());
    }

    writer.writeEndElement(); // kactus2:propertyValues

    writer.writeEndElement(); // spirit:vendorExtensions
    writer.writeEndElement(); // spirit:componentInstance
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::isValid()
//-----------------------------------------------------------------------------
bool ComponentInstance::isValid(QStringList& errorList, QString const& parentIdentifier) const
{
    bool valid = true;
    const QString thisIdentifier(QObject::tr("component instance %1").arg(instanceName_));

    if (instanceName_.isEmpty())
    {
        errorList.append(QObject::tr("No name specified for component instance "
                                     "within %1").arg(parentIdentifier));
        valid = false;
    }

    // if the instance name contains characters that are not allowed in vhdl
    VhdlNameValidator nameValidator;
    int pos = 0;
    QString instName(instanceName_);

    if (nameValidator.validate(instName, pos) != QValidator::Acceptable)
    {
        errorList.append(QObject::tr("The instance name %1 contains illegal characters.").arg(instName));
        valid = false;
    }

    if (!componentRef_.isValid(errorList, thisIdentifier))
    {
        valid = false;
    }

    for (QMap<QString, QString>::const_iterator i = configurableElementValues_.begin();
        i != configurableElementValues_.end(); ++i)
    {
            if (i.key().isEmpty())
            {
                errorList.append(QObject::tr("No reference id set for configurable"
                                             " element value in %1 within %2").arg(thisIdentifier).arg(parentIdentifier));
                valid = false;
            }

            if (i.value().isEmpty())
            {
                errorList.append(QObject::tr("No configurable element value set"
                                             " for %1 within %2").arg(thisIdentifier).arg(parentIdentifier));
                valid = false;
            }
    }

    return valid;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::isValid()
//-----------------------------------------------------------------------------
bool ComponentInstance::isValid() const
{
    if (instanceName_.isEmpty()) {
        return false;
    }

    // if the instance name contains characters that are not allowed in vhdl
    VhdlNameValidator nameValidator;
    int pos = 0;
    QString instName(instanceName_);

    if (nameValidator.validate(instName, pos) != QValidator::Acceptable)
    {
        return false;
    }

    if (!componentRef_.isValid())
    {
        return false;
    }

    for (QMap<QString, QString>::const_iterator i = configurableElementValues_.begin();
        i != configurableElementValues_.end(); ++i)
    {
            if (i.key().isEmpty())
            {
                return false;
            }

            if (i.value().isEmpty())
            {
                return false;
            }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setInstanceName()
//-----------------------------------------------------------------------------
void ComponentInstance::setInstanceName(QString const& name)
{
    instanceName_ = name;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setDisplayName()
//-----------------------------------------------------------------------------
void ComponentInstance::setDisplayName(QString const& displayName)
{
    displayName_ = displayName;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setDescription()
//-----------------------------------------------------------------------------
void ComponentInstance::setDescription(QString const& description)
{
    desc_ = description;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setComponentRef()
//-----------------------------------------------------------------------------
void ComponentInstance::setComponentRef(VLNV const& vlnv)
{
    componentRef_ = vlnv;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setConfigurableElementValues()
//-----------------------------------------------------------------------------
void ComponentInstance::setConfigurableElementValues(QMap<QString, QString> const& values)
{
    configurableElementValues_ = values;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setPosition()
//-----------------------------------------------------------------------------
void ComponentInstance::setPosition(QPointF const& pos)
{
    pos_ = pos;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setImported()
//-----------------------------------------------------------------------------
void ComponentInstance::setImported(bool imported)
{
    imported_ = imported;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setImportRef()
//-----------------------------------------------------------------------------
void ComponentInstance::setImportRef(QString const& nameRef)
{
    importRef_ = nameRef;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setPropertyValues()
//-----------------------------------------------------------------------------
void ComponentInstance::setPropertyValues(QMap<QString, QString> const& values)
{
    swPropertyValues_ = values;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::updateBusInterfacePosition()
//-----------------------------------------------------------------------------
void ComponentInstance::updateBusInterfacePosition(QString const& name, QPointF const& pos)
{
    portPositions_[name] = pos;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::updateAdHocPortPosition()
//-----------------------------------------------------------------------------
void ComponentInstance::updateAdHocPortPosition(QString const& name, QPointF const& pos)
{
    adHocPortPositions_[name] = pos;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::updateApiInterfacePosition()
//-----------------------------------------------------------------------------
void ComponentInstance::updateApiInterfacePosition(QString const& name, QPointF const& pos)
{
    apiInterfacePositions_[name] = pos;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::updateComInterfacePosition()
//-----------------------------------------------------------------------------
void ComponentInstance::updateComInterfacePosition(QString const& name, QPointF const& pos)
{
    comInterfacePositions_[name] = pos;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setPortAdHocVisibilities()
//-----------------------------------------------------------------------------
void ComponentInstance::setPortAdHocVisibilities(QMap<QString, bool> const& visibilities)
{
    portAdHocVisibilities_ = visibilities;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getInstanceName()
//-----------------------------------------------------------------------------
QString const& ComponentInstance::getInstanceName() const
{
    return instanceName_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getDisplayName()
//-----------------------------------------------------------------------------
QString const& ComponentInstance::getDisplayName() const
{
    return displayName_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getDescription()
//-----------------------------------------------------------------------------
QString const& ComponentInstance::getDescription() const
{
    return desc_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getComponentRef()
//-----------------------------------------------------------------------------
VLNV const& ComponentInstance::getComponentRef() const
{
    return componentRef_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getConfigurableElementValues()
//-----------------------------------------------------------------------------
QMap<QString, QString> const& ComponentInstance::getConfigurableElementValues() const
{
    return configurableElementValues_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getPosition()
//-----------------------------------------------------------------------------
QPointF const& ComponentInstance::getPosition() const
{
    return pos_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::isImported()
//-----------------------------------------------------------------------------
bool ComponentInstance::isImported() const
{
    return imported_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getImportRef()
//-----------------------------------------------------------------------------
QString const& ComponentInstance::getImportRef() const
{
    return importRef_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getPropertyValues()
//-----------------------------------------------------------------------------
QMap<QString, QString> const& ComponentInstance::getPropertyValues() const
{
    return swPropertyValues_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getBusInterfacePositions()
//-----------------------------------------------------------------------------
QMap<QString, QPointF> const& ComponentInstance::getBusInterfacePositions() const
{
    return portPositions_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getAdHocPortPositions()
//-----------------------------------------------------------------------------
QMap<QString, QPointF> const& ComponentInstance::getAdHocPortPositions() const
{
    return adHocPortPositions_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getApiInterfacePositions()
//-----------------------------------------------------------------------------
QMap<QString, QPointF> const& ComponentInstance::getApiInterfacePositions() const
{
    return apiInterfacePositions_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getComInterfacePositions()
//-----------------------------------------------------------------------------
QMap<QString, QPointF> const& ComponentInstance::getComInterfacePositions() const
{
    return comInterfacePositions_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getPortAdHocVisibilities()
//-----------------------------------------------------------------------------
QMap<QString, bool> const& ComponentInstance::getPortAdHocVisibilities() const
{
    return portAdHocVisibilities_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::operator=()
//-----------------------------------------------------------------------------
ComponentInstance& ComponentInstance::operator=(ComponentInstance const& other)
{
    if (this != &other)
    {
        instanceName_ = other.instanceName_;
        displayName_ = other.displayName_;
        desc_ = other.desc_;
        componentRef_ = other.componentRef_;
        configurableElementValues_ = other.configurableElementValues_;
        pos_ = other.pos_;
        imported_ = other.imported_;
        importRef_ = other.importRef_;
        portPositions_ = other.portPositions_;
        adHocPortPositions_ = other.adHocPortPositions_;
        apiInterfacePositions_ = other.apiInterfacePositions_;
        comInterfacePositions_ = other.comInterfacePositions_;
        portAdHocVisibilities_ = other.portAdHocVisibilities_;
        swPropertyValues_ = other.swPropertyValues_;
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::parsePropertyValues()
//-----------------------------------------------------------------------------
void ComponentInstance::parsePropertyValues(QDomNode& node)
{
    for (int i = 0; i < node.childNodes().count(); ++i)
    {
        QDomNode propNode = node.childNodes().at(i);

        if (propNode.nodeName() == "kactus2:propertyValue")
        {
            QString name = propNode.attributes().namedItem("kactus2:name").nodeValue();
            QString value = propNode.attributes().namedItem("kactus2:value").nodeValue();

            swPropertyValues_.insert(name, value);
        }
    }
}

bool ComponentInstance::hasConfElementValue( const QString& confElementName ) const {
	return configurableElementValues_.contains(confElementName);
}

QString ComponentInstance::getConfElementValue( const QString& confElementName ) const {
	return configurableElementValues_.value(confElementName, QString());
}
