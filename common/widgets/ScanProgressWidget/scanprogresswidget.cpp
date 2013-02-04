/* 
 *  	Created on: 8.2.2012
 *      Author: Antti Kamppi
 * 		filename: scanprogresswidget.cpp
 *		Project: Kactus 2
 */

#include "scanprogresswidget.h"

#include <QVBoxLayout>

ScanProgressWidget::ScanProgressWidget(QWidget *parent):
QDialog(parent),
progBar_(this),
messageLabel_(this) {

	setWindowTitle(tr("Scanning library..."));
    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&progBar_);
	layout->addWidget(&messageLabel_);
	layout->addStretch();

	progBar_.setOrientation(Qt::Horizontal);

	setFixedWidth(800);
    setFixedHeight(80);
}

ScanProgressWidget::~ScanProgressWidget() {
}

void ScanProgressWidget::setRange( int min, int max ) {
	progBar_.setRange(min, max);
}

void ScanProgressWidget::setMessage( const QString& message ) {
	messageLabel_.setText(message);
}

void ScanProgressWidget::setValue( int value ) {
	progBar_.setValue(value);
}
