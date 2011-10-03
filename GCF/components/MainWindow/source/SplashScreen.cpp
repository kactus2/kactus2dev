/****************************************************************************
**
** Copyright (C) VCreate Logic Private Limited, Bangalore
**
** Use of this file is limited according to the terms specified by
** VCreate Logic Private Limited, Bangalore.  Details of those terms
** are listed in licence.txt included as part of the distribution package
** of this file. This file may not be distributed without including the
** licence.txt file.
**
** Contact info@vcreatelogic.com if any conditions of this licensing are
** not clear to you.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "SplashScreen.h"

#include <QPixmap>
#include <QPainter>
#include <QApplication>

/**
\ingroup stdmainwindowcomponent
\class GCF::Components::SplashScreen GCF::Components::SplashScreen.h
\brief This class provides functions to create splash screen when application
is being launched.

Message is shown on the splash screen when the components are being loaded when application is launched.
Provides showMessage function to show message on the splash screen.

The splash screen closes itself upon mouse press anywhere on the screen.

shown below is example splash screen
\image html splashscreen.png

*/

/**
Constructor

\param pixmap The pixmap to be shown for the splash screen.
*/
GCF::Components::SplashScreen::SplashScreen(const QPixmap & pixmap):
QSplashScreen(pixmap)
{
    setWindowFlags(Qt::WindowStaysOnTopHint|Qt::SplashScreen);
    qApp->installEventFilter(this);
}

/**
Destructor
*/
GCF::Components::SplashScreen::~SplashScreen()
{

}

/**
Shows the message on the splash screen while loading components at start up.

\param compName Name of the component to be shown in the message.
*/
void GCF::Components::SplashScreen::showMessage(const QString& compName)
{
    QSplashScreen::showMessage(QString("Loading component...%1").arg(compName),Qt::AlignBottom, Qt::blue);
}

void GCF::Components::SplashScreen::mousePressEvent(QMouseEvent* me)
{
    close();
}

bool GCF::Components::SplashScreen::eventFilter(QObject*, QEvent* e)
{
    if(e->type() == QEvent::MouseButtonPress || e->type() == QEvent::KeyPress)
        close();

    return false;
}
