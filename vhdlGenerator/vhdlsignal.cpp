/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlsignal.cpp
 *		Project: Kactus 2
 */

#include "vhdlsignal.h"

#include "vhdlgeneral.h"

VhdlSignal::VhdlSignal( VhdlGenerator2* parent, 
					   const QString& name /*= QString()*/,
					   const QString& type /*= QString()*/,
					   int leftBound /*= -1*/, 
					   int rightBound /*= -1*/, 
					   const QString& description /*= QString()*/,
					   const QString& defaultValue /*= QString()*/):
VhdlObject(parent, name, type, defaultValue, description),
left_(leftBound),
right_(rightBound) {

	if (type_.isEmpty()) {
		type_ = VhdlGeneral::useDefaultType(left_, right_);
	}	
	Q_ASSERT(!type_.isEmpty());
}

VhdlSignal::VhdlSignal( VhdlComponentDeclaration* parent, 
					   const QString& name /*= QString()*/, 
					   const QString& type /*= QString()*/, 
					   int leftBound /*= -1*/, 
					   int rightBound /*= -1*/,
					   const QString& description /*= QString()*/,
					   const QString& defaultValue /*= QString()*/):
VhdlObject(parent, name, type, defaultValue, description),
left_(leftBound),
right_(rightBound) {

	if (type_.isEmpty()) {
		type_ = VhdlGeneral::useDefaultType(left_, right_);
	}
	Q_ASSERT(!type_.isEmpty());
}

VhdlSignal::~VhdlSignal() {
}

void VhdlSignal::write( QTextStream& stream ) const {
	Q_ASSERT(!name_.isEmpty());
	Q_ASSERT(!type_.isEmpty());

	if (!description_.isEmpty()) {
		stream << "\t";
		VhdlGeneral::writeDescription(description_, stream, QString("\t"));
	}
	stream << "\tsignal " << name_ << " : ";

	QString typeDefinition = VhdlGeneral::vhdlType2String(type_, left_, right_);

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
}

