//-----------------------------------------------------------------------------
// File: InputListener.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 04.02.2021
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------
#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "StdInputListener.h"

#include <PythonAPI/WriteChannel.h>

#include <QString>

#include <iostream>
#include <string>

StdInputListener::StdInputListener(WriteChannel* outputChannel, QObject* parent /*= nullptr*/) :
    QObject(parent),
    outputChannel_(outputChannel),
#ifdef Q_OS_WIN
    notifier_(new QWinEventNotifier(GetStdHandle(STD_INPUT_HANDLE), this))
{
    QObject::connect(notifier_, &QWinEventNotifier::activated, this, &StdInputListener::inputReadable);

#else
    notifier(new QSocketNotifier(_fileno(stdin), QSocketNotifier::Read, this))
{
    connect(notifier, &QSocketNotifier::activated, this, SLOT(inputReadable()));
#endif
}

void StdInputListener::inputReadable()
{    
    if (std::cin.eof())                          // Ctrl-D pressed 
    {
        emit inputFailure();
        return;
    }

    std::string line;
    std::getline(std::cin, line);

    outputChannel_->write(QString::fromStdString(line));
}
