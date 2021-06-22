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
#include <QToolBar>
#include <QListWidget>
#include <QThread>

#include <PythonAPI/ChannelRelay.h>

class MessageConsole;
class ScriptingTextEditor;
class PythonInterpreter;
class ScriptingHistory;

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
    virtual ~ScriptingConsole();

    //! Apply the application settings.
    void applySettings();

private slots:

    //! Handler for save button clicks.
    void onSaveAction();

    //! Handler for run button clicks.
    void onRunAction();

    //! Handler for registering user input commands.
    void onCommandInput(QString const& command);

    //! Handler for history button clicks.
    void onHistoryItemClicked(QListWidgetItem* item);


private:

    //! Setup widget layout.
    void setupLayout();

    //! Write channel for script output.
    ChannelRelay* outputChannel_;

    //! Write channel for script error output.
    ChannelRelay* errorChannel_;

    ScriptingHistory* history_;

    //! Interpreter instance for Python.
    PythonInterpreter* interpreter_;

    //! Text editor for script writing and run.
    ScriptingTextEditor* scriptEditor_;

    QListWidget* historyListing_;

    QToolBar* toolBar_;

    QThread scriptThread_;

};

#endif // SCRIPTING_CONSOLE_H
