//-----------------------------------------------------------------------------
// File: ScriptingConsole.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 20.09.2019
//
// Description:
// Widget for scripting.
//-----------------------------------------------------------------------------

#ifndef SCRIPTING_CONSOLE_H
#define SCRIPTING_CONSOLE_H

#include <QWidget>

#include <QPlainTextEdit>

#include <PythonAPI/ChannelRelay.h>

class MessageConsole;
class ScriptingTextEditor;
class PythonInterpreter;

//-----------------------------------------------------------------------------
//! Widget for scripting.
//-----------------------------------------------------------------------------
class ScriptingConsole : public QWidget
{
    Q_OBJECT
public:

    //! The constructor.
    explicit ScriptingConsole(QWidget* parent);
    
    //! The destructor.
    virtual ~ScriptingConsole() = default;

private:

    //! Setup widget layout.
    void setupLayout();

    //! Write channel for script output.
    ChannelRelay* outputChannel_;

    //! Write channel for script error output.
    ChannelRelay* errorChannel_;

    //! Interpreter instance for Python.
    PythonInterpreter* interpreter_;

    //! Text editor for script writing and run.
    ScriptingTextEditor* console_;

};

#endif // SCRIPTING_CONSOLE_H
