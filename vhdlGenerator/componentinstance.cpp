/* 
 *
 *  Created on: 2.11.2010
 *      Author: Antti Kamppi
 */

#include <models/generaldeclarations.h>

#include <exceptions/vhdl_error.h>

#include "componentinstance.h"
#include "connection.h"
#include "componenttype.h"

#include <QObject>
#include <QMap>
#include <QTextStream>
#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QStringList>

ComponentInstance::ComponentInstance(const QString& instanceName,
		const QString& typeName, 
		QSharedPointer<Component> component, 
		ComponentType* compDeclaration,
		QObject* parent):
QObject(parent),
instanceName_(instanceName), 
componentType_(typeName),
compDeclaration_(compDeclaration),
genericMap_(),
portMap_(), 
component_(component) {

	Q_ASSERT_X(!instanceName.isEmpty(), "ComponentInstance constructor",
		"Empty instance name given as parameter");
	Q_ASSERT_X(!typeName.isEmpty(), "ComponentInstance constructor",
		"Empty type name given as parameter");
	Q_ASSERT_X(component, "ComponentInstance constructor",
		"Null component pointer given as parameter");
}

ComponentInstance::~ComponentInstance() {
	portMap_.clear();
}

QSharedPointer<Component> ComponentInstance::getComponent() const {
	return component_;
}

void ComponentInstance::generateInstanceVhdl(QTextStream& stream) {

	// write the instance name and library
	stream << "\t" << instanceName_ << " : " << componentType_ << endl;

	// if generics exist
	if (!genericMap_.isEmpty()) {
		stream << "\t\tgeneric map (" << endl;

		// map each generic
		for (QMap<QString, QString>::iterator i = genericMap_.begin();
				i != genericMap_.end(); ++i) {

			// if there is no mapping for generic then skip it
			if (i.value().isEmpty()) {
				continue;
			}

			// write each mapping
			stream << "\t\t\t" << i.key() << " => " << i.value();

			// if this is the last generic
			if (i == --genericMap_.end()) {
				stream << ")" << endl;
			}
			// if theres still generics left
			else {
				stream << "," << endl;
			}
		} // all generics have been mapped
	}

	// ask the component declaration for list of declared ports.
	QStringList portDeclarations = compDeclaration_->getPortNames();

	// if ports exist
	if (!portMap_.isEmpty() || !portDeclarations.isEmpty()) {
		stream << "\t\tport map (" << endl;

		// map all ports
		for (QMap<QSharedPointer<Connection>,
				QSharedPointer<Connection> >::iterator i = portMap_.begin();
				i != portMap_.end(); ++i) {

			// write the port mapping
			stream << "\t\t\t";
			i.key()->writePortMap(stream);
			stream << " => ";
			i.value()->writePortMap(stream);

			// remove the port from the list
			portDeclarations.removeAll(i.key()->getName());

			// if this is the last port mapping
			if (i == --portMap_.end() && portDeclarations.isEmpty()) {
				stream << ");" << endl;
				return;
			}
			// if theres port mappings left
			else {
				stream << "," << endl;
			}
		} // all port mappings have been written.

		// if there are ports declared but not mapped
		for (int i = 0; i < portDeclarations.size(); ++i) {

			// map each unmapped port as open
			stream << "\t\t\t " << portDeclarations.at(i) << " => open";

			// if this was last port
			if (i == portDeclarations.size()-1) {
				stream << ");" << endl;
				return;
			}
			else 
				stream << "," << endl;
		}
	}
}

void ComponentInstance::createPortMap( const QString& signalName, 
									  int signalLeft, int signalRight,
									  const QString& portName, 
									  int portLeft, int portRight) {

	Connection* port = new Connection(portName, this);
	port->setBounds(portLeft, portRight);
	connect(port, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(port, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	Signal* signal = new Signal(signalName, this);
	signal->setBounds(signalLeft, signalRight);
	connect(signal, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(signal, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	portMap_.insert(QSharedPointer<Connection>(port),
		QSharedPointer<Signal>(signal));
}

void ComponentInstance::addPortMap(Connection* port, Connection* signal) {
	portMap_.insert(QSharedPointer<Connection>(port),
			QSharedPointer<Connection>(signal));
}

void ComponentInstance::setGenericMap(const QString instanceGeneric,
		const QString value) {

	// one generic can be mapped only once, the last given value stands
	genericMap_.insert(instanceGeneric, value);
	return;
}

QString ComponentInstance::getName() const {
	return instanceName_;
}

ComponentType* ComponentInstance::getCompDeclaration() {
	return compDeclaration_;
}

void ComponentInstance::createPortDeclaration( const Connection::EndPoint& endPoint ) {

	// get name and direction of the port
	QString portName = endPoint.portName_;
	General::Direction direction = component_->getPortDirection(portName);
	
	// find the port from the component
	Port* portModel = component_->getPort(portName);
	if (!portModel) {
		emit errorMessage(tr("The referenced port %1 was not found in "
			"component %2").arg(portName).arg(component_->getVlnv()->getName()));
		return;
	}

	// get the left and right bound
	int left = portModel->getLeftBound();
	int right = portModel->getRightBound();

	// create the port declaration for component type
	compDeclaration_->createPort(portName, direction, left, right);
}
