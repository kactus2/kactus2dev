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
#include "AddressSpace.h"
#include "MemoryMap.h"
#include "View.h"
#include "ComponentInstantiation.h"
#include "DesignInstantiation.h"
#include "DesignConfigurationInstantiation.h"
#include "Port.h"
#include "ComponentGenerator.h"
#include "Choice.h"
#include "FileSet.h"
#include "Cpu.h"
#include "OtherClockDriver.h"

#include "File.h"

#include <IPXACTmodels/kactusExtensions/SWView.h>
#include <IPXACTmodels/kactusExtensions/ComProperty.h>
#include <IPXACTmodels/kactusExtensions/SystemView.h>
#include <IPXACTmodels/kactusExtensions/ComInterface.h>
#include <IPXACTmodels/kactusExtensions/ApiInterface.h>
#include <IPXACTmodels/kactusExtensions/FileDependency.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Group.h>

#include <IPXACTmodels/common/Parameter.h>

#include <IPXACTmodels/common/VLNV.h>

//-----------------------------------------------------------------------------
// Function: Component::Component()
//-----------------------------------------------------------------------------
Component::Component(const VLNV &vlnv):
Document(vlnv),
busInterfaces_(new QList<QSharedPointer<BusInterface> > ()),
channels_(new QList<QSharedPointer<Channel> > ()),
remapStates_(new QList<QSharedPointer<RemapState> >()),
addressSpaces_(new QList<QSharedPointer<AddressSpace> > ()),
memoryMaps_(new QList<QSharedPointer<MemoryMap> > ()),
model_(new Model()),
componentGenerators_(new QList<QSharedPointer<ComponentGenerator> > ()),
choices_(new QList<QSharedPointer<Choice> >()),
fileSets_(new QList<QSharedPointer<FileSet> > ()),
cpus_(new QList<QSharedPointer<Cpu> > ()),
otherClockDrivers_(new QList<QSharedPointer<OtherClockDriver> > ()),
pendingFileDependencies_()
{

}

//-----------------------------------------------------------------------------
// Function: Component::Component()
//-----------------------------------------------------------------------------
Component::Component() :
Document(),
busInterfaces_(new QList<QSharedPointer<BusInterface> > ()),
channels_(new QList<QSharedPointer<Channel> > ()),
remapStates_(new QList<QSharedPointer<RemapState> >()),
addressSpaces_(new QList<QSharedPointer<AddressSpace> > ()),
memoryMaps_(new QList<QSharedPointer<MemoryMap> > ()),
model_(new Model()),
componentGenerators_(new QList<QSharedPointer<ComponentGenerator> > ()),
choices_(new QList<QSharedPointer<Choice> >()),
fileSets_(new QList<QSharedPointer<FileSet> > ()),
cpus_(new QList<QSharedPointer<Cpu> > ()),
otherClockDrivers_(new QList<QSharedPointer<OtherClockDriver> > ()),
pendingFileDependencies_()
{

}

//-----------------------------------------------------------------------------
// Function: Component::Component()
//-----------------------------------------------------------------------------
Component::Component(const Component &other):
Document(other),
busInterfaces_(new QList<QSharedPointer<BusInterface> > ()),
channels_(new QList<QSharedPointer<Channel> > ()),
remapStates_(new QList<QSharedPointer<RemapState> >()),
addressSpaces_(new QList<QSharedPointer<AddressSpace> >()),
memoryMaps_(new QList<QSharedPointer<MemoryMap> > ()),
model_(),
componentGenerators_(new QList<QSharedPointer<ComponentGenerator> > ()),
choices_(new QList<QSharedPointer<Choice> >()),
fileSets_(new QList<QSharedPointer<FileSet> > ()),
cpus_(new QList<QSharedPointer<Cpu> > ()),
otherClockDrivers_(new QList<QSharedPointer<OtherClockDriver> > ()),
pendingFileDependencies_(other.pendingFileDependencies_)
{
    copyBusInterfaces(other);
    copyChannels(other);
    copyRemapStates(other);
    copyAddressSpaces(other);
    copyMemoryMaps(other);
    copyModel(other);
    copyComponentGenerators(other);
    copyChoices(other);
    copyFileSets(other);
    copyCpus(other);
    copyOtherClockDrivers(other);
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

        busInterfaces_->clear();
        channels_->clear();
        remapStates_->clear();
        addressSpaces_->clear();
        memoryMaps_->clear();
        model_.clear();

        componentGenerators_->clear();
        choices_->clear();
        fileSets_->clear();
        cpus_->clear();
        otherClockDrivers_->clear();
        
        copyBusInterfaces(other);
        copyChannels(other);
        copyRemapStates(other);
        copyAddressSpaces(other);
        copyMemoryMaps(other);
        copyModel(other);
        copyComponentGenerators(other);
        copyChoices(other);
        copyFileSets(other);
        copyCpus(other);
        copyOtherClockDrivers(other);
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: Component::~Component()
//-----------------------------------------------------------------------------
Component::~Component()
{
    busInterfaces_.clear();
    channels_.clear();
    remapStates_.clear();
    addressSpaces_.clear();
    memoryMaps_.clear();
    model_.clear();
    componentGenerators_.clear();
    choices_.clear();
    fileSets_.clear();
    cpus_.clear();
    otherClockDrivers_.clear();
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
    if (!channels_->isEmpty())
    {
        return true;
    }

    foreach (QSharedPointer<BusInterface> busif, *busInterfaces_)
    {
        if (busif->hasBridge())
        {
            return true;
        }
    }

    return false;
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
    foreach (QSharedPointer<BusInterface> busif, *busInterfaces_)
    {
        if (busif->hasBridge())
        {
            return true;
        }
    }
    return false;
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
// Function: Component::isHierarchicalSW()
//-----------------------------------------------------------------------------
bool Component::isHierarchicalSW() const
{
    foreach (QSharedPointer<SWView> view, getSWViews())
    {
        if (view->getHierarchyRef().isValid())
        {
            return true;
        }
    }

    return false;
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
	// Search all ports.
	foreach (QSharedPointer<BusInterface> busif, *busInterfaces_)
	{
		// If the interface contains the port.
		if (busif->hasPhysicalPort(portName))
		{
			return busif;
		}
	}

	return QSharedPointer<BusInterface>();
}

//-----------------------------------------------------------------------------
// Function: Component::getInterfacesUsedByPort()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<BusInterface> > > Component::getInterfacesUsedByPort(QString const& portName)
    const
{
    QSharedPointer<QList<QSharedPointer<BusInterface> > > interfaces (new QList<QSharedPointer<BusInterface> > ());

    foreach (QSharedPointer<BusInterface> busInterface, *busInterfaces_)
    {
        if (busInterface->hasPhysicalPort(portName))
        {
            interfaces->append(busInterface);
        }
    }

    return interfaces;
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
// Function: Component::getSWViews()
//-----------------------------------------------------------------------------
QList<QSharedPointer<SWView> > Component::getSWViews() const
{
    QList<QSharedPointer<VendorExtension> > swViewExtensions =
        getGroupedExtensionsByType("kactus2:swViews", "kactus2:swView");

    QList<QSharedPointer<SWView> > swViews;
    foreach (QSharedPointer<VendorExtension> extension, swViewExtensions)
    {
        swViews.append(extension.dynamicCast<SWView>());
    }

    return swViews;
}

//-----------------------------------------------------------------------------
// Function: Component::setSWViews()
//-----------------------------------------------------------------------------
void Component::setSWViews(QList<QSharedPointer<SWView> > newSWViews)
{
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:swViews")
        {
            getVendorExtensions()->removeAll(extension);
            break;
        }
    }

    if (!newSWViews.isEmpty())
    {
        QSharedPointer<Kactus2Group> newSWViewGroup (new Kactus2Group("kactus2:swViews"));
        foreach (QSharedPointer<SWView> swView, newSWViews)
        {
            newSWViewGroup->addToGroup(swView);
        }

        getVendorExtensions()->append(newSWViewGroup);
    }
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::getSWProperties()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<ComProperty> > > Component::getSWProperties() const
{
    QList<QSharedPointer<VendorExtension> > swPropertiesExtensionList =
        getGroupedExtensionsByType("kactus2:properties", "kactus2:property");

    QSharedPointer<QList<QSharedPointer<ComProperty> > > swProperties( new QList<QSharedPointer<ComProperty> > );
    foreach (QSharedPointer<VendorExtension> extension, swPropertiesExtensionList)
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
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:properties")
        {
            getVendorExtensions()->removeAll(extension);
            break;
        }
    }

    if (!newProperties.isEmpty())
    {
        QSharedPointer<Kactus2Group> newPropertiesGroup (new Kactus2Group("kactus2:properties"));
        foreach (QSharedPointer<ComProperty> swProperty, newProperties)
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
        getGroupedExtensionsByType("kactus2:systemViews", "kactus2:systemView");

    QList<QSharedPointer<SystemView> > systemViews;
    foreach (QSharedPointer<VendorExtension> extension, systemViewExtensions)
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
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:systemViews")
        {
            getVendorExtensions()->removeAll(extension);
            break;
        }
    }

    if (!newSystemViews.isEmpty())
    {
        QSharedPointer<Kactus2Group> newSystemViewGroup (new Kactus2Group("kactus2:systemViews"));
        foreach (QSharedPointer<SystemView> systemView, newSystemViews)
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
        getGroupedExtensionsByType("kactus2:comInterfaces", "kactus2:comInterface");

    QList<QSharedPointer<ComInterface> > comInterfaces;
    foreach (QSharedPointer<VendorExtension> extension, comInterfaceExtensions)
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
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:comInterfaces")
        {
            getVendorExtensions()->removeAll(extension);
            break;
        }
    }

    if (!newComInterfaces.isEmpty())
    {
        QSharedPointer<Kactus2Group> newComInterfaceGroup (new Kactus2Group("kactus2:comInterfaces"));
        foreach (QSharedPointer<ComInterface> comInterface, newComInterfaces)
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
        getGroupedExtensionsByType("kactus2:apiInterfaces", "kactus2:apiInterface");

    QList<QSharedPointer<ApiInterface> > apiInterfaces;
    foreach (QSharedPointer<VendorExtension> extension, apiInterfaceExtensions)
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
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:apiInterfaces")
        {
            getVendorExtensions()->removeAll(extension);
            break;
        }
    }

    if (!newApiInterfaces.isEmpty())
    {
        QSharedPointer<Kactus2Group> newApiInterfaceGroup (new Kactus2Group("kactus2:apiInterfaces"));
        foreach (QSharedPointer<ApiInterface> apiInterface, newApiInterfaces)
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
        getGroupedExtensionsByType("kactus2:fileDependencies", "kactus2:fileDependency");

    QList<QSharedPointer<FileDependency> > fileDependencies;
    foreach (QSharedPointer<VendorExtension> extension, fileDependencyExtensions)
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
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:fileDependencies")
        {
            getVendorExtensions()->removeAll(extension);
            break;
        }
    }

    if (!newFileDependencies.isEmpty())
    {
        QSharedPointer<Kactus2Group> newFileDependencyGroup (new Kactus2Group("kactus2:fileDependencies"));
        foreach (QSharedPointer<FileDependency> fileDependency, newFileDependencies)
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
        getGroupedExtensionsByType("kactus2:sourceDirectories", "kactus2:sourceDirectory");

    QStringList sourceDirections;
    foreach (QSharedPointer<VendorExtension> extension, sourceDirectoryExtensions)
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
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:sourceDirectories")
        {
            getVendorExtensions()->removeAll(extension);
            break;
        }
    }

    if (!sourceDirs.isEmpty())
    {
        QSharedPointer<Kactus2Group> newSourceDirectoryGroup (new Kactus2Group("kactus2:sourceDirectories"));
        foreach (QString sourceDirectory, sourceDirs)
        {
            QSharedPointer<Kactus2Value> directoryExtension (
                new Kactus2Value("kactus2:sourceDirectory", sourceDirectory));
            newSourceDirectoryGroup->addToGroup(directoryExtension);
        }
        getVendorExtensions()->append(newSourceDirectoryGroup);
    }
}

//-----------------------------------------------------------------------------
// Function: Component::getIgnoredFiles()
//-----------------------------------------------------------------------------
QStringList Component::getIgnoredFiles() const
{
    QList<QSharedPointer<VendorExtension> > ignoredFileExtensions =
        getGroupedExtensionsByType("kactus2:ignoredFiles", "kactus2:ignoredFile");

    QStringList ignoredFiles;
    foreach (QSharedPointer<VendorExtension> extension, ignoredFileExtensions)
    {
        QSharedPointer<Kactus2Value> ignoredFile = extension.dynamicCast<Kactus2Value>();
        ignoredFiles.append(ignoredFile->value());
    }

    return ignoredFiles;
}

//-----------------------------------------------------------------------------
// Function: Component::setIgnoredFiles()
//-----------------------------------------------------------------------------
void Component::setIgnoredFiles(QStringList const& ignoredFiles)
{
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:ignoredFiles")
        {
            getVendorExtensions()->removeAll(extension);
            break;
        }
    }

    if (!ignoredFiles.isEmpty())
    {
        QSharedPointer<Kactus2Group> newIgnoredFilesGroup (new Kactus2Group("kactus2:ignoredFiles"));
        foreach (QString ignoredFile, ignoredFiles)
        {
            QSharedPointer<Kactus2Value> ignoredFileExtension (
                new Kactus2Value("kactus2:ignoredFile", ignoredFile));
            newIgnoredFilesGroup->addToGroup(ignoredFileExtension);
        }
        getVendorExtensions()->append(newIgnoredFilesGroup);
    }
}

//-----------------------------------------------------------------------------
// Function: Component::getAuthor()
//-----------------------------------------------------------------------------
QString Component::getAuthor() const
{
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:author")
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
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:author")
        {
            getVendorExtensions()->removeAll(extension);
        }
    }

    if (!author.isEmpty())
    {
        QSharedPointer<Kactus2Value> authorValue (new Kactus2Value("kactus2:author", author));
        getVendorExtensions()->append(authorValue);
    }
}

//-----------------------------------------------------------------------------
// Function: Component::getDependentFiles()
//-----------------------------------------------------------------------------
QStringList Component::getDependentFiles() const
{
    QStringList files;

    for (int i = 0; i < fileSets_->size(); ++i)
    {
        files += fileSets_->at(i)->getFilePaths();
    }
    for (int i = 0; i < componentGenerators_->size(); ++i)
    {
        files += componentGenerators_->at(i)->getGeneratorExe();
    }
    return files;
}

//-----------------------------------------------------------------------------
// Function: Component::getDependentVLNVs()
//-----------------------------------------------------------------------------
QList<VLNV> Component::getDependentVLNVs() const
{
    QList<VLNV> vlnvList;

    foreach (QSharedPointer<BusInterface> busif, *busInterfaces_)
    {
        if (busif->getBusType().isValid())
        {
            vlnvList.append(busif->getBusType());
        }

        foreach (QSharedPointer<AbstractionType> abstractionType, *busif->getAbstractionTypes())
        {
            if (abstractionType->getAbstractionRef()->isValid())
            {
                vlnvList.append(*abstractionType->getAbstractionRef().data());
            }
        }

    }

    vlnvList.append(model_->getViewReferences());

    foreach (QSharedPointer<SWView> swView, getSWViews())
    {
        if (swView->getHierarchyRef().isValid())
        {
            vlnvList.append(swView->getHierarchyRef());
        }
    }

    foreach(QSharedPointer<SystemView> systemView, getSystemViews())
    {
        if (systemView->getHierarchyRef().isValid())
        {
            vlnvList.append(systemView->getHierarchyRef());
        }
    }

    foreach (QSharedPointer<ComInterface> comIf, getComInterfaces())
    {
        if (comIf->getComType().isValid())
        {
            vlnvList.append(comIf->getComType());
        }
    }

    foreach (QSharedPointer<ApiInterface> apiIf, getApiInterfaces())
    {
        if (apiIf->getApiType().isValid())
        {
            vlnvList.append(apiIf->getApiType());
        }
    }

    return vlnvList;
}

//-----------------------------------------------------------------------------
// Function: component::getRemapStateNames()
//-----------------------------------------------------------------------------
QStringList Component::getRemapStateNames() const
{
    QStringList remapStateNames;
    foreach (QSharedPointer<RemapState> singleRemapState, *remapStates_)
    {
        remapStateNames.append(singleRemapState->name());
    }

    return remapStateNames;
}

//-----------------------------------------------------------------------------
// Function: Component::getMemoryMapNames()
//-----------------------------------------------------------------------------
QStringList Component::getMemoryMapNames() const
{
    QStringList memoryMapNames;
    foreach (QSharedPointer<MemoryMap> memMap, *memoryMaps_)
    {
        Q_ASSERT(memMap);
        memoryMapNames.append(memMap->name());
    }
    return memoryMapNames;
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
    foreach (QSharedPointer<View> view, *getViews())
    {
        if (view->name() == viewName)
        {
            return true;
        }
    }

    return false;
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
    QStringList cpuNames;
    foreach (QSharedPointer<Cpu> cpu, *cpus_)
    {
        if (cpu)
        {
            cpuNames.append(cpu->name());
        }
    }

    return cpuNames;
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

    foreach (QSharedPointer<SWView> swView, getSWViews())
    {
        if (swView->getHierarchyRef().isValid())
        {
            list.append(swView->getHierarchyRef());
        }
    }
    foreach (QSharedPointer<SystemView> sysView, getSystemViews())
    {
        if (sysView->getHierarchyRef().isValid())
        {
            list.append(sysView->getHierarchyRef());
        }
    }
    return list;
}

//-----------------------------------------------------------------------------
// Function: Component::hasSWViews()
//-----------------------------------------------------------------------------
bool Component::hasSWViews() const
{
    return !getSWViews().isEmpty();
}

//-----------------------------------------------------------------------------
// Function: Component::getSWViewNames()
//-----------------------------------------------------------------------------
QStringList Component::getSWViewNames() const
{
    QStringList list;
    foreach (QSharedPointer<SWView> view, getSWViews())
    {
        list.append(view->name());
    }
    return list;
}

//-----------------------------------------------------------------------------
// Function: Component::hasSWView()
//-----------------------------------------------------------------------------
bool Component::hasSWView( const QString& viewName ) const
{
    foreach (QSharedPointer<SWView> swView, getSWViews())
    {
        if (swView->name() == viewName)
        {
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
// Function: Component::getHierSWRef()
//-----------------------------------------------------------------------------
VLNV Component::getHierSWRef(const QString viewName) const
{
    foreach (QSharedPointer<SWView> swView, getSWViews())
    {
        if (swView->name() == viewName || (viewName.isEmpty() && swView->getHierarchyRef().isValid()))
        {
            return swView->getHierarchyRef();
        }
    }
    return VLNV();
}

//-----------------------------------------------------------------------------
// Function: Component::getHierSWRefs()
//-----------------------------------------------------------------------------
QList<VLNV> Component::getHierSWRefs() const
{
    QList<VLNV> refs;

    foreach (QSharedPointer<SWView> swView, getSWViews())
    {
        if (swView->getHierarchyRef().isValid())
        {
            refs.append(swView->getHierarchyRef());
        }
    }
    return refs;
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
    return findSystemView(viewName) != 0;
}

//-----------------------------------------------------------------------------
// Function: Component::getSystemViewNames()
//-----------------------------------------------------------------------------
QStringList Component::getSystemViewNames() const
{
    QStringList list;
    foreach (QSharedPointer<SystemView> view, getSystemViews())
    {
        list.append(view->name());
    }
    return list;
}

//-----------------------------------------------------------------------------
// Function: Component::findSystemView()
//-----------------------------------------------------------------------------
QSharedPointer<SystemView> Component::findSystemView(QString const& name) const
{
    foreach (QSharedPointer<SystemView> systemView, getSystemViews())
    {
        if (systemView->name() == name)
        {
            return systemView;
        }
    }

    return QSharedPointer<SystemView> ();
}

//-----------------------------------------------------------------------------
// Function: Component::findSystemView()
//-----------------------------------------------------------------------------
QSharedPointer<SystemView> Component::findSystemView( const VLNV& hierRef ) const
{
    QSharedPointer<SystemView> sysView;

    foreach (QSharedPointer<SystemView> view, getSystemViews())
    {
        if (view->getHierarchyRef() == hierRef)
        {
            sysView = view;
        }
    }

    return sysView;
}

//-----------------------------------------------------------------------------
// Function: Component::getHierSystemRef()
//-----------------------------------------------------------------------------
VLNV Component::getHierSystemRef(const QString viewName) const
{
    foreach (QSharedPointer<SystemView> view, getSystemViews())
    {
        if (view->name() == viewName || (viewName.isEmpty() && view->getHierarchyRef().isValid()))
        {
            return view->getHierarchyRef();
        }
    }

    return VLNV();
}

//-----------------------------------------------------------------------------
// Function: Component::getHierSystemRefs()
//-----------------------------------------------------------------------------
QList<VLNV> Component::getHierSystemRefs() const
{
    QList<VLNV> refs;

    foreach (QSharedPointer<SystemView> view, getSystemViews())
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
    QStringList fileSetNames;

    foreach (QSharedPointer<FileSet> fileSet, *fileSets_)
    {
        fileSetNames.append(fileSet->name());
    }
    return fileSetNames;
}

//-----------------------------------------------------------------------------
// Function: Component::getFileSet()
//-----------------------------------------------------------------------------
QSharedPointer<FileSet> Component::getFileSet( const QString& name ) const
{
    foreach (QSharedPointer<FileSet> fileSet, *fileSets_)
    {
        if (fileSet->name() == name)
        {
            return fileSet;
        }
    }

    return QSharedPointer<FileSet>();
}

//-----------------------------------------------------------------------------
// Function: Component::hasFileSet()
//-----------------------------------------------------------------------------
bool Component::hasFileSet( const QString& fileSetName ) const
{
    foreach (QSharedPointer<FileSet> fileSet, *fileSets_)
    {
        if (fileSet->name() == fileSetName)
        {
            return true;
        }
    }
    return false;
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

    foreach (QSharedPointer<FileSet> fileSet, *fileSets_)
    {
        foreach (QSharedPointer<File> file, *fileSet->getFiles())
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

    foreach (QSharedPointer<const FileSet> fileSet, *fileSets_)
    {
        files.append(fileSet->findFilesByFileType(fileType));
    }

    return files;
}

//-----------------------------------------------------------------------------
// Function: Component::findFileSetById()
//-----------------------------------------------------------------------------
QSharedPointer<FileSet> Component::findFileSetById(QString const& id ) const
{
    foreach (QSharedPointer<FileSet> fileSet, *fileSets_)
    {
        if (fileSet->getFileSetId() == id)
        {
            return fileSet;
        }
    }

    return QSharedPointer<FileSet>();
}

//-----------------------------------------------------------------------------
// Function: Component::getFilesFromFileSets()
//-----------------------------------------------------------------------------
QStringList Component::getFilesFromFileSets( const QStringList& fileSetNames, const QStringList& fileTypes ) const
{
    QStringList files;

    foreach (QSharedPointer<FileSet> fileSet, *fileSets_)
    {
        if (fileSetNames.contains(fileSet->name()))
        {
            foreach (QSharedPointer<File> file, *fileSet->getFiles())
            {
                foreach (QString fileType, *file->getFileTypes())
                {
                    if (fileTypes.contains(fileType))
                    {
                        files.append(file->name());
                        break;
                    }
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
    foreach (QSharedPointer<FileSet> fileSet, *fileSets_)
    {
        if (fileSet->contains(fileName))
        {
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
// Function: Component::getBusInterfaceNames()
//-----------------------------------------------------------------------------
QStringList Component::getBusInterfaceNames() const
{
    QStringList list;
    foreach (QSharedPointer<BusInterface> busif, *busInterfaces_)
    {
        list.append(busif->name());
    }
    return list;
}

//-----------------------------------------------------------------------------
// Function: Component::getBusInterface()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> Component::getBusInterface(QString const& name ) const
{
    foreach (QSharedPointer<BusInterface> busif, *busInterfaces_)
    {
        if (busif->name() == name)
        {
            return busif;
        }
    }

    return QSharedPointer<BusInterface>();
}

//-----------------------------------------------------------------------------
// Function: Component::getSlaveInterfaces()
//-----------------------------------------------------------------------------
QStringList Component::getSlaveInterfaces( const QString& memoryMap ) const
{
    QStringList names;

    foreach (const QSharedPointer<BusInterface> busif, *busInterfaces_)
    {
        if (busif->getInterfaceMode() == General::SLAVE)
        {
            if (busif->getMemoryMapRef() == memoryMap)
            {
                names.append(busif->name());
            }
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

    foreach (const QSharedPointer<BusInterface> busif, *busInterfaces_)
    {
        if (busif->getInterfaceMode() == General::MASTER || busif->getInterfaceMode() == General::MIRROREDMASTER)
        {
            if (busif->getAddressSpaceRef() == addressSpace)
            {
                names.append(busif->name());
            }
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
    foreach (QSharedPointer<const BusInterface> busif, *busInterfaces_)
    {
        if (busif->getInterfaceMode() == General::MASTER || busif->getInterfaceMode() == General::MIRROREDMASTER)
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
    foreach (QSharedPointer<BusInterface> busif, *busInterfaces_)
    {
        if (busif->name() == interfaceName)
        {
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
// Function: Component::getComInterface()
//-----------------------------------------------------------------------------
QSharedPointer<ComInterface> Component::getComInterface( QString const& name )
{
    foreach (QSharedPointer<ComInterface> comIf, getComInterfaces())
    {
        if (comIf->name() == name)
        {
            return comIf;
        }
    }

    return QSharedPointer<ComInterface>();
}

//-----------------------------------------------------------------------------
// Function: Component::getApiInterface()
//-----------------------------------------------------------------------------
QSharedPointer<ApiInterface> Component::getApiInterface( QString const& name )
{
    foreach (QSharedPointer<ApiInterface> apiIf, getApiInterfaces())
    {
        if (apiIf->name() == name)
        {
            return apiIf;
        }
    }
    return QSharedPointer<ApiInterface>();
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
QList<QSharedPointer<Port> > Component::getPorts( const QString& interfaceName ) const
{
    QList<QSharedPointer<Port> > ports;

    QStringList portNames;

    bool found = false;
    foreach (QSharedPointer<BusInterface> busif, *busInterfaces_)
    {
        if (busif->name() == interfaceName)
        {
            found = true;
            portNames = busif->getPhysicalPortNames();
            break;
        }
    }

    if (!found)
    {
        return ports;
    }

    foreach (QSharedPointer<Port> port, *getPorts())
    {
        ports.append(port);
    }

    QList<QSharedPointer<Port> > portsToReturn;

    foreach (QSharedPointer<Port> port, ports)
    {
        if (portNames.contains(port->name()))
        {
            portsToReturn.append(port);
        }
    }

    return portsToReturn;
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
    QStringList list;

    foreach (QSharedPointer<AddressSpace> addrSpace, *addressSpaces_)
    {
        list.append(addrSpace->name());
    }
    return list;
}

//-----------------------------------------------------------------------------
// Function: Component::hasLocalMemoryMaps()
//-----------------------------------------------------------------------------
bool Component::hasLocalMemoryMaps() const
{
    foreach (QSharedPointer<AddressSpace> addrSpace, *addressSpaces_)
    {
        if (addrSpace->hasLocalMemoryMap())
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: Component::getDependentDirs()
//-----------------------------------------------------------------------------
QStringList Component::getDependentDirs() const
{
    QStringList dirs;

    foreach (QSharedPointer<FileSet> fileSet, *fileSets_)
    {
        dirs.append(*fileSet->getDependencies().data());
    }
    return dirs;
}

//-----------------------------------------------------------------------------
// Function: Component::copyBusInterfaces_()
//-----------------------------------------------------------------------------
void Component::copyBusInterfaces(const Component& other) const
{
    foreach (QSharedPointer<BusInterface> busInterface, *other.busInterfaces_)
    {
        if (busInterface)
        {
            QSharedPointer<BusInterface> copy =
                QSharedPointer<BusInterface>(new BusInterface(*busInterface.data()));
            busInterfaces_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Component::copyChannels()
//-----------------------------------------------------------------------------
void Component::copyChannels(const Component& other) const
{
    foreach (QSharedPointer<Channel> channel, *other.channels_)
    {
        if (channel)
        {
            QSharedPointer<Channel> copy = QSharedPointer<Channel>(new Channel(*channel.data()));
            channels_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Component::copyRemapStates()
//-----------------------------------------------------------------------------
void Component::copyRemapStates(const Component& other) const
{
    foreach (QSharedPointer<RemapState> remapState, *other.remapStates_)
    {
        if (remapState)
        {
            QSharedPointer<RemapState> copy = QSharedPointer<RemapState>(new RemapState(*remapState.data()));
            remapStates_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Component::copyAddressSpaces()
//-----------------------------------------------------------------------------
void Component::copyAddressSpaces(const Component& other) const
{
    foreach (QSharedPointer<AddressSpace> addressSpace, *other.addressSpaces_)
    {
        if (addressSpace)
        {
            QSharedPointer<AddressSpace> copy =
                QSharedPointer<AddressSpace>(new AddressSpace(*addressSpace.data()));
            addressSpaces_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Component::copyMemoryMaps()
//-----------------------------------------------------------------------------
void Component::copyMemoryMaps(const Component& other) const
{
    foreach (QSharedPointer<MemoryMap> memoryMap, *other.memoryMaps_)
    {
        if (memoryMap)
        {
            QSharedPointer<MemoryMap> copy = QSharedPointer<MemoryMap>(new MemoryMap(*memoryMap.data()));
            memoryMaps_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Component::copyModel()
//-----------------------------------------------------------------------------
void Component::copyModel(const Component& other)
{
    if (other.model_)
    {
        model_ = QSharedPointer<Model> (new Model(*other.model_.data()));
    }
}

//-----------------------------------------------------------------------------
// Function: Component::copyComponentGenerators()
//-----------------------------------------------------------------------------
void Component::copyComponentGenerators(const Component& other) const
{
    foreach (QSharedPointer<ComponentGenerator> generator, *other.componentGenerators_)
    {
        if (generator)
        {
            QSharedPointer<ComponentGenerator> copy =
                QSharedPointer<ComponentGenerator>(new ComponentGenerator(*generator.data()));
            componentGenerators_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Component::copyChoices()
//-----------------------------------------------------------------------------
void Component::copyChoices(const Component& other) const
{
    foreach (QSharedPointer<Choice> choice, *other.choices_)
    {
        if (choice)
        {
            QSharedPointer<Choice> copy = QSharedPointer<Choice>(new Choice(*choice.data()));
            choices_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Component::copyFileSets()
//-----------------------------------------------------------------------------
void Component::copyFileSets(const Component& other) const
{
    foreach (QSharedPointer<FileSet> fileSet, *other.fileSets_)
    {
        if (fileSet)
        {
            QSharedPointer<FileSet> copy = QSharedPointer<FileSet>(new FileSet(*fileSet.data()));
            fileSets_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Component::copyCpus()
//-----------------------------------------------------------------------------
void Component::copyCpus(const Component& other) const
{
    foreach (QSharedPointer<Cpu> cpu, *other.cpus_)
    {
        if (cpu)
        {
            QSharedPointer<Cpu> copy = QSharedPointer<Cpu>(new Cpu(*cpu.data()));
            cpus_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Component::copyOtherClockDrivers()
//-----------------------------------------------------------------------------
void Component::copyOtherClockDrivers(const Component& other) const
{
    foreach (QSharedPointer<OtherClockDriver> driver, *other.otherClockDrivers_)
    {
        if (driver)
        {
            QSharedPointer<OtherClockDriver> copy =
                QSharedPointer<OtherClockDriver>(new OtherClockDriver(*driver.data()));
            otherClockDrivers_->append(copy);
        }
    }
}
