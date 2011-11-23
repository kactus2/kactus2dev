/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlgeneric.cpp
 *		Project: Kactus 2
 */

#include "vhdlgeneric.h"
#include "vhdlgeneral.h"

#include <models/modelparameter.h>

#include <QChar>

VhdlGeneric::VhdlGeneric( VhdlGenerator2* parent, 
						 const QString& name /*= QString()*/, 
						 const QString& type /*= QString()*/, 
						 const QString& defaultValue /*= QString()*/, 
						 const QString& description /*= QString()*/ ):
VhdlObject(parent, name, type, defaultValue, description) {
}

VhdlGeneric::VhdlGeneric( VhdlComponentDeclaration* parent,
						 const QString& name /*= QString()*/,
						 const QString& type /*= QString()*/, 
						 const QString& defaultValue /*= QString()*/,
						 const QString& description /*= QString()*/ ):
VhdlObject(parent, name, type, defaultValue, description) {
}

VhdlGeneric::VhdlGeneric( VhdlGenerator2* parent, ModelParameter* generic ):
VhdlObject(parent,
		   generic->getName(),
		   generic->getDataType(),
		   generic->getValue(),
		   generic->getDescription()) {
	Q_ASSERT(parent);
	Q_ASSERT(generic);
}

VhdlGeneric::VhdlGeneric( VhdlComponentDeclaration* parent, ModelParameter* generic ):
VhdlObject(parent,
		   generic->getName(),
		   generic->getDataType(),
		   generic->getValue(),
		   generic->getDescription()) {
	Q_ASSERT(parent);
	Q_ASSERT(generic);
}

VhdlGeneric::~VhdlGeneric() {
}

void VhdlGeneric::write( QTextStream& stream ) const {
	Q_ASSERT(!name_.isEmpty());
	Q_ASSERT(!type_.isEmpty());

	stream << name_ << " : " << type_;

	// check if type is string then quotations must be used for default value
	bool addQuotation = type_.compare(QString("string"), Qt::CaseInsensitive) == 0;
	
	// if a default value has been specified
	if (!defaultValue_.isEmpty()) {
		stream << " := ";
		
		// if default value does not start with quotation
		if (addQuotation && *defaultValue_.begin() != QChar('"')) {
			stream << "\"";
		}
		
		// write the default value
		stream << defaultValue_;

		// if default value does not end with quotation
		if (addQuotation && *(defaultValue_.end() - 1) != QChar('"')) {
			stream << "\"";
		}
	}	
}
