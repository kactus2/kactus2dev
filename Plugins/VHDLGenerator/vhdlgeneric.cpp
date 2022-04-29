/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlgeneric.cpp
 *		Project: Kactus 2
 */

#include "vhdlgeneric.h"

#include <IPXACTmodels/common/ModuleParameter.h>

#include <KactusAPI/include/ExpressionFormatter.h>

#include <QChar>

VhdlGeneric::VhdlGeneric(QSharedPointer<ModuleParameter> generic,  QSharedPointer<ExpressionFormatter> formatter):
VhdlTypedObject(generic->name(),
		        generic->getDataType(),
    	        generic->getValue(),
		        generic->description()),
                formatter_(formatter)
{
    if (type().isEmpty())
    {
        setType(QStringLiteral("integer"));
    }
}

void VhdlGeneric::write( QTextStream& stream ) const
{
	Q_ASSERT(!name().isEmpty());
	Q_ASSERT(!type().isEmpty());

    // align colons (:) at least roughly
	stream << getVhdlLegalName().leftJustified(16, ' ');
    stream<< " : " << type();

	// if a default value has been specified
	if (!defaultValue().isEmpty())
    {
		stream << " := ";
	
	    // check if type is string then quotations must be used for default value
	    bool addQuotation = type().compare(QLatin1String("string"), Qt::CaseInsensitive) == 0;
		
		// if default value does not start with quotation
		if (addQuotation && !defaultValue().startsWith(QChar('"')))
        {
			stream << "\"";
		}
		
		// write the default value
		stream << formatter_->formatReferringExpression(defaultValue());

		// if default value does not end with quotation
		if (addQuotation && !defaultValue().endsWith(QChar('"')))
        {
			stream << "\"";
		}
	}
}
