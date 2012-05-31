/* 
 *  	Created on: 31.5.2012
 *      Author: Antti Kamppi
 * 		filename: summarylabel.cpp
 *		Project: Kactus 2
 */

#include "summarylabel.h"

#include <QFont>

SummaryLabel::SummaryLabel( const QString& text, QWidget *parent ):
QLabel(text, parent) {

	QFont usedFont = font();
	usedFont.setWeight(QFont::Bold);
	usedFont.setPointSize(usedFont.pointSize() + SummaryLabel::POINTSIZE_INCREMENT);
	setFont(usedFont);
}

SummaryLabel::~SummaryLabel() {
}
