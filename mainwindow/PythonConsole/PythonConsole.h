//-----------------------------------------------------------------------------
// File: PythonConsole.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 20.09.2019
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#ifndef PYTHONCONSOLE_H
#define PYTHONCONSOLE_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QProcess>
#include <QSharedPointer>
#include <QTextStream>

class MessageConsole;
class ConsoleEditor;
class PythonInterpreter;

#include <PythonAPI/ChannelRelay.h>

class PythonConsole : public QWidget
{
    Q_OBJECT
public:

    //! The constructor.
    PythonConsole(QWidget* parent);
    
   
    //! The destructor.
    virtual ~PythonConsole() = default;

private:

    void setupLayout();

    ChannelRelay* outputChannel_;

    ChannelRelay* errorChannel_;

    PythonInterpreter* interpreter_;

    ConsoleEditor* console_;
   // QLineEdit* inputEditor_;

    QTextStream inputBuffer_;

};

#endif // PYTHONCONSOLE_H
