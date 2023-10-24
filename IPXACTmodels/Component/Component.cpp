//-----------------------------------------------------------------------------
// File: Component.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 06.10.2015
//
// Description:
// Describes the ipxact:component element.
//-----------------------------------------------------------------------------

#include "Component.h"
#include "Model.h"

#include "BusInterface.h"
#include "Channel.h"
#include "RemapState.h"
#include "Mode.h"
#include "AddressSpace.h"
#include "MemoryMap.h"
#include "View.h"
#include "ComponentInstantiation.h"
#include "DesignInstantiation.h"
#include "DesignConfigurationInstantiation.h"
#include "Port.h"
#include "PowerDomain.h"
#include "ComponentGenerator.h"
#include "Choice.h"
#include "FileSet.h"
#include "Cpu.h"
#include "OtherClockDriver.h"
#include "IndirectInterface.h"
#include "ResetType.h"

#include "File.h"

#include <IPXACTmodels/kactusExtensions/ComProperty.h>
#include <IPXACTmodels/kactusExtensions/SystemView.h>
#include <IPXACTmodels/kactusExtensions/ComInterface.h>
#include <IPXACTmodels/kactusExtensions/ApiInterface.h>
#include <IPXACTmodels/kactusExtensions/FileDependency.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Group.h>

#include <IPXACTmodels/common/Parameter.h>

#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/utilities/Search.h>
#include <IPXACTmodels/utilities/Copy.h>

//-----------------------------------------------------------------------------
// Function: Component::Component()
//-----------------------------------------------------------------------------
Component::Component(const VLNV &vlnv, Document::Revision revision):
Document(vlnv, revision)
{

}

//-----------------------------------------------------------------------------
// Function: Component::Component()
//-----------------------------------------------------------------------------
Component::Component(const Component &other):
Document(other)
{
    copyPowerDomains(other);
    copyBusInterfaces(other);
    copyIndirectInterfaces(other);
    copyChannels(other);
    copyRemapStates(other);
    copyModes(other);
    copyAddressSpaces(other);
    copyMemoryMaps(other);
    copyModel(other);
    copyComponentGenerators(other);
    copyChoices(other);
    copyFileSets(other);
    copyCpus(other);
    copyOtherClockDrivers(other);
    copyResetTypes(other);
}

//-----------------------------------------------------------------------------
// Function: Component::operator=()
//-----------------------------------------------------------------------------
Component& Component::operator=( const Component& other)
{
    if (this != &other)
    {
        Document::operator=(other);

        pendingFileDependencies_ = other.pendingFileDependencies_;

        powerDomains_->clear();
        busInterfaces_->clear();
        indirectInterfaces_->clear();
        channels_->clear();
        remapStates_->clear();
        modes_->clear();
        addressSpaces_->clear();
        memoryMaps_->clear();
        model_.clear();

        componentGenerators_->clear();
        choices_->clear();
        fileSets_->clear();
        cpus_->clear();
        otherClockDrivers_->clear();
        resetTypes_->clear();

        copyPowerDomains(other);
        copyBusInterfaces(other);
        copyIndirectInterfaces(other);
        copyChannels(other);
        copyRemapStates(other);
        copyModes(other);
        copyAddressSpaces(other);
        copyMemoryMaps(other);
        copyModel(other);
        copyComponentGenerators(other);
        copyChoices(other);
        copyFileSets(other);
        copyCpus(other);
        copyOtherClockDrivers(other);
        copyResetTypes(other);
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: Component::~Component()
//-----------------------------------------------------------------------------
Component::~Component()
{

}

//-----------------------------------------------------------------------------
// Function: Component::clone()
//-----------------------------------------------------------------------------
QSharedPointer<Document> Component::clone() const 
{
	return QSharedPointer<Document>(new Component(*this));
}

//-----------------------------------------------------------------------------
// Function: Component::isBus()
//-----------------------------------------------------------------------------
bool Component::isBus() const
{
    return isChannel() || isBridge();
}

//-----------------------------------------------------------------------------
// Function: Component::isChannel()
//-----------------------------------------------------------------------------
bool Component::isChannel() const
{
    return !channels_->isEmpty();
}

//-----------------------------------------------------------------------------
// Function: Component::isBridge()
//-----------------------------------------------------------------------------
bool Component::isBridge() const
{
    return std::any_of(busInterfaces_->cbegin(), busInterfaces_->cend(),
        [](auto busif) { return busif->hasBridge(); });
}

//-----------------------------------------------------------------------------
// Function: Component::isCpu()
//-----------------------------------------------------------------------------
bool Component::isCpu() const
{
    return !cpus_->isEmpty();
}

//-----------------------------------------------------------------------------
// Function: Component::isHierarchical()
//-----------------------------------------------------------------------------
bool Component::isHierarchical() const
{
    return model_->hasHierView();
}

//-----------------------------------------------------------------------------
// Function: Component::getPowerDomains()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<PowerDomain> > > Component::getPowerDomains() const
{
    return powerDomains_;
}

//-----------------------------------------------------------------------------
// Function: Component::getBusInterfaces()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<BusInterface> > > Component::getBusInterfaces() const
{
    return busInterfaces_;
}

//-----------------------------------------------------------------------------
// Function: Component::setBusInterfaces()
//-----------------------------------------------------------------------------
void Component::setBusInterfaces(QSharedPointer<QList<QSharedPointer<BusInterface> > > newBusInterfaces)
{
    busInterfaces_ = newBusInterfaces;
}

//-----------------------------------------------------------------------------
// Function: Component::getInterfaceForPort()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> Component::getInterfaceForPort( const QString& portName ) const
{
    auto it = std::find_if(busInterfaces_->cbegin(), busInterfaces_->cend(),
        [&portName](auto busif) { return busif->containsMappedPhysicalPort(portName); });

    if (it == busInterfaces_->cend())
    {
        return nullptr;
    }

    return *it;
}

//-----------------------------------------------------------------------------
// Function: Component::getInterfacesUsedByPort()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<BusInterface> > > Component::getInterfacesUsedByPort(QString const& portName)
    const
{
    QSharedPointer<QList<QSharedPointer<BusInterface> > > interfaces (new QList<QSharedPointer<BusInterface> > ());

    for (QSharedPointer<BusInterface> busInterface : *busInterfaces_)
    {
        if (busInterface->containsMappedPhysicalPort(portName))
        {
            interfaces->append(busInterface);
        }
    }

    return interfaces;
}

//-----------------------------------------------------------------------------
// Function: Component::getIndirectInterfaces()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<IndirectInterface> > > Component::getIndirectInterfaces() const
{
    return indirectInterfaces_;
}

//-----------------------------------------------------------------------------
// Function: Component::getChannels()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Channel> > > Component::getChannels() const
{
    return channels_;
}

//-----------------------------------------------------------------------------
// Function: Component::setChannels()
//-----------------------------------------------------------------------------
void Component::setChannels(QSharedPointer<QList<QSharedPointer<Channel> > > newChannels)
{
    channels_->clear();
    channels_ = newChannels;
}

//-----------------------------------------------------------------------------
// Function: Component::getRemapStates()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<RemapState> > > Component::getRemapStates() const
{
    return remapStates_;
}

//-----------------------------------------------------------------------------
// Function: Component::getModes()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Mode> > > Component::getModes() const
{
    return modes_;
}

//-----------------------------------------------------------------------------
// Function: Component::setRemapStates()
//-----------------------------------------------------------------------------
void Component::setRemapStates(QSharedPointer<QList<QSharedPointer<RemapState> > > newRemapStates)
{
    remapStates_ = newRemapStates;
}

//-----------------------------------------------------------------------------
// Function: Component::getAddressSpaces()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<AddressSpace> > > Component::getAddressSpaces() const
{
    return addressSpaces_;
}

//-----------------------------------------------------------------------------
// Function: Component::setAddressSpaces()
//-----------------------------------------------------------------------------
void Component::setAddressSpaces(QSharedPointer<QList<QSharedPointer<AddressSpace> > > newAddressSpaces)
{
    addressSpaces_->clear();
    addressSpaces_ = newAddressSpaces;
}

//-----------------------------------------------------------------------------
// Function: Component::getMemoryMaps()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<MemoryMap> > > Component::getMemoryMaps() const
{
    return memoryMaps_;
}

//-----------------------------------------------------------------------------
// Function: Component::setMemoryMaps()
//-----------------------------------------------------------------------------
void Component::setMemoryMaps(QSharedPointer<QList<QSharedPointer<MemoryMap> > > newMemoryMaps)
{
    memoryMaps_->clear();
    memoryMaps_ = newMemoryMaps;
}

//-----------------------------------------------------------------------------
// Function: Component::getModel()
//-----------------------------------------------------------------------------
QSharedPointer<Model> Component::getModel() const
{
    return model_;
}

//-----------------------------------------------------------------------------
// Function: Component::setModel()
//-----------------------------------------------------------------------------
void Component::setModel(QSharedPointer<Model> newModel)
{
    model_ = newModel;
}

//-----------------------------------------------------------------------------
// Function: Component::getViews()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<View> > > Component::getViews() const
{
    return model_->getViews();
}

//-----------------------------------------------------------------------------
// Function: Component::setViews()
//-----------------------------------------------------------------------------
void Component::setViews(QSharedPointer<QList<QSharedPointer<View> > > newViews)
{
    model_->setViews(newViews);
}

//-----------------------------------------------------------------------------
// Function: Component::getComponentInstantiations()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<ComponentInstantiation> > > Component::getComponentInstantiations() const
{
    return model_->getComponentInstantiations();
}

//-----------------------------------------------------------------------------
// Function: Component::setComponentInstantiations()
//-----------------------------------------------------------------------------
void Component::setComponentInstantiations(
    QSharedPointer<QList<QSharedPointer<ComponentInstantiation> > > newInstantiation)
{
    model_->setComponentInstantiations(newInstantiation);
}

//-----------------------------------------------------------------------------
// Function: Component::getDesignInstantiations()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<DesignInstantiation> > > Component::getDesignInstantiations() const
{
    return model_->getDesignInstantiations();
}

//-----------------------------------------------------------------------------
// Function: Component::setDesignInstantiations()
//-----------------------------------------------------------------------------
void Component::setDesignInstantiations(
    QSharedPointer<QList<QSharedPointer<DesignInstantiation> > > newInstantiations)
{
    model_->setDesignInstantiations(newInstantiations);
}

//-----------------------------------------------------------------------------
// Function: Component::getDesignConfigurationInstantiations()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<DesignConfigurationInstantiation> > > Component::
    getDesignConfigurationInstantiations() const
{
    return model_->getDesignConfigurationInstantiations();
}

//-----------------------------------------------------------------------------
// Function: Component::setDesignConfigurationInstantiations()
//-----------------------------------------------------------------------------
void Component::setDesignConfigurationInstantiations(
    QSharedPointer<QList<QSharedPointer<DesignConfigurationInstantiation> > > newInstantiations)
{
    model_->setDesignConfigurationInstantiations(newInstantiations);
}

//-----------------------------------------------------------------------------
// Function: Component::getPorts()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Port> > > Component::getPorts() const
{
    return model_->getPorts();
}

//-----------------------------------------------------------------------------
// Function: Component::setPorts()
//-----------------------------------------------------------------------------
void Component::setPorts(QSharedPointer<QList<QSharedPointer<Port> > > newPorts)
{
    model_->setPorts(newPorts);
}

//-----------------------------------------------------------------------------
// Function: Component::getCompGenerators()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<ComponentGenerator> > > Component::getComponentGenerators() const
{
    return componentGenerators_;
}

//-----------------------------------------------------------------------------
// Function: Component::setCompGenerators()
//-----------------------------------------------------------------------------
void Component::setComponentGenerators(QSharedPointer<QList<QSharedPointer<ComponentGenerator> > > newGenerators)
{
    componentGenerators_->clear();
    componentGenerators_= newGenerators;
}

//-----------------------------------------------------------------------------
// Function: Component::getChoices()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Choice> > > Component::getChoices() const
{
    return choices_;
}

//-----------------------------------------------------------------------------
// Function: Component::setChoices()
//-----------------------------------------------------------------------------
void Component::setChoices(QSharedPointer<QList<QSharedPointer<Choice> > > newChoices)
{
    choices_ = newChoices;
}

//-----------------------------------------------------------------------------
// Function: Component::getFileSets()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<FileSet> > > Component::getFileSets() const
{
    return fileSets_;
}

//-----------------------------------------------------------------------------
// Function: Component::setFileSets()
//-----------------------------------------------------------------------------
void Component::setFileSets(QSharedPointer<QList<QSharedPointer<FileSet> > > newFileSets)
{
    fileSets_->clear();
    fileSets_ = newFileSets;
}

//-----------------------------------------------------------------------------
// Function: Component::getCpus()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Cpu> > >Component::getCpus() const
{
    return cpus_;
}

//-----------------------------------------------------------------------------
// Function: Component::setCpus()
//-----------------------------------------------------------------------------
void Component::setCpus(QSharedPointer<QList<QSharedPointer<Cpu> > > newCpus)
{
    cpus_->clear();
    cpus_ = newCpus;
}

//-----------------------------------------------------------------------------
// Function: Component::getOtherClockDrivers()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<OtherClockDriver> > > Component::getOtherClockDrivers() const
{
    return otherClockDrivers_;
}

//-----------------------------------------------------------------------------
// Function: Component::setOtherClockDrivers()
//-----------------------------------------------------------------------------
void Component::setOtherClockDrivers(
    QSharedPointer<QList<QSharedPointer<OtherClockDriver> > > newOtherClockDrivers)
{
    otherClockDrivers_->clear();
    otherClockDrivers_ = newOtherClockDrivers;
}

//-----------------------------------------------------------------------------
// Function: Component::getResetTypes()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<ResetType> > > Component::getResetTypes() const
{
    return resetTypes_;
}

//-----------------------------------------------------------------------------
// Function: Component::setResetTypes()
//-----------------------------------------------------------------------------
void Component::setResetTypes(QSharedPointer<QList<QSharedPointer<ResetType>>> newResetTypes)
{
    resetTypes_ = newResetTypes;
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::getSWProperties()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<ComProperty> > > Component::getSWProperties() const
{
    QList<QSharedPointer<VendorExtension> > swPropertiesExtensionList =
        getGroupedExtensionsByType(QStringLiteral("kactus2:properties"), QStringLiteral("kactus2:property"));

    QSharedPointer<QList<QSharedPointer<ComProperty> > > swProperties( new QList<QSharedPointer<ComProperty> > );
    for (QSharedPointer<VendorExtension> extension : swPropertiesExtensionList)
    {
        swProperties->append(extension.dynamicCast<ComProperty>());
    }

    return swProperties;
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::setSWProperties()
//-----------------------------------------------------------------------------
void Component::setSWProperties(QList<QSharedPointer<ComProperty> > newProperties)
{
    for (QSharedPointer<VendorExtension> extension : *getVendorExtensions())
    {
        if (extension->type() == QLatin1String("kactus2:properties"))
        {
            getVendorExtensions()->removeAll(extension);
            break;
        }
    }

    if (!newProperties.isEmpty())
    {
        QSharedPointer<Kactus2Group> newPropertiesGroup (new Kactus2Group(QStringLiteral("kactus2:properties")));
        for (QSharedPointer<ComProperty> swProperty : newProperties)
        {
            newPropertiesGroup->addToGroup(swProperty);
        }

        getVendorExtensions()->append(newPropertiesGroup);
    }
}

//-----------------------------------------------------------------------------
// Function: Component::getSystemViews()
//-----------------------------------------------------------------------------
QList<QSharedPointer<SystemView> > Component::getSystemViews() const
{
    QList<QSharedPointer<VendorExtension> > systemViewExtensions =
        getGroupedExtensionsByType(QStringLiteral("kactus2:systemViews"), QStringLiteral("kactus2:systemView"));

    QList<QSharedPointer<SystemView> > systemViews;
    for (QSharedPointer<VendorExtension> extension : systemViewExtensions)
    {
        systemViews.append(extension.dynamicCast<SystemView>());
    }

    return systemViews;
}

//-----------------------------------------------------------------------------
// Function: Component::setSystemViews()
//-----------------------------------------------------------------------------
void Component::setSystemViews(QList<QSharedPointer<SystemView> > newSystemViews)
{
    for (QSharedPointer<VendorExtension> extension : *getVendorExtensions())
    {
        if (extension->type() == QLatin1String("kactus2:systemViews"))
        {
            getVendorExtensions()->removeAll(extension);
            break;
        }
    }

    if (!newSystemViews.isEmpty())
    {
        QSharedPointer<Kactus2Group> newSystemViewGroup (new Kactus2Group(QStringLiteral("kactus2:systemViews")));
        for (QSharedPointer<SystemView> systemView : newSystemViews)
        {
            newSystemViewGroup->addToGroup(systemView);
        }

        getVendorExtensions()->append(newSystemViewGroup);
    }
}

//-----------------------------------------------------------------------------
// Function: Component::getComInterfaces()
//-----------------------------------------------------------------------------
QList<QSharedPointer<ComInterface> > Component::getComInterfaces() const
{
    QList<QSharedPointer<VendorExtension> > comInterfaceExtensions =
        getGroupedExtensionsByType(QStringLiteral("kactus2:comInterfaces"), QStringLiteral("kactus2:comInterface"));

    QList<QSharedPointer<ComInterface> > comInterfaces;
    for (QSharedPointer<VendorExtension> extension : comInterfaceExtensions)
    {
        comInterfaces.append(extension.dynamicCast<ComInterface>());
    }

    return comInterfaces;
}

//-----------------------------------------------------------------------------
// Function: Component::setComInterfaces()
//-----------------------------------------------------------------------------
void Component::setComInterfaces(QList<QSharedPointer<ComInterface> > newComInterfaces)
{
    for (QSharedPointer<VendorExtension> extension : *getVendorExtensions())
    {
        if (extension->type() == QLatin1String("kactus2:comInterfaces"))
        {
            getVendorExtensions()->removeAll(extension);
            break;
        }
    }

    if (!newComInterfaces.isEmpty())
    {
        QSharedPointer<Kactus2Group> newComInterfaceGroup (new Kactus2Group(QStringLiteral("kactus2:comInterfaces")));
        for (QSharedPointer<ComInterface> comInterface : newComInterfaces)
        {
            newComInterfaceGroup->addToGroup(comInterface);
        }

        getVendorExtensions()->append(newComInterfaceGroup);
    }
}

//-----------------------------------------------------------------------------
// Function: Component::getApiInterfaces()
//-----------------------------------------------------------------------------
QList<QSharedPointer<ApiInterface> > Component::getApiInterfaces() const
{
    QList<QSharedPointer<VendorExtension> > apiInterfaceExtensions =
        getGroupedExtensionsByType(QStringLiteral("kactus2:apiInterfaces"), QStringLiteral("kactus2:apiInterface"));

    QList<QSharedPointer<ApiInterface> > apiInterfaces;
    for (QSharedPointer<VendorExtension> extension : apiInterfaceExtensions)
    {
        apiInterfaces.append(extension.dynamicCast<ApiInterface>());
    }

    return apiInterfaces;
}

//-----------------------------------------------------------------------------
// Function: Component::setApiInterfaces()
//-----------------------------------------------------------------------------
void Component::setApiInterfaces(QList<QSharedPointer<ApiInterface> > newApiInterfaces)
{
    for (QSharedPointer<VendorExtension> extension : *getVendorExtensions())
    {
        if (extension->type() == QLatin1String("kactus2:apiInterfaces"))
        {
            getVendorExtensions()->removeAll(extension);
            break;
        }
    }

    if (!newApiInterfaces.isEmpty())
    {
        QSharedPointer<Kactus2Group> newApiInterfaceGroup (new Kactus2Group(QStringLiteral("kactus2:apiInterfaces")));
        for (QSharedPointer<ApiInterface> apiInterface : newApiInterfaces)
        {
            newApiInterfaceGroup->addToGroup(apiInterface);
        }
        getVendorExtensions()->append(newApiInterfaceGroup);
    }
}

//-----------------------------------------------------------------------------
// Function: Component::getFileDependencies()
//-----------------------------------------------------------------------------
QList<QSharedPointer<FileDependency> > Component::getFileDependencies() const
{
    QList<QSharedPointer<VendorExtension> > fileDependencyExtensions =
        getGroupedExtensionsByType(QStringLiteral("kactus2:fileDependencies"), QStringLiteral("kactus2:fileDependency"));

    QList<QSharedPointer<FileDependency> > fileDependencies;
    for (QSharedPointer<VendorExtension> extension : fileDependencyExtensions)
    {
        fileDependencies.append(extension.dynamicCast<FileDependency>());
    }

    return fileDependencies;
}

//-----------------------------------------------------------------------------
// Function: Component::setFileDependendencies()
//-----------------------------------------------------------------------------
void Component::setFileDependendencies(QList<QSharedPointer<FileDependency> > newFileDependencies)
{
    for (QSharedPointer<VendorExtension> extension : *getVendorExtensions())
    {
        if (extension->type() == QLatin1String("kactus2:fileDependencies"))
        {
            getVendorExtensions()->removeAll(extension);
            break;
        }
    }

    if (!newFileDependencies.isEmpty())
    {
        QSharedPointer<Kactus2Group> newFileDependencyGroup (new Kactus2Group(QStringLiteral("kactus2:fileDependencies")));
        for (QSharedPointer<FileDependency> fileDependency : newFileDependencies)
        {
            newFileDependencyGroup->addToGroup(fileDependency);
        }
        getVendorExtensions()->append(newFileDependencyGroup);
    }
}

//-----------------------------------------------------------------------------
// Function: Component::getPendingFileDependencies()
//-----------------------------------------------------------------------------
QList< QSharedPointer<FileDependency> > Component::getPendingFileDependencies() const
{
    return pendingFileDependencies_;
}

//-----------------------------------------------------------------------------
// Function: Component::setPendingFileDependencies()
//-----------------------------------------------------------------------------
void Component::setPendingFileDependencies(QList<QSharedPointer<FileDependency> > newFileDependencies)
{
    pendingFileDependencies_ = newFileDependencies;
}

//-----------------------------------------------------------------------------
// Function: Component::getSourceDirectories()
//-----------------------------------------------------------------------------
QStringList Component::getSourceDirectories() const
{
    QList<QSharedPointer<VendorExtension> > sourceDirectoryExtensions =
        getGroupedExtensionsByType(QStringLiteral("kactus2:sourceDirectories"), QStringLiteral("kactus2:sourceDirectory"));

    QStringList sourceDirections;
    for (QSharedPointer<VendorExtension> extension : sourceDirectoryExtensions)
    {
        QSharedPointer<Kactus2Value> sourceDirectory = extension.dynamicCast<Kactus2Value>();
        sourceDirections.append(sourceDirectory->value());
    }

    return sourceDirections;
}

//-----------------------------------------------------------------------------
// Function: Component::setSourceDirectories()
//-----------------------------------------------------------------------------
void Component::setSourceDirectories(QStringList const& sourceDirs)
{
    for (QSharedPointer<VendorExtension> extension : *getVendorExtensions())
    {
        if (extension->type() == QLatin1String("kactus2:sourceDirectories"))
        {
            getVendorExtensions()->removeAll(extension);
            break;
        }
    }

    if (!sourceDirs.isEmpty())
    {
        QSharedPointer<Kactus2Group> newSourceDirectoryGroup (new Kactus2Group(QStringLiteral("kactus2:sourceDirectories")));
        for (QString const& sourceDirectory : sourceDirs)
        {
            QSharedPointer<Kactus2Value> directoryExtension (
                new Kactus2Value(QStringLiteral("kactus2:sourceDirectory"), sourceDirectory));
            newSourceDirectoryGroup->addToGroup(directoryExtension);
        }
        getVendorExtensions()->append(newSourceDirectoryGroup);
    }
}

//-----------------------------------------------------------------------------
// Function: Component::getAuthor()
//-----------------------------------------------------------------------------
QString Component::getAuthor() const
{
    for (QSharedPointer<VendorExtension> extension : *getVendorExtensions())
    {
        if (extension->type() == QLatin1String("kactus2:author"))
        {
            QSharedPointer<Kactus2Value> authorExtension = extension.dynamicCast<Kactus2Value>();
            return authorExtension->value();
        }
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: Component::setAuthor()
//-----------------------------------------------------------------------------
void Component::setAuthor(QString const& author)
{
    for (QSharedPointer<VendorExtension> extension : *getVendorExtensions())
    {
        if (extension->type() == QLatin1String("kactus2:author"))
        {
            getVendorExtensions()->removeAll(extension);
        }
    }

    if (!author.isEmpty())
    {
        QSharedPointer<Kactus2Value> authorValue (new Kactus2Value(QStringLiteral("kactus2:author"), author));
        getVendorExtensions()->append(authorValue);
    }
}

//-----------------------------------------------------------------------------
// Function: Component::getDependentFiles()
//-----------------------------------------------------------------------------
QStringList Component::getDependentFiles() const
{
    QStringList files;

    std::for_each(fileSets_->cbegin(), fileSets_->cend(), 
        [&files](auto& item) { files.append(item->getFilePaths()); });

    std::for_each(componentGenerators_->cbegin(), componentGenerators_->cend(), 
        [&files](auto& item) { files.append(item->getGeneratorExe()); });

    return files;
}

//-----------------------------------------------------------------------------
// Function: Component::getDependentVLNVs()
//-----------------------------------------------------------------------------
QList<VLNV> Component::getDependentVLNVs() const
{
    QList<VLNV> vlnvList;

    std::for_each(busInterfaces_->cbegin(), busInterfaces_->cend(),
        [&vlnvList](auto const& busif)
        {
            vlnvList.append(busif->getBusType());

            std::for_each(busif->getAbstractionTypes()->cbegin(), busif->getAbstractionTypes()->cend(),
                [&vlnvList](auto const& abstractionType) {
                    vlnvList.append(*abstractionType->getAbstractionRef());
                });
        });

    vlnvList.append(model_->getViewReferences());

    const auto systemViews = getSystemViews();
    std::for_each(systemViews.cbegin(), systemViews.cend(), [&vlnvList](auto const& view)
        {
            vlnvList.append(view->getHierarchyRef());
        });


    const auto comInterfaces = getComInterfaces();
    std::for_each(comInterfaces.cbegin(), comInterfaces.cend(),
        [&vlnvList](auto const& comIf)
        {
            vlnvList.append(comIf->getComType());
        });

    const auto  apiInterfaces = getApiInterfaces();

    std::for_each(apiInterfaces.cbegin(), apiInterfaces.cend(),
        [&vlnvList](auto const& apiIf)
        {
            vlnvList.append(apiIf->getApiType());
        });

    auto it = std::remove_if(vlnvList.begin(), vlnvList.end(), [](auto& vlnv) { return vlnv.isValid() == false; });
    vlnvList.erase(it, vlnvList.end());

    return vlnvList;
}

//-----------------------------------------------------------------------------
// Function: component::getPowerDomainNames()
//-----------------------------------------------------------------------------
QStringList Component::getPowerDomainNames() const
{
    return Search::getNames(powerDomains_);
}

//-----------------------------------------------------------------------------
// Function: component::getRemapStateNames()
//-----------------------------------------------------------------------------
QStringList Component::getRemapStateNames() const
{
    return Search::getNames(remapStates_);
}

//-----------------------------------------------------------------------------
// Function: Component::getMemoryMapNames()
//-----------------------------------------------------------------------------
QStringList Component::getMemoryMapNames() const
{
    return Search::getNames(memoryMaps_);
}

//-----------------------------------------------------------------------------
// Function: Component::hasViews()
//-----------------------------------------------------------------------------
bool Component::hasViews() const
{
    return model_->hasViews();
}

//-----------------------------------------------------------------------------
// Function: Component::hasView()
//-----------------------------------------------------------------------------
bool Component::hasView( const QString& viewName ) const
{
    const auto views = getViews();
    return std::any_of(views->cbegin(), views->cend(), [&viewName](auto& view)
        { return view->name() == viewName; });
}

//-----------------------------------------------------------------------------
// Function: Component::getViewNames()
//-----------------------------------------------------------------------------
QStringList Component::getViewNames() const
{
    return model_->getViewNames();
}

//-----------------------------------------------------------------------------
// Function: Component::getCpuNames()
//-----------------------------------------------------------------------------
QStringList Component::getCpuNames() const
{
    return Search::getNames(cpus_);
}

//-----------------------------------------------------------------------------
// Function: Component::getHierViews()
//-----------------------------------------------------------------------------
QStringList Component::getHierViews() const
{
    return model_->getHierViews();
}

//-----------------------------------------------------------------------------
// Function: Component::getFlatViews()
//-----------------------------------------------------------------------------
QStringList Component::getFlatViews() const
{
    return model_->getFlatViews();
}

//-----------------------------------------------------------------------------
// Function: Component::getHierRef()
//-----------------------------------------------------------------------------
VLNV Component::getHierRef(const QString viewName) const
{
    return model_->getHierRef(viewName);
}

//-----------------------------------------------------------------------------
// Function: Component::getHierRefs()
//-----------------------------------------------------------------------------
QList<VLNV> Component::getHierRefs() const
{
    return model_->getHierarchyRefs();

    QList<VLNV> list = model_->getHierarchyRefs();

    for (QSharedPointer<SystemView> sysView : getSystemViews())
    {
        if (sysView->getHierarchyRef().isValid())
        {
            list.append(sysView->getHierarchyRef());
        }
    }
    return list;
}

//-----------------------------------------------------------------------------
// Function: Component::hasSystemViews()
//-----------------------------------------------------------------------------
bool Component::hasSystemViews() const
{
    return !getSystemViews().isEmpty();
}

//-----------------------------------------------------------------------------
// Function: Component::hasSystemView()
//-----------------------------------------------------------------------------
bool Component::hasSystemView( const QString& viewName ) const
{
    return findSystemView(viewName) != nullptr;
}

//-----------------------------------------------------------------------------
// Function: Component::getSystemViewNames()
//-----------------------------------------------------------------------------
QStringList Component::getSystemViewNames() const
{
    return Search::getNames(getSystemViews());
}

//-----------------------------------------------------------------------------
// Function: Component::findSystemView()
//-----------------------------------------------------------------------------
QSharedPointer<SystemView> Component::findSystemView(QString const& name) const
{
    return Search::findByName(name, getSystemViews());
}

//-----------------------------------------------------------------------------
// Function: Component::findSystemView()
//-----------------------------------------------------------------------------
QSharedPointer<SystemView> Component::findSystemView( const VLNV& hierRef ) const
{
    const auto systemViews = getSystemViews();
    auto it = std::find_if(systemViews.cbegin(), systemViews.cend(),
        [&hierRef](auto const& view) { return view->getHierarchyRef() == hierRef; });

    if (it == systemViews.cend())
    {
        return nullptr;
    }

    return *it;
}

//-----------------------------------------------------------------------------
// Function: Component::getHierSystemRef()
//-----------------------------------------------------------------------------
VLNV Component::getHierSystemRef(QString const& viewName) const
{
    const auto systemViews = getSystemViews();
    auto it = std::find_if(systemViews.cbegin(), systemViews.cend(),
        [&viewName](auto const& view)
        { return view->name() == viewName || (viewName.isEmpty() && view->getHierarchyRef().isValid()); });

    if (it == systemViews.cend())
    {
        return VLNV();
    }

    return (*it)->getHierarchyRef();
}

//-----------------------------------------------------------------------------
// Function: Component::getHierSystemRefs()
//-----------------------------------------------------------------------------
QList<VLNV> Component::getHierSystemRefs() const
{
    QList<VLNV> refs;

    for (QSharedPointer<SystemView> view : getSystemViews())
    {
        if (view->getHierarchyRef().isValid())
        {
            refs.append(view->getHierarchyRef());
        }
    }
    return refs;
}

//-----------------------------------------------------------------------------
// Function: Component::getFileSetNames()
//-----------------------------------------------------------------------------
QStringList Component::getFileSetNames() const
{
    return Search::getNames(fileSets_);
}

//-----------------------------------------------------------------------------
// Function: Component::getFileSet()
//-----------------------------------------------------------------------------
QSharedPointer<FileSet> Component::getFileSet( const QString& name ) const
{
    return Search::findByName(name, *fileSets_);
}

//-----------------------------------------------------------------------------
// Function: Component::hasFileSet()
//-----------------------------------------------------------------------------
bool Component::hasFileSet( const QString& fileSetName ) const
{
    return std::any_of(fileSets_->cbegin(), fileSets_->cend(),
        [&fileSetName](auto const& fileset) { return fileset->name() == fileSetName; });
}

//-----------------------------------------------------------------------------
// Function: Component::hasFileSets()
//-----------------------------------------------------------------------------
bool Component::hasFileSets() const
{
    return !fileSets_->isEmpty();
}

//-----------------------------------------------------------------------------
// Function: Component::getFiles()
//-----------------------------------------------------------------------------
QList<QSharedPointer<File> > Component::getFiles(QString const& fileName) const
{
    QList<QSharedPointer<File> > fileList;

    for (QSharedPointer<FileSet> fileSet : *fileSets_)
    {
        for (QSharedPointer<File> file : *fileSet->getFiles())
        {
            if (file->name() == fileName)
            {
                fileList.append(file);
            }
        }
    }

    return fileList;
}

//-----------------------------------------------------------------------------
// Function: Component::findFilesByFileType()
//-----------------------------------------------------------------------------
QStringList Component::findFilesByFileType( const QString& fileType ) const
{
    QStringList files;

    std::for_each(fileSets_->cbegin(), fileSets_->cend(), 
        [&fileType, &files](auto const& fileSet){ files.append(fileSet->findFilesByFileType(fileType));  });

    return files;
}

//-----------------------------------------------------------------------------
// Function: Component::findFileSetById()
//-----------------------------------------------------------------------------
QSharedPointer<FileSet> Component::findFileSetById(QString const& id ) const
{
    auto it = std::find_if(fileSets_->cbegin(), fileSets_->cend(),
        [&id](auto const& fileSet){ return fileSet->getFileSetId() == id; });

    if (it == fileSets_->cend())
    {
        return nullptr;
    }

    return *it;
}

//-----------------------------------------------------------------------------
// Function: Component::getFilesFromFileSets()
//-----------------------------------------------------------------------------
QStringList Component::getFilesFromFileSets( const QStringList& fileSetNames, const QStringList& fileTypes ) const
{
    QStringList files;

    for (QSharedPointer<FileSet> fileSet : *fileSets_)
    {
        if (fileSetNames.contains(fileSet->name()))
        {
            for (QSharedPointer<File> file : *fileSet->getFiles())
            {
                if (file->matchesFileType(fileTypes))
                {
                    files.append(file->name());
                }
            }
        }
    }

    return files;
}

//-----------------------------------------------------------------------------
// Function: Component::hasFile()
//-----------------------------------------------------------------------------
bool Component::hasFile( const QString& fileName ) const
{
    return std::any_of(fileSets_->cbegin(), fileSets_->cend(),
        [&fileName](auto const& fileSet) { return fileSet->contains(fileName); });
}

//-----------------------------------------------------------------------------
// Function: Component::getBusInterfaceNames()
//-----------------------------------------------------------------------------
QStringList Component::getBusInterfaceNames() const
{
    return Search::getNames(busInterfaces_);
}

//-----------------------------------------------------------------------------
// Function: Component::getBusInterface()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> Component::getBusInterface(QString const& name ) const
{
    return Search::findByName(name, *busInterfaces_);
}

//-----------------------------------------------------------------------------
// Function: Component::getSlaveInterfaces()
//-----------------------------------------------------------------------------
QStringList Component::getSlaveInterfaces( const QString& memoryMap ) const
{
    QStringList names;

    for (const QSharedPointer<BusInterface> busif : *busInterfaces_)
    {
        if (busif->getInterfaceMode() == General::SLAVE && busif->getMemoryMapRef() == memoryMap)
        {
            names.append(busif->name());
        }
    }

    return names;
}

//-----------------------------------------------------------------------------
// Function: Component::getMasterInterfaces()
//-----------------------------------------------------------------------------
QStringList Component::getMasterInterfaces( const QString& addressSpace ) const
{
    QStringList names;

    for (const QSharedPointer<BusInterface> busif : *busInterfaces_)
    {
        if ((busif->getInterfaceMode() == General::MASTER || busif->getInterfaceMode() == General::MIRRORED_MASTER)
            && (busif->getAddressSpaceRef() == addressSpace))
        {
            names.append(busif->name());
        }
    }

    return names;
}

//-----------------------------------------------------------------------------
// Function: Component::getMasterInterfaces()
//-----------------------------------------------------------------------------
QStringList Component::getMasterInterfaces() const
{
    QStringList names;
    for (QSharedPointer<const BusInterface> busif : *busInterfaces_)
    {
        if (busif->getInterfaceMode() == General::MASTER || busif->getInterfaceMode() == General::MIRRORED_MASTER)
        {
            names.append(busif->name());
        }
    }
    return names;
}

//-----------------------------------------------------------------------------
// Function: Component::hasInterfaces()
//-----------------------------------------------------------------------------
bool Component::hasInterfaces() const
{
    return !busInterfaces_->isEmpty();
}

//-----------------------------------------------------------------------------
// Function: Component::hasInterface()
//-----------------------------------------------------------------------------
bool Component::hasInterface( const QString& interfaceName ) const
{
    return Search::findByName(interfaceName, *busInterfaces_).isNull() == false;
}

//-----------------------------------------------------------------------------
// Function: Component::getComInterface()
//-----------------------------------------------------------------------------
QSharedPointer<ComInterface> Component::getComInterface( QString const& name )
{
    return Search::findByName(name, getComInterfaces());
}

//-----------------------------------------------------------------------------
// Function: Component::getApiInterface()
//-----------------------------------------------------------------------------
QSharedPointer<ApiInterface> Component::getApiInterface( QString const& name )
{
    return Search::findByName(name, getApiInterfaces());
}

//-----------------------------------------------------------------------------
// Function: Component::getPort()
//-----------------------------------------------------------------------------
QSharedPointer<Port> Component::getPort( const QString& name ) const
{
    return model_->getPort(name);
}

//-----------------------------------------------------------------------------
// Function: Component::getPorts()
//-----------------------------------------------------------------------------
QList<QSharedPointer<Port> > Component::getPortsMappedInInterface(const QString& interfaceName) const
{
    QList<QSharedPointer<Port> > ports;

    auto busif = getBusInterface(interfaceName);

    if ( busif.isNull())
    {
        return ports;
    }

    QStringList portNames = busif->getAllMappedPhysicalPorts();
    
    for (QSharedPointer<Port> port : *getPorts())
    {
        if (portNames.contains(port->name()))
        {
            ports.append(port);
        }
    }

    return ports;
}

//-----------------------------------------------------------------------------
// Function: Component::hasPort()
//-----------------------------------------------------------------------------
bool Component::hasPort( const QString& name ) const
{
    return model_->hasPort(name);
}

//-----------------------------------------------------------------------------
// Function: Component::getPortNames()
//-----------------------------------------------------------------------------
QStringList Component::getPortNames() const
{
    return model_->getPortNames();
}

//-----------------------------------------------------------------------------
// Function: Component::hasPorts()
//-----------------------------------------------------------------------------
bool Component::hasPorts() const
{
    return model_->hasPorts();
}

//-----------------------------------------------------------------------------
// Function: Component::getAddressSpaceNames()
//-----------------------------------------------------------------------------
QStringList Component::getAddressSpaceNames() const 
{
    return Search::getNames(addressSpaces_);
}

//-----------------------------------------------------------------------------
// Function: Component::hasLocalMemoryMaps()
//-----------------------------------------------------------------------------
bool Component::hasLocalMemoryMaps() const
{
    return std::any_of(addressSpaces_->cbegin(), addressSpaces_->cend(),
        [](auto const& addrSpace) { return addrSpace->hasLocalMemoryMap(); });
}

//-----------------------------------------------------------------------------
// Function: Component::getDependentDirs()
//-----------------------------------------------------------------------------
QStringList Component::getDependentDirs() const
{
    QStringList dirs;

    for (QSharedPointer<FileSet> fileSet : *fileSets_)
    {
        dirs.append(*fileSet->getDependencies());
    }

    return dirs;
}

//-----------------------------------------------------------------------------
// Function: Component::copyPowerDomains()
//-----------------------------------------------------------------------------
void Component::copyPowerDomains(const Component& other) const
{
    Copy::copyList(other.powerDomains_, powerDomains_);
}

//-----------------------------------------------------------------------------
// Function: Component::copyBusInterfaces_()
//-----------------------------------------------------------------------------
void Component::copyBusInterfaces(const Component& other) const
{
    Copy::copyList(other.busInterfaces_, busInterfaces_);
}

//-----------------------------------------------------------------------------
// Function: Component::copyIndirectInterfaces()
//-----------------------------------------------------------------------------
void Component::copyIndirectInterfaces(Component const& other) const
{
    Copy::copyList(other.indirectInterfaces_, indirectInterfaces_);
}

//-----------------------------------------------------------------------------
// Function: Component::copyChannels()
//-----------------------------------------------------------------------------
void Component::copyChannels(const Component& other) const
{
    Copy::copyList(other.channels_, channels_);
}

//-----------------------------------------------------------------------------
// Function: Component::copyRemapStates()
//-----------------------------------------------------------------------------
void Component::copyRemapStates(const Component& other) const
{
    Copy::copyList(other.remapStates_, remapStates_);
}

//-----------------------------------------------------------------------------
// Function: Component::copyModes()
//-----------------------------------------------------------------------------
void Component::copyModes(const Component& other) const
{
    Copy::copyList(other.modes_, modes_);
}

//-----------------------------------------------------------------------------
// Function: Component::copyAddressSpaces()
//-----------------------------------------------------------------------------
void Component::copyAddressSpaces(const Component& other) const
{
    Copy::copyList(other.addressSpaces_, addressSpaces_);
}

//-----------------------------------------------------------------------------
// Function: Component::copyMemoryMaps()
//-----------------------------------------------------------------------------
void Component::copyMemoryMaps(const Component& other) const
{
    Copy::copyList(other.memoryMaps_, memoryMaps_);
}

//-----------------------------------------------------------------------------
// Function: Component::copyModel()
//-----------------------------------------------------------------------------
void Component::copyModel(const Component& other)
{
    if (other.model_)
    {
        model_ = QSharedPointer<Model> (new Model(*other.model_));
    }
}

//-----------------------------------------------------------------------------
// Function: Component::copyComponentGenerators()
//-----------------------------------------------------------------------------
void Component::copyComponentGenerators(const Component& other) const
{
    Copy::copyList(other.componentGenerators_, componentGenerators_);
}

//-----------------------------------------------------------------------------
// Function: Component::copyChoices()
//-----------------------------------------------------------------------------
void Component::copyChoices(const Component& other) const
{
    Copy::copyList(other.choices_, choices_);
}

//-----------------------------------------------------------------------------
// Function: Component::copyFileSets()
//-----------------------------------------------------------------------------
void Component::copyFileSets(const Component& other) const
{
    Copy::copyList(other.fileSets_, fileSets_);
}

//-----------------------------------------------------------------------------
// Function: Component::copyCpus()
//-----------------------------------------------------------------------------
void Component::copyCpus(const Component& other) const
{
    Copy::copyList(other.cpus_, cpus_);
}

//-----------------------------------------------------------------------------
// Function: Component::copyOtherClockDrivers()
//-----------------------------------------------------------------------------
void Component::copyOtherClockDrivers(const Component& other) const
{
    Copy::copyList(other.otherClockDrivers_, otherClockDrivers_);
}

//-----------------------------------------------------------------------------
// Function: Component::copyResetTypes()
//-----------------------------------------------------------------------------
void Component::copyResetTypes(const Component& other) const
{
    Copy::copyList(other.resetTypes_, resetTypes_);
}
