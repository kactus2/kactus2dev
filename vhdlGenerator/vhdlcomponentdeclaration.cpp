/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlcomponentdeclaration.cpp
 *		Project: Kactus 2
 */

#include "vhdlcomponentdeclaration.h"

#include "vhdlgenerator2.h"
#include <models/modelparameter.h>
#include <models/port.h>
#include <models/generaldeclarations.h>

// typedef for the container of generics
typedef QMap<QString, QSharedPointer<VhdlGeneric> > generics;

// typedef for the container of ports
typedef QMap<VhdlPortSorter, QSharedPointer<VhdlPort> > ports;

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

	// parse the generics for the component declaration
	QMap<QString, QSharedPointer<ModelParameter> > modelParams = component_->getModelParameters();
	foreach (QSharedPointer<ModelParameter> modelParam, modelParams) {
		QSharedPointer<VhdlGeneric> generic(new VhdlGeneric(this, modelParam.data()));
		generics_.insert(generic->name(), generic);
	}

	// parse the ports for the component declaration
	QMap<QString, QSharedPointer<Port> > ports = component_->getPorts();
	foreach (QSharedPointer<Port> port, ports) {
		
		// do not add ports with invalid direction or phantom direction
		if (port->getDirection() == General::DIRECTION_INVALID ||
			port->getDirection() == General::DIRECTION_PHANTOM) {
			continue;
		}

		// create the actual port
		QSharedPointer<VhdlPort> vhdlPort(new VhdlPort(this, port.data()));

		// create the sorter instance
		VhdlPortSorter sorter(vhdlPort->name(), port->getDirection());

		// this port can not be created yet
		Q_ASSERT(!ports_.contains(sorter));

		ports_.insert(sorter, vhdlPort);
	}
}

VhdlComponentDeclaration::~VhdlComponentDeclaration() {
}

void VhdlComponentDeclaration::write( QTextStream& stream ) const {
	// if component contains a description
	if (!description().isEmpty()) {
		stream << "\t-- " << description() << endl;
	}

	stream << "\tcomponent " << typeName_ << endl;

	// write the generic declarations
	stream << "\t\tgeneric (" << endl;
	for (generics::Iterator i = generics_.begin(); i != generics_.end(); ++i) {
		stream << "\t\t\t";
		i.value()->write(stream);

		// if this is not the last generic to write
		if (i + 1 != generics_.end()) {
			stream << ";" << endl;
		}
	}
	stream << ");" << endl;

	// write the port declarations
	stream << "\t\tport (" << endl;
	for (ports::iterator i = ports_.begin(); i != ports_.end(); ++i) {
		stream << "\t\t\t";
		i.value()->write(stream);

		// if this is not the last port to write
		if (i + 1 != ports_.end()) {
			stream << ";" << endl;
		}
	}
	stream << ");" << endl;
	
	stream << "\tend component;" << endl;
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
	// check each vhdl port
	foreach (QSharedPointer<VhdlPort> port, ports_) {
		
		// check each instance of this type
		foreach (QSharedPointer<VhdlComponentInstance> instantation, instantations_) {
			
			// if the named port is connected in this instance
			if (instantation->hasConnection(port->name())) {
				
				// port is needed so it can not be commented and search can be 
				// stopped for this port
				port->setCommented(false);
				break;
			}
		}
	}
}
