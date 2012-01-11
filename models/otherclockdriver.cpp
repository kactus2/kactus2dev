/* 
 *
 *  Created on: 6.8.2010
 *      Author: Antti Kamppi
 */

#include "otherclockdriver.h"
#include "generaldeclarations.h"


#include "../exceptions/write_error.h"

#include <QString>
#include <QList>
#include <QDomNode>
#include <QDomNamedNodeMap>
#include <QObject>
#include <QSharedPointer>
#include <QXmlStreamWriter>

// the constructor
OtherClockDriver::OtherClockDriver(QDomNode &clockNode): clockName_(QString()),
clockSource_(QString()), clockPeriod_(0), clockPulseOffset_(0),
clockPulseValue_(0), clockPulseDuration_(0) {

	// get the attribute spirit:clockName & spirit:clockSource
	QDomNamedNodeMap attributeMap = clockNode.attributes();

	clockName_ = attributeMap.namedItem(QString(
			"spirit:clockName")).nodeValue();
	clockName_ = General::removeWhiteSpace(clockName_);
	clockSource_ = attributeMap.namedItem(QString("spirit:clockSource")).
			nodeValue();

	for (int i = 0; i < clockNode.childNodes().count(); ++i) {
		QDomNode tempNode = clockNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:clockPeriod")) {
			clockPeriod_ = QSharedPointer<General::ClockStruct>(
					new General::ClockStruct(tempNode));
		}
		else if (tempNode.nodeName() == QString("spirit:clockPulseOffset")) {
			clockPulseOffset_ = QSharedPointer<General::ClockStruct>(
					new General::ClockStruct(tempNode));
		}
		else if (tempNode.nodeName() == QString("spirit:clockPulseValue")) {
			clockPulseValue_ = QSharedPointer<General::ClockPulseValue>(
					new General::ClockPulseValue(tempNode));
		}
		else if (tempNode.nodeName() == QString("spirit:clockPulseDuration")){
			clockPulseDuration_ = QSharedPointer<General::ClockStruct>(
					new General::ClockStruct(tempNode));
		}
	}

	// if mandatory elements are missing
// 	if (clockName_.isNull()) {
// 		throw Parse_error(QObject::tr("Mandatory element name missing in "
// 				"spirit:otherClockDriver"));
// 	}
// 
// 	if (!clockPeriod_) {
// 		throw Parse_error(QObject::tr("Mandatory element spirit:clockPeriod"
// 				" missing in spirit:otherClockDriver"));
// 	}
// 
// 	if (!clockPulseOffset_) {
// 		throw Parse_error(QObject::tr("Mandatory element spirit:"
// 				"clockPulseOffset missing in spirit:otherClockDriver"));
// 	}
// 
// 	if (!clockPulseValue_) {
// 		throw Parse_error(QObject::tr("Mandatory element "
// 				"spirit:clockPulseValue missing in spirit:otherClockDriver"));
// 	}
// 
// 	if (!clockPulseDuration_) {
// 		throw Parse_error(QObject::tr(
// 				"Mandatory element spirit:clockPulseDuration missing in "
// 				"spirit:otherClockDriver"));
// 	}
	return;
}

OtherClockDriver::OtherClockDriver(): clockName_(),
clockSource_(QString()), clockPeriod_(0), clockPulseOffset_(0),
clockPulseValue_(0), clockPulseDuration_(0) {

}

OtherClockDriver::OtherClockDriver( const OtherClockDriver &other ):
clockName_(other.clockName_),
clockSource_(other.clockSource_),
clockPeriod_(),
clockPulseOffset_(),
clockPulseValue_(),
clockPulseDuration_() {

	if (other.clockPeriod_) {
		clockPeriod_ = QSharedPointer<General::ClockStruct>(
			new General::ClockStruct(*other.clockPeriod_.data()));
	}

	if (other.clockPulseOffset_) {
		clockPulseOffset_ = QSharedPointer<General::ClockStruct>(
			new General::ClockStruct(*other.clockPulseOffset_.data()));
	}

	if (other.clockPulseValue_) {
		clockPulseValue_ = QSharedPointer<General::ClockPulseValue>(
			new General::ClockPulseValue(*other.clockPulseValue_.data()));
	}

	if (other.clockPulseDuration_) {
		clockPulseDuration_ = QSharedPointer<General::ClockStruct>(
			new General::ClockStruct(*other.clockPulseDuration_.data()));
	}
}


OtherClockDriver & OtherClockDriver::operator=( const OtherClockDriver &other ) {
	if (this != &other) {
		clockName_ = other.clockName_;
		clockSource_ = other.clockSource_;

		if (other.clockPeriod_) {
			clockPeriod_ = QSharedPointer<General::ClockStruct>(
				new General::ClockStruct(*other.clockPeriod_.data()));
		}
		else
			clockPeriod_ = QSharedPointer<General::ClockStruct>();

		if (other.clockPulseOffset_) {
			clockPulseOffset_ = QSharedPointer<General::ClockStruct>(
				new General::ClockStruct(*other.clockPulseOffset_.data()));
		}
		else
			clockPulseOffset_ = QSharedPointer<General::ClockStruct>();

		if (other.clockPulseValue_) {
			clockPulseValue_ = QSharedPointer<General::ClockPulseValue>(
				new General::ClockPulseValue(*other.clockPulseValue_.data()));
		}
		else
			clockPulseValue_ = QSharedPointer<General::ClockPulseValue>();

		if (other.clockPulseDuration_) {
			clockPulseDuration_ = QSharedPointer<General::ClockStruct>(
				new General::ClockStruct(*other.clockPulseDuration_.data()));
		}
		else
			clockPulseDuration_ = QSharedPointer<General::ClockStruct>();
	}
	return *this;
}


// the destructor
OtherClockDriver::~OtherClockDriver() {
}

void OtherClockDriver::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:otherClockDriver");
	// if mandatory name is missing
	if (clockName_.isEmpty()) {
		throw Write_error(QObject::tr("Mandatory clockName missing in spirit:"
				"otherClockDriver"));
	}
	else {
		writer.writeAttribute("spirit:clockName", clockName_);
	}

	// if optional attribute clockSource exists
	if (!clockSource_.isEmpty()) {
		writer.writeAttribute("spirit:clockSource", clockSource_);
	}

	// write the spirit:clockPeriod element
	if (!clockPeriod_) {
		throw Write_error(QObject::tr("Mandatory spirit:clockPeriod missing in"
				" spirit:otherClockDriver"));
	}
	else {
		// start the spirit:clockPeriod tag
		writer.writeStartElement("spirit:clockPeriod");

		// write the attributes for the element
		writer.writeAttribute("spirit:delayValueUnitType",
				General::timeUnit2Str(clockPeriod_->timeUnit_));
		General::writeAttributes(writer, clockPeriod_->attributes_);

		// write the value of the element and close the tag
                writer.writeCharacters(QString::number(clockPeriod_->value_));
		writer.writeEndElement(); // spirit:clockPeriod
	}

	// write the spirit:clockPulseOffset
	if (!clockPulseOffset_) {
		throw Write_error(QObject::tr("Mandatory spirit:clockPulseOffset "
				" missing in spirit:otherClockDriver"));
	}
	else {
		// start the spirit:clockPulseOffset tag
		writer.writeStartElement("spirit:clockPulseOffset");

		// write the attributes for the element
		writer.writeAttribute("spirit:delayValueUnitType",
						General::timeUnit2Str(clockPeriod_->timeUnit_));
		General::writeAttributes(writer, clockPulseOffset_->attributes_);

		// write the value of the element and close the tag
                writer.writeCharacters(QString::number(clockPulseOffset_->value_));
		writer.writeEndElement(); // spirit:clockPulseOffset
	}

	// write the spirit:clockPulseValue
	if (!clockPulseValue_) {
		throw Write_error(QObject::tr("Mandatory spirit:clockPulseValue "
				"missing in spirit:otherClockDriver"));
	}
	else {
		// start the spirit:clockPulseValue tag
		writer.writeStartElement("spirit:clockPulseValue");

		// write the attributes for the element
		General::writeAttributes(writer, clockPulseValue_->attributes_);

		// write the value of the element and close the tag
                writer.writeCharacters(QString::number(clockPulseValue_->value_));
		writer.writeEndElement(); // spirit:clockPulseValue
	}

	// write the spirit:clockPulseDuration
	if (!clockPulseDuration_) {
		throw Write_error(QObject::tr("Mandatory spirit:clockPulseDuration"
				" missing int spirit:otherClockDriver"));
	}
	else {
		// start the spirit:clockPulseDuration tag
		writer.writeStartElement("spirit:clockPulseDuration");

		// write the attributes for the element
		writer.writeAttribute("spirit:delayValueUnitType",
						General::timeUnit2Str(clockPeriod_->timeUnit_));
		General::writeAttributes(writer, clockPulseDuration_->attributes_);

		// write the value of the element and close the tag
                writer.writeCharacters(QString::number(clockPulseDuration_->value_));
		writer.writeEndElement(); // spirit:clockPulseDuration
	}

	writer.writeEndElement(); // spirit:otherClockDriver
}

QString OtherClockDriver::getClockName() const {
	return clockName_;
}

void OtherClockDriver::setClockName(const QString &clockName) {
	clockName_ = clockName;
}

void OtherClockDriver::setClockPulseDuration(
		General::ClockStruct *clockPulseDuration) {
	// delete the old clock pulse duration
	if (clockPulseDuration_) {
		clockPulseDuration_.clear();
	}
	clockPulseDuration_ = QSharedPointer<General::ClockStruct>(
			clockPulseDuration);
}

General::ClockStruct *OtherClockDriver::getClockPeriod()  {
	
	if (clockPeriod_)
		return clockPeriod_.data();

	clockPeriod_ = QSharedPointer<General::ClockStruct>(new General::ClockStruct(0));
	return clockPeriod_.data();
}

void OtherClockDriver::setClockPeriod(General::ClockStruct *clockPeriod) {
	// delete the old clock period
	if (clockPeriod_) {
		clockPeriod_.clear();
	}
	clockPeriod_ = QSharedPointer<General::ClockStruct>(clockPeriod);
}

General::ClockStruct *OtherClockDriver::getClockPulseOffset() {
	if (clockPulseOffset_)
		return clockPulseOffset_.data();

	clockPulseOffset_ = QSharedPointer<General::ClockStruct>(new General::ClockStruct(0));
	return clockPulseOffset_.data();
}

void OtherClockDriver::setClockPulseOffset(
		General::ClockStruct *clockPulseOffset) {
	// delete the old clock pulse offset
	if (clockPulseOffset_) {
		clockPulseOffset_.clear();
	}
	clockPulseOffset_ = QSharedPointer<General::ClockStruct>(clockPulseOffset);
}

void OtherClockDriver::setClockSource(const QString &clockSource) {
	clockSource_ = clockSource;
}

General::ClockPulseValue *OtherClockDriver::getClockPulseValue() {
	if (clockPulseValue_)
		return clockPulseValue_.data();

	clockPulseValue_ = QSharedPointer<General::ClockPulseValue>(
		new General::ClockPulseValue(0));
	return clockPulseValue_.data();
}

QString OtherClockDriver::getClockSource() const {
	return clockSource_;
}

void OtherClockDriver::setClockPulseValue(
		General::ClockPulseValue *clockPulseValue) {
	// delete the old clock pulse value
	if (clockPulseValue_) {
		clockPulseValue_.clear();
	}
	clockPulseValue_ = QSharedPointer<General::ClockPulseValue>(clockPulseValue);
}

General::ClockStruct *OtherClockDriver::getClockPulseDuration() {
	
	if (clockPulseDuration_)
		return clockPulseDuration_.data();

	clockPulseDuration_ = QSharedPointer<General::ClockStruct>(
		new General::ClockStruct(0));
	return clockPulseDuration_.data();
}

bool OtherClockDriver::isValid() const {

	if (clockName_.isEmpty())
		return false;
	else if (!clockPeriod_)
		return false;
	else if (!clockPulseOffset_)
		return false;
	else if (!clockPulseValue_)
		return false;
	else if (clockPulseValue_->value_ != 0 && clockPulseValue_->value_ != 1)
		return false;
	else if (!clockPulseDuration_)
		return false;
	else 
		return true;
}

bool OtherClockDriver::isValid( QStringList& errorList, const QString& parentIdentifier ) const {
	bool valid = true;

	if (clockName_.isEmpty()) {
		errorList.append(QObject::tr("Name missing in other clock driver within %1").arg(
			parentIdentifier));
		valid = false;
	}

	if (!clockPeriod_) {
		errorList.append(QObject::tr("Clock period missing in other clock driver"
			" %1 within %2").arg(clockName_).arg(parentIdentifier));
		valid = false;
	}

	if (!clockPulseOffset_) {
		errorList.append(QObject::tr("Clock pulse offset missing in other clock "
			"driver %1 within %2").arg(clockName_).arg(parentIdentifier));
		valid = false;
	}

	if (!clockPulseValue_) {
		errorList.append(QObject::tr("Clock pulse value missing in other clock "
			"driver %1 within %2").arg(clockName_).arg(parentIdentifier));
		valid = false;
	}
	else if (clockPulseValue_->value_ != 0 && clockPulseValue_->value_ != 1) {
		errorList.append(QObject::tr("Invalid value set for clock pulse value in"
			" other clock driver %1 within %2").arg(clockName_).arg(parentIdentifier));
		valid = false;
	}

	if (!clockPulseDuration_) {
		errorList.append(QObject::tr("Clock pulse duration missing in other clock"
			" driver %1 within %2").arg(clockName_).arg(parentIdentifier));
		valid = false;
	}

	return valid;
}
