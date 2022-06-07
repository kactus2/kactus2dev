//-----------------------------------------------------------------------------
// File: PythonSourceEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 20.09.2019
//
// Description:
// Widget for Python scripting.
//-----------------------------------------------------------------------------

#ifndef SCRIPTING_CONSOLE_H
#define SCRIPTING_CONSOLE_H

#include <QWidget>

#include <QLabel>
#include <QPlainTextEdit>
#include <QToolBar>
#include <QListWidget>
#include <QThread>

#include <PythonAPI/ChannelRelay.h>

#include "ScriptViewEditor.h"
#include "ScriptInputEditor.h"

class PythonInterpreter;

//-----------------------------------------------------------------------------
//! Widget for Python scripting.
//-----------------------------------------------------------------------------
class PythonSourceEditor : public QWidget
{
    Q_OBJECT
public:

    //! The constructor.
    explicit PythonSourceEditor(QWidget* parent);
    
    //! The destructor.
    virtual ~PythonSourceEditor();

    //! Apply the application settings.
    void applySettings();

private slots:


    void onOpenAction();

    //! Handler for save button clicks.
    void onSaveAction();

    void onSaveAsAction();

    //! Handler for run button clicks.
    void onRunAction();

private:

    //! Setup widget layout.
    void setupLayout();

    //! Write channel for script output.
    ChannelRelay* outputChannel_;

    //! Write channel for script error output.
    ChannelRelay* errorChannel_;

    QString openFile_;

    QLabel nameLabel_;

    //! Text editor for script writing and run.
    ScriptInputEditor* scriptEditor_;

    //! Text editor for script writing and run.
    ScriptViewEditor* scriptView_;

    //! Interpreter instance for Python.
    PythonInterpreter* interpreter_;

    QToolBar* toolBar_;

    QThread scriptThread_;

};

#endif // SCRIPTING_CONSOLE_H
