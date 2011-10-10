/* 
 *
 *  Created on: 2.11.2010
 *      Author: Antti Kamppi
 */

#include "componenttype.h"
#include "connection.h"

#include <models/generaldeclarations.h>
#include <models/component.h>
#include <models/port.h>
#include <models/modelparameter.h>
#include <models/model.h>
#include <models/wire.h>

#include <exceptions/vhdl_error.h>

#include <QObject>
#include <QList>
#include <QSharedPointer>

#include <QDebug>

ComponentType::VhdlGeneric::VhdlGeneric(const QString name, const QString type,
										const QString& defaultValue, 
										const QString& description):
name_(name),
type_(type),
defaultValue_(defaultValue),
description_(description) {
}

ComponentType::ComponentType(QSharedPointer<Component> component, QObject* parent):
QObject(parent),
typeName_(),
generics_(),
ports_(),
description_() {

	Q_ASSERT_X(component, "ComponentType constructor",
		"Null component pointer given as parameter");

	description_ = component->getDescription();

	// the name of the component
	typeName_ = component->getVlnv()->getName().remove(QString(".comp"));

	parseInterface(component, generics_);
}

ComponentType::~ComponentType() {
	generics_.clear();
	ports_.clear();
}

void ComponentType::parseInterface(QSharedPointer<Component> component,
		QMap<QString, QSharedPointer<VhdlGeneric> >& outputgenerics) {

	// get the generics for the component
	QMap<QString, QSharedPointer<ModelParameter> > modelParameters =
			component->getModel()->getModelParameters();

	// go through each modelParameter and add it to the list of generics
	for (QMap<QString, QSharedPointer<ModelParameter> >::iterator i =
			modelParameters.begin(); i != modelParameters.end(); ++i) {

		QString name = i.key();

		QString type = i.value()->getAttributes().value(
				QString("spirit:dataType"));

		QString defaultValue = i.value()->getValue();

		// name and type must be specified, default value is optional
		if (name.isEmpty() || type.isEmpty()) {
			throw Vhdl_error(tr("Invalid generic for component"));
		}

		// if the map already contains a generic with this name
		if (outputgenerics.contains(name)) {

			// delete the old generic to avoid memory leaks.
			delete outputgenerics.value(name).data();
		}

		// insert the generic into map and replace the old key
		outputgenerics.insert(name, QSharedPointer<VhdlGeneric>(
				new VhdlGeneric(name, type, defaultValue)));

	}
	// now generics have been declared

	// ports are set later when signals are created
}

void ComponentType::generateComponentVhdl(QTextStream& vhdlStream) {
	
	if (!description_.isEmpty()) {
		vhdlStream << "\t-- " << description_ << endl;
	}
	
	// write the component name
	vhdlStream << "\tcomponent " << typeName_ << endl;

	// if generics exist
	if (!generics_.isEmpty()) {

		vhdlStream << "\t\tgeneric (" << endl;

		for (QMap<QString, QSharedPointer<VhdlGeneric> >::iterator i =
				generics_.begin(); i != generics_.end(); ++i) {

			// write generic name and type
            QString type = i.value()->type_;
			vhdlStream << "\t\t\t" << i.value()->name_ << " : "
					<< type;

			// if a default value has been defined
            QString value = i.value()->defaultValue_;
			if (!value.isEmpty()) {
				vhdlStream << " := " << quoteGenericValue(type, value);
			}

			// if the last generic
			if (i == --generics_.end()) {
				vhdlStream << ");" << endl;
			}
			// if theres generics left
			else {
				vhdlStream << ";" << endl;
			}
		}
	} // now component generics have been written

	// if ports exist
	if (!ports_.isEmpty()) {

		vhdlStream << "\t\tport (" << endl;

		// write each port
		for (QMap<QString, QSharedPointer<Connection> >::iterator i = ports_.begin();
				i != ports_.end(); ++i) {

			QString portDescription = i.value()->getDescription();
			if (!portDescription.isEmpty()) {
				vhdlStream << "\t\t\t-- " << portDescription << endl;
			}
			// write port
			i.value()->writePortDeclaration(vhdlStream);

			// if this was the last port
			if (i == --ports_.end()) {
				vhdlStream << ");" << endl;
			}
			// if theres more ports left
			else {
				vhdlStream << ";" << endl;
			}
			// this port definition has been written

		}
		// all ports have been written
	}

	// end the component declaration
	vhdlStream << "\tend component;" << endl;
	return;
}

QString ComponentType::getType() const {
	return typeName_;
}

QString ComponentType::getInstanceType() const {
        return typeName_;
}

bool ComponentType::hasGeneric(const QString& name) const {
	if (generics_.contains(name)) {
		return true;
	}
	return false;
}

QString ComponentType::genericType(const QString& name) const {
    
    Q_ASSERT(generics_.contains(name));
    return generics_[name]->type_.toLower();
}

QStringList ComponentType::getPortNames() const {

	// add the name of each port to the list
	QStringList list;
	foreach (QSharedPointer<Connection> port, ports_) {
		
		if (port->getDirection() == General::OUT)
			list.append(port->getName());
	}
	return list;
}

void ComponentType::createPort( const QString& portName, 
							   General::Direction direction, 
							   int left, int right,
							   const QString& description) {

	// if port already exists
	if (ports_.contains(portName)) {
		
		// get the original bounds
		int oldLeft = ports_.value(portName)->getLeft();
		int oldRight = ports_.value(portName)->getRight();

		// set the new bounds so that port can grow but not shrink
		int newLeft = qMax(oldLeft, left);
		int newRight = qMin(oldRight, right);

		// save new bounds
		ports_.value(portName)->setBounds(newLeft, newRight);
		return;
	}

	Connection* port = new Connection(portName, direction, left, right, description, this);
	ports_.insert(portName, QSharedPointer<Connection>(port));
}

// TODO: This is butt ugly, exact same function in two different classes.
//       VHDL Generator could use some refactoring, lots of same kind of
//       things (e.g. generic mapping) are done in multiple different places.
QString ComponentType::quoteGenericValue(
    const QString& type, const QString& value) const {

    if (type.toLower() != "string") {
        return value;
    }
    
    QString quoted;
    if (!value.startsWith("\"")) {
        quoted += "\"";
    }
    
    quoted += value;
    
    if (!value.endsWith("\"")) {
        quoted += "\"";
    }
    return quoted;
}
