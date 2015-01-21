//-----------------------------------------------------------------------------
// File: SplashScreen.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 24.5.2011
//
// Description:
// Kactus2 splash screen.
//-----------------------------------------------------------------------------

#include "SplashScreen.h"

#include <version.h>
#include <QPainter>
#include <QLabel>
#include <QBitmap>

#include <QDebug>

//-----------------------------------------------------------------------------
// Function: SplashScreen()
//-----------------------------------------------------------------------------
SplashScreen::SplashScreen(QWidget* parent) : QSplashScreen(parent, QPixmap(":/common/graphics/splash.png"))
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
    painter->setPen(QPen(Qt::black, 1));

#if defined (_WIN64) || (__LP64__) || (_LP64)
    int bits = 64;
#else
    int bits = 32;
#endif

    QFont font = painter->font();
    font.setPixelSize(14);
    font.setBold(true);
    painter->setFont(font);

    painter->drawText(240, 240, tr("Version %1.%2.%3 %4-bit").arg(QString::number(VERSION_MAJOR),
                                                                  QString::number(VERSION_MINOR),
                                                                  QString::number(VERSION_BUILDNO),
                                                                  QString::number(bits)));
    // Draw the other information.
    font.setPixelSize(12);
    font.setBold(false);
    painter->setFont(font);    
 
    painter->drawText(QRectF(5, 240, 590, 160), Qt::AlignCenter,
        QString::fromLatin1("Copyright 2010-2015 Tampere University of Technology,\n" 
                            "Korkeakoulunkatu 10, FI-33720 Tampere, Finland\n"
                            "http://funbase.cs.tut.fi\n"
                            "kactus2@cs.tut.fi\n" 
                            "Kactus2 is available for dual licensing.\n"
                            "This software is licensed under the General Public License Version 2.\n"
                            "Contributors: Antti Kamppi, Esko Pekkarinen, Janne Virtanen, Joni-Matti M‰‰tt‰, \n"
                            "Juho J‰rvinen, Lauri Matilainen, Mikko Teuho, Timo D. H‰m‰l‰inen"));
}

void SplashScreen::mousePressEvent( QMouseEvent* event ) {
	event->accept();
	close();
}

void SplashScreen::keyPressEvent(QKeyEvent* event) {
	event->accept();
	close();
}
