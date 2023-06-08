//-----------------------------------------------------------------------------
// File: SplashScreen.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 24.5.2011
//
// Description:
// Kactus 2 splash screen.
//-----------------------------------------------------------------------------

#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QSplashScreen>
#include <QMouseEvent>
#include <QKeyEvent>

//-----------------------------------------------------------------------------
//! SplashScreen class.
//-----------------------------------------------------------------------------
class SplashScreen : public QSplashScreen
{
public:
    /*!
     *  Constructor.
     */
    explicit SplashScreen(QString const& versionString);

    // Disable copying.
    SplashScreen(SplashScreen const& rhs) = delete;
    SplashScreen& operator=(SplashScreen const& rhs) = delete;

    /*!
     *  Destructor.
     */
    ~SplashScreen() final = default;

protected:
    //! Draws the contents of the splash screen.
    virtual void drawContents(QPainter *painter);

    //! Event handler for mouse clicks
    virtual void mousePressEvent(QMouseEvent* event);

    //! Handler for key press events
    virtual void keyPressEvent(QKeyEvent* event);

private:

    //! Current version to display.
    QString versionString_;
};

//-----------------------------------------------------------------------------

#endif // SPLASHSCREEN_H
