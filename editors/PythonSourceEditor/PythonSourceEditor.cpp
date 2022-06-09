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
    nameLabel_(tr("Unnamed script"), this),
    scriptEditor_(this),
    highlighter_(scriptEditor_.document()),
    scriptView_(this),
    interpreter_(new PythonInterpreter(&outputChannel_, &errorChannel_, true)),    
    toolBar_(this),
    scriptThread_(this)
{    
    interpreter_->moveToThread(&scriptThread_);
    connect(&scriptThread_, SIGNAL(finished()), interpreter_, SLOT(deleteLater()));

    connect(&outputChannel_, SIGNAL(data(QString const&)),
        &scriptView_, SLOT(print(QString const&)), Qt::UniqueConnection);
    connect(&errorChannel_, SIGNAL(data(QString const&)),
        &scriptView_, SLOT(printError(QString const&)), Qt::UniqueConnection);

    bool enabled = interpreter_->initialize(false);

    setupToolbar(enabled);

    scriptView_.setReadOnly(true);
    
    if (enabled == false)
    {
        scriptEditor_.setPlaceholderText(tr("Could not initialize interpreter. Script disabled."));
        scriptEditor_.setReadOnly(true);
    }
    else
    {
        scriptThread_.start();
    }

    highlighter_.addMultilineCommentRule(QRegularExpression("[\'\"]{3}"),
        QRegularExpression("[\'\"]{3}"));
    PythonSourceHighlight highlightRules;
    highlightRules.apply(&highlighter_);

    setupLayout();

    applySettings();
}

//-----------------------------------------------------------------------------
// Function: PythonSourceEditor::applySettings()
//-----------------------------------------------------------------------------
PythonSourceEditor::~PythonSourceEditor()
{
    scriptThread_.quit();
    scriptThread_.wait();
}

//-----------------------------------------------------------------------------
// Function: PythonSourceEditor::applySettings()
//-----------------------------------------------------------------------------
void PythonSourceEditor::applySettings()
{
    QSettings settings;

    // Read indentation settings.
    IndentStyle style = static_cast<IndentStyle>(settings.value("Editor/IndentStyle",
        INDENT_STYLE_SPACES).toInt());
    unsigned int width = settings.value("Editor/IndentWidth", 4).toInt();

    bool useTabs = style == INDENT_STYLE_TAB;
    scriptEditor_.setIndentStyle(useTabs, width);

    // Read font settings.
    QFont font = settings.value("Editor/Font", QFont("Consolas", 10)).value<QFont>();
    scriptEditor_.setFont(font);
    scriptView_.setFont(font);

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
// Function: PythonSourceEditor::onOpenAction()
//-----------------------------------------------------------------------------
void PythonSourceEditor::onOpenAction()
{
    openFile_ = QFileDialog::getOpenFileName(this, tr("Open"), QString(), tr("Python File (*.py)"));

    if (openFile_.isEmpty() == false)
    {
        QFile outputFile(openFile_);
        outputFile.open(QFile::ReadOnly | QFile::Text);

        QTextStream output(&outputFile);
        scriptEditor_.setPlainText(output.readAll());

        outputFile.close();

        nameLabel_.setText(openFile_);
    }
}

//-----------------------------------------------------------------------------
// Function: PythonSourceEditor::onSaveAction()
//-----------------------------------------------------------------------------
void PythonSourceEditor::onSaveAction()
{
    if (openFile_.isEmpty())
    {
        onSaveAsAction();
    }
    else 
    {
        QFile outputFile(openFile_);
        outputFile.open(QFile::WriteOnly | QFile::Text);

        QTextStream output(&outputFile);
        output << scriptEditor_.toPlainText();

        outputFile.close();

        nameLabel_.setText(openFile_);
    }
}

//-----------------------------------------------------------------------------
// Function: PythonSourceEditor::onSaveAsAction()
//-----------------------------------------------------------------------------
void PythonSourceEditor::onSaveAsAction()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), QString(), tr("Python File (*.py)"));
    if (fileName.isEmpty() == false)
    {
        openFile_ = fileName;
        onSaveAction();
    }
}

//-----------------------------------------------------------------------------
// Function: PythonSourceEditor::onRunAction()
//-----------------------------------------------------------------------------
void PythonSourceEditor::onRunAction()
{
    QString script = scriptEditor_.getSelectedLines();
    scriptView_.printInput(script);
    interpreter_->executeString(script);
}

//-----------------------------------------------------------------------------
// Function: PythonSourceEditor::onRunAllAction()
//-----------------------------------------------------------------------------
void PythonSourceEditor::onRunAllAction()
{
    QString script = scriptEditor_.toPlainText();
    scriptView_.printInput(script);
    interpreter_->executeString(script);
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
        interpreter_->runFile(fileName);
    }
}

//-----------------------------------------------------------------------------
// Function: PythonSourceEditor::setupToolbar()
//-----------------------------------------------------------------------------
void PythonSourceEditor::setupToolbar(bool enableRun)
{
    toolBar_.setOrientation(Qt::Horizontal);
    toolBar_.setIconSize(QSize(20, 20));

    QAction* openAction = toolBar_.addAction(QIcon(":/icons/common/graphics/folder-horizontal-open.png"), QString(),
        this, SLOT(onOpenAction()));
    openAction->setToolTip(tr("Open script from file..."));
    addAction(openAction);

    QAction* saveAction = toolBar_.addAction(QIcon(":/icons/common/graphics/script-save.png"), QString(),
        this, SLOT(onSaveAction()));
    saveAction->setToolTip(tr("Save script"));
    addAction(saveAction);

    QAction* saveAsAction = toolBar_.addAction(QIcon(":/icons/common/graphics/script-save-as.png"), QString(),
        this, SLOT(onSaveAsAction()));
    saveAsAction->setToolTip(tr("Save script as..."));
    addAction(saveAsAction);

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
    runAllAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_R));
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
// Function: PythonSourceEditor::setupLayout()
//-----------------------------------------------------------------------------
void PythonSourceEditor::setupLayout()
{
    nameLabel_.setAlignment(Qt::AlignCenter);

    QVBoxLayout* layout = new QVBoxLayout(this);

    QSplitter* viewSplit = new QSplitter(this);
    viewSplit->setOrientation(Qt::Vertical);

    QWidget* editorContainer = new QWidget(this);

    QVBoxLayout* containerLayout = new QVBoxLayout(editorContainer);    
    
    containerLayout->addWidget(&nameLabel_);
    containerLayout->addWidget(&scriptEditor_);
    containerLayout->addWidget(&toolBar_); 
    containerLayout->setContentsMargins(0, 0, 0, 0);

    viewSplit->addWidget(editorContainer);
    viewSplit->addWidget(&scriptView_);
    viewSplit->setStretchFactor(0, 4);

    layout->addWidget(viewSplit);
    layout->setContentsMargins(0, 0, 2, 2);
}
