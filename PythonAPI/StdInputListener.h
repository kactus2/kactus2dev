//-----------------------------------------------------------------------------
// File: InputListener.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 04.02.2021
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#ifndef STDINPUTLISTENER_H
#define STDINPUTLISTENER_H

#include <QObject>

#ifdef Q_OS_WIN
#include <windows.h>
#include <QWinEventNotifier>
#else
#include <QSocketNotifier>
#endif

class WriteChannel;

class StdInputListener : public QObject
{
    Q_OBJECT
public:

    //! The constructor.
    StdInputListener(WriteChannel* outputChannel, QObject* parent = nullptr);

    //! The destructor.
    virtual ~StdInputListener() = default;

signals:

    void inputFailure();

public slots:

    void inputReadable();

private:

    WriteChannel* outputChannel_;

#ifdef Q_OS_WIN
    QWinEventNotifier* notifier_;
#else
    QSocketNotifier* notifier_;
#endif

};


#endif // STDINPUTLISTENER_H
