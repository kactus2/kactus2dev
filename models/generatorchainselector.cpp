/* 
 *
 *  Created on: 12.8.2010
 *      Author: Antti Kamppi
 */

#include "generatorchainselector.h"
#include <LibraryManager/vlnv.h>
#include "generaldeclarations.h"

#include "../exceptions/parse_error.h"

#include <QList>
#include <QString>
#include <QDomNode>
#include <QDomNamedNodeMap>

GeneratorChainSelector::GeneratorChainSelector(QDomNode& generatorChainNode):
unique_(false), operator_(General::OR), groupNames_(), generatorChainRef_() {

	// get the spirit:unique attribute
	QDomNamedNodeMap attributeMap = generatorChainNode.attributes();
	QString unique = attributeMap.namedItem(QString("spirit:unique")).
			nodeValue();
	unique_ = General::str2Bool(unique, false);

	for (int i = 0; i < generatorChainNode.childNodes().count(); ++i) {
		QDomNode tempNode = generatorChainNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:groupSelector")) {
			// get the selection operator
			QDomNamedNodeMap attributeMap = tempNode.attributes();

			operator_ = General::str2GroupSelector(attributeMap.namedItem(
					QString("spirit:multipleGroupSelectionOperator")).
					nodeValue(), General::OR);

			groupNames_.append(tempNode.childNodes().at(0).nodeValue());
		}

		else if (tempNode.nodeName() == QString("spirit:generatorChainRef")) {

			generatorChainRef_ =General::createVLNV(tempNode, VLNV::GENERATORCHAIN);
		}
	}

	// if mandatory fields are missing
	if (groupNames_.size() < 1) {
		throw Parse_error(QObject::tr("Mandatory element name missing in "
				"spirit:generatorChainSelector"));
	}
	return;
}

GeneratorChainSelector::GeneratorChainSelector( const GeneratorChainSelector& other ):
unique_(other.unique_),
operator_(other.operator_),
groupNames_(other.groupNames_),
generatorChainRef_(other.generatorChainRef_) {
}


GeneratorChainSelector& GeneratorChainSelector::operator=( const GeneratorChainSelector& other ) {
	if (this != &other) {
		unique_ = other.unique_;
		operator_ = other.operator_;
		groupNames_ = other.groupNames_;
		generatorChainRef_ = other.generatorChainRef_;
	}
	return *this;
}


GeneratorChainSelector::~GeneratorChainSelector() {
}

const QList<QString>& GeneratorChainSelector::getGroupNames() {
	return groupNames_;
}

void GeneratorChainSelector::setUnique(bool unique) {
	unique_ = unique;
}

VLNV GeneratorChainSelector::getGeneratorChainRef() const {
	return generatorChainRef_;
}

General::GroupSelectorOperator GeneratorChainSelector::getOperator() const {
	return operator_;
}

bool GeneratorChainSelector::getUnique() const {
	return unique_;
}

void GeneratorChainSelector::setGeneratorChainRef(const VLNV& generatorChainRef) {
	generatorChainRef_ = generatorChainRef;
}

void GeneratorChainSelector::setOperator(
		General::GroupSelectorOperator groupOperator) {
	operator_ = groupOperator;
}

void GeneratorChainSelector::setGroupNames(const QList<QString>& groupNames) {
	// delete old group names
	groupNames_.clear();

	groupNames_ = groupNames;
}