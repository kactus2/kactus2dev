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
#include <IPXACTmodels/kactusExtensions/Kactus2Position.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Value.h>

#include <QUuid>

//-----------------------------------------------------------------------------
// Function: ComponentInstance::ComponentInstance()
//-----------------------------------------------------------------------------
ComponentInstance::ComponentInstance() :
    Extendable(),
    NameGroup(),
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
    NameGroup(instanceName),
    componentRef_(componentRef)
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::ComponentInstance()
//-----------------------------------------------------------------------------
ComponentInstance::ComponentInstance(ComponentInstance const& other) :
    Extendable(other),
    NameGroup(other),
    isPresent_(other.isPresent_),
    componentRef_(new ConfigurableVLNVReference(*other.componentRef_))
{
    if (getUuid().isEmpty())
    {
        setUuid(QUuid::createUuid().toString());
    }

    for (auto const& link : *other.powerDomainLinks_)
    {
        powerDomainLinks_->append(QSharedPointer<PowerDomainLink>(link->clone()));
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::operator=()
//-----------------------------------------------------------------------------
ComponentInstance& ComponentInstance::operator=(ComponentInstance const& other)
{
    if (this != &other)
    {
        Extendable::operator=(other);
        NameGroup::operator=(other);

        isPresent_ = other.isPresent_;
        componentRef_ = other.componentRef_;

        if (getUuid().isEmpty())
        {
            setUuid(QUuid::createUuid().toString());
        }

        powerDomainLinks_->clear();
        for (auto const& link : *other.powerDomainLinks_)
        {
            powerDomainLinks_->append(QSharedPointer<PowerDomainLink>(link->clone()));
        }
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setInstanceName()
//-----------------------------------------------------------------------------
void ComponentInstance::setInstanceName(QString const& name)
{
    setName(name);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getInstanceName()
//-----------------------------------------------------------------------------
QString ComponentInstance::getInstanceName() const
{
    return name();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setComponentRef()
//-----------------------------------------------------------------------------
void ComponentInstance::setComponentRef(QSharedPointer<ConfigurableVLNVReference> newComponentRef)
{
    componentRef_ = newComponentRef;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getComponentRef()
//-----------------------------------------------------------------------------
QSharedPointer<ConfigurableVLNVReference> ComponentInstance::getComponentRef() const
{
    return componentRef_;
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
// Function: ComponentInstance::setConfigurableElementValues()
//-----------------------------------------------------------------------------
void ComponentInstance::setConfigurableElementValues(
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > newValues)
{
    componentRef_->setConfigurableElementValues(newValues);
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
// Function: ComponentInstance::setPosition()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<ComponentInstance::PowerDomainLink> > > ComponentInstance::getPowerDomainLinks() const
{
    return powerDomainLinks_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setPosition()
//-----------------------------------------------------------------------------
void ComponentInstance::setPosition(QPointF const& pos)
{
    auto extension = findVendorExtension(QStringLiteral("kactus2:position")).dynamicCast<Kactus2Position>();

    if (pos.isNull())
    {
        getVendorExtensions()->removeAll(extension);
    }
    else
    {
        if (extension == nullptr)
        {
            extension = QSharedPointer<Kactus2Position>(new Kactus2Position(QPoint()));
            getVendorExtensions()->append(extension);
        }

        extension->setPosition(pos);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setImportRef()
//-----------------------------------------------------------------------------
void ComponentInstance::setImportRef(QString const& nameRef)
{
    auto importExtension = findVendorExtension(QStringLiteral("kactus2:imported")).dynamicCast<Kactus2Placeholder>();

    if (!nameRef.isEmpty())
    {
        if (importExtension == nullptr)
        {
            importExtension = QSharedPointer<Kactus2Placeholder>(new Kactus2Placeholder(QStringLiteral("kactus2:imported")));
            getVendorExtensions()->append(importExtension);
        }

        importExtension->setAttribute(QStringLiteral("importRef"), nameRef);
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
    auto extension = findVendorExtension(QStringLiteral("kactus2:propertyValues"));
    getVendorExtensions()->removeAll(extension);

    if (!values.isEmpty())
    {
        QSharedPointer<Kactus2Group> propertyValueGroup(new Kactus2Group(QStringLiteral("kactus2:propertyValues")));

        for (auto it = values.cbegin(); it != values.cend(); ++it)
        {
            QSharedPointer<Kactus2Placeholder> propertyValue(
                new Kactus2Placeholder(QStringLiteral("kactus2:propertyValue")));
            propertyValue->setAttribute(QStringLiteral("name"), it.key());
            propertyValue->setAttribute(QStringLiteral("value"), it.value());

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
    updatePositionsMap(name, pos, QStringLiteral("kactus2:portPositions"), QStringLiteral("kactus2:portPosition"),
        QStringLiteral("busRef"));
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::removeBusInterfacePosition()
//-----------------------------------------------------------------------------
void ComponentInstance::removeBusInterfacePosition(QString const& interfaceName)
{
    removePosition(interfaceName, QStringLiteral("kactus2:portPositions"), QStringLiteral("kactus2:portPosition"),
        QStringLiteral("busRef"));
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::updateAdHocPortPosition()
//-----------------------------------------------------------------------------
void ComponentInstance::updateAdHocPortPosition(QString const& name, QPointF const& pos)
{
    updatePositionsMap(name, pos, QStringLiteral("kactus2:adHocVisibilities"), 
        QStringLiteral("kactus2:adHocVisible"), QStringLiteral("portName"));
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::updateApiInterfacePosition()
//-----------------------------------------------------------------------------
void ComponentInstance::updateApiInterfacePosition(QString const& name, QPointF const& pos)
{
    updatePositionsMap(name, pos, QStringLiteral("kactus2:apiInterfacePositions"), 
        QStringLiteral("kactus2:apiInterfacePosition"), QStringLiteral("apiRef"));
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::removeApiInterfacePosition()
//-----------------------------------------------------------------------------
void ComponentInstance::removeApiInterfacePosition(QString const& name)
{
    removePosition(name, QStringLiteral("kactus2:apiInterfacePositions"),
        QStringLiteral("kactus2:apiInterfacePosition"),
        QStringLiteral("apiRef"));
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::updateComInterfacePosition()
//-----------------------------------------------------------------------------
void ComponentInstance::updateComInterfacePosition(QString const& name, QPointF const& pos)
{
    updatePositionsMap(name, pos, QStringLiteral("kactus2:comInterfacePositions"), 
        QStringLiteral("kactus2:comInterfacePosition"), QStringLiteral("comRef"));
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::removeComInterfacePosition()
//-----------------------------------------------------------------------------
void ComponentInstance::removeComInterfacePosition(QString const& name)
{
    removePosition(name, QStringLiteral("kactus2:comInterfacePositions"),
        QStringLiteral("kactus2:comInterfacePosition"),
        QStringLiteral("comRef"));
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getPosition()
//-----------------------------------------------------------------------------
QPointF ComponentInstance::getPosition() const
{
    auto position = findVendorExtension(QStringLiteral("kactus2:position")).dynamicCast<Kactus2Position>();

    if (position != nullptr)
    {
        return position->position();
    }

    return QPointF();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setImported()
//-----------------------------------------------------------------------------
void ComponentInstance::setImported(bool isImported)
{
    setPlaceholderExtension(isImported, QStringLiteral("kactus2:imported"));
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::isImported()
//-----------------------------------------------------------------------------
bool ComponentInstance::isImported() const
{
    return findVendorExtension(QStringLiteral("kactus2:imported")) != nullptr;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getImportRef()
//-----------------------------------------------------------------------------
QString ComponentInstance::getImportRef() const
{
    auto importExtension = findVendorExtension(QStringLiteral("kactus2:imported")).dynamicCast<Kactus2Placeholder>();

    if (importExtension != nullptr)
    {
        return importExtension->getAttributeValue(QStringLiteral("importRef"));
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getPropertyValues()
//-----------------------------------------------------------------------------
QMap<QString, QString> ComponentInstance::getPropertyValues() const
{
    QMap<QString, QString> propertyValues;

    auto propertyGroup = findVendorExtension(QStringLiteral("kactus2:propertyValues")).dynamicCast<Kactus2Group>();

    if (propertyGroup != nullptr)
    {
        QList<QSharedPointer<VendorExtension> > valueExtensionList =
            propertyGroup->getByType(QStringLiteral("kactus2:propertyValue"));

        for (auto const& valueExtension : valueExtensionList)
        {
            QSharedPointer<Kactus2Placeholder> propertyValue = valueExtension.dynamicCast<Kactus2Placeholder>();
            QString name = propertyValue->getAttributeValue(QStringLiteral("name"));
            QString value = propertyValue->getAttributeValue(QStringLiteral("value"));

            propertyValues.insert(name, value);
        }
    }

    return propertyValues;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getBusInterfacePositions()
//-----------------------------------------------------------------------------
QMap<QString, QPointF> ComponentInstance::getBusInterfacePositions() const
{
    return getPositionMap(QStringLiteral("kactus2:portPositions"), QStringLiteral("kactus2:portPosition"), 
        QStringLiteral("busRef"));
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getAdHocPortPositions()
//-----------------------------------------------------------------------------
QMap<QString, QPointF> ComponentInstance::getAdHocPortPositions() const
{
    return getPositionMap(QStringLiteral("kactus2:adHocVisibilities"), QStringLiteral("kactus2:adHocVisible"),
        QStringLiteral("portName"));
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getApiInterfacePositions()
//-----------------------------------------------------------------------------
QMap<QString, QPointF> ComponentInstance::getApiInterfacePositions() const
{
    return getPositionMap(QStringLiteral("kactus2:apiInterfacePositions"), 
        QStringLiteral("kactus2:apiInterfacePosition"), QStringLiteral("apiRef"));
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getComInterfacePositions()
//-----------------------------------------------------------------------------
QMap<QString, QPointF> ComponentInstance::getComInterfacePositions() const
{
    return getPositionMap(QStringLiteral("kactus2:comInterfacePositions"), 
        QStringLiteral("kactus2:comInterfacePosition"), QStringLiteral("comRef"));
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getPortAdHocVisibilities()
//-----------------------------------------------------------------------------
QMap<QString, bool> ComponentInstance::getPortAdHocVisibilities() const
{
    QMap<QString, bool> portAdHocVisibilities;

    QList<QSharedPointer<VendorExtension> > adHocPortExtensionList =
        getGroupedExtensionsByType(QStringLiteral("kactus2:adHocVisibilities"),
        QStringLiteral("kactus2:adHocVisible"));

    for (auto const& extension : adHocPortExtensionList)
    {
        QSharedPointer<Kactus2Placeholder> portExtension = extension.dynamicCast<Kactus2Placeholder>();
        portAdHocVisibilities.insert(portExtension->getAttributeValue(QStringLiteral("portName")), true);
    }

    return portAdHocVisibilities;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setFileSetRef()
//-----------------------------------------------------------------------------
void ComponentInstance::setFileSetRef(QString const& fileSetName)
{
    setValueExtension(fileSetName, QStringLiteral("kactus2:fileSetRef"));
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setMapping()
//-----------------------------------------------------------------------------
void ComponentInstance::setMapping(QString const& hwRef)
{
    auto extension = findVendorExtension(QStringLiteral("kactus2:mapping")).dynamicCast<Kactus2Placeholder>();

    if (!hwRef.isEmpty())
    {
        if (extension == nullptr)
        {
            extension = QSharedPointer<Kactus2Placeholder>(new Kactus2Placeholder(QStringLiteral("kactus2:mapping")));
            getVendorExtensions()->append(extension);
        }
        
        extension->setAttribute(QStringLiteral("hwRef"), hwRef);

    }
    else
    {
        getVendorExtensions()->removeAll(extension);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getFileSetRef()
//-----------------------------------------------------------------------------
QString ComponentInstance::getFileSetRef() const
{
    auto extension = findVendorExtension(QStringLiteral("kactus2:fileSetRef")).dynamicCast<Kactus2Value>();
    if (extension != nullptr)
    {
        return extension->value();
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getMapping()
//-----------------------------------------------------------------------------
QString ComponentInstance::getMapping() const
{
    auto extension = findVendorExtension(QStringLiteral("kactus2:mapping")).dynamicCast<Kactus2Placeholder>();
    if (extension != nullptr)
    {
        return extension->getAttributeValue(QStringLiteral("hwRef"));
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getUuid()
//-----------------------------------------------------------------------------
QString ComponentInstance::getUuid() const
{
    auto extension = findVendorExtension(QStringLiteral("kactus2:uuid")).dynamicCast<Kactus2Value>();
    if (extension != nullptr)
    {
        return extension->value();
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::isDraft()
//-----------------------------------------------------------------------------
bool ComponentInstance::isDraft() const
{
    return findVendorExtension(QStringLiteral("kactus2:draft")) != nullptr;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setDraft()
//-----------------------------------------------------------------------------
void ComponentInstance::setDraft(bool instanceIsDraft)
{
    setPlaceholderExtension(instanceIsDraft, QStringLiteral("kactus2:draft"));
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setUuid()
//-----------------------------------------------------------------------------
void ComponentInstance::setUuid(QString const& newUuid)
{
    setValueExtension(newUuid, QStringLiteral("kactus2:uuid"));
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::hideAdHocPort()
//-----------------------------------------------------------------------------
void ComponentInstance::hideAdHocPort(QString const& portName)
{
    removePosition(portName, QStringLiteral("kactus2:adHocVisibilities"),
        QStringLiteral("kactus2:adHocVisible"),
        QStringLiteral("portName"));
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::updatePositionsMap()
//-----------------------------------------------------------------------------
void ComponentInstance::updatePositionsMap(QString const& newReferenceName, QPointF const& newPosition,
    QString const& groupIdentifier, QString const& itemIdentifier, QString const& referenceIdentifier) const
{
    if (newReferenceName.isEmpty() || newPosition.isNull())
    {
        return;
    }

    auto portGroup = findVendorExtension(groupIdentifier).dynamicCast<Kactus2Group>();
    if (portGroup == nullptr)
    {
        portGroup = QSharedPointer<Kactus2Group>(new Kactus2Group(groupIdentifier));
        getVendorExtensions()->append(portGroup);
    }

    QList<QSharedPointer<VendorExtension> > interfacePositions = portGroup->getByType(itemIdentifier);

    auto it = std::find_if(interfacePositions.begin(), interfacePositions.end(),
        [&referenceIdentifier, &newReferenceName](QSharedPointer<VendorExtension> positionExtension)
    {   return positionExtension.dynamicCast<Kactus2Placeholder>()->getAttributeValue(referenceIdentifier) == newReferenceName; });

    QSharedPointer<Kactus2Placeholder> positionExtension;
    if (it != interfacePositions.cend())
    {
        positionExtension = (*it).dynamicCast<Kactus2Placeholder>();
    }
    else
    {
        positionExtension = QSharedPointer<Kactus2Placeholder>(new Kactus2Placeholder(itemIdentifier));
        positionExtension->setAttribute(referenceIdentifier, newReferenceName);

        portGroup->addToGroup(positionExtension);
    }

    positionExtension->setAttribute(QStringLiteral("x"), QString::number(newPosition.x()));
    positionExtension->setAttribute(QStringLiteral("y"), QString::number(newPosition.y()));
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::removePosition()
//-----------------------------------------------------------------------------
void ComponentInstance::removePosition(QString const& interfaceName, QString const& groupIdentifier, 
    QString const& itemIdentifier, QString const& referenceIdentifier)
{
    if (interfaceName.isEmpty())
    {
        return;
    }

    QSharedPointer<Kactus2Group> portGroup = findVendorExtension(groupIdentifier).
        dynamicCast<Kactus2Group>();

    if (portGroup == nullptr)
    {
        return;
    }

    QList<QSharedPointer<VendorExtension> > interfacePositions = portGroup->getByType(itemIdentifier);

    auto it = std::find_if(interfacePositions.begin(), interfacePositions.end(),
        [&referenceIdentifier, &interfaceName](QSharedPointer<VendorExtension> positionExtension)
    { return positionExtension.dynamicCast<Kactus2Placeholder>()->getAttributeValue(referenceIdentifier) == interfaceName; });

    QSharedPointer<Kactus2Placeholder> positionExtension;
    if (it != interfacePositions.cend())
    {
        positionExtension = (*it).dynamicCast<Kactus2Placeholder>();
        portGroup->removeFromGroup(positionExtension);
    }

    if (portGroup->getByType(itemIdentifier).isEmpty())
    {
        getVendorExtensions()->removeAll(portGroup);
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

    for (auto const& extension : positionList)
    {
        auto positionExtension = extension.dynamicCast<Kactus2Placeholder>();
        QString referenceName = positionExtension->getAttributeValue(referenceIdentifier);
        int positionX = positionExtension->getAttributeValue(QStringLiteral("x")).toInt();
        int positionY = positionExtension->getAttributeValue(QStringLiteral("y")).toInt();

        portPositions.insert(referenceName , QPointF(positionX, positionY));
    }

    return portPositions;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setValueExtension()
//-----------------------------------------------------------------------------
void ComponentInstance::setValueExtension(QString const& value, QString const& extensionType)
{
    auto extension = findVendorExtension(extensionType).dynamicCast<Kactus2Value>();

    if (value.isEmpty() == false)
    {
        if (extension == nullptr)
        {
            extension = QSharedPointer<Kactus2Value>(new Kactus2Value(extensionType, QString()));
            getVendorExtensions()->append(extension);
        }

        extension->setValue(value);
    }
    else
    {
        getVendorExtensions()->removeAll(extension);
    }
}

