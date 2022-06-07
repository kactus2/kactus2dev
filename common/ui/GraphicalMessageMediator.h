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

#include <KactusAPI/include/MessageMediator.h>

#include <QStatusBar>

class MessageConsole;

//-----------------------------------------------------------------------------
//! Graphical output for user messages.
//-----------------------------------------------------------------------------
class GraphicalMessageMediator: public QObject, public MessageMediator
{
    Q_OBJECT
public:

    //! The constructor.
    GraphicalMessageMediator(QObject* parent = nullptr);

    //! The destructor.
    virtual ~GraphicalMessageMediator() = default;

    // Disable copying.
    GraphicalMessageMediator(GraphicalMessageMediator const& rhs) = delete;
    GraphicalMessageMediator& operator=(GraphicalMessageMediator const& rhs) = delete;

    //! Show the given message to the user.
    virtual void showMessage(QString const& message) const;

    //! Show the given error to the user.
    virtual void showError(QString const& error) const;

    //! Show the given failure message to the user.
    virtual void showFailure(QString const& error) const;

    //! Show the given status to the user.
    virtual void showStatusMessage(QString const& status) const;

signals:

    //! Show the given message to the user.
    void noticeMessage(QString const& message) const;

    //! Show the given error to the user.
    void errorMessage(QString const& error) const;

    //! Show the given status to the user.
    void statusMessage(QString const& status) const;
};

#endif // GRAPHICALMESSAGEMEDIATOR_H
