/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlcomponentdeclaration.cpp
 *		Project: Kactus 2
 */

#include "vhdlcomponentdeclaration.h"

#include "vhdlgenerator2.h"

VhdlComponentDeclaration::VhdlComponentDeclaration( QSharedPointer<Component> component,
												   VhdlGenerator2* parent ):
QObject(parent),
parent_(parent),
component_(component),
typeName_(component->getVlnv()->getName()),
generics_(),
ports_(),
instantations_() {

	Q_ASSERT(parent);
	Q_ASSERT(component);
	connect(this, SIGNAL(noticeMessage(const QString&)),
		parent, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
	connect(this, SIGNAL(errorMessage(const QString&)),
		parent, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
}

VhdlComponentDeclaration::~VhdlComponentDeclaration() {
}

void VhdlComponentDeclaration::write( QTextStream& stream ) {

}

QString VhdlComponentDeclaration::typeName() const {
	return typeName_;
}

QString VhdlComponentDeclaration::description() const {
	return component_->getDescription();
}

QSharedPointer<Component> VhdlComponentDeclaration::componentModel() const {
	return component_;
}

void VhdlComponentDeclaration::addInstantiation( 
	QSharedPointer<VhdlComponentInstance> instance ) {

	Q_ASSERT(!instantations_.contains(instance));

	instantations_.append(instance);
}

bool VhdlComponentDeclaration::hasGeneric( const QString& genericName ) const {
	return generics_.contains(genericName);
}

void VhdlComponentDeclaration::checkPortConnections() {
	
}
