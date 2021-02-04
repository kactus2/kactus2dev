//-----------------------------------------------------------------------------
// File: CLIConsole.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 03.02.2021
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#ifndef CLICONSOLE_H
#define CLICONSOLE_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <QObject>

#include <windows.h>
#include <QWinEventNotifier>
#include <string>
#include <QSocketNotifier>
#include <iostream>
#include <QProcess>

class PythonInterpreter : public QObject
{
    Q_OBJECT

public:
    explicit PythonInterpreter(QObject* parent = nullptr);

    virtual ~PythonInterpreter();

    bool initialize();
    

signals:
    void quit();

    void write(std::string text);

public slots:

    void execute(std::string const& line);

   
private:
#ifdef Q_OS_WIN
    QWinEventNotifier *m_notifier;
#else
    QSocketNotifier *m_notifier;
#endif
  
    std::string inputBuffer_;

    PyObject* globalContext_;

    PyObject* localContext_;

    bool runMultiline_;


private slots:
    void readCommand();

    void writeCommand();
   
};

#endif // CLICONSOLE_H