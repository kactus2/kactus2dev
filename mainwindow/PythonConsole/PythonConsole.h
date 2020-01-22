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

    void onProcessFinished(int, QProcess::ExitStatus);

    void onInputReceived(QString const& text);

private:

    void setupLayout();

    QProcess* process_;
    ConsoleEditor* console_;
    //QLineEdit* inputEditor_;
};

#endif // PYTHONCONSOLE_H
