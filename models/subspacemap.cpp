/* 
 *
 *  Created on: 29.7.2010
 *      Author: Antti Kamppi
 */

#include "memorymapitem.h"
#include "generaldeclarations.h"
#include "subspacemap.h"
#include "parameter.h"

#include <QList>
#include <QSharedPointer>
#include <QDomNode>
#include <QObject>
#include <QXmlStreamWriter>

// The constructor
SubspaceMap::SubspaceMap(QDomNode &memoryMapNode): MemoryMapItem(memoryMapNode),
parameters_() {
	for (int i = 0; i < memoryMapNode.childNodes().count(); ++i) {
		if (memoryMapNode.childNodes().at(i).nodeName() ==
				QString("spirit:parameters")) {

			// go through all parameters
			for (int j = 0;
					j < memoryMapNode.childNodes().at(i).childNodes().count();
					++j) {

				QDomNode parameterNode =
						memoryMapNode.childNodes().at(i).childNodes().at(j);
				Parameter *temp = new Parameter(parameterNode);
				parameters_.append(QSharedPointer<Parameter>(temp));
			}
		}
	}

	// if mandatory elements are missing
// 	if (!MemoryMapItem::attributes_.contains(QString("spirit:masterRef"))) {
// 		throw Parse_error(QObject::tr("Mandatory attribute spirit:masterRef"
// 				" missing in spirit:subspaceMap"));
// 	}
}

SubspaceMap::SubspaceMap( const SubspaceMap &other ):
MemoryMapItem(other),
parameters_() {

	foreach (QSharedPointer<Parameter> param, other.parameters_) {
		if (param) {
			QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(
				new Parameter(*param.data()));
			parameters_.append(copy);
		}
	}
}

SubspaceMap & SubspaceMap::operator=( const SubspaceMap &other ) {
	if (this != &other) {
		MemoryMapItem::operator=(other);

		parameters_.clear();
		foreach (QSharedPointer<Parameter> param, other.parameters_) {
			if (param) {
				QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(
					new Parameter(*param.data()));
				parameters_.append(copy);
			}
		}
	}
	return *this;
}

// The destructor
SubspaceMap::~SubspaceMap() {
	parameters_.clear();
}

QSharedPointer<MemoryMapItem> SubspaceMap::clone() const {
	return QSharedPointer<MemoryMapItem>(new SubspaceMap(*this));
}

void SubspaceMap::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:subspaceMap");
	// write the base class information
	MemoryMapItem::write(writer);

	if (parameters_.size() != 0) {
		writer.writeStartElement("spirit:parameters");

		// each parameter
		for (int i = 0; i < parameters_.size(); ++i) {
			parameters_.at(i)->write(writer);
		}

		writer.writeEndElement(); // spirit:parameters
	}

	writer.writeEndElement(); // spirit:subspaceMap
}

bool SubspaceMap::isValid( QStringList& errorList, 
						  const QString& parentIdentifier ) const {
	bool valid = true;
	const QString thisIdentifier(QObject::tr("subspace map %1").arg(name_));

	if (name_.isEmpty()) {
		errorList.append(QObject::tr("No name specified for subspace map within %1").arg(
			parentIdentifier));
		valid = false;
	}

	if (!attributes_.contains(QString("spirit:masterRef"))) {
		errorList.append(QObject::tr("No master reference set for %1 within %2").arg(
			thisIdentifier).arg(parentIdentifier));
		valid = false;
	}

	if (baseAddress_.isEmpty()) {
		errorList.append(QObject::tr("No base address set for %1 within %2").arg(
			thisIdentifier).arg(parentIdentifier));
		valid = false;
	}

	foreach (QSharedPointer<Parameter> param, parameters_) {
		if (!param->isValid(errorList, thisIdentifier)) {
			valid = false;
		}
	}

	return valid;
}

bool SubspaceMap::isValid() const {
	
	if (name_.isEmpty()) {
		return false;
	}

	if (!attributes_.contains(QString("spirit:masterRef"))) {
		return false;
	}

	if (baseAddress_.isEmpty()) {
		return false;
	}

	foreach (QSharedPointer<Parameter> param, parameters_) {
		if (!param->isValid()) {
			return false;
		}
	}
	return true;
}

const QList<QSharedPointer<Parameter> >& SubspaceMap::getParameters() {
	return parameters_;
}

void SubspaceMap::setParameters(const
		QList<QSharedPointer<Parameter> > &parameters) {
	// first delete old parameters
	parameters_.clear();

	// save the new parameters
	parameters_ = parameters;
}
