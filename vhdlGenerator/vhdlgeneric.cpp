/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlgeneric.cpp
 *		Project: Kactus 2
 */

#include "vhdlgeneric.h"

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

VhdlGeneric::~VhdlGeneric() {
}

void VhdlGeneric::write( QTextStream& stream ) {
	Q_ASSERT(!name_.isEmpty());
	Q_ASSERT(!type_.isEmpty());

	stream << name_ << " : " << type_;
	
	// if a default value has been specified
	if (!defaultValue_.isEmpty()) {
		stream << " := " << defaultValue_;
	}	
}
