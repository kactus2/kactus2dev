/* 
 *
 *  Created on: 30.8.2010
 *      Author: Antti Kamppi
 */

#include "librarycomponent.h"

#include "designconfabstractor.h"
#include "designconfiguration.h"
#include "generatorchain.h"
#include "vlnv.h"
#include "XmlUtils.h"

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
#include <QFile>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::InterconnectionConf::InterconnectionConf()
//-----------------------------------------------------------------------------
DesignConfiguration::InterconnectionConf::InterconnectionConf(QDomNode& interconnectionNode): 
interconnectionRef_(QString()), 
    abstractors_()
{
    for (int i = 0; i < interconnectionNode.childNodes().count(); ++i)
    {
        QDomNode tempNode = interconnectionNode.childNodes().at(i);
        if (tempNode.nodeName() == QString("spirit:interconnectionRef"))
        {
            interconnectionRef_ = tempNode.childNodes().at(0).nodeValue();
        }
        else if (tempNode.nodeName() == QString("spirit:abstractors"))
        {
            for (int j = 0; j < tempNode.childNodes().count(); ++j)
            {
                QDomNode abstractorNode = tempNode.childNodes().at(j);
                abstractors_.append(QSharedPointer<DesignConfAbstractor>(new DesignConfAbstractor(abstractorNode)));
            }
        }       
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::InterconnectionConf::InterconnectionConf()
//-----------------------------------------------------------------------------
DesignConfiguration::InterconnectionConf::InterconnectionConf( const InterconnectionConf& other ):
interconnectionRef_(other.interconnectionRef_),
abstractors_() {

	foreach (QSharedPointer<DesignConfAbstractor> abstractor, other.abstractors_) 
    {
		if (abstractor)
        {
			QSharedPointer<DesignConfAbstractor> copy = QSharedPointer<DesignConfAbstractor>(
				new DesignConfAbstractor(*abstractor.data()));
			abstractors_.append(copy);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::DesignConfiguration::InterconnectionConf::operator=()
//-----------------------------------------------------------------------------
DesignConfiguration::InterconnectionConf& 
DesignConfiguration::InterconnectionConf::operator=( const InterconnectionConf& other )
{
	if (this != &other)
    {
		interconnectionRef_ = other.interconnectionRef_;

		abstractors_.clear();
		foreach (QSharedPointer<DesignConfAbstractor> abstractor, other.abstractors_) 
        {
			if (abstractor)
            {
				QSharedPointer<DesignConfAbstractor> copy = QSharedPointer<DesignConfAbstractor>(
					new DesignConfAbstractor(*abstractor.data()));
				abstractors_.append(copy);
			}
		}
	}
	return *this;
}


//-----------------------------------------------------------------------------
// Function: DesignConfiguration::GeneratorChainConfiguration::GeneratorChainConfiguration()
//-----------------------------------------------------------------------------
DesignConfiguration::GeneratorChainConfiguration::GeneratorChainConfiguration(QDomNode& genChainNode):
generatorChainRef_(),
    configurableElements_()
{
    for (int i = 0; i < genChainNode.childNodes().count(); ++i) 
    {
        QDomNode tempNode = genChainNode.childNodes().at(i);

        if (tempNode.nodeName() == QString("spirit:generatorChainRef"))
        {
            generatorChainRef_ = VLNV::createVLNV(tempNode, VLNV::GENERATORCHAIN);
        }
        else if (tempNode.nodeName() == "spirit:configurableElementValues") 
        {
            for (int j = 0; j < tempNode.childNodes().count(); ++j)
            {
                QDomNode confElementNode = tempNode.childNodes().at(j);

                // get value of the element
                QString value = confElementNode.childNodes().at(0).nodeValue();

                // get the reference ID attribute
                QDomNamedNodeMap attributeMap = confElementNode.attributes();
                QString referenceID = attributeMap.namedItem(QString("spirit:referenceId")).nodeValue();

                configurableElements_.insert(referenceID, value);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::GeneratorChainConfiguration::GeneratorChainConfiguration()
//-----------------------------------------------------------------------------
DesignConfiguration::GeneratorChainConfiguration::GeneratorChainConfiguration(
    const GeneratorChainConfiguration& other ):
generatorChainRef_(other.generatorChainRef_),
configurableElements_(other.configurableElements_)
{
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::GeneratorChainConfiguration::operator=()
//-----------------------------------------------------------------------------
DesignConfiguration::GeneratorChainConfiguration& DesignConfiguration::GeneratorChainConfiguration::operator=(
	const GeneratorChainConfiguration& other )
{
    if (this != &other)
    {
        generatorChainRef_ = other.generatorChainRef_;
        configurableElements_ = other.configurableElements_;
    }
    return *this;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::GeneratorChainConfiguration::isValid()
//-----------------------------------------------------------------------------
bool DesignConfiguration::GeneratorChainConfiguration::isValid(QStringList& errorList, 
    const QString& parentIdentifier ) const
{
	bool valid = true;

	if (!generatorChainRef_.isValid(errorList,
		QObject::tr("generator chain configuration within %1").arg(parentIdentifier)))
    {
		valid = false;
	}

	for (QMap<QString, QString>::const_iterator i = configurableElements_.begin();
        i != configurableElements_.end(); ++i)
    {
        if (i.key().isEmpty())
        {
            errorList.append(QObject::tr("No configurable element value set"
                " for generator chain configuration within %1").arg(parentIdentifier));
            valid = false;
        }
        if (i.value().isEmpty())
        {
            errorList.append(QObject::tr("No reference id set for configurable"
                " element value in generator chain configuration within %1").arg(parentIdentifier));
            valid = false;
        }
    }

	return valid;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::GeneratorChainConfiguration::isValid()
//-----------------------------------------------------------------------------
bool DesignConfiguration::GeneratorChainConfiguration::isValid() const
{
    if (!generatorChainRef_.isValid())
    {
        return false;
    }

    for (QMap<QString, QString>::const_iterator i = configurableElements_.begin();
        i != configurableElements_.end(); ++i)
    {
        if (i.key().isEmpty() || i.value().isEmpty())
        {
            return false;
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::DesignConfiguration()
//-----------------------------------------------------------------------------
DesignConfiguration::DesignConfiguration(QDomDocument& doc):
LibraryComponent(doc), 
designRef_(),
generatorChainConfs_(), 
interconnectionConfs_(),
viewConfigurations_(), 
description_(), 
attributes_()
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
            generatorChainConfs_.append(QSharedPointer<GeneratorChainConfiguration>(
                new GeneratorChainConfiguration(tempNode)));
        }
        else if (tempNode.nodeName() ==	QString("spirit:interconnectionConfiguration"))
        {
            interconnectionConfs_.append(QSharedPointer<InterconnectionConf>(new InterconnectionConf(tempNode)));
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
            viewConfigurations_.insert(instanceName, viewName);
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
generatorChainConfs_(),
interconnectionConfs_(),
viewConfigurations_(),
description_(), 
attributes_()
{
	LibraryComponent::vlnv_->setType(VLNV::DESIGNCONFIGURATION);
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::DesignConfiguration()
//-----------------------------------------------------------------------------
DesignConfiguration::DesignConfiguration( const DesignConfiguration& other ):
LibraryComponent(other),
designRef_(other.designRef_),
generatorChainConfs_(),
interconnectionConfs_(),
viewConfigurations_(other.viewConfigurations_),
description_(other.description_),
attributes_(other.attributes_)
{
	foreach (QSharedPointer<GeneratorChainConfiguration> genConf, other.generatorChainConfs_)
    {
		if (genConf)
        {
			QSharedPointer<GeneratorChainConfiguration> copy = 
				QSharedPointer<GeneratorChainConfiguration>(
				new GeneratorChainConfiguration(*genConf.data()));
			generatorChainConfs_.append(copy);
		}
	}

	foreach (QSharedPointer<InterconnectionConf> intConf, other.interconnectionConfs_)
    {
		if (intConf)
        {
			QSharedPointer<InterconnectionConf> copy = QSharedPointer<InterconnectionConf>(
				new InterconnectionConf(*intConf.data()));
			interconnectionConfs_.append(copy);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::DesignConfiguration()
//-----------------------------------------------------------------------------
DesignConfiguration::DesignConfiguration():
LibraryComponent(),
designRef_(),
generatorChainConfs_(),
interconnectionConfs_(),
viewConfigurations_(),
description_(),
attributes_() {
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
		viewConfigurations_ = other.viewConfigurations_;
		description_ = other.description_;
		attributes_ = other.attributes_;

		generatorChainConfs_.clear();
		foreach (QSharedPointer<GeneratorChainConfiguration> genConf, other.generatorChainConfs_)
        {
			if (genConf)
            {
				QSharedPointer<GeneratorChainConfiguration> copy = QSharedPointer<GeneratorChainConfiguration>(
					new GeneratorChainConfiguration(*genConf.data()));
				generatorChainConfs_.append(copy);
			}
		}

		interconnectionConfs_.clear();
		foreach (QSharedPointer<InterconnectionConf> intConf, other.interconnectionConfs_)
        {
			if (intConf)
            {
				QSharedPointer<InterconnectionConf> copy = QSharedPointer<InterconnectionConf>(
					new InterconnectionConf(*intConf.data()));
				interconnectionConfs_.append(copy);
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
    generatorChainConfs_.clear();
    interconnectionConfs_.clear();
    viewConfigurations_.clear();
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::clone()
//-----------------------------------------------------------------------------
QSharedPointer<LibraryComponent> DesignConfiguration::clone() const
{
	return QSharedPointer<LibraryComponent>(new DesignConfiguration(*this));	
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::write()
//-----------------------------------------------------------------------------
void DesignConfiguration::write(QFile& file)
{
    // create a writer instance and set it to operate on the given file
    QXmlStreamWriter writer(&file);

    // writer automatically adds whitespaces to make document more readable
    writer.setAutoFormatting(true);
    // writer uses one tab as indentation
    writer.setAutoFormattingIndent(-1);

    // call the base class implementation to write the top comment and
    // vlvn info. It also starts the type element (i.e <spirit:busDefinition>)
    LibraryComponent::write(writer);

	// set the attributes
	setXMLNameSpaceAttributes(attributes_);

    // write the attributes for the spirit:designConfiguration element
    XmlUtils::writeAttributes(writer, attributes_);

    // call base class to write the VLNV info
    LibraryComponent::writeVLNV(writer);

	if (!LibraryComponent::description_.isEmpty())
    {
		writer.writeTextElement("spirit:description", LibraryComponent::description_);
	}

    writer.writeEmptyElement("spirit:designRef");
    designRef_.writeAsAttributes(writer);
    
    for (int i = 0; i < generatorChainConfs_.size(); ++i)
    {
    	writer.writeStartElement("spirit:generatorChainConfiguration");

    	writer.writeEmptyElement("spirit:generatorChainRef");
    	generatorChainConfs_.at(i)->generatorChainRef_.writeAsAttributes(writer);

    	if (generatorChainConfs_.at(i)->configurableElements_.size() != 0)
        {
    		writer.writeStartElement("spirit:configurableElementValues");

            for (QMap<QString, QString>::iterator j = generatorChainConfs_.at(i)->configurableElements_.begin();
                j != generatorChainConfs_.at(i)->configurableElements_.end(); ++j)
            {
                writer.writeStartElement("spirit:configurableElementValue");
                writer.writeAttribute("spirit:referenceId", j.key());
                writer.writeCharacters(j.value());
                writer.writeEndElement(); // spirit:configurableElementValue
            }

    		writer.writeEndElement(); // spirit:configurableElementValues
    	}

    	writer.writeEndElement(); // spirit::generatorChainConfiguration
    }

    for (int i = 0; i < interconnectionConfs_.size(); ++i)
    {
    	writer.writeStartElement("spirit:interconnectionConfiguration");
    	writer.writeTextElement("spirit:interconnectionRef", interconnectionConfs_.at(i)->interconnectionRef_);

    	if (interconnectionConfs_.at(i)->abstractors_.size() > 0)
        {
    		writer.writeStartElement("spirit:abstractors");

    		// all abstractors write themselves
    		for (int j = 0; j < interconnectionConfs_.at(i)->abstractors_.size(); ++j) {
    			interconnectionConfs_.at(i)->abstractors_.at(j)->write(writer);
    		}

    		writer.writeEndElement(); // spirit:abstractors
    	}

    	writer.writeEndElement(); // spirit:interconnectionConfiguration
    }

    for (QMap<QString, QString>::iterator i = viewConfigurations_.begin(); i != viewConfigurations_.end(); ++i)
    {
		// if the view name is not empty
		if (!i.value().isEmpty())
        {
			writer.writeStartElement("spirit:viewConfiguration");

			writer.writeTextElement("spirit:instanceName", i.key());
			writer.writeTextElement("spirit:viewName", i.value());

			writer.writeEndElement(); // spirit:viewConfiguration
		}
    }

    // if contains kactus2 attributes
    if (!kactus2Attributes_.isEmpty()) 
    {
        writer.writeStartElement("spirit:vendorExtensions");
        writer.writeStartElement("kactus2:extensions");

        writeKactus2Attributes(writer);

        writer.writeEndElement(); // kactus2:extensions

        writeVendorExtensions(writer);

        writer.writeEndElement(); // spirit:vendorExtensions
    }
    else if (!vendorExtensions_.isEmpty())
    {
        writer.writeStartElement("spirit:vendorExtensions");
        writeVendorExtensions(writer);
        writer.writeEndElement(); // spirit:vendorExtensions
    }

    writer.writeEndElement(); // spirit:designConfiguration
    writer.writeEndDocument();
    return;
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

	foreach (QSharedPointer<DesignConfiguration::GeneratorChainConfiguration> genConf, generatorChainConfs_)
    {
        if (!genConf->isValid(errorList, thisIdentifier))
        {
            valid = false;
        }
    }

	for (QMap<QString, QString>::const_iterator i = viewConfigurations_.begin(); i != viewConfigurations_.end(); 
        ++i)
    {
        if (i.key().isEmpty())
        {
            errorList.append(QObject::tr("No instance name set for view configuration within %1").arg(thisIdentifier));
            valid = false;
        }
        if (i.value().isEmpty())
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

    foreach (QSharedPointer<DesignConfiguration::GeneratorChainConfiguration> genConf, generatorChainConfs_)
    {
        if (!genConf->isValid())
        {
            return false;
        }
    }

    for (QMap<QString, QString>::const_iterator i = viewConfigurations_.begin(); i != viewConfigurations_.end(); 
        ++i)
    {
        if (i.key().isEmpty())
        {
            return false;
        }
        if (i.value().isEmpty())
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
void DesignConfiguration::setInterconnectionConfs(QList<QSharedPointer<InterconnectionConf> >& 
    interconnectionConfs)
{
    // delete old values
    interconnectionConfs_.clear();

    // save new values
    interconnectionConfs_ = interconnectionConfs;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::setViewConfigurations()
//-----------------------------------------------------------------------------
void DesignConfiguration::setViewConfigurations(QMap<QString,QString>& viewConfigurations)
{
    // delete old values
    viewConfigurations_.clear();

    // save new values
    viewConfigurations_ = viewConfigurations;
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
const QList<QSharedPointer<DesignConfiguration::InterconnectionConf> >& 
    DesignConfiguration::getInterconnectionConfs()
{
    return interconnectionConfs_;
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
const QMap<QString,QString>& DesignConfiguration::getViewConfigurations()
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
void DesignConfiguration::setGeneratorChainConfs(QList<QSharedPointer<GeneratorChainConfiguration> >&
    generatorChainConfs)
{
    // delete old generatorChainConfs
    generatorChainConfs_.clear();

    // save new generatorChainConfs
    generatorChainConfs_ = generatorChainConfs;
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
const QList<QSharedPointer<DesignConfiguration::GeneratorChainConfiguration> >&
DesignConfiguration::getGeneratorChainConfs()
{
    return generatorChainConfs_;
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
	for (int i = 0; i < generatorChainConfs_.size(); ++i)
    {
		if (generatorChainConfs_.at(i)->generatorChainRef_.isValid())
        {
			vlnvList.append(generatorChainConfs_.at(i)->generatorChainRef_);
		}
	}

	// Append all vlnvs referencing the abstractors that this designConfiguration depends on.
	for (int i = 0; i < interconnectionConfs_.size(); ++i)
    {
		for (int j = 0; j < interconnectionConfs_.at(i)->abstractors_.size(); ++j)
        {
			if (interconnectionConfs_.at(i)->abstractors_.at(j)->getAbstractorRef().isValid())
            {
				vlnvList.append(interconnectionConfs_.at(i)->abstractors_.at(j)->getAbstractorRef());
			}
		}
	}

	return vlnvList;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::addViewConfiguration()
//-----------------------------------------------------------------------------
void DesignConfiguration::addViewConfiguration( const QString& instanceName, const QString& viewName )
{
	if (!viewName.isEmpty()) {
		viewConfigurations_.insert(instanceName, viewName);
	}
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::removeViewConfiguration()
//-----------------------------------------------------------------------------
void DesignConfiguration::removeViewConfiguration( const QString& instanceName )
{
	viewConfigurations_.remove(instanceName);
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::getActiveView()
//-----------------------------------------------------------------------------
QString DesignConfiguration::getActiveView( const QString& instanceName ) const
{
	return viewConfigurations_.value(instanceName, QString());
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
        else
        {
            vendorExtensions_.append(QSharedPointer<VendorExtension>(new GenericVendorExtension(extensionNode)));
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
            QDomNode subNode = instanceNode.childNodes().at(j);
            if (subNode.nodeName() == "kactus2:uuid")
            {
                QString groupUUID = subNode.childNodes().at(0).nodeValue();
                QSharedPointer<VendorExtension> uuidValue(new Kactus2Value("kactus2:uuid", groupUUID));

                instanceGroup->addToGroup(uuidValue);
            }
            else if (subNode.nodeName() == "kactus2:configurableElementValue")                    
            {
                QSharedPointer<Kactus2Placeholder> valueHolder(
                    new Kactus2Placeholder("kactus2:configurableElementValue"));

                QDomNamedNodeMap attributes = subNode.attributes();
                valueHolder->setAttribute("referenceId", attributes.namedItem("referenceId").nodeValue());
                valueHolder->setAttribute("value", attributes.namedItem("value").nodeValue());

                instanceGroup->addToGroup(valueHolder);
            }
            else
            {
                instanceGroup->addToGroup(QSharedPointer<VendorExtension>(new GenericVendorExtension(subNode)));
            }
        }

        configurableElementsGroup->addToGroup(instanceGroup);
    }

    vendorExtensions_.append(configurableElementsGroup);
}
