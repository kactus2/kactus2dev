/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlcomponentdeclaration.cpp
 *		Project: Kactus 2
 */

#include "vhdlcomponentdeclaration.h"

#include "vhdlgeneral.h"
#include <IPXACTmodels/modelparameter.h>
#include <IPXACTmodels/port.h>
#include <IPXACTmodels/generaldeclarations.h>

VhdlComponentDeclaration::VhdlComponentDeclaration(QSharedPointer<Component> component, QObject* parent):
QObject(parent),
component_(component),
typeName_(component->getVlnv()->getName()),
generics_(),
ports_(),
instantations_() 
{
	Q_ASSERT(component);

	// parse the generics for the component declaration
	QList<QSharedPointer<ModelParameter> > modelParams = component_->getModelParameters();
	foreach (QSharedPointer<ModelParameter> modelParam, modelParams) {
		QSharedPointer<VhdlGeneric> generic(new VhdlGeneric(modelParam.data(), this));
		generics_.insert(generic->name(), generic);
	}

	// parse the ports for the component declaration
	QList<QSharedPointer<Port> > ports = component_->getPorts();
	foreach (QSharedPointer<Port> port, ports) {
		
		// do not add ports with invalid direction or phantom direction
		if (port->getDirection() == General::DIRECTION_INVALID ||
			port->getDirection() == General::DIRECTION_PHANTOM) {
			continue;
		}

		// create the actual port
		QSharedPointer<VhdlPort> vhdlPort(new VhdlPort(port.data(), this));

		// create the sorter instance
		VhdlPortSorter sorter(component_->getInterfaceNameForPort(vhdlPort->name()),
			vhdlPort->name(), port->getDirection());

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
		VhdlGeneral::writeDescription(description(), stream, QString("  "));
	}
	stream << "  " << "-- IP-XACT VLNV: " << component_->getVlnv()->toString() << endl;
	stream << "  " << "component " << typeName_ << endl;

	// write the generic declarations
	if (!generics_.isEmpty()) {
		stream << "  "<< "  "  << "generic (" << endl;
        for (QMap<QString, QSharedPointer<VhdlGeneric> >::const_iterator i = generics_.begin(); i != generics_.end(); ++i) {
			stream << "  " << "  " << "  " ;
			i.value()->write(stream);

			// if this is not the last generic to write
			if (i + 1 != generics_.end()) {
				stream << ";";
			}

			if (!i.value()->description().isEmpty()) {
				stream << " ";
				VhdlGeneral::writeDescription(i.value()->description(), stream);
			}
			else {
				stream << endl;
			}
		}
		stream << "  " << "  " << ");" << endl;
	}

	// write the port declarations
	if (!ports_.isEmpty() && VhdlPort::hasRealPorts(ports_)) {
		stream << "  " << "  " << "port (" << endl;
		QString previousInterface;
		for (QMap<VhdlPortSorter, QSharedPointer<VhdlPort> >::const_iterator i = ports_.begin(); i != ports_.end(); ++i) {
			
			// if the port is first in the interface, then introduce the interface
			if (i.key().interface() != previousInterface) {
				const QString interfaceName = i.key().interface();

				stream << endl << "  " << "  " << "  " << "-- ";

				if (interfaceName == QString("none")) {
					stream << "These ports are not in any interface" << endl;
				}
				else if (interfaceName == QString("several")) {
					stream << "There ports are contained in many interfaces" << endl;
				}
				else {	
					stream << "Interface: " << interfaceName << endl;
					const QString description = component_->getInterfaceDescription(
						interfaceName);
					if (!description.isEmpty()) {
						VhdlGeneral::writeDescription(description, stream, QString("      "));
					}
				}
				previousInterface = interfaceName;
			}
			
			stream << "  " << "  " << "  "; //3 indents
			i.value()->write(stream); // print the actual port definition


			// if this is not the last port to write
			if (i + 1 != ports_.end()) {
				stream << ";";
			}

			if (!i.value()->description().isEmpty()) {
				stream << " ";
				VhdlGeneral::writeDescription(i.value()->description(), stream);
			}
			else {
				stream << endl;
			}
		}
		stream << "    );" << endl;
	}

	stream << "  end component;" << endl;
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

QString VhdlComponentDeclaration::portType( const QString& portName ) const {

	// used to search for the correct port
	VhdlPortSorter sorter(component_->getInterfaceNameForPort(portName),
		portName, component_->getPortDirection(portName));
	
	// if the named port is found
	if (ports_.contains(sorter)) {
		return ports_.value(sorter)->type();
	}
	else {
		return QString("undefined port");
	}
}

bool VhdlComponentDeclaration::isScalarPort( const QString& portName ) const {

	VhdlPortSorter sorter(component_->getInterfaceNameForPort(portName),
		portName, component_->getPortDirection(portName));

	QString type;

	if (ports_.contains(sorter)) {
		type = ports_.value(sorter)->type();
		return VhdlGeneral::isScalarType(type);
	}
	// if port is not found then it is not scalar
	else {
		return false;
	}
}

void VhdlComponentDeclaration::setEntityName( const QString& entityName ) {
	typeName_ = entityName;
}

General::Direction VhdlComponentDeclaration::portDirection( const QString& portName ) const {
	return component_->getPortDirection(portName);
}

int VhdlComponentDeclaration::getPortPhysLeftBound( const QString& portName ) const {
	return component_->getPortLeftBound(portName);
}

int VhdlComponentDeclaration::getPortPhysRightBound( const QString& portName ) const {
	return component_->getPortRightBound(portName);
}	