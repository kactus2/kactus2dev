/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlgeneric.cpp
 *		Project: Kactus 2
 */

#include "vhdlgeneric.h"

#include <IPXACTmodels/common/ModuleParameter.h>

#include <QChar>

VhdlGeneric::VhdlGeneric(ModuleParameter* generic):
VhdlTypedObject(generic->name(),
		        generic->getDataType(),
    	        generic->getValue(),
		        generic->description()) 
{
	Q_ASSERT(generic);
}

VhdlGeneric::~VhdlGeneric() {
}

void VhdlGeneric::write( QTextStream& stream ) const {
	Q_ASSERT(!name().isEmpty());
	Q_ASSERT(!type().isEmpty());

    // align colons (:) at least roughly
	stream << getVhdlLegalName().leftJustified(16, ' ');
    stream<< " : " << type();

	// check if type is string then quotations must be used for default value
	bool addQuotation = type().compare(QString("string"), Qt::CaseInsensitive) == 0;
	
	// if a default value has been specified
	if (!defaultValue().isEmpty()) {
		stream << " := ";
		
		// if default value does not start with quotation
		if (addQuotation && !defaultValue().startsWith(QChar('"'))) {
			stream << "\"";
		}
		
		// write the default value
		stream << defaultValue();

		// if default value does not end with quotation
		if (addQuotation && !defaultValue().endsWith(QChar('"'))) {
			stream << "\"";
		}
	}	
}
