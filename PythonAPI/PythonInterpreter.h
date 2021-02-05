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

#include <string>
#include <QSocketNotifier>

#include <QProcess>

#include <PythonAPI/WriteChannel.h>

class PythonInterpreter : public QObject, public WriteChannel
{
    Q_OBJECT

public:
    explicit PythonInterpreter(WriteChannel* outputChannel, WriteChannel* errorChannel, QObject* parent = nullptr);

    virtual ~PythonInterpreter();

    bool initialize(); 

    virtual void write(QString const& text) override final;

    void execute(std::string const& line);
   
private:

    void printPrompt() const;
  
    std::string inputBuffer_;

    WriteChannel* outputChannel_;

    WriteChannel* errorChannel_; 

    PyObject* globalContext_;

    PyObject* localContext_;

    bool runMultiline_;

   
};

#endif // CLICONSOLE_H