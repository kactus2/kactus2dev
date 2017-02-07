//-----------------------------------------------------------------------------
// File: MessagePasser.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 02.02.2017
//
// Description:
// Class for passing messages to UI.
//-----------------------------------------------------------------------------

#ifndef MESSAGEPASSER_H
#define MESSAGEPASSER_H

#include <QString>
#include <QObject>

//-----------------------------------------------------------------------------
// Class for passing messages to UI.
//-----------------------------------------------------------------------------
class MessagePasser : public QObject
{
    Q_OBJECT

public:

	//! The constructor.
    MessagePasser();

	//! The destructor.
    ~MessagePasser();

    /*!
     *  Signals the given error message.
     */
    void sendError(QString const& message) const;

    /*!
     *  Signals the given notice message.
     */
    void sendNotice(QString const& message) const;

signals:

    // Emitted when an error message is received
    void errorMessage(QString const& message) const;

    // Emitted when a notice message is received
    void noticeMessage(QString const& message) const;

private:

	// Disable copying.
	MessagePasser(MessagePasser const& rhs);
	MessagePasser& operator=(MessagePasser const& rhs);
};

#endif // MESSAGEPASSER_H
