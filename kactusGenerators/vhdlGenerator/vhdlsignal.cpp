/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlsignal.cpp
 *		Project: Kactus 2
 */

#include "vhdlsignal.h"

#include "vhdlgeneral.h"

VhdlSignal::VhdlSignal( QObject* parent, 
					   const QString& name /*= QString()*/,
					   const QString& signalType /*= QString()*/,
					   int leftBound /*= -1*/, 
					   int rightBound /*= -1*/, 
					   const QString& description /*= QString()*/,
					   const QString& defaultValue /*= QString()*/):
VhdlObject(parent, name, signalType, defaultValue, description),
left_(leftBound),
right_(rightBound) {

	if (type().isEmpty()) {
		setType(VhdlGeneral::useDefaultType(left_, right_));
	}	
	Q_ASSERT(!type().isEmpty());
}

VhdlSignal::~VhdlSignal() {
}

void VhdlSignal::write( QTextStream& stream ) const {
	Q_ASSERT(!name().isEmpty());
	Q_ASSERT(!type().isEmpty());

	if (!description().isEmpty()) {
		VhdlGeneral::writeDescription(description(), stream, QString("  "));
	}
	stream << "  signal "; 
	stream << name().leftJustified(16, ' '); //align colons (:) at least roughly
	stream << " : ";
	QString typeDefinition = VhdlGeneral::vhdlType2String(type(), left_, right_);
	stream << typeDefinition << ";" << endl;
}

int VhdlSignal::left() const {
	return left_;
}

void VhdlSignal::setLeft( int left ) {
	left_ = left;
}

int VhdlSignal::right() const {
	return right_;
}

void VhdlSignal::setRight( int right ) {
	right_ = right;
}

void VhdlSignal::setBounds( int left, int right ) {
	left_ = left;
	right_ = right;

	// if scalar port changed to vectored
	if (type() == "std_logic" && left != right) {
		setType("std_logic_vector");
	}

	// if vectored port changed to scalar
	else if (type() == "std_logic_vector" && left == right) {
		setType("std_logic");
	}
}

