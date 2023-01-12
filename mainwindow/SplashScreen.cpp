//-----------------------------------------------------------------------------
// File: SplashScreen.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 24.5.2011
//
// Description:
// Kactus2 splash screen.
//-----------------------------------------------------------------------------

#include "SplashScreen.h"

#include <QPainter>
#include <QLabel>
#include <QBitmap>

//-----------------------------------------------------------------------------
// Function: SplashScreen::SplashScreen()
//-----------------------------------------------------------------------------
SplashScreen::SplashScreen(QString const& versionString, QWidget* parent /*= 0*/):
QSplashScreen(QPixmap(":/common/graphics/splash.png")),
    versionString_(versionString)
{
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
    setCursor(Qt::PointingHandCursor);
}

//-----------------------------------------------------------------------------
// Function: SplashScreen::~SplashScreen()
//-----------------------------------------------------------------------------
SplashScreen::~SplashScreen()
{

}

//-----------------------------------------------------------------------------
// Function: drawContents()
//-----------------------------------------------------------------------------
void SplashScreen::drawContents(QPainter *painter)
{
    // Draw the version text.
    painter->setPen(QPen(Qt::black, 1));

    QFont font = painter->font();
    font.setPixelSize(14);
    font.setBold(true);
    painter->setFont(font);

    painter->drawText(QRectF(5, 220, 590, 35), Qt::AlignCenter,
        tr("Version %1\nQt version %2")
        .arg(versionString_, qVersion()));

    // Draw the other information.
    font.setPixelSize(12);
    font.setBold(false);
    painter->setFont(font);
 
    painter->drawText(QRectF(5, 240, 590, 160), Qt::AlignCenter,
        QString::fromLatin1("Copyright 2010-2023 Tampere University,\n"
                            "Korkeakoulunkatu 10, FI-33720 Tampere, Finland\n"
                            "https://research.tuni.fi/system-on-chip/tools\n"
                            "kactus2@tuni.fi\n"
                            "Kactus2 is available for dual licensing.\n"
                            "This software is licensed under the General Public License Version 2.\n"
                            "Contributors: Antti Kamppi, Esko Pekkarinen, Janne Virtanen, Joni-Matti Maatta, \n"
                            "Juho Järvinen, Lauri Matilainen, Mikko Teuho, Timo D. Hämäläinen"));
}

//-----------------------------------------------------------------------------
// Function: SplashScreen::mousePressEvent()
//-----------------------------------------------------------------------------
void SplashScreen::mousePressEvent( QMouseEvent* event )
{
    event->accept();
    close();
}

//-----------------------------------------------------------------------------
// Function: SplashScreen::keyPressEvent()
//-----------------------------------------------------------------------------
void SplashScreen::keyPressEvent(QKeyEvent* event)
{
    event->accept();
    close();
}
