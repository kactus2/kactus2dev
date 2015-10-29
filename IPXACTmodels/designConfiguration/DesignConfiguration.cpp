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
#include <IPXACTmodels/XmlUtils.h>

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
DesignConfiguration::DesignConfiguration(VLNV const& vlnv):
Document(vlnv), 
designRef_(),
generatorChainConfigurations_(new QList<QSharedPointer<ConfigurableVLNVReference> >),
interconnectionConfigurations_(new QList<QSharedPointer<InterconnectionConfiguration> > ),
viewConfigurations_(new QList<QSharedPointer<ViewConfiguration> > )
{
    setVlnv(vlnv);
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::DesignConfiguration()
//-----------------------------------------------------------------------------
DesignConfiguration::DesignConfiguration(DesignConfiguration const& other):
Document(other),
designRef_(other.designRef_),
generatorChainConfigurations_(new QList<QSharedPointer<ConfigurableVLNVReference> >),
interconnectionConfigurations_(new QList<QSharedPointer<InterconnectionConfiguration> >),
viewConfigurations_(new QList<QSharedPointer<ViewConfiguration> >)
{
    foreach (QSharedPointer<ConfigurableVLNVReference> generatorChainConf, *other.generatorChainConfigurations_)
    {
        if (generatorChainConf)
        {
            QSharedPointer<ConfigurableVLNVReference> copy = QSharedPointer<ConfigurableVLNVReference>(
                new ConfigurableVLNVReference(*generatorChainConf.data()));
            generatorChainConfigurations_->append(copy);
        }
    }
    foreach (QSharedPointer<InterconnectionConfiguration> configuration, *other.interconnectionConfigurations_)
    {
        if (configuration)
        {
            QSharedPointer<InterconnectionConfiguration> copy = QSharedPointer<InterconnectionConfiguration>(
                new InterconnectionConfiguration(*configuration.data()));
            interconnectionConfigurations_->append(copy);
        }
    }

    foreach (QSharedPointer<ViewConfiguration> configuration, *other.viewConfigurations_)
    {
        if (configuration)
        {
            QSharedPointer<ViewConfiguration> copy = QSharedPointer<ViewConfiguration>(
                new ViewConfiguration(*configuration.data()));
            viewConfigurations_->append(copy);
        }
    }

}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::DesignConfiguration()
//-----------------------------------------------------------------------------
DesignConfiguration::DesignConfiguration():
Document(),
designRef_(),
generatorChainConfigurations_(new QList<QSharedPointer<ConfigurableVLNVReference> >),
interconnectionConfigurations_(new QList<QSharedPointer<InterconnectionConfiguration> >),
viewConfigurations_(new QList<QSharedPointer<ViewConfiguration> > )
{

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
        foreach (QSharedPointer<ConfigurableVLNVReference> generatorChainConf, *other.generatorChainConfigurations_)
        {
            if (generatorChainConf)
            {
                QSharedPointer<ConfigurableVLNVReference> copy = QSharedPointer<ConfigurableVLNVReference>
                    (new ConfigurableVLNVReference(*generatorChainConf.data()));
                generatorChainConfigurations_->append(copy);
			}
		}

        interconnectionConfigurations_->clear();
        foreach (QSharedPointer<InterconnectionConfiguration> configuration, *other.interconnectionConfigurations_)
        {
            if (configuration)
            {
                QSharedPointer<InterconnectionConfiguration> copy = QSharedPointer<InterconnectionConfiguration>(
                    new InterconnectionConfiguration(*configuration.data()));
                interconnectionConfigurations_->append(copy);
			}
		}

        viewConfigurations_->clear();
        foreach (QSharedPointer<ViewConfiguration> configuration, *other.viewConfigurations_)
        {
            if (configuration)
            {
                QSharedPointer<ViewConfiguration> copy = QSharedPointer<ViewConfiguration>(
                    new ViewConfiguration(*configuration.data()));
                viewConfigurations_->append(copy);
            }
        }
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::~DesignConfiguration()
//-----------------------------------------------------------------------------
DesignConfiguration::~DesignConfiguration()
{
    generatorChainConfigurations_.clear();
    interconnectionConfigurations_.clear();
    viewConfigurations_.clear();
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
    // delete old values
    interconnectionConfigurations_->clear();

    // save new values
    interconnectionConfigurations_ = interconnectionConfs;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::setViewConfigurations()
//-----------------------------------------------------------------------------
void DesignConfiguration::setViewConfigurations(
    QSharedPointer<QList<QSharedPointer<ViewConfiguration> > > newViewConfigurations)
{
    // delete old values
    viewConfigurations_->clear();

    // save new values
    viewConfigurations_ = newViewConfigurations;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::setDesignRef()
//-----------------------------------------------------------------------------
void DesignConfiguration::setDesignRef(const VLNV& designRef)
{
    // save new designRef
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
    // delete old generatorChainConfs
    generatorChainConfigurations_->clear();

    // save new generatorChainConfs
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
    for (int i = 0; i < generatorChainConfigurations_->size(); ++i)
    {
        if (generatorChainConfigurations_->at(i)->isValid())
        {
            VLNV generatorChainVLNV = *generatorChainConfigurations_->at(i);
            vlnvList.append(generatorChainVLNV);
		}
	}

	// Append all vlnvs referencing the abstractors that this designConfiguration depends on.
    for (int i = 0; i < interconnectionConfigurations_->size(); ++i)
    {
        vlnvList.append(interconnectionConfigurations_->at(i)->getDependantVLNVs());
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
    foreach (QSharedPointer<ViewConfiguration> configuration, *viewConfigurations_)
    {
        if (configuration->getInstanceName() == instanceName)
        {
            viewConfigurations_->removeAll(configuration);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::getActiveView()
//-----------------------------------------------------------------------------
QString DesignConfiguration::getActiveView(QString const& instanceName) const
{
    foreach (QSharedPointer<ViewConfiguration> configuration, *viewConfigurations_)
    {
        if (configuration->getInstanceName() == instanceName)
        {
            return configuration->getViewReference();
        }
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
// Function: DesignConfiguration::getConfigurableElementValues()
//-----------------------------------------------------------------------------
QMap<QString, QString> DesignConfiguration::getConfigurableElementValues(QString const& instanceUUID) const
{
    QMap<QString, QString> configurableElements;
    foreach(QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:configurableElementValues")
        {
            QSharedPointer<Kactus2Group> cevsGroup = extension.dynamicCast<Kactus2Group>();
            foreach(QSharedPointer<VendorExtension> instanceNode, cevsGroup->getByType("kactus2:componentInstance"))
            {
                QSharedPointer<Kactus2Group> instanceGroup = instanceNode.dynamicCast<Kactus2Group>();

                QString groupUUID = "";
                QList<QSharedPointer<VendorExtension> > uuidValues = instanceGroup->getByType("kactus2:uuid");
                if (!uuidValues.isEmpty())
                {
                    groupUUID = uuidValues.first().dynamicCast<Kactus2Value>()->value();
                }

                if (groupUUID == instanceUUID)
                {
                    foreach(QSharedPointer<VendorExtension> value,
                        instanceGroup->getByType("kactus2:configurableElementValue"))
                    {
                        QSharedPointer<Kactus2Placeholder> valueHolder = value.dynamicCast<Kactus2Placeholder>();
                        configurableElements.insert(valueHolder->getAttributeValue("referenceId"), 
                            valueHolder->getAttributeValue("value"));
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
void DesignConfiguration::setConfigurableElementValues(QString const& instanceUUID, 
    QMap<QString, QString> const& configurableElementValues)
{
    if (configurableElementValues.isEmpty())
    {
        return;
    }

    QMap<QString, QString> valuesToSet = configurableElementValues;

    QSharedPointer<Kactus2Group> instanceGroup =
        findOrCreateInstanceExtension(instanceUUID).dynamicCast<Kactus2Group>();

    foreach(QSharedPointer<VendorExtension> value, instanceGroup->getByType("kactus2:configurableElementValue"))
    {
        QSharedPointer<Kactus2Placeholder> existingValue = value.dynamicCast<Kactus2Placeholder>();

        QString referenceId = existingValue->getAttributeValue("referenceId");
        if (valuesToSet.contains(referenceId))
        {
            QString updatedValue = valuesToSet.value(referenceId);
            existingValue->setAttribute("value", updatedValue);
            valuesToSet.remove(referenceId);
        }
        else
        {
            instanceGroup->removeFromGroup(existingValue);
        }
    }

    foreach (QString newId, valuesToSet.keys())
    {
        QSharedPointer<Kactus2Placeholder> valueHolder(new Kactus2Placeholder("kactus2:configurableElementValue"));
        valueHolder->setAttribute("referenceId", newId);
        valueHolder->setAttribute("value", valuesToSet.value(newId));
        instanceGroup->addToGroup(valueHolder);
    }
}

//-----------------------------------------------------------------------------
// Function: designconfiguration::getKactsu2ViewOverrides()
//-----------------------------------------------------------------------------
QMap<QString, QString> DesignConfiguration::getKactus2ViewOverrides() const
{
    QSharedPointer<Kactus2Group> overrideGroup;
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:viewOverrides")
        {
            overrideGroup = extension.dynamicCast<Kactus2Group>();
            break;
        }
    }

    QMap <QString, QString> viewOverrides;

    if (overrideGroup)
    {
        foreach (QSharedPointer<VendorExtension> extension, overrideGroup->getByType("kactus2:instanceView"))
        {
            QSharedPointer<Kactus2Placeholder> viewExtension = extension.dynamicCast<Kactus2Placeholder>();

            if (viewExtension)
            {
                QString viewOverrideId = viewExtension->getAttributeValue("id");
                QString viewOverrideValue = viewExtension->getAttributeValue("viewName");

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
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:viewOverrides")
        {
            getVendorExtensions()->removeAll(extension);
            break;
        }
    }

    if (!kactus2ViewOverrides.isEmpty())
    {
        QSharedPointer<Kactus2Group> overrideGroup (new Kactus2Group("kactus2:viewOverrides"));

        QMap<QString, QString>::const_iterator overrideIndex = kactus2ViewOverrides.constBegin();
        while (overrideIndex != kactus2ViewOverrides.constEnd())
        {
            QSharedPointer<Kactus2Placeholder> treeItemExtension(new Kactus2Placeholder("kactus2:instanceView"));
            treeItemExtension->setAttribute("id", overrideIndex.key());
            treeItemExtension->setAttribute("viewName", overrideIndex.value());

            overrideGroup->addToGroup(treeItemExtension);

            ++overrideIndex;
        }

        getVendorExtensions()->append(overrideGroup);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::findOrCreateInstanceExtension()
//-----------------------------------------------------------------------------
QSharedPointer<VendorExtension> DesignConfiguration::findOrCreateInstanceExtension(QString const& instanceUUID)
{
    QSharedPointer<Kactus2Group> cevGroup;
    foreach(QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:configurableElementValues")
        {
            cevGroup = extension.dynamicCast<Kactus2Group>();
            break;
        }
    }

    if (cevGroup.isNull())
    {
        cevGroup = QSharedPointer<Kactus2Group>(new Kactus2Group("kactus2:configurableElementValues"));
        getVendorExtensions()->append(cevGroup);
    }

    QSharedPointer<Kactus2Group> targetInstanceGroup;
    foreach (QSharedPointer<VendorExtension> instanceNode, cevGroup->getByType("kactus2:componentInstance"))
    {
        QSharedPointer<Kactus2Group> instanceGroup = instanceNode.dynamicCast<Kactus2Group>();
        QString groupUUID = "";
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
