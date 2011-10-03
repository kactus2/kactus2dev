/* 
 *
 *  Created on: 18.10.2010
 *      Author: Antti Kamppi
 */

#include "vector.h"
#include "generaldeclarations.h"

#include "../exceptions/parse_error.h"
#include "../exceptions/write_error.h"

#include <QDomNode>
#include <QXmlStreamWriter>
#include <QString>
#include <QMap>

Vector::Vector(QDomNode &vectorNode): left_(-1), leftAttributes_(),
right_(-1), rightAttributes_() {

	// go through all child nodes
	for (int i = 0; i < vectorNode.childNodes().count(); ++i) {
		QDomNode tempNode = vectorNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:left")) {

			left_ = tempNode.childNodes().at(0).nodeValue().toInt();

			// get the attributes for the left-element
			General::parseAttributes(tempNode, leftAttributes_);
		}

		else if (tempNode.nodeName() == QString("spirit:right")) {

			right_ = tempNode.childNodes().at(0).nodeValue().toInt();

			// get the attributes for the right-element
			General::parseAttributes(tempNode, rightAttributes_);
		}
	}

	// if left bound was not specified
	if (left_ < 0) {
		throw Parse_error(QString("Mandatory element spirit:left missing in"
				" spirit:vector"));
	}

	// if right was not specified
	if (right_ < 0) {
		throw Parse_error(QString("Mandatory element spirit:right missing"
				" in spirit:vector"));
	}
	return;
}

//-----------------------------------------------------------------------------
// Function: Vector()
//-----------------------------------------------------------------------------
Vector::Vector(Vector const& other) : 
left_(other.left_), 
leftAttributes_(other.leftAttributes_),
right_(other.right_), 
rightAttributes_(other.rightAttributes_) {
}

Vector::Vector( int left, int right ): left_(left), leftAttributes_(),
right_(right), rightAttributes_() {

}

Vector::Vector(): left_(-1), leftAttributes_(),
right_(-1), rightAttributes_() {

}

Vector& Vector::operator=( const Vector& other ) {
	if (this != &other) {
		left_ = other.left_;
		leftAttributes_ = other.leftAttributes_;
		right_ = other.right_;
		rightAttributes_ = other.rightAttributes_;
	}
	return *this;
}

Vector::~Vector() {

}

void Vector::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:vector");

	// if left bound is invalid
	if (left_ < 0) {
		throw Write_error(QObject::tr("Invalid left bound in spirit:vector"));
	}
	else {
		// start the spirit:left tag
		writer.writeStartElement("spirit:left");

		// write the attributes for the element
		General::writeAttributes(writer, leftAttributes_);

		// write the value of the element and close the tag
		writer.writeCharacters(QString::number(left_));
		writer.writeEndElement(); // spirit:left
	}

	// if right bound is invalid
	if (right_ < 0) {
		throw Write_error(QObject::tr("Invalid right bound in spirit:vector"));
	}
	else {
		// start the spirit:right tag
		writer.writeStartElement("spirit:right");

		// write the attributes for the element
		General::writeAttributes(writer, rightAttributes_);

		// write the value of the element and close the tag
		writer.writeCharacters(QString::number(right_));
		writer.writeEndElement(); // spirit:right
	}

	writer.writeEndElement(); // spirit:vector
}

int Vector::getLeft() const {
	return left_;
}

int Vector::getRight() const {
	return right_;
}

const QMap<QString, QString>& Vector::getLeftAttributes() {
	return leftAttributes_;
}

const QMap<QString, QString>& Vector::getRightAttributes() {
	return rightAttributes_;
}

bool Vector::isValid() const {
	
	if (left_ < 0 || right_ < 0)
		return false;

	return true;
}

void Vector::setLeft( int left ) {
	left_ = left;
}

void Vector::setRight( int right ) {
	right_ = right;
}


