/* 
 *
 *  Created on: 18.10.2010
 *      Author: Antti Kamppi
 */

#include "memoryblockdata.h"
#include "generaldeclarations.h"

#include "../exceptions/write_error.h"

#include <QSharedPointer>
#include <QList>
#include <QXmlStreamWriter>

MemoryBlockData::MemoryBlockData(General::Usage usage,
								 General::BooleanValue volatileValue,
								 General::Access access, const QList<QSharedPointer<Parameter> >&
								 parameters):
usage_(usage), volatile_(volatileValue),
access_(access), parameters_(parameters) {
}

MemoryBlockData::MemoryBlockData( const MemoryBlockData& other ):
usage_(other.usage_),
volatile_(other.volatile_),
access_(other.access_),
parameters_() {

	foreach (QSharedPointer<Parameter> param, other.parameters_) {
		if (param) {
			QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(
				new Parameter(*param.data()));
			parameters_.append(copy);
		}
	}
}

MemoryBlockData& MemoryBlockData::operator=( const MemoryBlockData& other ) {
	if (this != &other) {
		usage_ = other.usage_;
		volatile_ = other.volatile_;
		access_ = other.access_;

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

MemoryBlockData::~MemoryBlockData() {
	parameters_.clear();
}

void MemoryBlockData::write(QXmlStreamWriter& writer) {
	// if usage is defined
	if (usage_ != General::UNSPECIFIED_USAGE) {
		writer.writeTextElement("spirit:usage", General::usage2Str(usage_));
	}

	if (volatile_ != General::BOOL_UNSPECIFIED) {
		writer.writeTextElement("spirit:volatile",
				General::booleanValue2Str(volatile_));
	}

	if (access_ != General::UNSPECIFIED_ACCESS) {
		writer.writeTextElement("spirit:access", General::access2Str(access_));
	}

	if (parameters_.size() != 0) {
		writer.writeStartElement("spirit:parameters");

		// go through each parameter
		for (int i = 0; i < parameters_.size(); ++i) {
			parameters_.at(i)->write(writer);
		}

		writer.writeEndElement(); // spirit:parameters
	}
}

General::Usage MemoryBlockData::getUsage() const {
	return usage_;
}

General::BooleanValue MemoryBlockData::getVolatile() const {
	return volatile_;
}

General::Access MemoryBlockData::getAccess() const {
	return access_;
}

void MemoryBlockData::setUsage(General::Usage usage) {
	usage_ = usage;
}

void MemoryBlockData::setVolatile(General::BooleanValue volatileValue) {
	volatile_ = volatileValue;
}

void MemoryBlockData::setAccess(General::Access access) {
	access_ = access;
}

const QList<QSharedPointer<Parameter> >&
MemoryBlockData::getParameters() const {
	return parameters_;
}

void MemoryBlockData::setParameters(
		QList<QSharedPointer<Parameter> >& parameters) {
	// clear old parameters
	parameters_.clear();

	parameters_ = parameters;
}
