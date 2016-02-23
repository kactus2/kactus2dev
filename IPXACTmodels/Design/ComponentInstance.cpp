//-----------------------------------------------------------------------------
// File: ComponentInstance.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 17.08.2015
//
// Description:
// Describes the ipxact:componentInstance element.
//-----------------------------------------------------------------------------

#include "ComponentInstance.h"

#include <IPXACTmodels/kactusExtensions/Kactus2Group.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Placeholder.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Value.h>
#include <IPXACTmodels/validators/namevalidator.h>

#include <QRegExpValidator>
#include <QUuid>

//-----------------------------------------------------------------------------
// Function: ComponentInstance::ComponentInstance()
//-----------------------------------------------------------------------------
ComponentInstance::ComponentInstance() :
Extendable(),
instanceName_(),
displayName_(),
description_(),
isPresent_(),
componentRef_(new ConfigurableVLNVReference())
{
    if (getUuid().isEmpty())
    {
        setUuid(QUuid::createUuid().toString());
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::ComponentInstance()
//-----------------------------------------------------------------------------
ComponentInstance::ComponentInstance(QString const& instanceName, QSharedPointer<ConfigurableVLNVReference> componentRef):
Extendable(),
    instanceName_(instanceName),
    displayName_(),
    description_(),
    isPresent_(),
    componentRef_(componentRef)
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::ComponentInstance()
//-----------------------------------------------------------------------------
ComponentInstance::ComponentInstance(ComponentInstance const& other) :
Extendable(other),
    instanceName_(other.instanceName_),
    displayName_(other.displayName_),
    description_(other.description_),
    isPresent_(other.isPresent_),
    componentRef_(new ConfigurableVLNVReference(*other.componentRef_.data()))
{
    if (getUuid().isEmpty())
    {
        setUuid(QUuid::createUuid().toString());
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::~ComponentInstance()
//-----------------------------------------------------------------------------
ComponentInstance::~ComponentInstance()
{
    componentRef_.clear();
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
    description_ = description;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setComponentRef()
//-----------------------------------------------------------------------------
void ComponentInstance::setComponentRef(QSharedPointer<ConfigurableVLNVReference> newComponentRef)
{
    componentRef_ = newComponentRef;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setConfigurableElementValues()
//-----------------------------------------------------------------------------
void ComponentInstance::setConfigurableElementValues(
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > newValues)
{
    componentRef_->setConfigurableElementValues(newValues);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setPosition()
//-----------------------------------------------------------------------------
void ComponentInstance::setPosition(QPointF const& pos)
{
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type().compare("kactus2:position") == 0)
        {
            getVendorExtensions()->removeAll(extension);
            break;
        }
    }
    if (!pos.isNull())
    {
        QSharedPointer<Kactus2Placeholder> positionExtension (new Kactus2Placeholder("kactus2:position"));
        positionExtension->setAttribute("x", QString::number(pos.x()));
        positionExtension->setAttribute("y", QString::number(pos.y()));

        getVendorExtensions()->append(positionExtension);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setImportRef()
//-----------------------------------------------------------------------------
void ComponentInstance::setImportRef(QString const& nameRef)
{
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type().compare("kactus2:imported") == 0)
        {
            getVendorExtensions()->removeAll(extension);
            break;
        }
    }

    if (!nameRef.isEmpty())
    {
        QSharedPointer<Kactus2Placeholder> importExtension (new Kactus2Placeholder("kactus2:imported"));
        importExtension->setAttribute("importRef", nameRef);

        getVendorExtensions()->append(importExtension);
    }
    else
    {
        setImported(true);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setPropertyValues()
//-----------------------------------------------------------------------------
void ComponentInstance::setPropertyValues(QMap<QString, QString> const& values)
{
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:propertyValues")
        {
            getVendorExtensions()->removeAll(extension);
            break;
        }
    }

    if (!values.isEmpty())
    {
        QSharedPointer<Kactus2Group> propertyValueGroup(new Kactus2Group("kactus2:propertyValues"));

        QMapIterator <QString, QString> valueIterator(values);

        while (valueIterator.hasNext())
        {
            valueIterator.next();

            QSharedPointer<Kactus2Placeholder> propertyValue (new Kactus2Placeholder("kactus2:propertyValue"));
            propertyValue->setAttribute("name", valueIterator.key());
            propertyValue->setAttribute("value", valueIterator.value());

            propertyValueGroup->addToGroup(propertyValue);
        }

        getVendorExtensions()->append(propertyValueGroup);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::updateBusInterfacePosition()
//-----------------------------------------------------------------------------
void ComponentInstance::updateBusInterfacePosition(QString const& name, QPointF const& pos)
{
    updatePositionsMap(name, pos, "kactus2:portPositions", "kactus2:portPosition", "busRef");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::removeBusInterfacePosition()
//-----------------------------------------------------------------------------
void ComponentInstance::removeBusInterfacePosition(QString const& interfaceName)
{
    if (!interfaceName.isEmpty())
    {
        QSharedPointer<Kactus2Group> portGroup;
        QList<QSharedPointer<VendorExtension> > interfacePositions;

        foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
        {
            if (extension->type() == QLatin1String("kactus2:portPositions"))
            {
                portGroup = extension.dynamicCast<Kactus2Group>();
                interfacePositions = portGroup->getByType(QLatin1String("kactus2:portPosition"));
                break;
            }
        }

        foreach(QSharedPointer<VendorExtension> extension, interfacePositions)
        {
            QSharedPointer<Kactus2Placeholder> positionExtension = extension.dynamicCast<Kactus2Placeholder>();
            if (positionExtension->getAttributeValue(QLatin1String("busRef")) == interfaceName)
            {
                portGroup->removeFromGroup(positionExtension);
            }            
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::updateAdHocPortPosition()
//-----------------------------------------------------------------------------
void ComponentInstance::updateAdHocPortPosition(QString const& name, QPointF const& pos)
{
    updatePositionsMap(name, pos, "kactus2:adHocVisibilities", "kactus2:adHocVisible", "portName");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::updateApiInterfacePosition()
//-----------------------------------------------------------------------------
void ComponentInstance::updateApiInterfacePosition(QString const& name, QPointF const& pos)
{
    updatePositionsMap(name, pos, "kactus2:apiInterfacePositions", "kactus2:apiInterfacePosition", "apiRef");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::removeApiInterfacePosition()
//-----------------------------------------------------------------------------
void ComponentInstance::removeApiInterfacePosition(QString const& name)
{
    QSharedPointer<Kactus2Group> apiGroup;
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:apiInterfacePositions")
        {
            apiGroup = extension.dynamicCast<Kactus2Group>();
            break;
        }
    }

    if (apiGroup)
    {
        foreach (QSharedPointer<VendorExtension> extension, apiGroup->getByType("kactus2:apiInterfacePosition"))
        {
            QSharedPointer<Kactus2Placeholder> positionExtension = extension.dynamicCast<Kactus2Placeholder>();
            if (positionExtension->getAttributeValue("apiRef") == name)
            {
                apiGroup->removeFromGroup(extension);
            }
        }

        if (apiGroup->getByType("kactus2:apiInterfacePosition").isEmpty())
        {
            getVendorExtensions()->removeAll(apiGroup);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::updateComInterfacePosition()
//-----------------------------------------------------------------------------
void ComponentInstance::updateComInterfacePosition(QString const& name, QPointF const& pos)
{
    updatePositionsMap(name, pos, "kactus2:comInterfacePositions", "kactus2:comInterfacePosition", "comRef");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::removeComInterfacePosition()
//-----------------------------------------------------------------------------
void ComponentInstance::removeComInterfacePosition(QString const& name)
{
    QSharedPointer<Kactus2Group> comGroup;
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:comInterfacePositions")
        {
            comGroup = extension.dynamicCast<Kactus2Group>();
            break;
        }
    }

    if (comGroup)
    {
        foreach (QSharedPointer<VendorExtension> extension, comGroup->getByType("kactus2:comInterfacePosition"))
        {
            QSharedPointer<Kactus2Placeholder> positionExtension = extension.dynamicCast<Kactus2Placeholder>();
            if (positionExtension->getAttributeValue("comRef") == name)
            {
                comGroup->removeFromGroup(extension);
            }
        }

        if (comGroup->getByType("kactus2:comInterfacePosition").isEmpty())
        {
            getVendorExtensions()->removeAll(comGroup);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getInstanceName()
//-----------------------------------------------------------------------------
QString ComponentInstance::getInstanceName() const
{
    return instanceName_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getDisplayName()
//-----------------------------------------------------------------------------
QString ComponentInstance::getDisplayName() const
{
    return displayName_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getDescription()
//-----------------------------------------------------------------------------
QString ComponentInstance::getDescription() const
{
    return description_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getComponentRef()
//-----------------------------------------------------------------------------
QSharedPointer<ConfigurableVLNVReference> ComponentInstance::getComponentRef() const
{
    return componentRef_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getConfigurableElementValues()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > ComponentInstance::getConfigurableElementValues()
    const
{
    return componentRef_->getConfigurableElementValues();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getPosition()
//-----------------------------------------------------------------------------
QPointF ComponentInstance::getPosition() const
{
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:position")
        {
            QSharedPointer<Kactus2Placeholder> position = extension.dynamicCast<Kactus2Placeholder>();

            int positionX = position->getAttributeValue("x").toInt();
            int positionY = position->getAttributeValue("y").toInt();
            return QPointF(positionX, positionY);
        }
    }

    return QPointF();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setImported()
//-----------------------------------------------------------------------------
void ComponentInstance::setImported(bool isImported)
{
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type().compare("kactus2:imported") == 0)
        {
            getVendorExtensions()->removeAll(extension);
            break;
        }
    }

    if (isImported)
    {
        QSharedPointer<Kactus2Placeholder> importExtension (new Kactus2Placeholder("kactus2:imported"));

        getVendorExtensions()->append(importExtension);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::isImported()
//-----------------------------------------------------------------------------
bool ComponentInstance::isImported() const
{
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type().compare("kactus2:imported") == 0)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getImportRef()
//-----------------------------------------------------------------------------
QString ComponentInstance::getImportRef() const
{
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type().compare("kactus2:imported") == 0)
        {
            QSharedPointer<Kactus2Placeholder> importExtension = extension.dynamicCast<Kactus2Placeholder>();
            return importExtension->getAttributeValue("importRef");
        }
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getPropertyValues()
//-----------------------------------------------------------------------------
QMap<QString, QString> ComponentInstance::getPropertyValues() const
{
    QMap<QString, QString> propertyValues;

    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type().compare("kactus2:propertyValues") == 0)
        {
            QSharedPointer<Kactus2Group> propertyGroup = extension.dynamicCast<Kactus2Group>();
            QList<QSharedPointer<VendorExtension> > valueExtensionList =
                propertyGroup->getByType("kactus2:propertyValue");

            foreach (QSharedPointer<VendorExtension> valueExtension, valueExtensionList)
            {
                QSharedPointer<Kactus2Placeholder> propertyValue = valueExtension.dynamicCast<Kactus2Placeholder>();
                QString name = propertyValue->getAttributeValue("name");
                QString value = propertyValue->getAttributeValue("value");

                propertyValues.insert(name, value);
            }
            break;
        }
    }

    return propertyValues;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getBusInterfacePositions()
//-----------------------------------------------------------------------------
QMap<QString, QPointF> ComponentInstance::getBusInterfacePositions() const
{
    return getPositionMap("kactus2:portPositions", "kactus2:portPosition", "busRef");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getAdHocPortPositions()
//-----------------------------------------------------------------------------
QMap<QString, QPointF> ComponentInstance::getAdHocPortPositions() const
{
    return getPositionMap("kactus2:adHocVisibilities", "kactus2:adHocVisible", "portName");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getApiInterfacePositions()
//-----------------------------------------------------------------------------
QMap<QString, QPointF> ComponentInstance::getApiInterfacePositions() const
{
    return getPositionMap("kactus2:apiInterfacePositions", "kactus2:apiInterfacePosition", "apiRef");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getComInterfacePositions()
//-----------------------------------------------------------------------------
QMap<QString, QPointF> ComponentInstance::getComInterfacePositions() const
{
    return getPositionMap("kactus2:comInterfacePositions", "kactus2:comInterfacePosition", "comRef");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getPortAdHocVisibilities()
//-----------------------------------------------------------------------------
QMap<QString, bool> ComponentInstance::getPortAdHocVisibilities() const
{
    QMap<QString, bool> portAdHocVisibilities;

    QList<QSharedPointer<VendorExtension> > adHocPortExtensionList =
        getGroupedExtensionsByType("kactus2:adHocVisibilities", "kactus2:adHocVisible");

    foreach (QSharedPointer<VendorExtension> extension, adHocPortExtensionList)
    {
        QSharedPointer<Kactus2Placeholder> portExtension = extension.dynamicCast<Kactus2Placeholder>();
        portAdHocVisibilities.insert(portExtension->getAttributeValue("portName"), true);
    }

    return portAdHocVisibilities;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::operator=()
//-----------------------------------------------------------------------------
ComponentInstance& ComponentInstance::operator=(ComponentInstance const& other)
{
    if (this != &other)
    {
        Extendable::operator=(other);

        instanceName_ = other.instanceName_;
        displayName_ = other.displayName_;
        description_ = other.description_;
        isPresent_ = other.isPresent_;
        componentRef_ = other.componentRef_;

        if (getUuid().isEmpty())
        {
            setUuid(QUuid::createUuid().toString());
        }
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::parsePropertyValues()
//-----------------------------------------------------------------------------
void ComponentInstance::parsePropertyValues(QDomNode& node)
{
    QMap<QString, QString> newPropertyValues;

    for (int i = 0; i < node.childNodes().count(); ++i)
    {
        QDomNode propNode = node.childNodes().at(i);

        if (propNode.nodeName() == "kactus2:propertyValue")
        {
            QString name = propNode.attributes().namedItem("kactus2:name").nodeValue();
            QString value = propNode.attributes().namedItem("kactus2:value").nodeValue();

            newPropertyValues.insert(name, value);
        }
    }

    setPropertyValues(newPropertyValues);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getUuid()
//-----------------------------------------------------------------------------
QString ComponentInstance::getUuid() const
{
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type().compare("kactus2:uuid") == 0)
        {
            QSharedPointer<Kactus2Value> uuidExtension = extension.dynamicCast<Kactus2Value>();
            return (uuidExtension->value());
        }
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::isDraft()
//-----------------------------------------------------------------------------
bool ComponentInstance::isDraft() const
{
    foreach(QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:draft")
        {
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setDraft()
//-----------------------------------------------------------------------------
void ComponentInstance::setDraft(bool instanceIsDraft)
{
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:draft")
        {
            if (!instanceIsDraft)
            {
                getVendorExtensions()->removeAll(extension);
            }
            return;
        }
    }

    if (instanceIsDraft)
    {
        QSharedPointer<Kactus2Placeholder> draftExtension (new Kactus2Placeholder("kactus2:draft"));
        getVendorExtensions()->append(draftExtension);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getIsPresent()
//-----------------------------------------------------------------------------
QString ComponentInstance::getIsPresent() const
{
    return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setIsPresent()
//-----------------------------------------------------------------------------
void ComponentInstance::setIsPresent(QString const& newIsPresent)
{
    isPresent_ = newIsPresent;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setUuid()
//-----------------------------------------------------------------------------
void ComponentInstance::setUuid(QString const& newUuid)
{
    foreach(QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type().compare("kactus2:uuid") == 0)
        {
            getVendorExtensions()->removeAll(extension);
            break;
        }
    }

    QSharedPointer<Kactus2Value> uuidExtension (new Kactus2Value("kactus2:uuid", newUuid));
    getVendorExtensions()->append(uuidExtension);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::hideAdHocPort()
//-----------------------------------------------------------------------------
void ComponentInstance::hideAdHocPort(QString const& portName)
{
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:adHocVisibilities")
        {
            QSharedPointer<Kactus2Group> visibilityGroup = extension.dynamicCast<Kactus2Group>();

            foreach (QSharedPointer<VendorExtension> visibility, visibilityGroup->getByType("kactus2:adHocVisible"))
            {
                QSharedPointer<Kactus2Placeholder> adhocExtension = visibility.dynamicCast<Kactus2Placeholder>();
                if (adhocExtension->getAttributeValue("portName") == portName)
                {
                    visibilityGroup->removeFromGroup(visibility);
                    break;
                }
            }

            if (visibilityGroup->getByType("kactus2:adHocVisible").isEmpty())
            {
                getVendorExtensions()->removeAll(extension);
            }

            return;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::updatePositionsMap()
//-----------------------------------------------------------------------------
void ComponentInstance::updatePositionsMap(QString const& newReferenceName, QPointF const& newPosition,
    QString const& groupIdentifier, QString const& itemIdentifier, QString const& referenceIdentifier) const
{
    if (!newReferenceName.isEmpty() && !newPosition.isNull())
    {
        QSharedPointer<Kactus2Group> portGroup;
        QList<QSharedPointer<VendorExtension> > interfacePositions;

        foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
        {
            if (extension->type().compare(groupIdentifier) == 0)
            {
                portGroup = extension.dynamicCast<Kactus2Group>();
                interfacePositions = portGroup->getByType(itemIdentifier);
                break;
            }
        }

        foreach(QSharedPointer<VendorExtension> extension, interfacePositions)
        {
            QSharedPointer<Kactus2Placeholder> positionExtension = extension.dynamicCast<Kactus2Placeholder>();
            if (positionExtension->getAttributeValue(referenceIdentifier) == newReferenceName)
            {
                positionExtension->setAttribute("x", QString::number(newPosition.x()));
                positionExtension->setAttribute("y", QString::number(newPosition.y()));
                return;
            }            
        }


        QSharedPointer<Kactus2Placeholder> interfacePositionExtension (
            new Kactus2Placeholder(itemIdentifier));
        interfacePositionExtension->setAttribute(referenceIdentifier, newReferenceName);
        interfacePositionExtension->setAttribute("x", QString::number(newPosition.x()));
        interfacePositionExtension->setAttribute("y", QString::number(newPosition.y()));

        interfacePositions.append(interfacePositionExtension);

        if (!portGroup)
        {
            portGroup = QSharedPointer<Kactus2Group>(new Kactus2Group(groupIdentifier));
            getVendorExtensions()->append(portGroup);
        }    

        portGroup->addToGroup(interfacePositionExtension);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getPositionMap()
//-----------------------------------------------------------------------------
QMap<QString, QPointF> ComponentInstance::getPositionMap(QString const& groupIdentifier,
    QString const& itemIdentifier, QString const& referenceIdentifier) const
{
    QList<QSharedPointer<VendorExtension> > positionList =
        getGroupedExtensionsByType(groupIdentifier, itemIdentifier);

    QMap<QString, QPointF> portPositions;

    foreach (QSharedPointer<VendorExtension> extension, positionList)
    {
        QSharedPointer<Kactus2Placeholder> positionExtension = extension.dynamicCast<Kactus2Placeholder>();
        QString referenceName = positionExtension->getAttributeValue(referenceIdentifier);
        int positionX = positionExtension->getAttributeValue("x").toInt();
        int positionY = positionExtension->getAttributeValue("y").toInt();

        portPositions.insert(referenceName , QPointF(positionX, positionY));
    }

    return portPositions;
}