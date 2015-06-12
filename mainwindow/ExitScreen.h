//-----------------------------------------------------------------------------
// File: ExitScreen.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 12.06.2015
//
// Description:
// Exit screen for Kactus2.
//-----------------------------------------------------------------------------

#ifndef EXITSCREEN_H
#define EXITSCREEN_H

#include <QDialog>
#include <QLabel>

//-----------------------------------------------------------------------------
//! ExitScreen class.
//-----------------------------------------------------------------------------
class ExitScreen : public QDialog
{
    Q_OBJECT
public:

    /*!
     *  Constructor.
     */
    ExitScreen(QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~ExitScreen();

protected:

    virtual void showEvent(QShowEvent* event);

private slots:

    void onTimerExpired();

    void onShowHideToggled(bool hide);

private:
    // Disable copying.
    ExitScreen(ExitScreen const& rhs);
    ExitScreen& operator=(ExitScreen const& rhs);

    //! Countdown to automatic close.
    int closeCounter_;

    //! Label for displaying countdown timer.
    QLabel* countdownLabel_;
};

//-----------------------------------------------------------------------------

#endif // EXITSCREEN_H
