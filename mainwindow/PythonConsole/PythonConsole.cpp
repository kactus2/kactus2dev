//-----------------------------------------------------------------------------
// File: PythonConsole.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 20.09.2019
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "PythonConsole.h"
#include "ConsoleEditor.h"

#include <QVBoxLayout>
#include <QApplication>

#include <mainwindow/MessageConsole/messageconsole.h>


#include <PythonAPI/PythonInterpreter.h>


#include <windows.h>

//-----------------------------------------------------------------------------
// Function: PythonConsole::PythonConsole()
//-----------------------------------------------------------------------------
PythonConsole::PythonConsole(QWidget* parent): 
    QWidget(parent),
    console_(new ConsoleEditor(this)),
    inputEditor_(new QLineEdit(this)),
    interpreter_(new PythonInterpreter(this))

{    
    //inputEditor_->setPlaceholderText(QStringLiteral("Type your commands here."));

  /*  connect(process_, SIGNAL(readyRead()),
        this, SLOT(onStandardOutputRead()), Qt::UniqueConnection);
    connect(process_, SIGNAL(readyReadStandardOutput()),
        this, SLOT(onStandardOutputRead()), Qt::UniqueConnection);
    connect(process_, SIGNAL(readyReadStandardError()),
        this, SLOT(onStandardErrorRead()), Qt::UniqueConnection);
    connect(process_, SIGNAL(finished(int, QProcess::ExitStatus)),
        this, SLOT(onProcessFinished(int, QProcess::ExitStatus)), Qt::UniqueConnection);*/


    interpreter_->initialize();

    connect(console_, SIGNAL(entered(QString const&)),
        this, SLOT(onInputReceived(QString const&)));

    connect(interpreter_, SIGNAL(quit()), this, SLOT(onProcessFinished()), Qt::UniqueConnection);

    connect(inputEditor_, SIGNAL(editingFinished()),
        this, SLOT(onInputReceived()));

    setupLayout();
}

void PythonConsole::onStandardOutputRead()
{
   // QString output(process_->readAllStandardOutput());
    //output.chop(2);
    //console_->appendPlainText();
   // console_->print(output);
}

void PythonConsole::onStandardErrorRead()
{
   // QString output(process_->readAllStandardError());
    //console_->appendPlainText(output);
    //console_->printError(output);
}

void PythonConsole::onProcessFinished()
{    
    console_->appendPlainText(tr("Process finished unexpectedly."));
}

void PythonConsole::onInputReceived()
{
    QString line = inputEditor_->text();
    line.append('\n');
    interpreter_->execute(line.toStdString());


    //process_->write(text.toLatin1());
  /*  if (inputEditor_->text().isEmpty() == false)
    {
        QString text = inputEditor_->text();
        console_->appendPlainText(text);

        text.append('\n');
        process_->write(text.toLatin1());
        inputEditor_->clear();
    }*/
}


//-----------------------------------------------------------------------------
// Function: PythonConsole::PythonConsole()
//-----------------------------------------------------------------------------
void PythonConsole::setupLayout()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(console_);
    layout->addWidget(inputEditor_);
    layout->setContentsMargins(0, 0, 0, 2);

}
