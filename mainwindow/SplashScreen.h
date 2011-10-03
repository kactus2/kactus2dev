//-----------------------------------------------------------------------------
// File: SplashScreen.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 24.5.2011
//
// Description:
// Kactus 2 splash screen.
//-----------------------------------------------------------------------------

#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QSplashScreen>

//-----------------------------------------------------------------------------
//! SplashScreen class.
//-----------------------------------------------------------------------------
class SplashScreen : public QSplashScreen
{
public:
    /*!
     *  Constructor.
     */
    SplashScreen(QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~SplashScreen();

protected:
    //! Draws the contents of the splash screen.
    virtual void drawContents(QPainter *painter);

private:
    // Disable copying.
    SplashScreen(SplashScreen const& rhs);
    SplashScreen& operator=(SplashScreen const& rhs);
};

//-----------------------------------------------------------------------------

#endif // SPLASHSCREEN_H
