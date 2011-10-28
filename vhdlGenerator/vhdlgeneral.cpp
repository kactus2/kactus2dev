/* 
 *  	Created on: 27.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlgeneral.cpp
 *		Project: Kactus 2
 */

#include "vhdlgeneral.h"

#include <QString>

QString VhdlGeneral::useDefaultType( const int leftBound, const int rightBound ) {
	
	QString result("std_logic");

	// if port is larger than one bit
	int size = leftBound - rightBound + 1;
	if (size > 1) {
		result = QString("std_logic_vector");
	}

	return result;
}

QString VhdlGeneral::getDefaultVhdlTypeDef( const QString& typeName ) {

	// types for package numeric_std
	if (typeName.compare(QString("signed"), Qt::CaseInsensitive) == 0 ||
		typeName.compare(QString("unsigned"), Qt::CaseInsensitive) == 0) {
		return QString("IEEE.numeric_std.all");
	}

	// types for package std_logic_1164
	else if (typeName.compare(QString("std_logic_vector"), Qt::CaseInsensitive) == 0 ||
		typeName.compare(QString("std_ulogic_vector"), Qt::CaseInsensitive) == 0 ||
		typeName.compare(QString("std_logic"), Qt::CaseInsensitive) == 0 ||
		typeName.compare(QString("std_ulogic"), Qt::CaseInsensitive) == 0) {

			return QString("IEEE.std_logic_1164.all");
	}
	// types for standard package or user defined types
	else {
		return QString();
	}
}

QString VhdlGeneral::vhdlType2String( const QString& type,
									 int leftBound /*= -1*/,
									 int rightBound /*= -1*/ ) {
	
	// the scalar types and types with predefined bounds
	if (0 == type.compare(QString("std_logic"), Qt::CaseInsensitive) ||
		0 == type.compare(QString("bit"), Qt::CaseInsensitive) ||
		0 == type.compare(QString("boolean"), Qt::CaseInsensitive) ||
		0 == type.compare(QString("character"), Qt::CaseInsensitive) ||
		0 == type.compare(QString("severity_level"), Qt::CaseInsensitive) ||
		0 == type.compare(QString("std_ulogic"), Qt::CaseInsensitive) ||
		0 == type.compare(QString("time"), Qt::CaseInsensitive) ||
		0 == type.compare(QString("natural"), Qt::CaseInsensitive) ||
		0 == type.compare(QString("positive"), Qt::CaseInsensitive)) {
		
		// return the type as such because the bounds are not used.
		return type;
	}
	
	// if the type is an array
	else if (0 == type.compare(QString("bit_vector"), Qt::CaseInsensitive) ||
		0 == type.compare(QString("std_logic_vector"), Qt::CaseInsensitive) ||
		0 == type.compare(QString("std_ulogic_vector"), Qt::CaseInsensitive) ||
		0 == type.compare(QString("string"), Qt::CaseInsensitive)) {
		
		QString result(type);
		result += QString("(%1 downto %2)").arg(leftBound).arg(rightBound);
		return result;
	}

	// if the type is numeric
	else if (0 == type.compare(QString("integer"), Qt::CaseInsensitive) ||
		0 == type.compare(QString("real"), Qt::CaseInsensitive)) {

		QString result(type);
		result += QString(" range %1 to %2").arg(rightBound).arg(leftBound);
		return result;
	}

	// if user defined type then return it as such
	else {
		return type;
	}
}
