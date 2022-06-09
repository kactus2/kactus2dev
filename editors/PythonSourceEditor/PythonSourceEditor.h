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

#include <Plugins/common/LanguageHighlighter.h>

class PythonInterpreter;

//-----------------------------------------------------------------------------
//! Widget for Python scripting.
//-----------------------------------------------------------------------------
class PythonSourceEditor : public QWidget
{
    Q_OBJECT
public:

    /*!
     * The constructor.
     *
     *     @param [in] parent  The parent widget.
     *
     *     @return 
     */
     explicit PythonSourceEditor(QWidget* parent = nullptr);

    //! The destructor.
    virtual ~PythonSourceEditor();

    //! Apply the application settings.
    void applySettings();

private slots:

    //! Handler for open button clicks.
    void onOpenAction();

    //! Handler for save button clicks.
    void onSaveAction();

    //! Handler for save as button clicks.
    void onSaveAsAction();

    //! Handler for run button clicks.
    void onRunAction();

    //! Handler for run all button clicks.
    void onRunAllAction();

    //! Handler for run file button clicks.
    void onRunFileAction();

private:

    //! Setup the toolbar and actions.
    void setupToolbar(bool enableRun);

    //! Setup widget layout.
    void setupLayout();

    //! Write channel for script output.
    ChannelRelay outputChannel_;

    //! Write channel for script error output.
    ChannelRelay errorChannel_;

    //! The currently open script file.
    QString openFile_;

    //! Label for showing currently open file name.
    QLabel nameLabel_;


    //! Text editor for script writing and run.
    ScriptInputEditor scriptEditor_;

    LanguageHighlighter highlighter_;

    //! Text editor for script writing and run.
    ScriptViewEditor scriptView_;

    //! Interpreter instance for Python.
    PythonInterpreter* interpreter_;

    //! Toolbar for actions.
    QToolBar toolBar_;

    //! Thread for running the scripts independent of the editor.
    QThread scriptThread_;

};

#endif // SCRIPTING_CONSOLE_H
