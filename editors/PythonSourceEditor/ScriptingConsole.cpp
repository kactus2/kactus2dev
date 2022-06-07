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

#include <QHBoxLayout>
#include <QFileDialog>
#include <QTextStream>
#include <QSplitter>

#include <PythonAPI/PythonInterpreter.h>

#include <PythonAPI/ChannelRelay.h>

//-----------------------------------------------------------------------------
// Function: ScriptingConsole::ScriptingConsole()
//-----------------------------------------------------------------------------
ScriptingConsole::ScriptingConsole(QWidget* parent):
    QWidget(parent),
    outputChannel_(new ChannelRelay(this)),
    errorChannel_(new ChannelRelay(this)),
    scriptEditor_(new ScriptingTextEditor(this)),
    scriptView_(new ScriptingTextEditor(this)),
    interpreter_(new PythonInterpreter(outputChannel_, errorChannel_, true)),    
    toolBar_(new QToolBar(this)),
    scriptThread_(this)
{    
    interpreter_->moveToThread(&scriptThread_);
    connect(&scriptThread_, SIGNAL(finished()), interpreter_, SLOT(deleteLater()));

    connect(outputChannel_, SIGNAL(data(QString const&)),
        scriptView_, SLOT(print(QString const&)), Qt::UniqueConnection);
    connect(errorChannel_, SIGNAL(data(QString const&)),
        scriptView_, SLOT(printError(QString const&)), Qt::UniqueConnection);

    connect(scriptEditor_, SIGNAL(write(QString const&)),
        interpreter_, SLOT(write(QString const&)), Qt::UniqueConnection);

    bool enabled = interpreter_->initialize(false);


    QAction* saveAction = toolBar_->addAction(QIcon(":/icons/common/graphics/script-save.png"), QString(), 
        this, SLOT(onSaveAction()));
    saveAction->setToolTip(QStringLiteral("Save script"));
    saveAction->setEnabled(enabled);

    QAction* runAction = toolBar_->addAction(QIcon(":/icons/common/graphics/script-open.png"), QString(),
        this, SLOT(onRunAction()));
    runAction->setToolTip(QStringLiteral("Run script from file"));
    runAction->setEnabled(enabled);

    if (enabled == false)
    {
        scriptEditor_->printError(tr("Could not initialize interpreter. Script disabled."));
        scriptEditor_->setReadOnly(true);
    }
    else
    {
          scriptThread_.start();
    }


    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: ScriptingConsole::applySettings()
//-----------------------------------------------------------------------------
ScriptingConsole::~ScriptingConsole()
{
    scriptThread_.quit();
    scriptThread_.wait();
}

//-----------------------------------------------------------------------------
// Function: ScriptingConsole::applySettings()
//-----------------------------------------------------------------------------
void ScriptingConsole::applySettings()
{
    scriptEditor_->applySettings();
}

//-----------------------------------------------------------------------------
// Function: ScriptingConsole::onOpenAction()
//-----------------------------------------------------------------------------
void ScriptingConsole::onOpenAction()
{
    openFile_ = QFileDialog::getOpenFileName(this, tr("Open"), QString(), tr("Python File (*.py)"));

    if (openFile_.isEmpty() == false)
    {
        QFile outputFile(openFile_);
        outputFile.open(QFile::ReadOnly | QFile::Text);

        QTextStream output(&outputFile);
        scriptEditor_->setPlainText(output.readAll());

        outputFile.close();
    }
}

//-----------------------------------------------------------------------------
// Function: ScriptingConsole::onSaveAction()
//-----------------------------------------------------------------------------
void ScriptingConsole::onSaveAction()
{
    if (openFile_.isEmpty())
    {
        openFile_ = QFileDialog::getSaveFileName(this, tr("Save As"), QString(), tr("Python File (*.py)"));
    }

    if (openFile_.isEmpty() == false)
    {
        QFile outputFile(openFile_);
        outputFile.open(QFile::WriteOnly | QFile::Text);

        QTextStream output(&outputFile);
        output << scriptEditor_->toPlainText();

        outputFile.close();
    }
}

//-----------------------------------------------------------------------------
// Function: ScriptingConsole::onSaveAsAction()
//-----------------------------------------------------------------------------
void ScriptingConsole::onSaveAsAction()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), QString(), tr("Python File (*.py)"));
    if (fileName.isEmpty() == false)
    {
        QFile outputFile(fileName);
        outputFile.open(QFile::WriteOnly | QFile::Text);

        QTextStream output(&outputFile);
        output << scriptEditor_->toPlainText();

        outputFile.close();

        openFile_ = fileName;
    }
}

//-----------------------------------------------------------------------------
// Function: ScriptingConsole::onRunAction()
//-----------------------------------------------------------------------------
void ScriptingConsole::onRunAction()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select file to run"), QString(), tr("Python File (*.py)"));
    if (fileName.isEmpty() == false)
    {
        interpreter_->runFile(fileName);
    }
}

//-----------------------------------------------------------------------------
// Function: ScriptingConsole::setupLayout()
//-----------------------------------------------------------------------------
void ScriptingConsole::setupLayout()
{
    toolBar_->setOrientation(Qt::Horizontal);

    QVBoxLayout* layout = new QVBoxLayout(this);

    QSplitter* viewSplit = new QSplitter(this);
    viewSplit->setOrientation(Qt::Vertical);

    QWidget* editorContainer = new QWidget(this);
    editorContainer->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout* containerLayout = new QVBoxLayout(editorContainer);
    containerLayout->addWidget(toolBar_);
    containerLayout->addWidget(scriptEditor_);

    viewSplit->addWidget(editorContainer);
    viewSplit->addWidget(scriptView_);
    viewSplit->setStretchFactor(0, 4);

    layout->addWidget(viewSplit);
    layout->setContentsMargins(0, 0, 2, 2);
}
