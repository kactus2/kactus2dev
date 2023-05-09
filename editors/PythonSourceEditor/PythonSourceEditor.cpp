//-----------------------------------------------------------------------------
// File: PythonSourceEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 20.09.2019
//
// Description:
// Widget for Python scripting.
//-----------------------------------------------------------------------------

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "PythonSourceEditor.h"

#include "PythonSourceHighlight.h"

#include <QHBoxLayout>
#include <QFileDialog>
#include <QTextStream>
#include <QSplitter>
#include <QSettings>
#include <QApplication>

#include <common/widgets/assistedTextEdit/AssistedTextEdit.h>
#include <common/widgets/assistedTextEdit/HighlightStyleDesc.h>

#include <PythonAPI/PythonInterpreter.h>
#include <PythonAPI/ChannelRelay.h>

//-----------------------------------------------------------------------------
// Function: PythonSourceEditor::PythonSourceEditor()
//-----------------------------------------------------------------------------
PythonSourceEditor::PythonSourceEditor(QWidget* parent):
    QWidget(parent),
    outputChannel_(this),
    errorChannel_(this),
    tabs_(this),
    highlighter_(nullptr),
    scriptView_(this),
    interpreter_(new PythonInterpreter(&outputChannel_, &errorChannel_, false)),    
    toolBar_(this),
    progressBar_(this),
    scriptThread_(this)
{    
    connect(&outputChannel_, SIGNAL(data(QString const&)),
        &scriptView_, SLOT(print(QString const&)), Qt::UniqueConnection);
    connect(&errorChannel_, SIGNAL(data(QString const&)),
        &scriptView_, SLOT(printError(QString const&)), Qt::UniqueConnection);

    bool scriptEnabled = setupInterpreter();

    setupTabs();

    setupToolbar(scriptEnabled);

    setupProgressBar();

    highlighter_.addMultilineCommentRule(QRegularExpression("[\'\"]{3}"),
        QRegularExpression("[\'\"]{3}"));
    PythonSourceHighlight highlightRules;
    highlightRules.apply(&highlighter_);

    setupLayout();

    onNewAction();
}

//-----------------------------------------------------------------------------
// Function: PythonSourceEditor::applySettings()
//-----------------------------------------------------------------------------
PythonSourceEditor::~PythonSourceEditor()
{
    interpreter_->finalize();
    scriptThread_.quit();
    scriptThread_.wait();
}

//-----------------------------------------------------------------------------
// Function: PythonSourceEditor::applySettings()
//-----------------------------------------------------------------------------
void PythonSourceEditor::applySettings(QSettings& settings)
{
    // Read font settings.
    QFont font = settings.value("Editor/Font", QFont("Consolas", 10)).value<QFont>();
    scriptView_.setFont(font);

    for (int i = 0; i < tabs_.count(); ++i)
    {
        auto editor = static_cast<ScriptInputEditor*>(tabs_.widget(i));
        applySettings(editor);
    }

    // Read highlight style settings.
    for (int i = 0; i < LanguageHighlighter::STYLE_COUNT; ++i)
    {
        HighlightStyleDesc styleDesc = settings.value("Editor/Highlight/" +
            LanguageHighlighter::getStyleName(static_cast<LanguageHighlighter::StyleType>(i)),
            QVariant::fromValue(LanguageHighlighter::DEFAULT_STYLES[i])).value<HighlightStyleDesc>();

        highlighter_.setStyle(static_cast<LanguageHighlighter::StyleType>(i), styleDesc);
    }

    highlighter_.rehighlight();
}

//-----------------------------------------------------------------------------
// Function: PythonSourceEditor::getIdentifyingVLNV()
//-----------------------------------------------------------------------------
void PythonSourceEditor::onNewAction()
{
    createEditor(tr("New"));
}

//-----------------------------------------------------------------------------
// Function: PythonSourceEditor::onOpenAction()
//-----------------------------------------------------------------------------
void PythonSourceEditor::onOpenAction()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open"), QString(), tr("Python File (*.py)"));

    if (filePath.isEmpty() == false)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);

        auto editor = createEditor(filePath);

        QFile outputFile(filePath);
        outputFile.open(QFile::ReadOnly | QFile::Text);

        QTextStream output(&outputFile);
        editor->setPlainText(output.readAll());

        outputFile.close();

        updateTabLabel(filePath, false);
      
        QApplication::restoreOverrideCursor();
    }
}

//-----------------------------------------------------------------------------
// Function: PythonSourceEditor::onSaveAction()
//-----------------------------------------------------------------------------
void PythonSourceEditor::onSaveAction()
{
    QString filePath = tabs_.tabToolTip(tabs_.currentIndex());

    if (filePath.isEmpty())
    {
        onSaveAsAction();
    }
    else 
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);

        auto editor = static_cast<ScriptInputEditor*>(tabs_.currentWidget());

        QFile outputFile(filePath);
        outputFile.open(QFile::WriteOnly | QFile::Text);

        QTextStream output(&outputFile);
        output << editor->toPlainText();

        outputFile.close();

        updateTabLabel(filePath, false);

        QApplication::restoreOverrideCursor();
    }
}

//-----------------------------------------------------------------------------
// Function: PythonSourceEditor::onSaveAsAction()
//-----------------------------------------------------------------------------
void PythonSourceEditor::onSaveAsAction()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save As"), QString(), tr("Python File (*.py)"));
    if (filePath.isEmpty() == false)
    {
        tabs_.setTabToolTip(tabs_.currentIndex(), filePath);
        onSaveAction();
    }
}

//-----------------------------------------------------------------------------
// Function: PythonSourceEditor::onRunAction()
//-----------------------------------------------------------------------------
void PythonSourceEditor::onRunAction()
{
    QString script = static_cast<ScriptInputEditor*>(tabs_.currentWidget())->getSelectedLines();
    scriptView_.printInput(script);

    progressBar_.reset();
    progressBar_.setRange(0, 0);

    emit executeString(script);
}

//-----------------------------------------------------------------------------
// Function: PythonSourceEditor::onRunAllAction()
//-----------------------------------------------------------------------------
void PythonSourceEditor::onRunAllAction()
{
    QString script = static_cast<ScriptInputEditor*>(tabs_.currentWidget())->toPlainText();
    scriptView_.printInput(script);

    progressBar_.reset();
    progressBar_.setRange(0, 0);

    emit executeString(script);
}

//-----------------------------------------------------------------------------
// Function: PythonSourceEditor::onRunFileAction()
//-----------------------------------------------------------------------------
void PythonSourceEditor::onRunFileAction()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select file to run"), QString(),
        tr("Python File (*.py)"));
    if (fileName.isEmpty() == false)
    {
        scriptView_.printInput(tr("Run script file '%1'.").arg(fileName));

        progressBar_.reset();
        progressBar_.setRange(0, 0);

        emit executeFile(fileName);
    }
}

//-----------------------------------------------------------------------------
// Function: PythonSourceEditor::onRunComplete()
//-----------------------------------------------------------------------------
void PythonSourceEditor::onRunComplete()
{
    progressBar_.setRange(0, 1);
    progressBar_.setValue(1);
}

//-----------------------------------------------------------------------------
// Function: PythonSourceEditor::onTabChanged()
//-----------------------------------------------------------------------------
void PythonSourceEditor::onTabChanged(int currentIndex)
{
    if (currentIndex >= 0)
    {
        disconnect(undoAction_, SIGNAL(triggered(bool)), nullptr, nullptr);
        disconnect(redoAction_, SIGNAL(triggered(bool)), nullptr, nullptr);

        auto editor = static_cast<ScriptInputEditor*>(tabs_.widget(currentIndex));
        highlighter_.setDocument(editor->document());

        undoAction_->setEnabled(editor->document()->isUndoAvailable());
        redoAction_->setEnabled(editor->document()->isRedoAvailable());

        connect(undoAction_, SIGNAL(triggered(bool)), editor, SLOT(undo()));
        connect(redoAction_, SIGNAL(triggered(bool)), editor, SLOT(redo()));
        connect(editor, SIGNAL(undoAvailable(bool)), undoAction_, SLOT(setEnabled(bool)));
        connect(editor, SIGNAL(redoAvailable(bool)), redoAction_, SLOT(setEnabled(bool)));
    }
}

//-----------------------------------------------------------------------------
// Function: PythonSourceEditor::onTabClosed()
//-----------------------------------------------------------------------------
void PythonSourceEditor::onTabClosed(int index)
{
    tabs_.removeTab(index);

    if (tabs_.count() == 0)
    {
        onNewAction();
    }
}

//-----------------------------------------------------------------------------
// Function: PythonSourceEditor::onTabModified()
//-----------------------------------------------------------------------------
void PythonSourceEditor::onTabModified(bool modified)
{
    updateTabLabel(tabs_.tabToolTip(tabs_.currentIndex()), modified);
}

//-----------------------------------------------------------------------------
// Function: PythonSourceEditor::updateTabLabel()
//-----------------------------------------------------------------------------
void PythonSourceEditor::updateTabLabel(QString const& filePath, bool modified)
{
    QFileInfo info(filePath);

    QString tabText = info.fileName();
    if (filePath.isEmpty())
    {
        //! Unsaved documents have no filepath, so use name saved on tab text instead.
        tabText = tabs_.tabText(tabs_.currentIndex());
    }

    if (modified)
    {
        tabText.append("*");
    }
    else if (tabText.endsWith(QChar('*')) && modified == false)
    {
        //! Unsaved documents have no filepath and may be modified before, so need to remove the asterisk.
        tabText.chop(1);
    }

    tabs_.setTabText(tabs_.currentIndex(), tabText);
    tabs_.setTabToolTip(tabs_.currentIndex(), info.canonicalFilePath());
}

//-----------------------------------------------------------------------------
// Function: PythonSourceEditor::createEditor()
//-----------------------------------------------------------------------------
ScriptInputEditor* PythonSourceEditor::createEditor(QString const& label)
{
    auto editor = new ScriptInputEditor();
    applySettings(editor);

    tabs_.addTab(editor, label);
    tabs_.setCurrentWidget(editor);

    connect(editor, SIGNAL(modificationChanged(bool)), this, SLOT(onTabModified(bool)), Qt::UniqueConnection);

    return editor;
}

//-----------------------------------------------------------------------------
// Function: PythonSourceEditor::applySettings()
//-----------------------------------------------------------------------------
void PythonSourceEditor::applySettings(ScriptInputEditor* editor) const
{
    QSettings settings;

    // Read indentation settings.
    IndentStyle style = static_cast<IndentStyle>(settings.value("Editor/IndentStyle",
        INDENT_STYLE_SPACES).toInt());
    unsigned int width = settings.value("Editor/IndentWidth", 4).toInt();
    bool useTabs = style == INDENT_STYLE_TAB;

    // Read font settings.
    QFont font = settings.value("Editor/Font", QFont("Consolas", 10)).value<QFont>();

    editor->setFont(font);
    editor->setIndentStyle(useTabs, width);
}

//-----------------------------------------------------------------------------
// Function: PythonSourceEditor::setupInterpreter()
//-----------------------------------------------------------------------------
bool PythonSourceEditor::setupInterpreter()
{
    interpreter_->moveToThread(&scriptThread_);
    connect(&scriptThread_, SIGNAL(finished()), interpreter_, SLOT(deleteLater()));

    bool enabled = interpreter_->initialize(false);
    if (enabled)
    {
        //! Interpreter runs on different thread so all calls must go through signals for parallel execution.
        connect(this, SIGNAL(executeLine(QString const&)),
            interpreter_, SLOT(write(QString const&)), Qt::UniqueConnection);
        connect(this, SIGNAL(executeString(QString const&)),
            interpreter_, SLOT(executeString(QString const&)), Qt::UniqueConnection);
        connect(this, SIGNAL(executeFile(QString const&)),
            interpreter_, SLOT(runFile(QString const&)), Qt::UniqueConnection);

        connect(interpreter_, SIGNAL(executeDone()),
            this, SLOT(onRunComplete()), Qt::UniqueConnection);

        scriptThread_.start();
    }

    return enabled;
}
//-----------------------------------------------------------------------------
// Function: PythonSourceEditor::setupTabs()
//-----------------------------------------------------------------------------
void PythonSourceEditor::setupTabs()
{
    tabs_.setTabsClosable(true);
    connect(&tabs_, SIGNAL(tabCloseRequested(int)), this, SLOT(onTabClosed(int)), Qt::UniqueConnection);
    connect(&tabs_, SIGNAL(currentChanged(int)), this, SLOT(onTabChanged(int)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: PythonSourceEditor::setupToolbar()
//-----------------------------------------------------------------------------
void PythonSourceEditor::setupToolbar(bool enableRun)
{
    toolBar_.setOrientation(Qt::Horizontal);
    toolBar_.setIconSize(QSize(20, 20));

    QAction* newAction = toolBar_.addAction(QIcon(":/icons/common/graphics/script-new.png"), QString(),
        this, SLOT(onNewAction()));
    newAction->setToolTip(tr("New script (Ctrl+N)"));
    newAction->setShortcut(QKeySequence::New);
    newAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    addAction(newAction);

    QAction* openAction = toolBar_.addAction(QIcon(":/icons/common/graphics/folder-horizontal-open.png"), QString(),
        this, SLOT(onOpenAction()));
    openAction->setToolTip(tr("Open script from file... (Ctrl+O)"));
    openAction->setShortcut(QKeySequence::Open);
    openAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    addAction(openAction);

    QAction* saveAction = toolBar_.addAction(QIcon(":/icons/common/graphics/script-save.png"), QString(),
        this, SLOT(onSaveAction()));
    saveAction->setToolTip(tr("Save script (Ctrl+S"));
    saveAction->setShortcut(QKeySequence::Save);
    saveAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    addAction(saveAction);

    QAction* saveAsAction = toolBar_.addAction(QIcon(":/icons/common/graphics/script-save-as.png"), QString(),
        this, SLOT(onSaveAsAction()));
    saveAsAction->setToolTip(tr("Save script as..."));
    addAction(saveAsAction);

    toolBar_.addSeparator();

    undoAction_ = toolBar_.addAction(QIcon(":/icons/common/graphics/edit-undo.png"), QString());
    undoAction_->setToolTip(tr("Undo (Ctrl+Z)"));
    undoAction_->setShortcut(QKeySequence::Undo);
    undoAction_->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    addAction(undoAction_);

    redoAction_ = toolBar_.addAction(QIcon(":/icons/common/graphics/edit-redo.png"), QString());
    redoAction_->setToolTip(tr("Redo (Ctrl+Y)"));
    redoAction_->setShortcut(QKeySequence::Redo);
    redoAction_->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    addAction(redoAction_);

    toolBar_.addSeparator();

    QAction* runAction = toolBar_.addAction(QIcon(":/icons/common/graphics/script-run.png"), QString(),
        this, SLOT(onRunAction()));
    runAction->setToolTip(tr("Run selected line(s) (Ctrl+R)"));
    runAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    runAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    runAction->setEnabled(enableRun);
    addAction(runAction);

    QAction* runAllAction = toolBar_.addAction(QIcon(":/icons/common/graphics/script-run-all.png"), QString(),
        this, SLOT(onRunAllAction()));
    runAllAction->setToolTip(tr("Run all (Ctrl+Shift+R)"));
    runAllAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_R));
    runAllAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    runAllAction->setEnabled(enableRun);
    addAction(runAllAction);

    QAction* runFileAction = toolBar_.addAction(QIcon(":/icons/common/graphics/script-run-file.png"), QString(),
        this, SLOT(onRunFileAction()));
    runFileAction->setToolTip(tr("Run file..."));
    runFileAction->setEnabled(enableRun);
    addAction(runFileAction);

    toolBar_.addSeparator();

    QAction* clearAction = toolBar_.addAction(QIcon(":/icons/common/graphics/cleanup.png"), QString(),
        &scriptView_, SLOT(clear()));
    clearAction->setToolTip(tr("Clear output"));
    addAction(clearAction);
}

//-----------------------------------------------------------------------------
// Function: PythonSourceEditor::setupProgressBar()
//-----------------------------------------------------------------------------
void PythonSourceEditor::setupProgressBar()
{
    progressBar_.setTextVisible(false);
    progressBar_.setRange(0, 1);
    progressBar_.reset();

    progressBar_.setMaximumHeight(progressBar_.sizeHint().height() / 2);
}

//-----------------------------------------------------------------------------
// Function: PythonSourceEditor::setupLayout()
//-----------------------------------------------------------------------------
void PythonSourceEditor::setupLayout()
{
    QWidget* editorContainer = new QWidget(this);
    QVBoxLayout* editorLayout = new QVBoxLayout(editorContainer);

    editorLayout->addWidget(&tabs_);
    editorLayout->setContentsMargins(0, 0, 0, 0);

    QWidget* viewContainer = new QWidget(this);
    QVBoxLayout* viewLayout = new QVBoxLayout(viewContainer);
    viewLayout->addWidget(&progressBar_);
    viewLayout->addWidget(&scriptView_);
    viewLayout->setContentsMargins(0, 0, 0, 0);

    QSplitter* viewSplit = new QSplitter(this);
    viewSplit->setOrientation(Qt::Vertical);
    viewSplit->addWidget(editorContainer);
    viewSplit->addWidget(viewContainer);
    viewSplit->setStretchFactor(0, 4);


    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&toolBar_);
    layout->addWidget(viewSplit);
    layout->setContentsMargins(0, 0, 4, 2);
}
