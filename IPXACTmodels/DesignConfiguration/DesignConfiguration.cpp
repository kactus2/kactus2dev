//-----------------------------------------------------------------------------
// File: DesignConfiguration.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 03.08.2015
//
// Description:
// Implementation for ipxact:designConfiguration element.
//-----------------------------------------------------------------------------

#include "DesignConfiguration.h"

#include <IPXACTmodels/common/GenericVendorExtension.h>
#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Group.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Placeholder.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Value.h>

#include <IPXACTmodels/common/VLNV.h>

#include <QList>
#include <QDomNode>
#include <QMap>
#include <QString>
#include <QSharedPointer>
#include <QDomNamedNodeMap>
#include <QObject>

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::DesignConfiguration()
//-----------------------------------------------------------------------------
DesignConfiguration::DesignConfiguration(VLNV const& vlnv, Revision revision):
Document(vlnv, revision)
{

}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::DesignConfiguration()
//-----------------------------------------------------------------------------
DesignConfiguration::DesignConfiguration(DesignConfiguration const& other):
Document(other),
designRef_(other.designRef_)
{
    for (QSharedPointer<ConfigurableVLNVReference> generatorChainConf : *other.generatorChainConfigurations_)
    {
        if (generatorChainConf)
        {
            auto copy = QSharedPointer<ConfigurableVLNVReference>(
                new ConfigurableVLNVReference(*generatorChainConf));
            generatorChainConfigurations_->append(copy);
        }
    }
    for (QSharedPointer<InterconnectionConfiguration> configuration : *other.interconnectionConfigurations_)
    {
        if (configuration)
        {
            auto copy = QSharedPointer<InterconnectionConfiguration>(
                new InterconnectionConfiguration(*configuration));
            interconnectionConfigurations_->append(copy);
        }
    }

    for (QSharedPointer<ViewConfiguration> configuration : *other.viewConfigurations_)
    {
        if (configuration)
        {
            auto copy = QSharedPointer<ViewConfiguration>(new ViewConfiguration(*configuration));
            viewConfigurations_->append(copy);
        }
    }

}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::operator=()
//-----------------------------------------------------------------------------
DesignConfiguration& DesignConfiguration::operator=( const DesignConfiguration& other )
{
	if (this != &other)
    {
		Document::operator=(other);
		designRef_ = other.designRef_;

        generatorChainConfigurations_->clear();
        for (QSharedPointer<ConfigurableVLNVReference> generatorChainConf : *other.generatorChainConfigurations_)
        {
            if (generatorChainConf)
            {
                auto copy = QSharedPointer<ConfigurableVLNVReference>
                    (new ConfigurableVLNVReference(*generatorChainConf));
                generatorChainConfigurations_->append(copy);
			}
		}

        interconnectionConfigurations_->clear();
        for  (QSharedPointer<InterconnectionConfiguration> configuration : *other.interconnectionConfigurations_)
        {
            if (configuration)
            {
                auto copy = QSharedPointer<InterconnectionConfiguration>(
                    new InterconnectionConfiguration(*configuration));
                interconnectionConfigurations_->append(copy);
			}
		}

        viewConfigurations_->clear();
        for (QSharedPointer<ViewConfiguration> configuration : *other.viewConfigurations_)
        {
            if (configuration)
            {
                auto copy = QSharedPointer<ViewConfiguration>(
                    new ViewConfiguration(*configuration));
                viewConfigurations_->append(copy);
            }
        }
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::clone()
//-----------------------------------------------------------------------------
QSharedPointer<Document> DesignConfiguration::clone() const
{
	return QSharedPointer<Document>(new DesignConfiguration(*this));	
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::setVlnv()
//-----------------------------------------------------------------------------
void DesignConfiguration::setVlnv(const VLNV& vlnv)
{
    VLNV designConfigurationVLNV(vlnv);
    designConfigurationVLNV.setType(VLNV::DESIGNCONFIGURATION);
    Document::setVlnv(vlnv);    
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::setInterconnectionConfs()
//-----------------------------------------------------------------------------
void DesignConfiguration::setInterconnectionConfs(
    QSharedPointer<QList<QSharedPointer<InterconnectionConfiguration> > > interconnectionConfs)
{
    interconnectionConfigurations_->clear();

    interconnectionConfigurations_ = interconnectionConfs;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::setViewConfigurations()
//-----------------------------------------------------------------------------
void DesignConfiguration::setViewConfigurations(
    QSharedPointer<QList<QSharedPointer<ViewConfiguration> > > newViewConfigurations)
{
    viewConfigurations_->clear();

    viewConfigurations_ = newViewConfigurations;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::setDesignRef()
//-----------------------------------------------------------------------------
void DesignConfiguration::setDesignRef(const VLNV& designRef)
{
    designRef_ = designRef;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::getInterconnectionConfs()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<InterconnectionConfiguration> > > DesignConfiguration::getInterconnectionConfs()
{
    return interconnectionConfigurations_;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::getViewConfigurations()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<ViewConfiguration> > > DesignConfiguration::getViewConfigurations()
{
    return viewConfigurations_;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::setGeneratorChainConfs()
//-----------------------------------------------------------------------------
void DesignConfiguration::setGeneratorChainConfs(
    QSharedPointer<QList<QSharedPointer<ConfigurableVLNVReference> > > generatorChainConfs)
{
    generatorChainConfigurations_->clear();

    generatorChainConfigurations_ = generatorChainConfs;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::getDesignRef()
//-----------------------------------------------------------------------------
VLNV DesignConfiguration::getDesignRef() const
{
    return designRef_;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::getGeneratorChainConfs()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<ConfigurableVLNVReference> > > DesignConfiguration::getGeneratorChainConfs()
{
    return generatorChainConfigurations_;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::getDependentFiles()
//-----------------------------------------------------------------------------
QStringList DesignConfiguration::getDependentFiles() const
{
    return QStringList();
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::getDependentVLNVs()
//-----------------------------------------------------------------------------
QList<VLNV> DesignConfiguration::getDependentVLNVs() const
{
	QList<VLNV> vlnvList;

	// Append the vlnv of the design that is associated with this designConfiguration.
	if (designRef_.isValid())
    {
		vlnvList.append(designRef_);
	}

	// Append all vlnvs representing generator chains associated with this design configuration.
    for (auto const& generatorChainConfiguration : *generatorChainConfigurations_)
    {
        if (generatorChainConfiguration->isValid())
        {
            vlnvList.append(*generatorChainConfiguration);
		}
	}

	// Append all vlnvs referencing the abstractors that this designConfiguration depends on.
    for (auto const& interconnectionConfiguration : *interconnectionConfigurations_)
    {
        vlnvList.append(interconnectionConfiguration->getDependantVLNVs());
	}

	return vlnvList;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::addViewConfiguration()
//-----------------------------------------------------------------------------
void DesignConfiguration::addViewConfiguration(std::string const& instanceName, std::string const& viewName)
{
	if (viewName.empty() == false)
    {
        QSharedPointer<ViewConfiguration> newViewConfig (new ViewConfiguration(instanceName));
        newViewConfig->setViewReference(viewName);

        viewConfigurations_->append(newViewConfig);
	}
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::removeViewConfiguration()
//-----------------------------------------------------------------------------
void DesignConfiguration::removeViewConfiguration(std::string const& instanceName)
{
    auto configuration = getViewConfiguration(instanceName);

    viewConfigurations_->removeAll(configuration);
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::getViewConfiguration()
//-----------------------------------------------------------------------------
QSharedPointer<ViewConfiguration> DesignConfiguration::getViewConfiguration(std::string const& instanceName) const
{
    QSharedPointer<ViewConfiguration> configuration = nullptr;

    auto it = std::find_if(viewConfigurations_->cbegin(), viewConfigurations_->cend(),
        [instanceName](auto const& viewConfiguration) {return viewConfiguration->getInstanceName() == instanceName; });

    if (it != viewConfigurations_->cend())
    {
        configuration = *it;
    }

    return configuration;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::getActiveView()
//-----------------------------------------------------------------------------
std::string DesignConfiguration::getActiveView(std::string const& instanceName) const
{
    if (auto configuration = getViewConfiguration(instanceName); configuration)
    {
        return configuration->getViewReference();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::hasActiveView()
//-----------------------------------------------------------------------------
bool DesignConfiguration::hasActiveView(std::string const& instanceName) const
{
	return getActiveView(instanceName).empty() == false;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::setDesignConfigImplementation()
//-----------------------------------------------------------------------------
void DesignConfiguration::setDesignConfigImplementation(KactusAttribute::Implementation implementation)
{
    setImplementation(implementation);
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::getDesignConfigImplementation()
//-----------------------------------------------------------------------------
KactusAttribute::Implementation DesignConfiguration::getDesignConfigImplementation() const
{
    return getImplementation();
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::getConfigurableElementValues()
//-----------------------------------------------------------------------------
QMap<std::string, std::string> DesignConfiguration::getConfigurableElementValues(std::string const& instanceUUID) const
{
    QMap<std::string, std::string> configurableElements;
    std::string instanceIdentifier = "kactus2:componentInstance";
    for (QSharedPointer<VendorExtension> extension : *getVendorExtensions())
    {
        if (extension->type() == "kactus2:configurableElementValues")
        {
            QSharedPointer<Kactus2Group> cevsGroup = extension.dynamicCast<Kactus2Group>();
            for (QSharedPointer<VendorExtension> instanceNode :
                cevsGroup->getByType(instanceIdentifier))
            {
                QSharedPointer<Kactus2Group> instanceGroup = instanceNode.dynamicCast<Kactus2Group>();

                std::string groupUUID;
                QList<QSharedPointer<VendorExtension> > uuidValues = instanceGroup->getByType("kactus2:uuid");
                if (!uuidValues.isEmpty())
                {
                    groupUUID = uuidValues.first().dynamicCast<Kactus2Value>()->value();
                }

                if (groupUUID == instanceUUID)
                {
                    std::string elementIdentifier = "kactus2:configurableElementValue";
                    for (QSharedPointer<VendorExtension> value : instanceGroup->getByType(elementIdentifier))
                    {
                        QSharedPointer<Kactus2Placeholder> valueHolder = value.dynamicCast<Kactus2Placeholder>();
                        configurableElements.insert(valueHolder->getAttributeValue(std::string("referenceId")),
                            valueHolder->getAttributeValue(std::string("value")));
                    }
                }
            }
        }
    }

    return configurableElements;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::setConfigurableElementValues()
//-----------------------------------------------------------------------------
void DesignConfiguration::setConfigurableElementValues(std::string const& instanceUUID,
    QMap<std::string, std::string> const& configurableElementValues)
{
    if (configurableElementValues.isEmpty())
    {
        return;
    }

    QMap<std::string, std::string> valuesToSet = configurableElementValues;

    QSharedPointer<Kactus2Group> instanceGroup =
        findOrCreateInstanceExtension(instanceUUID).dynamicCast<Kactus2Group>();

    std::string elementIdentifier = "kactus2:configurableElementValue";
                   
    for (QSharedPointer<VendorExtension> value : instanceGroup->getByType(elementIdentifier))
    {
        QSharedPointer<Kactus2Placeholder> existingValue = value.dynamicCast<Kactus2Placeholder>();

        std::string referenceId = existingValue->getAttributeValue(std::string("referenceId"));
        if (valuesToSet.contains(referenceId))
        {
            std::string updatedValue = valuesToSet.value(referenceId);
            existingValue->setAttribute(std::string("value"), updatedValue);
            valuesToSet.remove(referenceId);
        }
        else
        {
            instanceGroup->removeFromGroup(existingValue);
        }
    }

    for (auto const& newId : valuesToSet.keys())
    {
        QSharedPointer<Kactus2Placeholder> valueHolder(new Kactus2Placeholder("kactus2:configurableElementValue"));
        valueHolder->setAttribute(std::string("referenceId"), newId);
        valueHolder->setAttribute(std::string("value"), valuesToSet.value(newId));
        instanceGroup->addToGroup(valueHolder);
    }
}

//-----------------------------------------------------------------------------
// Function: designconfiguration::getKactsu2ViewOverrides()
//-----------------------------------------------------------------------------
QMap<std::string, std::string> DesignConfiguration::getKactus2ViewOverrides() const
{
    QSharedPointer<Kactus2Group> overrideGroup =
        findVendorExtension("kactus2:viewOverrides").dynamicCast<Kactus2Group>();

    QMap <std::string, std::string> viewOverrides;

    if (overrideGroup)
    {
        std::string overrideIndentifier = "kactus2:instanceView";
        for (QSharedPointer<VendorExtension> extension : overrideGroup->getByType(overrideIndentifier))
        {
            QSharedPointer<Kactus2Placeholder> viewExtension = extension.dynamicCast<Kactus2Placeholder>();

            if (viewExtension)
            {
                auto viewOverrideId = viewExtension->getAttributeValue(std::string("id"));
                auto viewOverrideValue = viewExtension->getAttributeValue(std::string("viewName"));

                viewOverrides.insert(viewOverrideId, viewOverrideValue);
            }
        }
    }

    return viewOverrides;
}

//-----------------------------------------------------------------------------
// Function: designconfiguration::setKactus2ViewOverrides()
//-----------------------------------------------------------------------------
void DesignConfiguration::setKactus2ViewOverrides(QMap<std::string, std::string> kactus2ViewOverrides)
{
    auto extension = findVendorExtension("kactus2:viewOverrides");

    getVendorExtensions()->removeAll(extension);

    if (!kactus2ViewOverrides.isEmpty())
    {
        QSharedPointer<Kactus2Group> overrideGroup (new Kactus2Group("kactus2:viewOverrides"));

        for (auto const& id : kactus2ViewOverrides.keys())
        {
            QSharedPointer<Kactus2Placeholder> treeItemExtension(new Kactus2Placeholder("kactus2:instanceView"));
            treeItemExtension->setAttribute(std::string("id"), id);
            treeItemExtension->setAttribute(std::string("viewName"), kactus2ViewOverrides.value(id));

            overrideGroup->addToGroup(treeItemExtension);
        }

        getVendorExtensions()->append(overrideGroup);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::findOrCreateInstanceExtension()
//-----------------------------------------------------------------------------
QSharedPointer<VendorExtension> DesignConfiguration::findOrCreateInstanceExtension(std::string const& instanceUUID)
{
    QSharedPointer<Kactus2Group> cevGroup =
        findVendorExtension("kactus2:configurableElementValues").dynamicCast<Kactus2Group>();

    if (cevGroup == nullptr)
    {
        cevGroup = QSharedPointer<Kactus2Group>(new Kactus2Group("kactus2:configurableElementValues"));
        getVendorExtensions()->append(cevGroup);
    }

    QSharedPointer<Kactus2Group> targetInstanceGroup;
    std::string instanceIdentifier = "kactus2:componentInstance";
    for  (QSharedPointer<VendorExtension> instanceNode : cevGroup->getByType(instanceIdentifier))
    {
        QSharedPointer<Kactus2Group> instanceGroup = instanceNode.dynamicCast<Kactus2Group>();
        std::string groupUUID;
        QList<QSharedPointer<VendorExtension> > uuidValues = instanceGroup->getByType("kactus2:uuid");
        if (!uuidValues.isEmpty())
        {
            groupUUID = uuidValues.first().dynamicCast<Kactus2Value>()->value();
        }

        if (groupUUID == instanceUUID)
        {
            targetInstanceGroup  = instanceNode.dynamicCast<Kactus2Group>();
            break;
        }
    }

    if (targetInstanceGroup.isNull())
    {
        targetInstanceGroup = QSharedPointer<Kactus2Group>(new Kactus2Group("kactus2:componentInstance"));
        targetInstanceGroup->addToGroup(QSharedPointer<Kactus2Value>(
            new Kactus2Value("kactus2:uuid", instanceUUID)));
        cevGroup->addToGroup(targetInstanceGroup);
    }

    return targetInstanceGroup;
}
