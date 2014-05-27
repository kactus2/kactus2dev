
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

#include <QSharedPointer>
#include <QUuid>
#include "VendorExtension.h"
#include <IPXACTmodels/kactusExtensions/Kactus2Position.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Value.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Placeholder.h>

//-----------------------------------------------------------------------------
// Function: ComponentInstance::ComponentInstance()
//-----------------------------------------------------------------------------
ComponentInstance::ComponentInstance(QString instanceName, QString displayName,
                                     QString description, VLNV const& componentRef,
                                     QPointF const& position,
									 const QString& uuid)
    : instanceName_(instanceName),
      displayName_(displayName),
      desc_(description),
      componentRef_(componentRef),
      configurableElementValues_(),
      pos_(),
      imported_(),
      portPositions_(),
      adHocPortPositions_(),
      apiInterfacePositions_(),
      comInterfacePositions_(),
      portAdHocVisibilities_(),
      swPropertyValues_(),
	  uuid_(),
      vendorExtensions_()
{
    createImportVendorExtension();
    createPositionVendorExtension(position);
    createUuidVendorExtension();
    uuid_->setValue(uuid);
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
      pos_(),
      imported_(),
      portPositions_(other.portPositions_),
      adHocPortPositions_(other.adHocPortPositions_),
      apiInterfacePositions_(other.apiInterfacePositions_),
      comInterfacePositions_(other.comInterfacePositions_),
      portAdHocVisibilities_(other.portAdHocVisibilities_),
      swPropertyValues_(other.swPropertyValues_),
	  uuid_()
{
    createImportVendorExtension();
    createPositionVendorExtension(QPointF());
    createUuidVendorExtension();

    copyVendorExtensions(other.vendorExtensions_);    
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
      imported_(),
      portPositions_(),
      adHocPortPositions_(),
      apiInterfacePositions_(),
      comInterfacePositions_(),
      portAdHocVisibilities_(),
      swPropertyValues_(),
	  uuid_(),
      vendorExtensions_()
{
    createImportVendorExtension();
    createPositionVendorExtension(QPointF());
    createUuidVendorExtension();

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
            parseVendorExtensions(node);
        }
    }
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
        portPositions_ = other.portPositions_;
        adHocPortPositions_ = other.adHocPortPositions_;
        apiInterfacePositions_ = other.apiInterfacePositions_;
        comInterfacePositions_ = other.comInterfacePositions_;
        portAdHocVisibilities_ = other.portAdHocVisibilities_;
        swPropertyValues_ = other.swPropertyValues_;

        createImportVendorExtension();
        createPositionVendorExtension(QPointF());
        createUuidVendorExtension();

        copyVendorExtensions(other.vendorExtensions_);
    }

    return *this;
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

    // Write the bus, API and COM interface positions.
    XmlUtils::writePositionsMap(writer, portPositions_, "kactus2:portPosition", "kactus2:busRef");
    XmlUtils::writePositionsMap(writer, apiInterfacePositions_, "kactus2:apiInterfacePosition", "kactus2:apiRef");
    XmlUtils::writePositionsMap(writer, apiInterfacePositions_, "kactus2:comInterfacePosition", "kactus2:comRef");            

    // Write the port ad-hoc visibilities.
    XmlUtils::writeAdHocVisibilities(writer, portAdHocVisibilities_, adHocPortPositions_);

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

    XmlUtils::writeVendorExtensions(writer, vendorExtensions_);

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
    pos_->setPosition(pos);
    
    if (!vendorExtensions_.contains(pos_))
    {
        vendorExtensions_.append(pos_);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setImported()
//-----------------------------------------------------------------------------
void ComponentInstance::setImported(bool imported)
{    
    if (imported)
    {
        if (!vendorExtensions_.contains(imported_))
        {
            vendorExtensions_.append(imported_);
        }
    }
    else
    {
        vendorExtensions_.removeAll(imported_);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setImportRef()
//-----------------------------------------------------------------------------
void ComponentInstance::setImportRef(QString const& nameRef)
{
    imported_->setAttribute("kactus2:importRef", nameRef);

    if (!vendorExtensions_.contains(imported_))
    {
        vendorExtensions_.append(imported_);
    }
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
QPointF ComponentInstance::getPosition() const
{
    return pos_->position();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::isImported()
//-----------------------------------------------------------------------------
bool ComponentInstance::isImported() const
{
    return vendorExtensions_.contains(imported_);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getImportRef()
//-----------------------------------------------------------------------------
QString ComponentInstance::getImportRef() const
{
    return imported_->getAttributeValue("kactus2:importRef");
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

QString ComponentInstance::getUuid() const {
	return uuid_->value();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setVendorExtensions()
//-----------------------------------------------------------------------------
void ComponentInstance::setVendorExtensions(QList<QSharedPointer<VendorExtension> > const& vendorExtensions)
{
    vendorExtensions_.clear();
    copyVendorExtensions(vendorExtensions);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getVendorExtensions()
//-----------------------------------------------------------------------------
QList<QSharedPointer<VendorExtension> > ComponentInstance::getVendorExtensions() const
{
    return vendorExtensions_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::parseVendorExtensions()
//-----------------------------------------------------------------------------
void ComponentInstance::parseVendorExtensions(QDomNode const& extensionsNode)
{
    for (int i = 0; i < extensionsNode.childNodes().size(); ++i)
    {
        QDomNode extensionNode = extensionsNode.childNodes().at(i);

        if (extensionNode.nodeName() == "kactus2:position")
        {            
            QPointF position = XmlUtils::parsePoint(extensionNode);
            setPosition(position);
        }
        else if (extensionNode.nodeName() == "kactus2:portPositions")
        {
            XmlUtils::parsePositionsMap(extensionNode, "kactus2:portPosition",
                "kactus2:busRef", portPositions_);
        }
        else if (extensionNode.nodeName() == "kactus2:apiInterfacePositions")
        {
            XmlUtils::parsePositionsMap(extensionNode, "kactus2:apiInterfacePosition",
                "kactus2:apiRef", apiInterfacePositions_);
        }
        else if (extensionNode.nodeName() == "kactus2:comInterfacePositions")
        {
            XmlUtils::parsePositionsMap(extensionNode, "kactus2:comInterfacePosition",
                "kactus2:comRef", comInterfacePositions_);
        }
        else if (extensionNode.nodeName() == "kactus2:adHocVisibilities")
        {
            XmlUtils::parseAdHocVisibilities(extensionNode, portAdHocVisibilities_, adHocPortPositions_);
        }
        else if (extensionNode.nodeName() == "kactus2:imported")
        {
            QString importRef = extensionNode.attributes().namedItem("kactus2:importRef").nodeValue();    
            setImported(true);            
            setImportRef(importRef);
        }
        else if (extensionNode.nodeName() == "kactus2:propertyValues")
        {
            parsePropertyValues(extensionNode);
        }
        else if (extensionNode.nodeName() == "kactus2:uuid") {                                    
            QString uuid = extensionNode.childNodes().at(0).nodeValue();
            uuid_->setValue(uuid);       
        }
        else
        {
            QSharedPointer<VendorExtension> extension = XmlUtils::createVendorExtensionFromNode(extensionNode); 
            vendorExtensions_.append(extension);
        }
    }
}


//-----------------------------------------------------------------------------
// Function: ComponentInstance::copyVendorExtensions()
//-----------------------------------------------------------------------------
void ComponentInstance::copyVendorExtensions(QList<QSharedPointer<VendorExtension> > const& extensions)
{
    foreach (QSharedPointer<VendorExtension> extension, extensions) 
    {        
        if (extension->type() == "kactus2:position")
        {
            setPosition(extension.dynamicCast<Kactus2Position>()->position());
        }
        else if (extension->type() == "kactus2:uuid")
        {
            uuid_->setValue(extension.dynamicCast<Kactus2Value>()->value());
        }
        else if (extension->type() == "kactus2:imported")
        {
            setImported(true);            
            setImportRef(extension.dynamicCast<Kactus2Placeholder>()->getAttributeValue("kactus2:importRef"));
        }
        else
        {
            vendorExtensions_.append(QSharedPointer<VendorExtension>(extension->clone()));
        }
    }
}
//-----------------------------------------------------------------------------
// Function: ComponentInstance::createImportVendorExtension()
//-----------------------------------------------------------------------------
void ComponentInstance::createImportVendorExtension()
{
    imported_ = QSharedPointer<Kactus2Placeholder>(new Kactus2Placeholder("kactus2:imported"));
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::createPostionVendorExtension()
//-----------------------------------------------------------------------------
void ComponentInstance::createPositionVendorExtension(QPointF const& position)
{
    pos_ = QSharedPointer<Kactus2Position>(new Kactus2Position(position));
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::createUuidVendorExtension()
//-----------------------------------------------------------------------------
void ComponentInstance::createUuidVendorExtension()
{
    QString uuid = QUuid::createUuid().toString();
    uuid_ = QSharedPointer<Kactus2Value>(new Kactus2Value("kactus2:uuid", uuid));
    vendorExtensions_.append(uuid_);
}
