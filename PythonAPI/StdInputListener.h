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

#include <QSharedPointer>
#include <QTextStream>

#include <windows.h>
#include <QWinEventNotifier>

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

    QTextStream inputBuffer_;

    QWinEventNotifier* notifier_;
};


#endif // STDINPUTLISTENER_H
