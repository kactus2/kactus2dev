//-----------------------------------------------------------------------------
// File: SplashScreen.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 24.5.2011
//
// Description:
// Kactus 2 splash screen.
//-----------------------------------------------------------------------------

#include "SplashScreen.h"

#include <version.h>
#include <QPainter>
#include <QLabel>

#include <QDebug>

//-----------------------------------------------------------------------------
// Function: SplashScreen()
//-----------------------------------------------------------------------------
SplashScreen::SplashScreen(QWidget* parent) : QSplashScreen(parent, QPixmap(":/graphics/splash.png"))
{
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
}

//-----------------------------------------------------------------------------
// Function: ~SplashScreen()
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
    painter->setPen(QPen(Qt::white, 1));

#if defined (_WIN64) || (__LP64__) || (_LP64)
    int bits = 64;
#else
    int bits = 32;
#endif

    painter->drawText(300, 125, tr("Version %1.%2.%3 %4-bit").arg(QString::number(VERSION_MAJOR),
                                                                  QString::number(VERSION_MINOR),
                                                                  QString::number(VERSION_BUILDNO),
                                                                  QString::number(bits)));

    // Draw the other information.
    QFont font = painter->font();
    font.setPixelSize(11);
    painter->setFont(font);
 
    painter->drawText(QRectF(270, 280, 320, 150), Qt::AlignLeft,
                      QString::fromLatin1("This software is licensed under the GPL2 General Public License.\n"
                                          "See license.txt for further details.\n\n"
                                          "Contributors: Juho J‰rvinen, Antti Kamppi, Joni-Matti M‰‰tt‰,\n"
                                          "Esko Pekkarinen, Lauri Matilainen, Timo D. H‰m‰l‰inen\n\n"
                                          "Kactus2 Website: http://funbase.cs.tut.fi/"));
}

void SplashScreen::mousePressEvent( QMouseEvent* event ) {
	event->accept();
	close();
}

void SplashScreen::keyPressEvent(QKeyEvent* event) {
	event->accept();
	close();
}
