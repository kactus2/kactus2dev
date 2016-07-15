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

#include <QPainter>
#include <QLabel>
#include <QBitmap>

//-----------------------------------------------------------------------------
// Function: SplashScreen()
//-----------------------------------------------------------------------------
SplashScreen::SplashScreen(QString versionString, QWidget* parent /*= 0*/) : 
versionString_(versionString), QSplashScreen(parent, QPixmap(":/common/graphics/splash.png"))
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
        QString::fromLatin1("Copyright 2010-2016 Tampere University of Technology,\n" 
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
