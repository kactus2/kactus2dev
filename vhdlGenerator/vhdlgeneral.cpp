/* 
 *  	Created on: 27.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlgeneral.cpp
 *		Project: Kactus 2
 */

#include "vhdlgeneral.h"

#include <QString>
#include <QStringList>
#include <QChar>
#include <QObject>

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

void VhdlGeneral::writeDescription( const QString& description, 
								   QTextStream& stream, 
								   const QString& lineSeparator /*= QString("")*/,
								   bool addStartComment /*= true*/ ) {

	if (description.isEmpty()) {
		stream << endl;
		return;
	}

	// split the description into lines whenever there is a line separator
	QStringList lines = description.split(QString("\n"));
	
	// at least one line has to be because description was not empty
	Q_ASSERT(!lines.isEmpty());

	if (addStartComment) {
		stream << "-- ";
	}
	stream << lines.first() << endl;
	for (int i = 1; i < lines.size(); ++i) {
		stream << lineSeparator << "-- " << lines.at(i) << endl;
	}
}

bool VhdlGeneral::isScalarType( const QString& typeName ) {
	if (typeName.compare(QString("bit"), Qt::CaseInsensitive) == 0 ||
		typeName.compare(QString("boolean"), Qt::CaseInsensitive) == 0 ||
		typeName.compare(QString("character"), Qt::CaseInsensitive) == 0 ||
		typeName.compare(QString("severity_level"), Qt::CaseInsensitive) == 0 ||
		typeName.compare(QString("std_logic"), Qt::CaseInsensitive) == 0 ||
		typeName.compare(QString("std_ulogic"), Qt::CaseInsensitive) == 0 ||
		typeName.compare(QString("natural"), Qt::CaseInsensitive) == 0 ||
		typeName.compare(QString("positive"), Qt::CaseInsensitive) == 0 ||
		typeName.compare(QString("integer"), Qt::CaseInsensitive) == 0 ||
		typeName.compare(QString("real"), Qt::CaseInsensitive) == 0) {
			return true;
	}
	else {
		return false;
	}
}

bool VhdlGeneral::checkVhdlTypeMatch( const QString& type1, const QString& type2 ) {

	// if the types are the same then of course they are compatible
	if (type1 == type2) {
		return true;
	}

	// std_logic and std_logic_vector can be connected to each other
	else if (type1 == "std_logic" && type2 == "std_logic_vector") {
		return true;
	}
	else if (type1 == "std_logic_vector" && type2 == "std_logic") {
		return true;
	}

	// other types can't be connected to each other
	else {
		return false;
	}
}

QString VhdlGeneral::convertDefaultValue( const QString& originalDefaultValue, const QString& type ) {

	// if there is no original default value given
	if (originalDefaultValue.isEmpty()) {
		return QObject::tr("No default value given");
	}

	// the types that do not need "" or ''
	if (type == "boolean" ||
		type == "integer" ||
		type == "natural" ||
		type == "positive" ||
		type == "real" ||
		type == "severity_level" ||
		type == "time") {
		return originalDefaultValue;
	}
	// the types that use ''
	else if (type == "bit" ||
		type == "character" ||
		type == "std_logic" ||
		type == "std_ulogic") {
		
			// char for '
			QChar charToAdd(39);

			// remove the white spaces from start and end
			QString result(originalDefaultValue.simplified());

			// if theres only one character then add ' to both start and end
			if (originalDefaultValue.size() == 1) {
				return QString("'%1'").arg(originalDefaultValue);
			}

			// if start is missing '
			else if (result.at(0) != charToAdd) {
				result.prepend(charToAdd);
			}

			// if end is missing '
			if (result.at(result.size() - 1) != charToAdd) {
				result.append(charToAdd);
			} 

			return result;
	}
	
	// types that use ""
	else if (type == "bit_vector" ||
		type == "std_logic_vector" ||
		type == "std_ulogic_vector" ||
		type == "string" ||
		type == "signed" ||
		type == "unsigned") {

		// char for "
		QChar charToAdd(34);

		// remove white space from start and end
		QString result(originalDefaultValue.simplified());

		// if the first letter is a decimal digit
		if (result.at(0).isDigit()) {
			result.prepend(charToAdd);

			// if end is missing "
			if (result.at(result.size() - 1) != charToAdd) {
				result.append(charToAdd);
			}
		}

		return result;
	}
	else {
		return originalDefaultValue;
	}
}
