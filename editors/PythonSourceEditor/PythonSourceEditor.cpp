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


#include <QHBoxLayout>
#include <QFileDialog>
#include <QTextStream>
#include <QSplitter>

#include <PythonAPI/PythonInterpreter.h>

#include <PythonAPI/ChannelRelay.h>

//-----------------------------------------------------------------------------
// Function: PythonSourceEditor::PythonSourceEditor()
//-----------------------------------------------------------------------------
PythonSourceEditor::PythonSourceEditor(QWidget* parent):
    QWidget(parent),
    outputChannel_(new ChannelRelay(this)),
    errorChannel_(new ChannelRelay(this)),
    nameLabel_(tr("Transcript"), this),
    scriptEditor_(new ScriptInputEditor(this)),
    scriptView_(new ScriptViewEditor(this)),
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
    scriptView_->setReadOnly(true);

    QAction* runAction = toolBar_->addAction(QIcon(":/icons/common/graphics/script-open.png"), QString(),
        this, SLOT(onOpenAction()));
    runAction->setToolTip(QStringLiteral("Open script from file..."));
    runAction->setEnabled(enabled);

    QAction* saveAction = toolBar_->addAction(QIcon(":/icons/common/graphics/script-save.png"), QString(), 
        this, SLOT(onSaveAction()));
    saveAction->setToolTip(QStringLiteral("Save script"));
    saveAction->setEnabled(enabled);

    QAction* saveAsAction = toolBar_->addAction(QIcon(":/icons/common/graphics/script-save-as.png"), QString(),
        this, SLOT(onSaveAsAction()));
    saveAsAction->setToolTip(QStringLiteral("Save script as..."));
    saveAsAction->setEnabled(enabled);

    toolBar_->addSeparator();

    if (enabled == false)
    {
        scriptEditor_->setPlaceholderText(tr("Could not initialize interpreter. Script disabled."));
        scriptEditor_->setReadOnly(true);
    }
    else
    {
          scriptThread_.start();
    }


    setupLayout();
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
    scriptEditor_->applySettings();
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
        scriptEditor_->setPlainText(output.readAll());

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
        openFile_ = QFileDialog::getSaveFileName(this, tr("Save As"), QString(), tr("Python File (*.py)"));
    }

    if (openFile_.isEmpty() == false)
    {
        QFile outputFile(openFile_);
        outputFile.open(QFile::WriteOnly | QFile::Text);

        QTextStream output(&outputFile);
        output << scriptEditor_->toPlainText();

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
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select file to run"), QString(), tr("Python File (*.py)"));
    if (fileName.isEmpty() == false)
    {
        interpreter_->runFile(fileName);
    }
}

//-----------------------------------------------------------------------------
// Function: PythonSourceEditor::setupLayout()
//-----------------------------------------------------------------------------
void PythonSourceEditor::setupLayout()
{
    nameLabel_.setAlignment(Qt::AlignCenter);

    toolBar_->setOrientation(Qt::Horizontal);

    QVBoxLayout* layout = new QVBoxLayout(this);

    QSplitter* viewSplit = new QSplitter(this);
    viewSplit->setOrientation(Qt::Vertical);

    QWidget* editorContainer = new QWidget(this);

    QVBoxLayout* containerLayout = new QVBoxLayout(editorContainer);    
    containerLayout->addWidget(toolBar_);
    containerLayout->addWidget(&nameLabel_);
    containerLayout->addWidget(scriptEditor_);
    containerLayout->setContentsMargins(0, 0, 0, 0);

    viewSplit->addWidget(editorContainer);
    viewSplit->addWidget(scriptView_);
    viewSplit->setStretchFactor(0, 4);

    layout->addWidget(viewSplit);
    layout->setContentsMargins(0, 0, 2, 2);
}
