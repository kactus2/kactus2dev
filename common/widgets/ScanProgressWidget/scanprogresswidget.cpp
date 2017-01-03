//-----------------------------------------------------------------------------
// File: scanprogresswidget.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 08.02.2012
//
// Description:
// Displays a progress bar and a message of what is being scanned.
//-----------------------------------------------------------------------------

#include "scanprogresswidget.h"

#include <QVBoxLayout>
#include <QDialogButtonBox>

//-----------------------------------------------------------------------------
// Function: ScanProgressWidget::ScanProgressWidget()
//-----------------------------------------------------------------------------
ScanProgressWidget::ScanProgressWidget(QWidget *parent):
QDialog(parent),
    progressBar_(this),
    messageLabel_(this)
{
	setWindowTitle(tr("Scanning library..."));

    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Cancel, this);
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&progressBar_);
	layout->addWidget(&messageLabel_);
    layout->addWidget(buttons);
	layout->addStretch();

	progressBar_.setOrientation(Qt::Horizontal);

	setFixedWidth(800);
    setFixedHeight(105);
}

//-----------------------------------------------------------------------------
// Function: ScanProgressWidget::~ScanProgressWidget()
//-----------------------------------------------------------------------------
ScanProgressWidget::~ScanProgressWidget()
{
}

//-----------------------------------------------------------------------------
// Function: ScanProgressWidget::ScanProgressWidget()
//-----------------------------------------------------------------------------
void ScanProgressWidget::setRange(int min, int max)
{
	progressBar_.setRange(min, max);
}

//-----------------------------------------------------------------------------
// Function: ScanProgressWidget::ScanProgressWidget()
//-----------------------------------------------------------------------------
void ScanProgressWidget::setMessage(QString const& message)
{
	messageLabel_.setText(message);
}

//-----------------------------------------------------------------------------
// Function: ScanProgressWidget::ScanProgressWidget()
//-----------------------------------------------------------------------------
void ScanProgressWidget::setValue(int value)
{
	progressBar_.setValue(value);
}
