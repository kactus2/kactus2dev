/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlobject.cpp
 *		Project: Kactus 2
 */

#include "vhdlobject.h"

#include "vhdlcomponentdeclaration.h"
#include "vhdlgenerator2.h"

VhdlObject::VhdlObject( VhdlGenerator2* parent, 
					   const QString& name /*= QString()*/, 
					   const QString& type /*= QString()*/, 
					   const QString& defaultValue /*= QString()*/, 
					   const QString& description /*= QString()*/ ):
QObject(parent),
name_(name),
type_(type),
description_(description),
defaultValue_(defaultValue) {

	connect(this, SIGNAL(noticeMessage(const QString&)),
		parent, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
	connect(this, SIGNAL(errorMessage(const QString&)),
		parent, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
}

VhdlObject::VhdlObject( VhdlComponentDeclaration* parent, 
					   const QString& name /*= QString()*/,
					   const QString& type /*= QString()*/, 
					   const QString& defaultValue /*= QString()*/, 
					   const QString& description /*= QString()*/ ):
QObject(parent),
name_(name),
type_(type),
description_(description),
defaultValue_(defaultValue) {

	connect(this, SIGNAL(noticeMessage(const QString&)),
		parent, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
	connect(this, SIGNAL(errorMessage(const QString&)),
		parent, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
}

VhdlObject::~VhdlObject() {
}

QString VhdlObject::name() const {
	return name_;
}

void VhdlObject::setName( const QString& name ) {
	name_ = name;
}

QString VhdlObject::type() const {
	return type_;
}

void VhdlObject::setType( const QString& type ) {
	type_ = type;
}

QString VhdlObject::description() const {
	return description_;
}

void VhdlObject::setDescription( const QString& description ) {
	description_ = description;
}

QString VhdlObject::defaultValue() const {
	return defaultValue_;
}

void VhdlObject::setDefaultValue( const QString& defaultValue ) {
	defaultValue_ = defaultValue;
}
