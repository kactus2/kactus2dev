/* 
 *
 *  Created on: 11.8.2010
 *      Author: Antti Kamppi
 */

#include "generaldeclarations.h"
#include "generatorchain.h"
#include "generatorchainselector.h"
#include "generator.h"
#include "librarycomponent.h"
#include "choice.h"



#include <QList>
#include <QString>
#include <QDomNode>
#include <QDomNamedNodeMap>
#include <QSharedPointer>
#include <QFile>
#include <QXmlStreamWriter>
#include <QMap>

GeneratorChain::ComponentGeneratorSelector::ComponentGeneratorSelector(
		QDomNode &selectorNode): operator_(General::OR), groupNames_() {

	QDomNode groupNode;

	for (int k = 0; k < selectorNode.childNodes().count(); ++k) {
		if (selectorNode.childNodes().at(k).nodeName() ==
				QString("spirit:groupSelector")) {
			groupNode = selectorNode.childNodes().at(k);
		}
	}

	// get the groupSelectionOperator attribute
	QDomNamedNodeMap attributeMap = groupNode.attributes();
	operator_ = General::str2GroupSelector(attributeMap.namedItem(QString(
			"spirit:multipleGroupSelectionOperator")).nodeValue(),
			General::OR);

	for (int i = 0; i < groupNode.childNodes().count(); ++i) {
		if (groupNode.childNodes().at(i).nodeName() ==
				QString("spirit:name")) {
			groupNames_.append(groupNode.childNodes().at(i).childNodes().
					at(0).nodeValue());
		}
	}
}

GeneratorChain::GeneratorChain(QDomDocument &doc):
LibraryComponent(doc),
hidden_(false), 
displayName_(QString()),
genChainSelectors_(),
compGenSelectors_(),
generators_(),
chainGroups_(), 
choices_(),
attributes_() {

	LibraryComponent::vlnv_->setType(VLNV::GENERATORCHAIN);

	// find the IP-Xact root element (spirit:component or spirit:design ...)
	QDomNodeList nodeList = doc.childNodes();
	int i = 0;
	// search for the first element with children (=document type)
	while (!nodeList.at(i).hasChildNodes()) {
		++i;
	}

	QDomNode generatorChainNode = doc.childNodes().item(i);

	// parse the attributes
	General::parseAttributes(generatorChainNode, attributes_);

	// get the hidden attribute
	QDomNamedNodeMap attributeMap = generatorChainNode.attributes();
	QString hidden = attributeMap.namedItem("spirit:hidden").nodeValue();
	hidden_ = General::str2Bool(hidden, false);

	for (int i = 0; i < generatorChainNode.childNodes().count(); ++i) {
		QDomNode tempNode = generatorChainNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:generatorChainSelector")) {
			genChainSelectors_.append(QSharedPointer<GeneratorChainSelector>(
				new GeneratorChainSelector(tempNode)));
		}

		else if (tempNode.nodeName() ==
			QString("spirit:componentGeneratorSelector")) {
				compGenSelectors_.append(QSharedPointer<ComponentGeneratorSelector>(
					new ComponentGeneratorSelector(tempNode)));
		}

		else if (tempNode.nodeName() == QString("spirit:generator")) {
			generators_.append(QSharedPointer<Generator>(
				new Generator(tempNode)));
		}

		else if (tempNode.nodeName() == QString("spirit:chainGroup")) {
			chainGroups_.append(tempNode.childNodes().at(0).nodeValue());
		}

		else if (tempNode.nodeName() == QString("spirit:displayName")) {
			displayName_ = tempNode.childNodes().at(0).nodeValue();
		}

		else if (tempNode.nodeName() == QString("spirit:choices")) {

			// go through all choices
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QDomNode choiceNode = tempNode.childNodes().at(j);
				choices_.append(QSharedPointer<Choice>(
					new Choice(choiceNode)));
			}
		}
	}
	return;
}

GeneratorChain::GeneratorChain( const GeneratorChain &other ):
LibraryComponent(other),
hidden_(other.hidden_),
displayName_(other.displayName_),
genChainSelectors_(),
compGenSelectors_(),
generators_(),
chainGroups_(),
choices_(),
attributes_(other.attributes_) {

	foreach (QSharedPointer<GeneratorChainSelector> genChain, other.genChainSelectors_) {
		if (genChain) {
			QSharedPointer<GeneratorChainSelector> copy = QSharedPointer<GeneratorChainSelector>(
				new GeneratorChainSelector(*genChain.data()));
			genChainSelectors_.append(copy);
		}
	}

	foreach (QSharedPointer<ComponentGeneratorSelector> selector, other.compGenSelectors_) {
		if (selector) {
			QSharedPointer<ComponentGeneratorSelector> copy = QSharedPointer<ComponentGeneratorSelector>(
				new ComponentGeneratorSelector(*selector.data()));
			compGenSelectors_.append(copy);
		}
	}

	foreach (QSharedPointer<Generator> generator, other.generators_) {
		if (generator) {
			QSharedPointer<Generator> copy = QSharedPointer<Generator>(
				new Generator(*generator.data()));
			generators_.append(copy);
		}
	}

	foreach (QSharedPointer<Choice> choice, other.choices_) {
		if (choice) {
			QSharedPointer<Choice> copy = QSharedPointer<Choice>(
				new Choice(*choice.data()));
			choices_.append(copy);
		}
	}
}


GeneratorChain & GeneratorChain::operator=( const GeneratorChain &other ) {
	if (this != &other) {
		LibraryComponent::operator=(other);

		hidden_ = other.hidden_;
		displayName_ = other.displayName_;
		chainGroups_ = other.chainGroups_;
		attributes_ = other.attributes_;

		genChainSelectors_.clear();
		foreach (QSharedPointer<GeneratorChainSelector> genChain, other.genChainSelectors_) {
			if (genChain) {
				QSharedPointer<GeneratorChainSelector> copy = QSharedPointer<GeneratorChainSelector>(
					new GeneratorChainSelector(*genChain.data()));
				genChainSelectors_.append(copy);
			}
		}

		compGenSelectors_.clear();
		foreach (QSharedPointer<ComponentGeneratorSelector> selector, other.compGenSelectors_) {
			if (selector) {
				QSharedPointer<ComponentGeneratorSelector> copy = QSharedPointer<ComponentGeneratorSelector>(
					new ComponentGeneratorSelector(*selector.data()));
				compGenSelectors_.append(copy);
			}
		}

		generators_.clear();
		foreach (QSharedPointer<Generator> generator, other.generators_) {
			if (generator) {
				QSharedPointer<Generator> copy = QSharedPointer<Generator>(
					new Generator(*generator.data()));
				generators_.append(copy);
			}
		}

		choices_.clear();
		foreach (QSharedPointer<Choice> choice, other.choices_) {
			if (choice) {
				QSharedPointer<Choice> copy = QSharedPointer<Choice>(
					new Choice(*choice.data()));
				choices_.append(copy);
			}
		}
	}
	return *this;
}


GeneratorChain::~GeneratorChain() {
	genChainSelectors_.clear();
	compGenSelectors_.clear();
	generators_.clear();
	choices_.clear();
	return;
}

QSharedPointer<LibraryComponent> GeneratorChain::clone() const {
	return QSharedPointer<LibraryComponent>(new GeneratorChain(*this));
}

void GeneratorChain::write(QFile& file) {
	// create a writer instance and set it to operate on the given file
	QXmlStreamWriter writer(&file);

	// writer automatically adds whitespaces to make document more readable
	writer.setAutoFormatting(true);
	// writer uses one tab as indentation
	writer.setAutoFormattingIndent(-1);

	// call the base class implementation to write the top comment and
	// vlnv info. It also starts the <spirit:busDefinition> element
	LibraryComponent::write(writer);

	// set the attributes
	setXMLNameSpaceAttributes(attributes_);

	// write the attributes for the spirit:component element
	General::writeAttributes(writer, attributes_);

	// call base class to write the VLNV info
	LibraryComponent::writeVLNV(writer);

	// if description is specified in the base class
	if (!LibraryComponent::description_.isEmpty()) {
		writer.writeTextElement("spirit:description", description_);
	}

	// own code here

	writer.writeEndElement(); // spirit:generatorChain
	writer.writeEndDocument();
	return;

}

bool GeneratorChain::isValid( QStringList& errorList ) const {
	bool valid = true;
	QString thisIdentifier(QObject::tr("containing generator chain"));

	if (!vlnv_) {
		errorList.append(QObject::tr("No vlnv specified for the generator chain."));
		valid = false;
	}
	else if (!vlnv_->isValid(errorList, thisIdentifier)) {
		valid = false;
	}
	else {
		thisIdentifier = QObject::tr("generator chain %1").arg(vlnv_->toString());
	}

	// TODO add the validation here when the generators are supported
	return false;
}

bool GeneratorChain::isValid() const {
	if (!vlnv_) {
		return false;
	}
	else if (!vlnv_->isValid()) {
		return false;
	}

	// TODO add the validation here when the generators are supported
	return false;
}

// get the attributes
const QMap<QString, QString>& GeneratorChain::getAttributes() {
	return attributes_;
}

// set the attributes
void GeneratorChain::setAttributes(const QMap<QString, QString>& attributes) {
	attributes_.clear();
	attributes_ = attributes;
}

void GeneratorChain::setChainGroups(const QList<QString> &chainGroups) {
	// delete the old chain groups
	chainGroups_.clear();
	chainGroups_ = chainGroups;
}

void GeneratorChain::setCompGenSelectors(const
		QList<QSharedPointer<ComponentGeneratorSelector> > &compGenSelectors) {
	// delete old component generator selector
	compGenSelectors_.clear();

	// save the new compGenSelectors
	compGenSelectors_ = compGenSelectors;
}

const QList<QSharedPointer<Choice> >& GeneratorChain::getChoices() {
	return choices_;
}

const QList<QSharedPointer<GeneratorChainSelector> >&
GeneratorChain::getGenChainSelectors() {
	return genChainSelectors_;
}

const QList<QSharedPointer<Generator> >& GeneratorChain::getGenerators() {
	return generators_;
}

void GeneratorChain::setChoices(const
                QList<QSharedPointer<Choice> > &choices) {
	// delete the old choices
	choices_.clear();

	// save the new choices
	choices_ = choices;
}

void GeneratorChain::setHidden(bool &hidden) {
	hidden_ = hidden;
}

void GeneratorChain::setGenChainSelectors(const
		QList<QSharedPointer<GeneratorChainSelector> > &genChainSelectors) {
	// delete the old generator chain selectors
	genChainSelectors_.clear();

	// save the new generator chain selectors
	genChainSelectors_ = genChainSelectors;
}

void GeneratorChain::setGenerators(const
		QList<QSharedPointer<Generator> > &generators) {
	// delete the old generators
	generators_.clear();

	// save the new generators
	generators_ = generators;
}

const QList<QString>& GeneratorChain::getChainGroups() {
	return chainGroups_;
}

const QList<QSharedPointer<GeneratorChain::ComponentGeneratorSelector> >&
GeneratorChain::getCompGenSelectors() {
	return compGenSelectors_;
}

bool GeneratorChain::getHidden() const {
	return hidden_;
}

QString GeneratorChain::getDisplayName() const {
	return displayName_;
}

void GeneratorChain::setDisplayName(const QString &displayName) {
	displayName_ = displayName;
}

const QStringList GeneratorChain::getDependentFiles() const {
	QStringList fileList;

	for (int i = 0; i < generators_.size(); ++i) {
		fileList.append(generators_.at(i)->getGeneratorExe());
	}
	return fileList;
}

const QList<VLNV> GeneratorChain::getDependentVLNVs() const {
        QList<VLNV> vlnvList;

	// get the vlnvs of the generator chains that are included in this
	// generator chain
	for (int i = 0; i < genChainSelectors_.size(); ++i) {

		if (genChainSelectors_.at(i)->getGeneratorChainRef().isValid()) {
			vlnvList.append(genChainSelectors_.at(i)->getGeneratorChainRef());
		}
	}
	return vlnvList;
}

void GeneratorChain::setVlnv( const VLNV& vlnv ) {
	LibraryComponent::setVlnv(vlnv);
	LibraryComponent::vlnv_->setType(VLNV::GENERATORCHAIN);
}
