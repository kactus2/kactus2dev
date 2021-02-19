//-----------------------------------------------------------------------------
// File: ScriptingConsole.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 20.09.2019
//
// Description:
// Widget for scripting.
//-----------------------------------------------------------------------------

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "ScriptingConsole.h"
#include "ScriptingTextEditor.h"

#include <QVBoxLayout>

#include <PythonAPI/PythonInterpreter.h>

#include <PythonAPI/ChannelRelay.h>

//-----------------------------------------------------------------------------
// Function: ScriptingConsole::ScriptingConsole()
//-----------------------------------------------------------------------------
ScriptingConsole::ScriptingConsole(QWidget* parent): 
    QWidget(parent),
    outputChannel_(new ChannelRelay(this)),
    errorChannel_(new ChannelRelay(this)),
    interpreter_(new PythonInterpreter(outputChannel_, errorChannel_, this)),
    console_(new ScriptingTextEditor(interpreter_, this))
{    
    connect(outputChannel_, SIGNAL(data(QString const&)),
        console_, SLOT(print(QString const&)), Qt::UniqueConnection);
    connect(errorChannel_, SIGNAL(data(QString const&)),
        console_, SLOT(printError(QString const&)), Qt::UniqueConnection);

    interpreter_->initialize();

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: ScriptingConsole::ScriptingConsole()
//-----------------------------------------------------------------------------
void ScriptingConsole::setupLayout()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(console_);
    layout->setContentsMargins(0, 0, 0, 2 );
}
