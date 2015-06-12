//-----------------------------------------------------------------------------
// File: ExitScreen.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 12.06.2015
//
// Description:
// Exit screen for Kactus2.
//-----------------------------------------------------------------------------

#include "ExitScreen.h"

#include <QPalette>
#include <QLabel>
#include <QBitmap>
#include <QTimer>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QSettings>

//-----------------------------------------------------------------------------
// Function: ExitScreen::ExitScreen()
//-----------------------------------------------------------------------------
ExitScreen::ExitScreen(QWidget* parent) : QDialog(parent), 
    closeCounter_(7), 
    countdownLabel_(new QLabel(this))
{
    setFixedSize(600, 400);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);

    QPalette defaultPalette = palette();
    defaultPalette.setBrush(QPalette::Background, QBrush(QImage(":/common/graphics/splash.png")));
    setPalette(defaultPalette);

    QLabel* feedbackText = new QLabel(tr("Thank you for using Kactus2.<br >"
    "Please give us 3 minutes and help us make Kactus2 even better. "
    "Visit our feedback page at:<br>"), this);
    
    feedbackText->setAlignment(Qt::AlignCenter);

    QFont feedbackTextFont = feedbackText->font();
    feedbackTextFont.setPixelSize(12);
    feedbackTextFont.setBold(true);
    feedbackText->setFont(feedbackTextFont);

    QLabel* feedbackUrl = new QLabel(tr(
    "<a href=\"http://funbase.cs.tut.fi/feedback\">http://funbase.cs.tut.fi/feedback</a>"), this);

    feedbackUrl->setAlignment(Qt::AlignCenter);
    feedbackUrl->setOpenExternalLinks(true);

    QFont urlFont = feedbackUrl->font();
    urlFont.setPixelSize(14);
    urlFont.setBold(true);
    feedbackUrl->setFont(urlFont);

    QCheckBox* hideCheckBox = new QCheckBox(tr("Do not show this window again"), this);
    connect(hideCheckBox, SIGNAL(toggled(bool)), this, SLOT(onShowHideToggled(bool)));

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addSpacing(220);
    layout->addWidget(feedbackText);
    layout->addWidget(feedbackUrl);
    layout->addStretch();
    layout->addWidget(countdownLabel_, 1, Qt::AlignBottom | Qt::AlignCenter);
    layout->addWidget(hideCheckBox);
}

//-----------------------------------------------------------------------------
// Function: ExitScreen::~ExitScreen()
//-----------------------------------------------------------------------------
ExitScreen::~ExitScreen()
{

}

//-----------------------------------------------------------------------------
// Function: ExitScreen::showEvent()
//-----------------------------------------------------------------------------
void ExitScreen::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);

    QTimer* timer = new QTimer(this);
    timer->setInterval(1000);

    connect(timer, SIGNAL(timeout()), this, SLOT(onTimerExpired()));
    timer->start();
}

//-----------------------------------------------------------------------------
// Function: ExitScreen::onTimerExpired()
//-----------------------------------------------------------------------------
void ExitScreen::onTimerExpired()
{
    closeCounter_--;

    if (closeCounter_ <= 0)
    {
        accept();
    }

    countdownLabel_->setText(tr("This window will close  in %1 seconds").arg(closeCounter_));
}

//-----------------------------------------------------------------------------
// Function: ExitScreen::onShowHideToggled()
//-----------------------------------------------------------------------------
void ExitScreen::onShowHideToggled(bool hide)
{
    QSettings settings;
    settings.setValue("General/showExitScreen", !hide);
}
