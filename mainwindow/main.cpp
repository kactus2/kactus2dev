//-----------------------------------------------------------------------------
// File: main.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 24.5.2011
//
// Description:
// Kactus 2 main entry point.
//-----------------------------------------------------------------------------

#include "mainwindow.h"
#include "SplashScreen.h"

#include <QApplication>
#include <QDebug>
#include <QPalette>
#include <QTimer>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(kactus);
    QApplication a(argc, argv);

    // Set the palette to use nice pastel colors.
    QPalette palette = a.palette();
    palette.setColor(QPalette::Active, QPalette::Highlight, QColor(33, 135, 237));
    palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(166, 200, 234));
    palette.setColor(QPalette::Inactive, QPalette::Highlight, QColor(166, 200, 234));
    a.setPalette(palette);

    // Show the splash screen.
    SplashScreen splash;
    splash.show();
    splash.showMessage("");
    a.processEvents();

    // Create the main window and close the splash after 1.5 seconds.
    MainWindow w;
    QTimer::singleShot(1500, &splash, SLOT(close()));
    QTimer::singleShot(1500, &w, SLOT(show()));
	return a.exec();
}
