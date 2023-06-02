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
    if (getUuid().empty())
    {
        setUuid(QUuid::createUuid().toString().toStdString());
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::ComponentInstance()
//-----------------------------------------------------------------------------
ComponentInstance::ComponentInstance(std::string const& instanceName, QSharedPointer<ConfigurableVLNVReference> componentRef):
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
    if (getUuid().empty())
    {
        setUuid(QUuid::createUuid().toString().toStdString());
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

        if (getUuid().empty())
        {
            setUuid(QUuid::createUuid().toString().toStdString());
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
void ComponentInstance::setInstanceName(std::string const& name)
{
    setName(name);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getInstanceName()
//-----------------------------------------------------------------------------
std::string ComponentInstance::getInstanceName() const
{
    return name().toStdString();
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
std::string ComponentInstance::getIsPresent() const
{
    return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setIsPresent()
//-----------------------------------------------------------------------------
void ComponentInstance::setIsPresent(std::string const& newIsPresent)
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
    auto extension = findVendorExtension("kactus2:position").dynamicCast<Kactus2Position>();

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
void ComponentInstance::setImportRef(std::string const& nameRef)
{
    auto importExtension = findVendorExtension("kactus2:imported").dynamicCast<Kactus2Placeholder>();

    if (!nameRef.empty())
    {
        if (importExtension == nullptr)
        {
            importExtension = QSharedPointer<Kactus2Placeholder>(new Kactus2Placeholder("kactus2:imported"));
            getVendorExtensions()->append(importExtension);
        }

        importExtension->setAttribute("importRef", nameRef);
    }
    else
    {
        setImported(true);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setPropertyValues()
//-----------------------------------------------------------------------------
void ComponentInstance::setPropertyValues(QMap<std::string, std::string> const& values)
{
    auto extension = findVendorExtension("kactus2:propertyValues");
    getVendorExtensions()->removeAll(extension);

    if (!values.isEmpty())
    {
        QSharedPointer<Kactus2Group> propertyValueGroup(new Kactus2Group("kactus2:propertyValues"));

        for (auto it = values.cbegin(); it != values.cend(); ++it)
        {
            QSharedPointer<Kactus2Placeholder> propertyValue(new Kactus2Placeholder("kactus2:propertyValue"));
            propertyValue->setAttribute("name", it.key());
            propertyValue->setAttribute("value", it.value());

            propertyValueGroup->addToGroup(propertyValue);
        }

        getVendorExtensions()->append(propertyValueGroup);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::updateBusInterfacePosition()
//-----------------------------------------------------------------------------
void ComponentInstance::updateBusInterfacePosition(std::string_view name, QPointF const& pos)
{
    updatePositionsMap(name, pos, "kactus2:portPositions", "kactus2:portPosition", "busRef");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::removeBusInterfacePosition()
//-----------------------------------------------------------------------------
void ComponentInstance::removeBusInterfacePosition(std::string_view interfaceName)
{
    removePosition(interfaceName, "kactus2:portPositions", "kactus2:portPosition", "busRef");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::updateAdHocPortPosition()
//-----------------------------------------------------------------------------
void ComponentInstance::updateAdHocPortPosition(std::string const& name, QPointF const& pos)
{
    updatePositionsMap(name, pos, "kactus2:adHocVisibilities", "kactus2:adHocVisible", "portName");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::updateApiInterfacePosition()
//-----------------------------------------------------------------------------
void ComponentInstance::updateApiInterfacePosition(std::string const& name, QPointF const& pos)
{
    updatePositionsMap(name, pos, "kactus2:apiInterfacePositions", "kactus2:apiInterfacePosition", "apiRef");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::removeApiInterfacePosition()
//-----------------------------------------------------------------------------
void ComponentInstance::removeApiInterfacePosition(std::string const& name)
{
    removePosition(name, "kactus2:apiInterfacePositions", "kactus2:apiInterfacePosition", "apiRef");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::updateComInterfacePosition()
//-----------------------------------------------------------------------------
void ComponentInstance::updateComInterfacePosition(std::string const& name, QPointF const& pos)
{
    updatePositionsMap(name, pos, "kactus2:comInterfacePositions", "kactus2:comInterfacePosition", "comRef");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::removeComInterfacePosition()
//-----------------------------------------------------------------------------
void ComponentInstance::removeComInterfacePosition(std::string const& name)
{
    removePosition(name, "kactus2:comInterfacePositions", "kactus2:comInterfacePosition", "comRef");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getPosition()
//-----------------------------------------------------------------------------
QPointF ComponentInstance::getPosition() const
{
    auto position = findVendorExtension("kactus2:position").dynamicCast<Kactus2Position>();

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
    setPlaceholderExtension(isImported, "kactus2:imported");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::isImported()
//-----------------------------------------------------------------------------
bool ComponentInstance::isImported() const
{
    return findVendorExtension("kactus2:imported") != nullptr;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getImportRef()
//-----------------------------------------------------------------------------
std::string ComponentInstance::getImportRef() const
{
    auto importExtension = findVendorExtension("kactus2:imported").dynamicCast<Kactus2Placeholder>();

    if (importExtension != nullptr)
    {
        return importExtension->getAttributeValue(std::string("importRef"));
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getPropertyValues()
//-----------------------------------------------------------------------------
QMap<std::string, std::string> ComponentInstance::getPropertyValues() const
{
    QMap<std::string, std::string> propertyValues;

    auto propertyGroup = findVendorExtension("kactus2:propertyValues").dynamicCast<Kactus2Group>();

    if (propertyGroup != nullptr)
    {
        QList<QSharedPointer<VendorExtension> > valueExtensionList =
            propertyGroup->getByType("kactus2:propertyValue");

        for (auto const& valueExtension : valueExtensionList)
        {
            QSharedPointer<Kactus2Placeholder> propertyValue = valueExtension.dynamicCast<Kactus2Placeholder>();
            std::string name = propertyValue->getAttributeValue(std::string("name"));
            std::string value = propertyValue->getAttributeValue(std::string("value"));

            propertyValues.insert(name, value);
        }
    }

    return propertyValues;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getBusInterfacePositions()
//-----------------------------------------------------------------------------
QMap<std::string, QPointF> ComponentInstance::getBusInterfacePositions() const
{
    return getPositionMap("kactus2:portPositions", "kactus2:portPosition", "busRef");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getAdHocPortPositions()
//-----------------------------------------------------------------------------
QMap<std::string, QPointF> ComponentInstance::getAdHocPortPositions() const
{
    return getPositionMap("kactus2:adHocVisibilities", "kactus2:adHocVisible", "portName");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getApiInterfacePositions()
//-----------------------------------------------------------------------------
QMap<std::string, QPointF> ComponentInstance::getApiInterfacePositions() const
{
    return getPositionMap("kactus2:apiInterfacePositions", "kactus2:apiInterfacePosition", "apiRef");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getComInterfacePositions()
//-----------------------------------------------------------------------------
QMap<std::string, QPointF> ComponentInstance::getComInterfacePositions() const
{
    return getPositionMap("kactus2:comInterfacePositions", 
        "kactus2:comInterfacePosition", "comRef");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getPortAdHocVisibilities()
//-----------------------------------------------------------------------------
QMap<std::string, bool> ComponentInstance::getPortAdHocVisibilities() const
{
    QMap<std::string, bool> portAdHocVisibilities;

    QList<QSharedPointer<VendorExtension> > adHocPortExtensionList =
        getGroupedExtensionsByType("kactus2:adHocVisibilities", "kactus2:adHocVisible");

    for (auto const& extension : adHocPortExtensionList)
    {
        QSharedPointer<Kactus2Placeholder> portExtension = extension.dynamicCast<Kactus2Placeholder>();
        portAdHocVisibilities.insert(portExtension->getAttributeValue(std::string("portName")), true);
    }

    return portAdHocVisibilities;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setFileSetRef()
//-----------------------------------------------------------------------------
void ComponentInstance::setFileSetRef(std::string const& fileSetName)
{
    setValueExtension(fileSetName, "kactus2:fileSetRef");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setMapping()
//-----------------------------------------------------------------------------
void ComponentInstance::setMapping(std::string const& hwRef)
{
    auto extension = findVendorExtension("kactus2:mapping").dynamicCast<Kactus2Placeholder>();

    if (!hwRef.empty())
    {
        if (extension == nullptr)
        {
            extension = QSharedPointer<Kactus2Placeholder>(new Kactus2Placeholder("kactus2:mapping"));
            getVendorExtensions()->append(extension);
        }
        
        extension->setAttribute(QStringLiteral("hwRef"), QString::fromStdString(hwRef));

    }
    else
    {
        getVendorExtensions()->removeAll(extension);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getFileSetRef()
//-----------------------------------------------------------------------------
std::string ComponentInstance::getFileSetRef() const
{
    auto extension = findVendorExtension("kactus2:fileSetRef").dynamicCast<Kactus2Value>();
    if (extension != nullptr)
    {
        return extension->value();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getMapping()
//-----------------------------------------------------------------------------
std::string ComponentInstance::getMapping() const
{
    auto extension = findVendorExtension("kactus2:mapping").dynamicCast<Kactus2Placeholder>();
    if (extension != nullptr)
    {
        return extension->getAttributeValue(std::string("hwRef"));
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getUuid()
//-----------------------------------------------------------------------------
std::string ComponentInstance::getUuid() const
{
    auto extension = findVendorExtension("kactus2:uuid").dynamicCast<Kactus2Value>();
    if (extension != nullptr)
    {
        return extension->value();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::isDraft()
//-----------------------------------------------------------------------------
bool ComponentInstance::isDraft() const
{
    return findVendorExtension("kactus2:draft") != nullptr;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setDraft()
//-----------------------------------------------------------------------------
void ComponentInstance::setDraft(bool instanceIsDraft)
{
    setPlaceholderExtension(instanceIsDraft, "kactus2:draft");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setUuid()
//-----------------------------------------------------------------------------
void ComponentInstance::setUuid(std::string const& newUuid)
{
    setValueExtension(newUuid, "kactus2:uuid");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::hideAdHocPort()
//-----------------------------------------------------------------------------
void ComponentInstance::hideAdHocPort(std::string const& portName)
{
    removePosition(portName, "kactus2:adHocVisibilities",
        "kactus2:adHocVisible", "portName");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::updatePositionsMap()
//-----------------------------------------------------------------------------
void ComponentInstance::updatePositionsMap(std::string_view newReferenceName, QPointF const& newPosition,
    std::string const& groupIdentifier, std::string const& itemIdentifier, std::string const& referenceIdentifier) const
{
    if (newReferenceName.empty() || newPosition.isNull())
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
        [&referenceIdentifier, &newReferenceName](auto const& positionExtension)
        { return positionExtension.dynamicCast<Kactus2Placeholder>()->getAttributeValue(referenceIdentifier) == newReferenceName; });

    QSharedPointer<Kactus2Placeholder> positionExtension;
    if (it != interfacePositions.cend())
    {
        positionExtension = (*it).dynamicCast<Kactus2Placeholder>();
    }
    else
    {
        positionExtension = QSharedPointer<Kactus2Placeholder>(new Kactus2Placeholder(itemIdentifier));
        positionExtension->setAttribute(referenceIdentifier, std::string(newReferenceName));

        portGroup->addToGroup(positionExtension);
    }

    positionExtension->setAttribute(QStringLiteral("x"), QString::number(newPosition.x()));
    positionExtension->setAttribute(QStringLiteral("y"), QString::number(newPosition.y()));
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::removePosition()
//-----------------------------------------------------------------------------
void ComponentInstance::removePosition(std::string_view interfaceName, std::string const& groupIdentifier,
    std::string const& itemIdentifier, std::string const& referenceIdentifier)
{
    if (interfaceName.empty())
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
        [&referenceIdentifier, &interfaceName](auto const& positionExtension)
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
QMap<std::string, QPointF> ComponentInstance::getPositionMap(std::string const& groupIdentifier,
    std::string const& itemIdentifier, std::string const& referenceIdentifier) const
{
    QList<QSharedPointer<VendorExtension> > positionList =
        getGroupedExtensionsByType(groupIdentifier, itemIdentifier);

    QMap<std::string, QPointF> portPositions;

    for (auto const& extension : positionList)
    {
        auto positionExtension = extension.dynamicCast<Kactus2Placeholder>();
        auto referenceName = positionExtension->getAttributeValue(referenceIdentifier);
        int positionX = std::stoi(positionExtension->getAttributeValue(std::string("x")));
        int positionY = std::stoi(positionExtension->getAttributeValue(std::string("y")));

        portPositions.insert(referenceName , QPointF(positionX, positionY));
    }

    return portPositions;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setValueExtension()
//-----------------------------------------------------------------------------
void ComponentInstance::setValueExtension(std::string const& value, std::string const& extensionType)
{
    auto extension = findVendorExtension(extensionType).dynamicCast<Kactus2Value>();

    if (value.empty() == false)
    {
        if (extension == nullptr)
        {
            extension = QSharedPointer<Kactus2Value>(new Kactus2Value(extensionType, std::string()));
            getVendorExtensions()->append(extension);
        }

        extension->setValue(value);
    }
    else
    {
        getVendorExtensions()->removeAll(extension);
    }
}

