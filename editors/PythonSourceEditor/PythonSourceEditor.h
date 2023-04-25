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
#include <QProgressBar>
#include <QTabWidget>
#include <QThread>

#include <PythonAPI/ChannelRelay.h>

#include "ScriptViewEditor.h"
#include "ScriptInputEditor.h"

#include <Plugins/common/LanguageHighlighter.h>

#include <common/widgets/tabDocument/TabDocument.h>

class PythonInterpreter;

//-----------------------------------------------------------------------------
//! Widget for Python scripting.
//-----------------------------------------------------------------------------
class PythonSourceEditor : public TabDocument
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
    void applySettings(QSettings& settings) override;


    VLNV getIdentifyingVLNV() const override;


    void refresh() override;


    QSharedPointer<IEditProvider> getEditProvider() const override;


    unsigned int getSupportedWindows() const override;


    bool save() override;


    bool saveAs() override;



signals:
    
    void executeLine(QString const& line);

    void executeString(QString const& string);

    void executeFile(QString const& path);

private slots:

    //! Hander for new button clicks.
    void onNewAction();

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

    //! Handler for updating run indicator when run is completed.
    void onRunComplete();

    //! Handler for editor tab changes.
    void onTabChanged(int currentIndex);

    //! Handler for editor tab closing.
    void onTabClosed(int index);

    //! Handler for editor content modification changes.
    void onTabModified(bool modified);

private:

    
    /*!
     * Update the tab label and path in tooltip.
     *
     *     @param [in] filePath  Path to the edited file.
     *     @param [in] modified  True, if file content is modified, otherwise false.
     */
     void updateTabLabel(QString const& filePath, bool modified);

     /*!
      * Create a new editor and add it in the tabs.
      *
      *     @param [in] label  The label on the tab for the editor.
      *
      *     @return The created editor.
      */
      ScriptInputEditor* createEditor(QString const& label);

    /*!
     * Apply formatting settings to a single editor.
     *
     *     @param [in] editor  The editor to apply to.
     */
    void applySettings(ScriptInputEditor* editor) const;


    /*!
     * Setup the Python interpreter.
     *
     *     @return True, if the setup was successful and interpreter stated, otherwise false. 
     */
     bool setupInterpreter();

     //! Setup the editor tabs.
     void setupTabs();

    /*
     * Setup the toolbar and actions.
     *
     *     @param[in] enableRun     Enable flag for script run actions.
     */
    void setupToolbar(bool enableRun);

    //! Setup the progress bar for script run status indication.
    void setupProgressBar();

    //! Setup widget layout.
    void setupLayout();

    //! Write channel for script output.
    ChannelRelay outputChannel_;

    //! Write channel for script error output.
    ChannelRelay errorChannel_;

    QAction* undoAction_ = nullptr;

    QAction* redoAction_ = nullptr;

    //! Widget for holding all the tabs for file editors.
    QTabWidget tabs_;

    //! Syntax highlighter.
    LanguageHighlighter highlighter_;

    //! Text editor for script writing and run.
    ScriptViewEditor scriptView_;

    //! Interpreter instance for Python.
    PythonInterpreter* interpreter_;

    //! Toolbar for actions.
    QToolBar toolBar_;

    //! Progress bar for displaying editor status.
    QProgressBar progressBar_;

    //! Thread for running the scripts independent of the editor.
    QThread scriptThread_;

};

#endif // SCRIPTING_CONSOLE_H
