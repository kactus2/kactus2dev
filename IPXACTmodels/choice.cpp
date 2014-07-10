/* $Id
 *
 *  Created on: 15.10.2010
 *      Author: Antti Kamppi
 */

#include "choice.h"
#include "generaldeclarations.h"
#include "XmlUtils.h"

#include <QList>
#include <QString>
#include <QXmlStreamWriter>

// struct constructor
Choice::Choice(QDomNode &choice) : choiceName_(),
enumerations_() {

	QDomNodeList children = choice.childNodes();
	for (int i = 0; i < children.size(); ++i) {

		// get name
		if (children.at(i).nodeName() == QString("spirit:name")) {
			choiceName_ = children.at(i).childNodes().at(0).nodeValue();
			choiceName_ = XmlUtils::removeWhiteSpace(choiceName_);
		}

		// get enumerations
		else if (children.at(i).nodeName() == QString("spirit:enumeration")) {

			// get the name of the enumeration
			QString value =
					children.at(i).childNodes().at(0).nodeValue();

			// get the attributes
			QDomNamedNodeMap attributeMap = children.at(i).attributes();

			QString attText = attributeMap.namedItem(QString(
					"spirit:text")).nodeValue();
			QString attHelp = attributeMap.namedItem(QString(
					"spirit:help")).nodeValue();

                        enumerations_.append(General::Enumeration(value, attText, attHelp));
		}
	}

	// name is mandatory
// 	if (choiceName_.isNull()) {
// 		throw Parse_error(QObject::tr(
// 				"Mandatory element spirit:name missing in spirit:choice"));
// 		return;
// 	}
// 
// 	// at least 1 enumeration must be found
// 	if (enumerations_.size() < 1) {
// 		throw Parse_error(QObject::tr("Mandatory element spirit:enumeration "
// 				"missing in spirit:choice"));
// 		return;
// 	}
	return;
}

Choice::Choice( const Choice& other ):
choiceName_(other.choiceName_),
enumerations_(other.enumerations_) {
}

Choice& Choice::operator=( const Choice& other ) {
	if (this != &other) {
		choiceName_ = other.choiceName_;
		enumerations_ = other.enumerations_;
	}
	return *this;
}

Choice::~Choice() {
}

void Choice::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:choice");

	writer.writeTextElement("spirit:name", choiceName_);

	// if atleast one enumeration exists
    for (int i = 0; i < enumerations_.size(); ++i) {

        // open the spirit:enumeration tag
        writer.writeStartElement("spirit:enumeration");

        // if attribute exists
        if (!enumerations_.at(i).attribute_text_.isEmpty()) {
            writer.writeAttribute("spirit:text",
                enumerations_.at(i).attribute_text_);
        }

        // if attribute exists
        if (!enumerations_.at(i).attribute_help_.isEmpty()) {
            writer.writeAttribute("spirit:help",
                enumerations_.at(i).attribute_help_);
        }

        // write the enumeration value and close the enumeration-tag
        writer.writeCharacters(enumerations_.at(i).value_);
        writer.writeEndElement(); // spirit:enumeration
    }

	writer.writeEndElement(); // spirit:choice
	return;
}

bool Choice::isValid( QStringList& errorList, const QString& parentIdentifier ) const {
	bool valid = true;
	
	if (choiceName_.isEmpty()) {
		errorList.append(QObject::tr("No name specified for choice within %1").arg(
			parentIdentifier));
		valid = false;
	}

	if (enumerations_.isEmpty()) {
		errorList.append(QObject::tr("At least one enumeration is required in"
			" choice %1 within %2").arg(choiceName_).arg(parentIdentifier));
		valid = false;
	}

	return valid;
}

bool Choice::isValid() const {
	if (choiceName_.isEmpty()) {
		return false;
	}

	if (enumerations_.isEmpty()) {
		return false;
	}
	return true;
}

QString Choice::getName() const {
	return choiceName_;
}

void Choice::setName(const QString& name) {
	choiceName_ = name;
}

const QList<General::Enumeration>& Choice::getEnumerations() const {
	return enumerations_;
}

void Choice::setEnumerations(QList<General::Enumeration>& enumerations) {
	enumerations_.clear();
	enumerations_ = enumerations;
}
