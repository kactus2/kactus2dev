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
            VLNV generatorChainVLNV = *generatorChainConfiguration;
            vlnvList.append(generatorChainVLNV);
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
void DesignConfiguration::addViewConfiguration(QString const& instanceName, QString const& viewName)
{
	if (!viewName.isEmpty())
    {
        QSharedPointer<ViewConfiguration> newViewConfig (new ViewConfiguration(instanceName));
        newViewConfig->setViewReference(viewName);

        viewConfigurations_->append(newViewConfig);
	}
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::removeViewConfiguration()
//-----------------------------------------------------------------------------
void DesignConfiguration::removeViewConfiguration(QString const& instanceName)
{
    viewConfigurations_->removeAll(getViewConfiguration(instanceName));
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::getViewConfiguration()
//-----------------------------------------------------------------------------
QSharedPointer<ViewConfiguration> DesignConfiguration::getViewConfiguration(QString const& instanceName) const
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
QString DesignConfiguration::getActiveView(QString const& instanceName) const
{
    auto configuration = getViewConfiguration(instanceName);

    if (configuration)
    {
        return configuration->getViewReference();
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::hasActiveView()
//-----------------------------------------------------------------------------
bool DesignConfiguration::hasActiveView(QString const& instanceName) const
{
	return !getActiveView(instanceName).isEmpty();
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
// Function: designconfiguration::getKactsu2ViewOverrides()
//-----------------------------------------------------------------------------
QMap<QString, QString> DesignConfiguration::getKactus2ViewOverrides() const
{
    QSharedPointer<Kactus2Group> overrideGroup =
        findVendorExtension(QStringLiteral("kactus2:viewOverrides")).dynamicCast<Kactus2Group>();

    QMap <QString, QString> viewOverrides;

    if (overrideGroup)
    {
        auto overrideIndentifier = QStringLiteral("kactus2:instanceView");
        for (QSharedPointer<VendorExtension> extension : 
            overrideGroup->getByType(overrideIndentifier))
        {
            QSharedPointer<Kactus2Placeholder> viewExtension = extension.dynamicCast<Kactus2Placeholder>();

            if (viewExtension)
            {
                QString viewOverrideId = viewExtension->getAttributeValue(QStringLiteral("id"));
                QString viewOverrideValue = viewExtension->getAttributeValue(QStringLiteral("viewName"));

                viewOverrides.insert(viewOverrideId, viewOverrideValue);
            }
        }
    }

    return viewOverrides;
}

//-----------------------------------------------------------------------------
// Function: designconfiguration::setKactus2ViewOverrides()
//-----------------------------------------------------------------------------
void DesignConfiguration::setKactus2ViewOverrides(QMap<QString, QString> kactus2ViewOverrides)
{
    auto extension = findVendorExtension(QLatin1String("kactus2:viewOverrides"));

    getVendorExtensions()->removeAll(extension);

    if (!kactus2ViewOverrides.isEmpty())
    {
        QSharedPointer<Kactus2Group> overrideGroup (new Kactus2Group(QStringLiteral("kactus2:viewOverrides")));

        for (auto const& id : kactus2ViewOverrides.keys())
        {
            QSharedPointer<Kactus2Placeholder> treeItemExtension(
                new Kactus2Placeholder(QStringLiteral("kactus2:instanceView")));
            treeItemExtension->setAttribute(QStringLiteral("id"), id);
            treeItemExtension->setAttribute(QStringLiteral("viewName"), kactus2ViewOverrides.value(id));

            overrideGroup->addToGroup(treeItemExtension);
        }

        getVendorExtensions()->append(overrideGroup);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::findOrCreateInstanceExtension()
//-----------------------------------------------------------------------------
QSharedPointer<VendorExtension> DesignConfiguration::findOrCreateInstanceExtension(QString const& instanceUUID)
{
    QSharedPointer<Kactus2Group> cevGroup =
        findVendorExtension(QLatin1String("kactus2:configurableElementValues")).dynamicCast<Kactus2Group>();

    if (cevGroup == nullptr)
    {
        cevGroup = QSharedPointer<Kactus2Group>(new Kactus2Group(QStringLiteral("kactus2:configurableElementValues")));
        getVendorExtensions()->append(cevGroup);
    }

    QSharedPointer<Kactus2Group> targetInstanceGroup;
    QString instanceIdentifier = QStringLiteral("kactus2:componentInstance");
    for  (QSharedPointer<VendorExtension> instanceNode : cevGroup->getByType(instanceIdentifier))
    {
        QSharedPointer<Kactus2Group> instanceGroup = instanceNode.dynamicCast<Kactus2Group>();
        QString groupUUID;
        QList<QSharedPointer<VendorExtension> > uuidValues = instanceGroup->getByType(QStringLiteral("kactus2:uuid"));
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
        targetInstanceGroup = QSharedPointer<Kactus2Group>(new Kactus2Group(QStringLiteral("kactus2:componentInstance")));
        targetInstanceGroup->addToGroup(QSharedPointer<Kactus2Value>(
            new Kactus2Value(QStringLiteral("kactus2:uuid"), instanceUUID)));
        cevGroup->addToGroup(targetInstanceGroup);
    }

    return targetInstanceGroup;
}
