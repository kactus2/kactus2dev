/* 
 *
 *  Created on: 1.4.2011
 *      Author: Antti Kamppi
 * 		filename: hexspinbox.cpp
 */

#include "hexspinbox.h"

#include <QRegExp>

HexSpinBox::HexSpinBox(QWidget *parent): QSpinBox(parent), validator_(NULL) {

	setRange(0, 0xFFFF);
	validator_ = new QRegExpValidator(QRegExp("[0-9A-Fa-f]{1,8}"), this);
}

HexSpinBox::~HexSpinBox() {
}

QValidator::State HexSpinBox::validate( QString& text, int& pos ) const {
	return validator_->validate(text, pos);
}

int HexSpinBox::valueFromText( const QString &text ) const {
	bool ok;
	return text.toInt(&ok, 16);
}

QString HexSpinBox::textFromValue( int value ) const {
	QString text("0x");
	text += QString::number(value, 16).toUpper();
	return text;
}
