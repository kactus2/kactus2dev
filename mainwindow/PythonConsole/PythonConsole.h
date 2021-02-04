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

class MessageConsole;
class ConsoleEditor;
class PythonInterpreter;

class PythonConsole : public QWidget
{
    Q_OBJECT
public:

    //! The constructor.
    PythonConsole(QWidget* parent);
    
   
    //! The destructor.
    ~PythonConsole() = default;

private slots:
    void onStandardOutputRead();

    void onStandardErrorRead();

    void onProcessFinished();

    void onInputReceived();

private:

    void setupLayout();


    ConsoleEditor* console_;
    QLineEdit* inputEditor_;

    PythonInterpreter* interpreter_;
};

#endif // PYTHONCONSOLE_H
