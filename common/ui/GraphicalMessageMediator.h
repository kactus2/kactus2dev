//-----------------------------------------------------------------------------
// File: GraphicalMessageMediator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 13.02.2018
//
// Description:
// Graphical output for user messages.
//-----------------------------------------------------------------------------

#ifndef GRAPHICALMESSAGEMEDIATOR_H
#define GRAPHICALMESSAGEMEDIATOR_H

#include "MessageMediator.h"

#include <QStatusBar>

class MessageConsole;

//-----------------------------------------------------------------------------
//! Graphical output for user messages.
//-----------------------------------------------------------------------------
class GraphicalMessageMediator: public MessageMediator
{
public:

    //! The constructor.
    GraphicalMessageMediator();

    //! The destructor.
    virtual ~GraphicalMessageMediator();

    //! Show the given message to the user.
    virtual void showMessage(QString const& message) const;

    //! Show the given error to the user.
    virtual void showError(QString const& error) const;

    //! Show the given failure message to the user.
    virtual void showFailure(QString const& error) const;

    //! Show the given status to the user.
    virtual void showStatusMessage(QString const& status) const;

    //! Set the status bar to show status.
    void setMessageConsole(MessageConsole* console);

    //! Set the status bar to show status.
    void setStatusBar(QStatusBar* statusBar);

private:

    // Disable copying.
    GraphicalMessageMediator(GraphicalMessageMediator const& rhs);
    GraphicalMessageMediator& operator=(GraphicalMessageMediator const& rhs);

    //! Graphical console for displaying messages.
    MessageConsole* console_; 

    //! Status bar for displaying status info.
    QStatusBar* statusBar_;
};

#endif // GRAPHICALMESSAGEMEDIATOR_H
