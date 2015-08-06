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

#include "IPXACTmodels/librarycomponent.h"
#include <IPXACTmodels/generatorchain.h>
#include <IPXACTmodels/vlnv.h>
#include <IPXACTmodels/XmlUtils.h>
#include <IPXACTmodels/GenericVendorExtension.h>
#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Group.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Placeholder.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Value.h>

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
DesignConfiguration::DesignConfiguration(QDomDocument& doc):
LibraryComponent(doc), 
designRef_(),
generatorChainConfigurations_(),
interconnectionConfigurations_(),
viewConfigurations_(), 
description_(), 
attributes_(),
assertions_(new QList<QSharedPointer<Assertion> >),
parameters_(new QList<QSharedPointer<Parameter> >)
{
	LibraryComponent::vlnv_->setType(VLNV::DESIGNCONFIGURATION);

	// find the IP-Xact root element (spirit:component or spirit:design ...)
	QDomNodeList nodeList = doc.childNodes();
	int i = 0;
	// search for the first element with children (=document type)
	while (!nodeList.at(i).hasChildNodes())
    {
		// if the node is for a header comment
		if (nodeList.at(i).isComment())
        {
			topComments_.append(nodeList.at(i).nodeValue());
		}
		++i;
	}

	QDomNode designConfNode = doc.childNodes().item(i);

	// parse the attributes
	attributes_ = XmlUtils::parseAttributes(designConfNode);

	// go through all childNodes
	for (int i = 0; i < designConfNode.childNodes().count(); ++i)
    {
		QDomNode tempNode = designConfNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:designRef"))
        {
            designRef_ = VLNV::createVLNV(tempNode, VLNV::DESIGN);
        }
        else if (tempNode.nodeName() ==	QString("spirit:generatorChainConfiguration")) 
        {
            //generatorChainConfs_.append(QSharedPointer<GeneratorChainConfiguration>(
            //    new GeneratorChainConfiguration(tempNode)));
            generatorChainConfigurations_.append(QSharedPointer<ConfigurableVLNVReference>
                (new ConfigurableVLNVReference(tempNode, VLNV::GENERATORCHAIN)));
        }
        else if (tempNode.nodeName() ==	QString("ipxact:interconnectionConfiguration"))
        {
            //interconnectionConfs_.append(QSharedPointer<InterconnectionConf>(new InterconnectionConf(tempNode)));
        }
		else if (tempNode.nodeName() == QString("spirit:viewConfiguration"))
        {
            QString instanceName;
            QString viewName;

            // search both child elements of the viewConfiguration element
            for (int j = 0; j < tempNode.childNodes().count(); ++j) 
            {
                QDomNode viewNode = tempNode.childNodes().at(j);

                if (viewNode.nodeName() == QString("spirit:instanceName"))
                {
                    instanceName = viewNode.childNodes().at(0).nodeValue();
                }
                else if (viewNode.nodeName() == QString("spirit:viewName"))
                {
                    viewName = viewNode.childNodes().at(0).nodeValue();
                }
            }

            // add the child elements into the QMap
            //viewConfigurations_.insert(instanceName, viewName);
        }
        else if (tempNode.nodeName() ==	QString("spirit:description"))
        {
            description_ = tempNode.childNodes().at(0).nodeValue();
        }
        else if (tempNode.nodeName() == "spirit:vendorExtensions")
        {
            parseVendorExtensions(tempNode);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::DesignConfiguration()
//-----------------------------------------------------------------------------
DesignConfiguration::DesignConfiguration( const VLNV& vlnv ):
LibraryComponent(vlnv), 
designRef_(),
generatorChainConfigurations_(),
interconnectionConfigurations_(),
viewConfigurations_(),
description_(), 
attributes_(),
assertions_(new QList<QSharedPointer<Assertion> > ),
parameters_(new QList<QSharedPointer<Parameter> >)
{
	LibraryComponent::vlnv_->setType(VLNV::DESIGNCONFIGURATION);
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::DesignConfiguration()
//-----------------------------------------------------------------------------
DesignConfiguration::DesignConfiguration( const DesignConfiguration& other ):
LibraryComponent(other),
designRef_(other.designRef_),
generatorChainConfigurations_(),
interconnectionConfigurations_(),
viewConfigurations_(),
description_(other.description_),
attributes_(other.attributes_),
assertions_(new QList<QSharedPointer<Assertion> > ),
parameters_(new QList<QSharedPointer<Parameter> > )
{
    foreach (QSharedPointer<ConfigurableVLNVReference> generatorChainConf, other.generatorChainConfigurations_)
    {
        if (generatorChainConf)
        {
            QSharedPointer<ConfigurableVLNVReference> copy = QSharedPointer<ConfigurableVLNVReference>(
                new ConfigurableVLNVReference(*generatorChainConf.data()));
            generatorChainConfigurations_.append(copy);
        }
    }
    foreach (QSharedPointer<InterconnectionConfiguration> configuration, other.interconnectionConfigurations_)
    {
        if (configuration)
        {
            QSharedPointer<InterconnectionConfiguration> copy = QSharedPointer<InterconnectionConfiguration>(
                new InterconnectionConfiguration(*configuration.data()));
            interconnectionConfigurations_.append(copy);
        }
    }

    foreach (QSharedPointer<ViewConfiguration> configuration, other.viewConfigurations_)
    {
        if (configuration)
        {
            QSharedPointer<ViewConfiguration> copy = QSharedPointer<ViewConfiguration>(
                new ViewConfiguration(*configuration.data()));
            viewConfigurations_.append(copy);
        }
    }

    foreach (QSharedPointer<Assertion> assertion, *other.assertions_)
    {
        if (assertion)
        {
            QSharedPointer<Assertion> copy = QSharedPointer<Assertion>(new Assertion(*assertion.data()));
            assertions_->append(copy);
        }
    }
    foreach (QSharedPointer<Parameter> parameter, *other.parameters_)
    {
        if (parameter)
        {
            QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(new Parameter(*parameter.data()));
            parameters_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::DesignConfiguration()
//-----------------------------------------------------------------------------
DesignConfiguration::DesignConfiguration():
LibraryComponent(),
designRef_(),
generatorChainConfigurations_(),
interconnectionConfigurations_(),
viewConfigurations_(),
description_(),
attributes_(),
assertions_(new QList<QSharedPointer<Assertion> >),
parameters_(new QList<QSharedPointer<Parameter> > )
{

}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::operator=()
//-----------------------------------------------------------------------------
DesignConfiguration& DesignConfiguration::operator=( const DesignConfiguration& other )
{
	if (this != &other)
    {
		LibraryComponent::operator=(other);
	
		designRef_ = other.designRef_;
		description_ = other.description_;
		attributes_ = other.attributes_;

        generatorChainConfigurations_.clear();
        foreach (QSharedPointer<ConfigurableVLNVReference> generatorChainConf, other.generatorChainConfigurations_)
        {
            if (generatorChainConf)
            {
                QSharedPointer<ConfigurableVLNVReference> copy = QSharedPointer<ConfigurableVLNVReference>
                    (new ConfigurableVLNVReference(*generatorChainConf.data()));
                generatorChainConfigurations_.append(copy);
			}
		}

        interconnectionConfigurations_.clear();
        foreach (QSharedPointer<InterconnectionConfiguration> configuration, other.interconnectionConfigurations_)
        {
            if (configuration)
            {
                QSharedPointer<InterconnectionConfiguration> copy = QSharedPointer<InterconnectionConfiguration>(
                    new InterconnectionConfiguration(*configuration.data()));
                interconnectionConfigurations_.append(copy);
			}
		}

        viewConfigurations_.clear();
        foreach (QSharedPointer<ViewConfiguration> configuration, other.viewConfigurations_)
        {
            if (configuration)
            {
                QSharedPointer<ViewConfiguration> copy = QSharedPointer<ViewConfiguration>(
                    new ViewConfiguration(*configuration.data()));
                viewConfigurations_.append(copy);
            }
        }

        foreach (QSharedPointer<Assertion> assertion, *other.assertions_)
        {
            if (assertion)
            {
                QSharedPointer<Assertion> copy = QSharedPointer<Assertion>(new Assertion(*assertion.data()));
                assertions_->append(copy);
            }
        }
        foreach (QSharedPointer<Parameter> parameter, *other.parameters_)
        {
            if (parameter)
            {
                QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(new Parameter(*parameter.data()));
                parameters_->append(copy);
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
    assertions_.clear();
    parameters_.clear();
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::clone()
//-----------------------------------------------------------------------------
QSharedPointer<LibraryComponent> DesignConfiguration::clone() const
{
	return QSharedPointer<LibraryComponent>(new DesignConfiguration(*this));	
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::isValid()
//-----------------------------------------------------------------------------
bool DesignConfiguration::isValid( QStringList& errorList ) const
{
	bool valid = true;
	QString thisIdentifier(QObject::tr("containing design configuration"));

	if (!vlnv_)
    {
		errorList.append(QObject::tr("No vlnv specified for the design configuration."));
		valid = false;
	}
	else if (!vlnv_->isValid(errorList, thisIdentifier))
    {
		valid = false;
	}
	else
    {
		thisIdentifier = QObject::tr("design configuration %1").arg(vlnv_->toString());
	}

	if (!designRef_.isValid(errorList, thisIdentifier))
    {
		valid = false;
	}

    foreach (QSharedPointer<ConfigurableVLNVReference> generatorChainConf, generatorChainConfigurations_)
    {
        if (!generatorChainConf->isValid(errorList, thisIdentifier))
        {
            valid = false;
        }
    }

    foreach (QSharedPointer<ViewConfiguration> configuration, viewConfigurations_)
    {
        if (configuration->getInstanceName().isEmpty())
        {
            errorList.append(QObject::tr("No instance name set for view configuration within %1").arg(thisIdentifier));
            valid = false;
        }
        if (configuration->getViewReference().isEmpty())
        {
            errorList.append(QObject::tr("No view name set for view configuration within %1").arg(thisIdentifier));
            valid = false;
        }
    }

	return valid;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::isValid()
//-----------------------------------------------------------------------------
bool DesignConfiguration::isValid() const
{
	if (!vlnv_)
    {
		return false;
	}
	else if (!vlnv_->isValid())
    {
		return false;
	}

	if (!designRef_.isValid())
    {
		return false;
	}

    foreach (QSharedPointer<ConfigurableVLNVReference> generatorChainConf, generatorChainConfigurations_)
    {
        if (!generatorChainConf->isValid())
        {
            return false;
        }
    }

    foreach (QSharedPointer<ViewConfiguration> configuration, viewConfigurations_)
    {
        if (configuration->getInstanceName().isEmpty() || configuration->getViewReference().isEmpty())
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::setVlnv()
//-----------------------------------------------------------------------------
void DesignConfiguration::setVlnv( const VLNV& vlnv )
{
    LibraryComponent::setVlnv(vlnv);
    LibraryComponent::vlnv_->setType(VLNV::DESIGNCONFIGURATION);
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::getAttributes()
//-----------------------------------------------------------------------------
const QMap<QString, QString>& DesignConfiguration::getAttributes()
{
    return attributes_;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::setAttributes()
//-----------------------------------------------------------------------------
void DesignConfiguration::setAttributes(const QMap<QString, QString>& attributes)
{
    attributes_ = attributes;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::setInterconnectionConfs()
//-----------------------------------------------------------------------------
void DesignConfiguration::setInterconnectionConfs(QList<QSharedPointer<InterconnectionConfiguration> >&
    interconnectionConfs)
{
    // delete old values
    interconnectionConfigurations_.clear();

    // save new values
    interconnectionConfigurations_ = interconnectionConfs;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::setViewConfigurations()
//-----------------------------------------------------------------------------
void DesignConfiguration::setViewConfigurations(QList<QSharedPointer<ViewConfiguration> > newViewConfigurations)
{
    // delete old values
    viewConfigurations_.clear();

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
const QList<QSharedPointer<InterconnectionConfiguration> >& DesignConfiguration::getInterconnectionConfs()
{
    return interconnectionConfigurations_;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::setDescription()
//-----------------------------------------------------------------------------
void DesignConfiguration::setDescription(const QString& description)
{
    description_ = description;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::getViewConfigurations()
//-----------------------------------------------------------------------------
QList<QSharedPointer<ViewConfiguration> > DesignConfiguration::getViewConfigurations()
{
    return viewConfigurations_;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::getDescription()
//-----------------------------------------------------------------------------
QString DesignConfiguration::getDescription() const
{
    return description_;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::setGeneratorChainConfs()
//-----------------------------------------------------------------------------
void DesignConfiguration::setGeneratorChainConfs(QList<QSharedPointer<ConfigurableVLNVReference> > generatorChainConfs)
{
    // delete old generatorChainConfs
    generatorChainConfigurations_.clear();

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
const QList<QSharedPointer<ConfigurableVLNVReference> > DesignConfiguration::getGeneratorChainConfs()
{
    return generatorChainConfigurations_;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::getDependentFiles()
//-----------------------------------------------------------------------------
const QStringList DesignConfiguration::getDependentFiles() const 
{
    // DesignConfiguration does not have any dependency files, it only has vlvn dependencies.
    return QStringList();
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::getDependentVLNVs()
//-----------------------------------------------------------------------------
const QList<VLNV> DesignConfiguration::getDependentVLNVs() const
{
	QList<VLNV> vlnvList;

	// Append the vlnv of the design that is associated with this designConfiguration.
	if (designRef_.isValid())
    {
		vlnvList.append(designRef_);
	}

	// Append all vlnvs representing generator chains associated with this design configuration.
    for (int i = 0; i < generatorChainConfigurations_.size(); ++i)
    {
        if (generatorChainConfigurations_.at(i)->isValid())
        {
            VLNV generatorChainVLVN = *generatorChainConfigurations_.at(i);
            vlnvList.append(generatorChainVLVN);
		}
	}

	// Append all vlnvs referencing the abstractors that this designConfiguration depends on.
    for (int i = 0; i < interconnectionConfigurations_.size(); ++i)
    {
        vlnvList.append(interconnectionConfigurations_.at(i)->getDependantVLNVs());
	}

	return vlnvList;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::addViewConfiguration()
//-----------------------------------------------------------------------------
void DesignConfiguration::addViewConfiguration( const QString& instanceName, const QString& viewName )
{
	if (!viewName.isEmpty())
    {
        QSharedPointer<ViewConfiguration> newViewConfig (new ViewConfiguration(instanceName));
        newViewConfig->setViewReference(viewName);
	}
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::removeViewConfiguration()
//-----------------------------------------------------------------------------
void DesignConfiguration::removeViewConfiguration( const QString& instanceName )
{
    foreach (QSharedPointer<ViewConfiguration> configuration, viewConfigurations_)
    {
        if (configuration->getInstanceName() == instanceName)
        {
            viewConfigurations_.removeAll(configuration);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::getActiveView()
//-----------------------------------------------------------------------------
QString DesignConfiguration::getActiveView( const QString& instanceName ) const
{
    foreach (QSharedPointer<ViewConfiguration> configuration, viewConfigurations_)
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
bool DesignConfiguration::hasActiveView( const QString& instanceName ) const
{
	return !getActiveView(instanceName).isEmpty();
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::setDesignConfigImplementation()
//-----------------------------------------------------------------------------
void DesignConfiguration::setDesignConfigImplementation(KactusAttribute::Implementation implementation)
{
    kactus2Attributes_.insert("kts_implementation", KactusAttribute::valueToString(implementation));
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::getDesignConfigImplementation()
//-----------------------------------------------------------------------------
KactusAttribute::Implementation DesignConfiguration::getDesignConfigImplementation() const
{
    KactusAttribute::Implementation implementation = KactusAttribute::HW;

    if (!kactus2Attributes_.contains(QString("kts_implementation")))
        return implementation;
    else
        KactusAttribute::stringToValue(kactus2Attributes_.value(QString("kts_implementation")), implementation);
    return implementation;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::getConfigurableElementValues()
//-----------------------------------------------------------------------------
QMap<QString, QString> DesignConfiguration::getConfigurableElementValues(QString const& instanceUUID) const
{
    QMap<QString, QString> configurableElements;
    foreach(QSharedPointer<VendorExtension> extension, vendorExtensions_)
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
    foreach (QSharedPointer<VendorExtension> extension, vendorExtensions_)
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
    foreach (QSharedPointer<VendorExtension> extension, vendorExtensions_)
    {
        if (extension->type() == "kactus2:viewOverrides")
        {
            vendorExtensions_.removeAll(extension);
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

        vendorExtensions_.append(overrideGroup);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::findOrCreateInstanceExtension()
//-----------------------------------------------------------------------------
QSharedPointer<VendorExtension> DesignConfiguration::findOrCreateInstanceExtension(QString const& instanceUUID)
{
    QSharedPointer<Kactus2Group> cevGroup;
    foreach(QSharedPointer<VendorExtension> extension, vendorExtensions_)
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
        vendorExtensions_.append(cevGroup);
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

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::parseVendorExtensions()
//-----------------------------------------------------------------------------
void DesignConfiguration::parseVendorExtensions(QDomNode const& extensionsNode)
{
    QDomNodeList childNodes = extensionsNode.childNodes();
    for (int i = 0; i < childNodes.size(); i++)
    {
        QDomNode extensionNode = childNodes.at(i);

        if (extensionNode.nodeName() == "kactus2:extensions")
        {
            for (int j = 0; j < extensionNode.childNodes().size(); j++)
            {
                QDomNode ktsExtensionNode = extensionNode.childNodes().at(j);
                if (ktsExtensionNode.nodeName() == "kactus2:kts_attributes")
                {
                    parseKactus2Attributes(ktsExtensionNode);
                }
            }
        }
        else if (extensionNode.nodeName() == "kactus2:configurableElementValues")
        {
            parseConfigurableElementValues(extensionNode);
        }

        else if (extensionNode.nodeName() == "kactus2:viewOverrides")
        {
            parseViewConfigurationValues(extensionNode);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::parseConfigurableElementValues()
//-----------------------------------------------------------------------------
void DesignConfiguration::parseConfigurableElementValues(QDomNode const& extensionNode)
{
    QSharedPointer<Kactus2Group> configurableElementsGroup(new Kactus2Group("kactus2:configurableElementValues"));
    for (int i = 0; i < extensionNode.childNodes().size(); i++)
    {
        QSharedPointer<Kactus2Group> instanceGroup(new Kactus2Group("kactus2:componentInstance"));

        QDomNode instanceNode = extensionNode.childNodes().at(i);
        for (int j = 0; j < instanceNode.childNodes().size(); j++)
        {
            QDomNode leafNode = instanceNode.childNodes().at(j);
            if (leafNode.nodeName() == "kactus2:uuid")
            {
                QString groupUUID = leafNode.childNodes().at(0).nodeValue();
                QSharedPointer<VendorExtension> uuidValue(new Kactus2Value("kactus2:uuid", groupUUID));

                instanceGroup->addToGroup(uuidValue);
            }
            else if (leafNode.nodeName() == "kactus2:configurableElementValue")                    
            {
                QSharedPointer<Kactus2Placeholder> valueHolder(
                    new Kactus2Placeholder("kactus2:configurableElementValue"));

                QDomNamedNodeMap attributes = leafNode.attributes();
                valueHolder->setAttribute("referenceId", attributes.namedItem("referenceId").nodeValue());
                valueHolder->setAttribute("value", attributes.namedItem("value").nodeValue());

                instanceGroup->addToGroup(valueHolder);
            }
        }

        configurableElementsGroup->addToGroup(instanceGroup);
    }

    vendorExtensions_.append(configurableElementsGroup);
}

//-----------------------------------------------------------------------------
// Function: designconfiguration::parseViewConfigurationValues()
//-----------------------------------------------------------------------------
void DesignConfiguration::parseViewConfigurationValues(QDomNode const& extensionNode)
{
    QSharedPointer<Kactus2Group> viewGroup(new Kactus2Group("kactus2:viewOverrides"));

    for (int i = 0; i < extensionNode.childNodes().size(); ++i)
    {
        QDomNode instanceNode = extensionNode.childNodes().at(i);
        QDomNamedNodeMap attributes = instanceNode.attributes();

        QSharedPointer<Kactus2Placeholder> viewItem(new Kactus2Placeholder("kactus2:instanceView"));
        viewItem->setAttribute("id", attributes.namedItem("id").nodeValue());
        viewItem->setAttribute("viewName", attributes.namedItem("viewName").nodeValue());

        viewGroup->addToGroup(viewItem);
    }

    vendorExtensions_.append(viewGroup);
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::getAssertions()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Assertion> > > DesignConfiguration::getAssertions() const
{
    return assertions_;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::setAssertions()
//-----------------------------------------------------------------------------
void DesignConfiguration::setAssertions(QSharedPointer<QList<QSharedPointer<Assertion> > > newAssertions)
{
    assertions_ = newAssertions;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::getParameters()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Parameter> > > DesignConfiguration::getParameters() const
{
    return parameters_;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::setParameters()
//-----------------------------------------------------------------------------
void DesignConfiguration::setParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters)
{
    parameters_ = newParameters;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::write()
//-----------------------------------------------------------------------------
void DesignConfiguration::write(QFile& file)
{

}