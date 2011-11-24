/* 
 *
 *  Created on: 30.8.2010
 *      Author: Antti Kamppi
 */

#include "librarycomponent.h"
#include "designconfiguration.h"
#include "generaldeclarations.h"
#include <LibraryManager/vlnv.h>
#include "designconfabstractor.h"
#include "generatorchain.h"

#include "../exceptions/parse_error.h"
#include "../exceptions/write_error.h"

#include <QList>
#include <QDomNode>
#include <QMap>
#include <QString>
#include <QSharedPointer>
#include <QDomNamedNodeMap>
#include <QObject>
#include <QFile>
#include <QXmlStreamWriter>

// struct constructor
DesignConfiguration::InterconnectionConf::InterconnectionConf(
    QDomNode& interconnectionNode): interconnectionRef_(QString()),
				    abstractors_() {

    // go through all child items
    for (int i = 0; i < interconnectionNode.childNodes().count(); ++i) {
	QDomNode tempNode = interconnectionNode.childNodes().at(i);

	if (tempNode.nodeName() == QString("spirit:interconnectionRef")) {
	    interconnectionRef_ = tempNode.childNodes().at(0).nodeValue();
	}

	else if (tempNode.nodeName() == QString("spirit:abstractors")) {
	    // go through each abstractor element
	    for (int j = 0; j < tempNode.childNodes().count(); ++j) {
		QDomNode abstractorNode = tempNode.childNodes().at(j);
		abstractors_.append(QSharedPointer<DesignConfAbstractor>(
					new DesignConfAbstractor(abstractorNode)));
	    }
	}
    }

    // if mandatory elements are missing
    if (interconnectionRef_.isNull()) {
	throw Parse_error(QObject::tr("Mandatory element spirit:connectionRef"
				      " missing in spirit:interconnectionConfiguration"));
    }

    if (abstractors_.size() == 0) {
	throw Parse_error(QObject::tr("Mandatory element spirit:abstractor"
				      " missing in spirit:interconnectionConfiguration"));
    }
    return;
}

DesignConfiguration::InterconnectionConf::InterconnectionConf( const InterconnectionConf& other ):
interconnectionRef_(other.interconnectionRef_),
abstractors_() {

	foreach (QSharedPointer<DesignConfAbstractor> abstractor, other.abstractors_) {
		
		if (abstractor) {
			QSharedPointer<DesignConfAbstractor> copy = QSharedPointer<DesignConfAbstractor>(
				new DesignConfAbstractor(*abstractor.data()));
			abstractors_.append(copy);
		}
	}
}

DesignConfiguration::InterconnectionConf& 
DesignConfiguration::InterconnectionConf::operator=( const InterconnectionConf& other ) {
	if (this != &other) {
		interconnectionRef_ = other.interconnectionRef_;

		abstractors_.clear();
		foreach (QSharedPointer<DesignConfAbstractor> abstractor, other.abstractors_) {

			if (abstractor) {
				QSharedPointer<DesignConfAbstractor> copy = QSharedPointer<DesignConfAbstractor>(
					new DesignConfAbstractor(*abstractor.data()));
				abstractors_.append(copy);
			}
		}
	}
	return *this;
}

// struct constructor
DesignConfiguration::GeneratorChainConfiguration::GeneratorChainConfiguration(
    QDomNode& genChainNode): generatorChainRef_(),
			     configurableElements_() {
    // go through all child elements
    for (int i = 0; i < genChainNode.childNodes().count(); ++i) {
	QDomNode tempNode = genChainNode.childNodes().at(i);

	if (tempNode.nodeName() == QString("spirit:generatorChainRef")) {
	    generatorChainRef_ = General::createVLNV(tempNode, VLNV::GENERATORCHAIN);
	}

	else if (tempNode.nodeName() ==
		 QString("spirit:configurableElementValues")) {
	    for (int j = 0; j < tempNode.childNodes().count(); ++j) {
		QDomNode confElementNode = tempNode.childNodes().at(j);

		// get value of the element
		QString value = confElementNode.childNodes().at(0).nodeValue();

		// get the reference ID attribute
		QDomNamedNodeMap attributeMap = confElementNode.attributes();
		QString referenceID = attributeMap.namedItem(QString(
								 "spirit:referenceId")).nodeValue();

		// if one of the mandatory elements is missing
		if (value.isNull()) {
		    throw Parse_error(QObject::tr("Mandatory element spirit:"
						  "configurableElementValue is missing in spirit:"
						  "generatorChainConfiguration"));
		}

		if (referenceID.isNull()) {
		    throw Parse_error(QObject::tr("Mandatory element spirit:"
						  "referenceId is missing in spirit:configurable"
						  "ElementValue"));
		}
		configurableElements_.insert(referenceID, value);
	    }
	}
    }

    // if mandatory element generatorChainRef is missing
    if (!generatorChainRef_.isValid()) {
	throw Parse_error(QObject::tr("Mandatory element spirit:generator:"
				      "ChainRef is missing in spirit:generatorChainConfiguration"));
    }
    return;
}

DesignConfiguration::GeneratorChainConfiguration::GeneratorChainConfiguration(
	const GeneratorChainConfiguration& other ):
generatorChainRef_(other.generatorChainRef_),
configurableElements_(other.configurableElements_) {
}

DesignConfiguration::GeneratorChainConfiguration& 
DesignConfiguration::GeneratorChainConfiguration::operator=( 
	const GeneratorChainConfiguration& other ) {

		if (this != &other) {
			generatorChainRef_ = other.generatorChainRef_;
			configurableElements_ = other.configurableElements_;
		}
		return *this;
}

// class constructor
DesignConfiguration::DesignConfiguration(QDomDocument& doc):
LibraryComponent(doc), 
designRef_(),
generatorChainConfs_(), 
interconnectionConfs_(),
viewConfigurations_(), 
description_(), 
attributes_() {

	LibraryComponent::vlnv_->setType(VLNV::DESIGNCONFIGURATION);

	// find the IP-Xact root element (spirit:component or spirit:design ...)
	QDomNodeList nodeList = doc.childNodes();
	int i = 0;
	// search for the first element with children (=document type)
	while (!nodeList.at(i).hasChildNodes()) {
		++i;
	}

	QDomNode designConfNode = doc.childNodes().item(i);

	// parse the attributes
	General::parseAttributes(designConfNode, attributes_);

	// go through all childNodes
	for (int i = 0; i < designConfNode.childNodes().count(); ++i) {
		QDomNode tempNode = designConfNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:designRef")) {
			designRef_ = General::createVLNV(tempNode, VLNV::DESIGN);
		}

		else if (tempNode.nodeName() ==
			QString("spirit:generatorChainConfiguration")) {
				generatorChainConfs_.append(
					QSharedPointer<GeneratorChainConfiguration>(
					new GeneratorChainConfiguration(tempNode)));
		}

		else if (tempNode.nodeName() ==
			QString("spirit:interconnectionConfiguration")) {
				interconnectionConfs_.append(
					QSharedPointer<InterconnectionConf>(
					new InterconnectionConf(tempNode)));
		}

		else if (tempNode.nodeName() ==
			QString("spirit:viewConfiguration")) {

				QString instanceName;
				QString viewName;

				// search both child elements of the viewConfiguration element
				for (int j = 0; j < tempNode.childNodes().count(); ++j) {
					QDomNode viewNode = tempNode.childNodes().at(j);

					if (viewNode.nodeName() == QString("spirit:instanceName")) {
						instanceName = viewNode.childNodes().at(0).nodeValue();
					}
					else if (viewNode.nodeName() ==
						QString("spirit:viewName")) {
							viewName = viewNode.childNodes().at(0).nodeValue();
					}
				}

				// add the child elements into the QMap
				viewConfigurations_.insert(instanceName, viewName);
		}

		else if (tempNode.nodeName() ==
			QString("spirit:description")) {
				description_ = tempNode.childNodes().at(0).nodeValue();
		}
	}

	// if mandatory elemets are missing
	if (!designRef_.isValid()) {
		throw Parse_error(QObject::tr("Mandatory element spirit:designRef missing "
			"in spirit:designConfiguration"));
	}
	return;
}

DesignConfiguration::DesignConfiguration( const VLNV& vlnv ):
LibraryComponent(vlnv), 
designRef_(),
generatorChainConfs_(),
interconnectionConfs_(),
viewConfigurations_(),
description_(), 
attributes_() {
	LibraryComponent::vlnv_->setType(VLNV::DESIGNCONFIGURATION);
}

DesignConfiguration::DesignConfiguration( const DesignConfiguration& other ):
LibraryComponent(other),
designRef_(other.designRef_),
generatorChainConfs_(),
interconnectionConfs_(),
viewConfigurations_(other.viewConfigurations_),
description_(other.description_),
attributes_(other.attributes_) {

	foreach (QSharedPointer<GeneratorChainConfiguration> genConf, other.generatorChainConfs_) {
		if (genConf) {
			QSharedPointer<GeneratorChainConfiguration> copy = 
				QSharedPointer<GeneratorChainConfiguration>(
				new GeneratorChainConfiguration(*genConf.data()));
			generatorChainConfs_.append(copy);
		}
	}

	foreach (QSharedPointer<InterconnectionConf> intConf, other.interconnectionConfs_) {
		if (intConf) {
			QSharedPointer<InterconnectionConf> copy = QSharedPointer<InterconnectionConf>(
				new InterconnectionConf(*intConf.data()));
			interconnectionConfs_.append(copy);
		}
	}
}

DesignConfiguration::DesignConfiguration():
LibraryComponent(),
designRef_(),
generatorChainConfs_(),
interconnectionConfs_(),
viewConfigurations_(),
description_(),
attributes_() {
}

DesignConfiguration& DesignConfiguration::operator=( const DesignConfiguration& other ) {
	if (this != &other) {
		LibraryComponent::operator=(other);
	
		designRef_ = other.designRef_;
		viewConfigurations_ = other.viewConfigurations_;
		description_ = other.description_;
		attributes_ = other.attributes_;

		generatorChainConfs_.clear();
		foreach (QSharedPointer<GeneratorChainConfiguration> genConf, other.generatorChainConfs_) {
			if (genConf) {
				QSharedPointer<GeneratorChainConfiguration> copy = 
					QSharedPointer<GeneratorChainConfiguration>(
					new GeneratorChainConfiguration(*genConf.data()));
				generatorChainConfs_.append(copy);
			}
		}

		interconnectionConfs_.clear();
		foreach (QSharedPointer<InterconnectionConf> intConf, other.interconnectionConfs_) {
			if (intConf) {
				QSharedPointer<InterconnectionConf> copy = QSharedPointer<InterconnectionConf>(
					new InterconnectionConf(*intConf.data()));
				interconnectionConfs_.append(copy);
			}
		}
	}
	return *this;
}

// destructor
DesignConfiguration::~DesignConfiguration() {
    generatorChainConfs_.clear();
    interconnectionConfs_.clear();
    viewConfigurations_.clear();
}

QSharedPointer<LibraryComponent> DesignConfiguration::clone() {
	return QSharedPointer<LibraryComponent>(new DesignConfiguration(*this));	
}

void DesignConfiguration::write(QFile& file) {
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
    General::writeAttributes(writer, attributes_);

    // call base class to write the VLNV info
    LibraryComponent::writeVLNV(writer);

    // if mandatory reference to a design is missing
    if (!designRef_.isValid()) {
    	throw Write_error(QObject::tr("Mandatory element spirit:designRef "
    			"missing in spirit:designConfiguration"));
    }
    else {
    	writer.writeEmptyElement("spirit:designRef");
    	General::writeVLNVAttributes(writer, &designRef_);
    }


    for (int i = 0; i < generatorChainConfs_.size(); ++i) {
    	writer.writeStartElement("spirit:generatorChainConfiguration");

    	// if mandatory reference to a generator chain is missing
    	if (!generatorChainConfs_.at(i)->generatorChainRef_.isValid()) {
    		throw Write_error(QObject::tr("Mandatory element "
    				"spirit:generatorChainRef missing in "
    				"spirit:generatorChainConfiguration"));
    	}
    	else {
    		writer.writeEmptyElement("spirit:generatorChainRef");
    		General::writeVLNVAttributes(writer,
    				&generatorChainConfs_.at(i)->generatorChainRef_);
    	}

    	if (generatorChainConfs_.at(i)->configurableElements_.size() != 0) {
    		writer.writeStartElement("spirit:configurableElementValues");

                for (QMap<QString, QString>::iterator j =
    				generatorChainConfs_.at(i)->configurableElements_.begin();
                                j != generatorChainConfs_.at(i)->configurableElements_.end();
                                ++j) {

    			writer.writeStartElement("spirit:configurableElementValue");
                        writer.writeAttribute("spirit:referenceId", j.key());
                        writer.writeCharacters(j.value());
    			writer.writeEndElement(); // spirit:configurableElementValue
    		}

    		writer.writeEndElement(); // spirit:configurableElementValues
    	}

    	writer.writeEndElement(); // spirit::generatorChainConfiguration
    }

    for (int i = 0; i < interconnectionConfs_.size(); ++i) {
    	writer.writeStartElement("spirit:interconnectionConfiguration");

    	// if mandatory reference to an interconnection is missing
    	if (interconnectionConfs_.at(i)->interconnectionRef_.isEmpty()) {
    		throw Write_error(QObject::tr("Mandatory element "
    				"spirit:interconnectionRef is missing in "
    				"spirit:interconnectionConfiguration"));
    	}
    	else {
                writer.writeTextElement("spirit:interconnectionRef",
                                        interconnectionConfs_.at(i)->interconnectionRef_);
    	}

    	// at least one abstractor must exist
    	if (interconnectionConfs_.at(i)->abstractors_.size() == 0) {
    		throw Write_error(QObject::tr("Mandatory element spirit:abstractors"
    				" missing in spirit:interconnectionConfiguration"));
    	}
    	else {
    		writer.writeStartElement("spirit:abstractors");

    		// all abstractors write themselves
    		for (int j = 0; j < interconnectionConfs_.at(i)->abstractors_.size();
    				++j) {
    			interconnectionConfs_.at(i)->abstractors_.at(j)->write(writer);
    		}

    		writer.writeEndElement(); // spirit:abstractors
    	}

    	writer.writeEndElement(); // spirit:interconnectionConfiguration
    }

    for (QMap<QString, QString>::iterator i = viewConfigurations_.begin();
    		i != viewConfigurations_.end(); ++i) {
    	writer.writeStartElement("spirit:viewConfiguration");

    	writer.writeTextElement("spirit:instanceName", i.key());
    	writer.writeTextElement("spirit:viewName", i.value());

    	writer.writeEndElement(); // spirit:viewConfiguration
    }

    if (!LibraryComponent::description_.isEmpty()) {
    	writer.writeTextElement("spirit:description",
    			LibraryComponent::description_);
    }

    writer.writeEndElement(); // spirit:designConfiguration
    writer.writeEndDocument();
    return;
}

// get the attributes
const QMap<QString, QString>& DesignConfiguration::getAttributes() {
    return attributes_;
}

// set the attributes
void DesignConfiguration::setAttributes(const QMap<QString, QString>& attributes) {
    attributes_ = attributes;
}

void DesignConfiguration::setInterconnectionConfs(
    QList<QSharedPointer<InterconnectionConf> >& interconnectionConfs) {
    // delete old values
    interconnectionConfs_.clear();

    // save new values
    interconnectionConfs_ = interconnectionConfs;
}

void DesignConfiguration::setViewConfigurations(
    QMap<QString,QString>& viewConfigurations) {
    // delete old values
    viewConfigurations_.clear();

    // save new values
    viewConfigurations_ = viewConfigurations;
}

void DesignConfiguration::setDesignRef(const VLNV& designRef) {

    // save new designRef
    designRef_ = designRef;
}

const QList<QSharedPointer<DesignConfiguration::InterconnectionConf> >&
DesignConfiguration::getInterconnectionConfs() {
    return interconnectionConfs_;
}

void DesignConfiguration::setDescription(const QString& description) {
    description_ = description;
}

const QMap<QString,QString>& DesignConfiguration::getViewConfigurations() {
    return viewConfigurations_;
}

QString DesignConfiguration::getDescription() const {
    return description_;
}

void DesignConfiguration::setGeneratorChainConfs(
    QList<QSharedPointer<GeneratorChainConfiguration> >&
    generatorChainConfs) {
    // delete old generatorChainConfs
    generatorChainConfs_.clear();

    // save new generatorChainConfs
    generatorChainConfs_ = generatorChainConfs;
}

VLNV DesignConfiguration::getDesignRef() const {
    return designRef_;
}

const QList<QSharedPointer<DesignConfiguration::GeneratorChainConfiguration> >&
DesignConfiguration::getGeneratorChainConfs() {
    return generatorChainConfs_;
}

// DesignConfiguration does not have any dependency files, it only has vlvn
// dependencies.
const QStringList DesignConfiguration::getDependentFiles() const {
    return QStringList();
}

const QList<VLNV> DesignConfiguration::getDependentVLNVs() const {
	QList<VLNV> vlnvList;

	// append the vlnv of the design that is associated with this
	// designConfiguration
	if (designRef_.isValid()) {
		vlnvList.append(designRef_);
	}

	// append all vlnvs representing generator chains associated with this
	// design configuration.
	for (int i = 0; i < generatorChainConfs_.size(); ++i) {
		if (generatorChainConfs_.at(i)->generatorChainRef_.isValid()) {
			vlnvList.append(generatorChainConfs_.at(i)->generatorChainRef_);
		}
	}

	// append all vlnvs referencing the abstractors that this
	// designConfiguration depends on.
	for (int i = 0; i < interconnectionConfs_.size(); ++i) {
		for (int j = 0; j < interconnectionConfs_.at(i)->abstractors_.size();
				++j) {

			if (interconnectionConfs_.at(i)->abstractors_.
					at(j)->getAbstractorRef().isValid()) {
				vlnvList.append(interconnectionConfs_.at(i)->abstractors_.
						at(j)->getAbstractorRef());
			}
		}
	}
	return vlnvList;
}

void DesignConfiguration::addViewConfiguration( const QString& instanceName, 
											   const QString& viewName ) {

	viewConfigurations_.insert(instanceName, viewName);
}

void DesignConfiguration::removeViewConfiguration( const QString& instanceName ) {
	viewConfigurations_.remove(instanceName);
}

QString DesignConfiguration::getActiveView( const QString& instanceName ) const {
	
	// if instance has an active view specified.
	if (viewConfigurations_.contains(instanceName))
		return viewConfigurations_.value(instanceName);

	// if no view is set for component instance
	return QString();
}

bool DesignConfiguration::hasActiveView( const QString& instanceName ) const {
	return viewConfigurations_.contains(instanceName);
}

void DesignConfiguration::setVlnv( const VLNV& vlnv ) {
	LibraryComponent::setVlnv(vlnv);
	LibraryComponent::vlnv_->setType(VLNV::DESIGNCONFIGURATION);
}
