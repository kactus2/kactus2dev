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
pathLabel_(this) {

	setWindowTitle(tr("Scanning library..."));

	QLabel* pathInfo = new QLabel(tr("Scanning path:"), this);
	
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&progBar_);
	layout->addWidget(pathInfo);
	layout->addWidget(&pathLabel_);
	layout->addStretch();

	progBar_.setOrientation(Qt::Horizontal);

	setMinimumWidth(800);

	setAttribute(Qt::WA_DeleteOnClose, true);
}

ScanProgressWidget::~ScanProgressWidget() {
}

void ScanProgressWidget::setRange( int min, int max ) {
	progBar_.setRange(min, max);
}

void ScanProgressWidget::setPath( const QString& path ) {
	pathLabel_.setText(path);
}

void ScanProgressWidget::setValue( int value ) {
	progBar_.setValue(value);
}
