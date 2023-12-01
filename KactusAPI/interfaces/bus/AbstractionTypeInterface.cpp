//-----------------------------------------------------------------------------
// File: AbstractionTypeInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 01.03.2021
//
// Description:
// Interface for accessing abstraction types.
//-----------------------------------------------------------------------------

#include <AbstractionTypeInterface.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/common/ConfigurableVLNVReference.h>
#include <IPXACTmodels/Component/AbstractionType.h>
#include <IPXACTmodels/Component/validators/AbstractionTypeValidator.h>

#include <PortMapInterface.h>

#include <KactusAPI/include/LibraryInterface.h>

namespace
{
    const QString VIEW_SEPARATOR = QLatin1String("; ");
};

//-----------------------------------------------------------------------------
// Function: AbstractionTypeInterface::AbstractionTypeInterface()
//-----------------------------------------------------------------------------
AbstractionTypeInterface::AbstractionTypeInterface(PortMapInterface* portMapInterface,
    QSharedPointer<AbstractionTypeValidator> validator, LibraryInterface* library):
portMapInterface_(portMapInterface),
validator_(validator),
library_(library)
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeInterface::setAbstractionTypes()
//-----------------------------------------------------------------------------
void AbstractionTypeInterface::setAbstractionTypes(
    QSharedPointer<QList<QSharedPointer<AbstractionType>>> newAbstractions, General::InterfaceMode busMode,
    QString const& systemGroup)
{
    abstractions_ = newAbstractions;

    portMapInterface_->setupBusMode(busMode);
    portMapInterface_->setupSystemGroup(systemGroup);
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeInterface::setBusMode()
//-----------------------------------------------------------------------------
void AbstractionTypeInterface::setBusMode(General::InterfaceMode busMode)
{
    portMapInterface_->setupBusMode(busMode);
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeInterface::()
//-----------------------------------------------------------------------------
void AbstractionTypeInterface::setSystemGroup(QString const& systemGroup)
{
    portMapInterface_->setupSystemGroup(systemGroup);
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeInterface::setupAbstractionForPortMapInterface()
//-----------------------------------------------------------------------------
bool AbstractionTypeInterface::setupAbstractionTypeForPortMapInterface(int const& abstractionTypeIndex)
{
    QSharedPointer<AbstractionType> abstraction = getAbstraction(abstractionTypeIndex);
    if (abstraction)
    {
        portMapInterface_->setupPortMaps(abstraction);

        return setupAbstractionDefinitionForPortMapInterface(abstractionTypeIndex);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeInterface::setupAbstractionDefinitionForPortMapInterface()
//-----------------------------------------------------------------------------
bool AbstractionTypeInterface::setupAbstractionDefinitionForPortMapInterface(int const& abstractionTypeIndex)
{
    QSharedPointer<ConfigurableVLNVReference> absDefReference = getAbstractionReference(abstractionTypeIndex);
    if (absDefReference)
    {
        QSharedPointer<const Document> absDefDocument = library_->getModelReadOnly(*absDefReference.data());
        if (absDefDocument)
        {
            QSharedPointer<const AbstractionDefinition> absDef =
                absDefDocument.dynamicCast<const AbstractionDefinition>();

            if (absDef)
            {
                portMapInterface_->setupAbstractionDefinition(absDef);
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeInterface::getAbstraction()
//-----------------------------------------------------------------------------
QSharedPointer<AbstractionType> AbstractionTypeInterface::getAbstraction(int const& abstractionIndex) const
{
    if (abstractionIndex >= 0 && abstractionIndex < itemCount())
    {
        return abstractions_->at(abstractionIndex);
    }
    else
    {
        return QSharedPointer<AbstractionType>();
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeInterface::getIndexedAbstraction()
//-----------------------------------------------------------------------------
std::string AbstractionTypeInterface::getIndexedAbstraction(int const& itemIndex) const
{
    std::string abstractionString = "";

    if (itemIndex >= 0 && itemIndex < itemCount())
    {
        abstractionString = abstractions_->at(itemIndex)->getAbstractionRef()->toString().toStdString();
    }

    return abstractionString;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeInterface::getAbstractionReference()
//-----------------------------------------------------------------------------
QSharedPointer<ConfigurableVLNVReference> AbstractionTypeInterface::getAbstractionReference(int const& typeIndex)
const
{
    QSharedPointer<AbstractionType> selectedType = getAbstraction(typeIndex);
    if (selectedType)
    {
        return selectedType->getAbstractionRef();
    }

    return QSharedPointer<ConfigurableVLNVReference>();
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeInterface::getAbstractionReferenceString()
//-----------------------------------------------------------------------------
std::string AbstractionTypeInterface::getAbstractionReferenceString() const
{
    std::string referenceString = "";
    int count = itemCount();
    if (count == 1)
    {
        referenceString = abstractions_->first()->getAbstractionRef()->toString().toStdString();
    }
    else if (count > 1)
    {
        referenceString = "[multiple]";
    }

    return referenceString;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeInterface::getAbstractionReferences()
//-----------------------------------------------------------------------------
std::vector<std::string> AbstractionTypeInterface::getAbstractionReferences() const
{
    std::vector<std::string> references;

    for (auto abstractionType : *abstractions_)
    {
        references.push_back(abstractionType->getAbstractionRef()->toString().toStdString());
    }

    return references;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeInterface::hasAbstractionReference()
//-----------------------------------------------------------------------------
bool AbstractionTypeInterface::hasAbstractionReference(int const& typeIndex) const
{
    QSharedPointer<AbstractionType> selectedType = getAbstraction(typeIndex);
    if (selectedType && selectedType->getAbstractionRef())
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeInterface::itemCount()
//-----------------------------------------------------------------------------
int AbstractionTypeInterface::itemCount() const
{
    if (abstractions_)
    {
        return abstractions_->count();
    }
    else
    {
        return -1;
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeInterface::getItemNames()
//-----------------------------------------------------------------------------
std::vector<std::string> AbstractionTypeInterface::getItemNames() const
{
    std::vector<std::string> abstractionVLNVs;

    if (abstractions_)
    {
        for (auto abstraction : *abstractions_)
        {
            abstractionVLNVs.push_back(abstraction->getAbstractionRef()->toString().toStdString());
        }
    }

    return abstractionVLNVs;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeInterface::setAbstraction ()
//-----------------------------------------------------------------------------
bool AbstractionTypeInterface::setAbstraction(int const& abstractionIndex, std::string const& newVendor,
    std::string const& newLibrary, std::string const& newName, std::string const& newVersion)
{
    QSharedPointer<AbstractionType> selectedAbstractionType = getAbstraction(abstractionIndex);
    if (!selectedAbstractionType)
    {
        return false;
    }


    QSharedPointer<ConfigurableVLNVReference> newAbstractionReference(new ConfigurableVLNVReference());
    newAbstractionReference->setVendor(QString::fromStdString(newVendor));
    newAbstractionReference->setLibrary(QString::fromStdString(newLibrary));
    newAbstractionReference->setName(QString::fromStdString(newName));
    newAbstractionReference->setVersion(QString::fromStdString(newVersion));

    if (selectedAbstractionType->getAbstractionRef())
    {
        QSharedPointer<ConfigurableVLNVReference> oldAbstractionReference =
            selectedAbstractionType->getAbstractionRef();

        newAbstractionReference->setConfigurableElementValues(
            oldAbstractionReference->getConfigurableElementValues());
        newAbstractionReference->setType(oldAbstractionReference->getType());
    }
    else
    {
        newAbstractionReference->setType(VLNV::ABSTRACTIONDEFINITION);
    }

    selectedAbstractionType->setAbstractionRef(newAbstractionReference);
    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeInterface::getViewReferences()
//-----------------------------------------------------------------------------
std::vector<std::string> AbstractionTypeInterface::getViewReferences(int const& abstractionIndex) const
{
    std::vector<std::string> viewReferences;
    QSharedPointer<AbstractionType> selectedType = getAbstraction(abstractionIndex);
    if (selectedType && selectedType->getViewReferences())
    {
        for (auto view : *selectedType->getViewReferences())
        {
            viewReferences.push_back(view.toStdString());
        }
    }

    return viewReferences;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeInterface::getCombinedViews()
//-----------------------------------------------------------------------------
std::string AbstractionTypeInterface::getCombinedViews(int const& abstractionIndex) const
{
    QString combiViews;
    QSharedPointer<AbstractionType> selectedType = getAbstraction(abstractionIndex);
    if (selectedType && selectedType->getViewReferences())
    {
        combiViews = selectedType->getViewReferences()->join(VIEW_SEPARATOR);
    }

    return combiViews.toStdString();
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeInterface::setViewReferences()
//-----------------------------------------------------------------------------
bool AbstractionTypeInterface::setViewReferences(int const& abstractionIndex, std::vector<std::string> newViews)
{
    QSharedPointer<AbstractionType> selectedAbstractionType = getAbstraction(abstractionIndex);
    if (!selectedAbstractionType)
    {
        return false;
    }

    QSharedPointer<QStringList> viewReferences(new QStringList());
    for (auto view : newViews)
    {
        viewReferences->append(QString::fromStdString(view));
    }

    selectedAbstractionType->setViewReferences(viewReferences);
    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeInterface::getPhysicalPortsFromPortMaps()
//-----------------------------------------------------------------------------
std::vector<std::string> AbstractionTypeInterface::getPhysicalPortsFromPortMaps(int const& abstractionIndex) const
{
    std::vector<std::string> physicalPorts;

    QSharedPointer<AbstractionType> selectedAbstractionType = getAbstraction(abstractionIndex);
    if (selectedAbstractionType)
    {
        for (auto portName : selectedAbstractionType->getPhysicalPortNames())
        {
            physicalPorts.push_back(portName.toStdString());
        }
    }

    return physicalPorts;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeInterface::validateItems()
//-----------------------------------------------------------------------------
bool AbstractionTypeInterface::validateItems() const
{
    for (int i = 0; i < abstractions_->size(); ++i)
    {
        if (!hasValidAbstractionReference(i) || !hasValidViewReferences(i))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeInterface::hasValidAbstractionReference()
//-----------------------------------------------------------------------------
bool AbstractionTypeInterface::hasValidAbstractionReference(int const& typeIndex) const
{
    QSharedPointer<AbstractionType> selectedType = getAbstraction(typeIndex);
    if (!selectedType)
    {
        return false;
    }

    return validator_->hasValidAbstractionReference(selectedType);
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeInterface::hasValidViewReferences()
//-----------------------------------------------------------------------------
bool AbstractionTypeInterface::hasValidViewReferences(int const& typeIndex) const
{
    QSharedPointer<AbstractionType> selectedType = getAbstraction(typeIndex);
    if (!selectedType)
    {
        return false;
    }

    return validator_->hasValidViewReferences(selectedType, abstractions_);
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeInterface::addAbstraction()
//-----------------------------------------------------------------------------
void AbstractionTypeInterface::addAbstraction(int const& typeIndex)
{
    QSharedPointer<AbstractionType> newAbstraction(new AbstractionType());

    abstractions_->insert(typeIndex, newAbstraction);
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeInterface::addAbstractionType()
//-----------------------------------------------------------------------------
void AbstractionTypeInterface::addAbstractionType(std::string const& newVendor, std::string const& newLibrary,
    std::string const& newName, std::string const& newVersion) const
{
    QSharedPointer<ConfigurableVLNVReference> abstractionReference(new ConfigurableVLNVReference());
    abstractionReference->setType(VLNV::ABSTRACTIONDEFINITION);
    abstractionReference->setVendor(QString::fromStdString(newVendor));
    abstractionReference->setLibrary(QString::fromStdString(newLibrary));
    abstractionReference->setName(QString::fromStdString(newName));
    abstractionReference->setVersion(QString::fromStdString(newVersion));
    abstractionReference->setType(VLNV::ABSTRACTIONDEFINITION);

    QSharedPointer<AbstractionType> newAbstractionType(new AbstractionType());
    newAbstractionType->setAbstractionRef(abstractionReference);

    abstractions_->append(newAbstractionType);
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeInterface::removeAbstraction()
//-----------------------------------------------------------------------------
bool AbstractionTypeInterface::removeAbstraction(int const& typeIndex)
{
    if (typeIndex >= 0 && typeIndex < itemCount())
    {
        abstractions_->removeAt(typeIndex);
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeInterface::getPortMapInterface()
//-----------------------------------------------------------------------------
PortMapInterface* AbstractionTypeInterface::getPortMapInterface() const
{
    return portMapInterface_;
}
