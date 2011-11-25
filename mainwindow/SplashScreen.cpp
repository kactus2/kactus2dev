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
    painter->drawText(300, 125, tr("Version ") + QString::number(VERSION_MAJOR) + "." +
        QString::number(VERSION_MINOR) + tr(" (Build ") + QString::number(VERSION_BUILDNO) + ")");

    // Draw the other information.
    painter->drawText(QRectF(270, 280, 310, 150), Qt::AlignLeft,
                      "This software is licensed under the GPL2 General Public License.\n"
                      "See license.txt for further details.\n\n"
                      "Contributors: Juho J‰rvinen, Antti Kamppi, Joni-Matti M‰‰tt‰,\n"
                      "Lauri Matilainen, Timo D. H‰m‰l‰inen\n\n"
                      "Kactus2 Website: http://funbase.cs.tut.fi/");
}

void SplashScreen::mousePressEvent( QMouseEvent* event ) {
	event->accept();
	close();
}
