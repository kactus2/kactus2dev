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


#include <PythonAPI/ChannelRelay.h>

#include <windows.h>

//-----------------------------------------------------------------------------
// Function: PythonConsole::PythonConsole()
//-----------------------------------------------------------------------------
PythonConsole::PythonConsole(QWidget* parent): 
    QWidget(parent),
    outputChannel_(new ChannelRelay(this)),
    errorChannel_(new ChannelRelay(this)),
    interpreter_(new PythonInterpreter(outputChannel_, errorChannel_, this)),
    console_(new ConsoleEditor(interpreter_, this))
    //inputEditor_(new QLineEdit(this)), 

{    
    connect(outputChannel_, SIGNAL(data(QString const&)),
        console_, SLOT(print(QString const&)), Qt::UniqueConnection);
    connect(errorChannel_, SIGNAL(data(QString const&)),
        console_, SLOT(printError(QString const&)), Qt::UniqueConnection);

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

  //  connect(interpreter_, SIGNAL(quit()), this, SLOT(onProcessFinished()), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: PythonConsole::PythonConsole()
//-----------------------------------------------------------------------------
void PythonConsole::setupLayout()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(console_);
    //layout->addWidget(inputEditor_);
    layout->setContentsMargins(0, 0, 0, 2);

}
