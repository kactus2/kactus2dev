//-----------------------------------------------------------------------------
// File: HDLGenerationDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 26.01.2017
//
// Description:
// Dialog for setting file generation options.
//-----------------------------------------------------------------------------

#include "HDLGenerationDialog.h"

#include "GenerationControl.h"

#include <Plugins/VerilogImport/VerilogSyntax.h>

#include <QDialogButtonBox>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QGroupBox>
#include <QTextCursor>
#include <QGridLayout>

//-----------------------------------------------------------------------------
// Function: HDLGenerationDialog::HDLGenerationDialog()
//-----------------------------------------------------------------------------
HDLGenerationDialog::HDLGenerationDialog(QSharedPointer<GenerationControl> configuration,
	QWidget *parent) : 
	QDialog(parent, Qt::WindowTitleHint | Qt::WindowCloseButtonHint), 
    configuration_(configuration),
    viewSelection_(new ViewSelectionWidget(configuration->getViewSelection())),
    fileOutput_(new FileOutputWidget(configuration->getOutputControl())),
    generalWarningLabel_(new QLabel), 
    previewer_(new QPlainTextEdit(this)),
    console_(new MessageConsole(this))
{
    setWindowTitle(tr("File generation for %1").arg(configuration->getViewSelection()->getTargetLanguage()));

    // Create font for previewing.
    QFont font("Courier");
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setPointSize(9);

    // Create the previewer.
    previewer_->setFont(font);
    previewer_->setTabStopWidth(4 * previewer_->fontMetrics().width(' '));
    previewer_->setReadOnly(true);
    previewer_->setCursorWidth(0);
    previewer_->setLineWrapMode(QPlainTextEdit::NoWrap);
    previewer_->setMinimumWidth(850);

    // Add everything it their proper position in the final layout.
	QVBoxLayout* leftLayout = new QVBoxLayout();
    leftLayout->addWidget(viewSelection_);
    leftLayout->addWidget(fileOutput_);
    leftLayout->addWidget(generalWarningLabel_);

    // Layout for things coming to the bottom part of the dialog.

    // Add Ok and cancel give the dialog results.
    QDialogButtonBox* dialogButtons = new QDialogButtonBox(QDialogButtonBox::Cancel, 
        Qt::Horizontal);
    dialogButtons->addButton("Write files", QDialogButtonBox::AcceptRole);

    QGridLayout* grid = new QGridLayout(this);

    QGroupBox* leftBox = new QGroupBox("Settings");
    leftBox->setLayout(leftLayout);

    QGroupBox* logBox = new QGroupBox("Log");
    QVBoxLayout* botLayout = new QVBoxLayout();
    logBox->setLayout(botLayout);

    console_->setMinimumWidth(500);
    botLayout->addWidget(console_);

    grid->addWidget(leftBox, 0, 0, 2, 1);
    grid->addWidget(logBox, 2, 0, 2, 1);

    QGroupBox* rightBox = new QGroupBox("Preview");
    QVBoxLayout* rightLayout = new QVBoxLayout();
    rightLayout->addWidget(previewer_);
    rightBox->setLayout(rightLayout);

    grid->addWidget(rightBox, 0, 1, 4, 1);
    grid->addWidget(dialogButtons, 4, 1, 1, 1);

    // Finally, connect the relevant events to their handler functions.

    // Connect the view selection.
    connect(viewSelection_, SIGNAL(viewChanged()), 
        this, SLOT(onViewChanged()), Qt::UniqueConnection);

    // Connect file output.
    connect(fileOutput_, SIGNAL(selectedFileChanged(QSharedPointer<GenerationOutput>)), 
        this, SLOT(onSelectedFileChanged(QSharedPointer<GenerationOutput>)), Qt::UniqueConnection);

    // Connect the dialog buttons to their respective functions.
    connect(dialogButtons, SIGNAL(accepted()), this, SLOT(accept()), Qt::UniqueConnection);
    connect(dialogButtons, SIGNAL(rejected()), this, SLOT(reject()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: HDLGenerationDialog::~HDLGenerationDialog()
//-----------------------------------------------------------------------------
HDLGenerationDialog::~HDLGenerationDialog()
{
}

//-----------------------------------------------------------------------------
// Function: HDLGenerationDialog::setPreviewHighlighter()
//-----------------------------------------------------------------------------
void HDLGenerationDialog::setPreviewHighlighter(QSyntaxHighlighter* highlighter)
{
    highlighter->setDocument(previewer_->document());
}

//-----------------------------------------------------------------------------
// Function: HDLGenerationDialog::accept()
//-----------------------------------------------------------------------------
void HDLGenerationDialog::accept()
{
	// Check it is sane.
    QString warning;
    if (!configuration_->validSelections(warning))
	{
        // If not, tell user why not.
		generalWarningLabel_->setText(warning);
		return;
	}

    if (!configuration_->writeDocuments())
    {
        return;
    }

    QDialog::accept();
}

//-----------------------------------------------------------------------------
// Function: HDLGenerationDialog::onViewChanged()
//-----------------------------------------------------------------------------
void HDLGenerationDialog::onViewChanged()
{
    configuration_->parseDocuments();
    fileOutput_->onOutputFilesChanged();
}

//-----------------------------------------------------------------------------
// Function: HDLGenerationDialog::onNoticeMessage()
//-----------------------------------------------------------------------------
void HDLGenerationDialog::onNoticeMessage(QString const& message)
{
    console_->onNoticeMessage(message);
}

//-----------------------------------------------------------------------------
// Function: HDLGenerationDialog::onErrorMessage()
//-----------------------------------------------------------------------------
void HDLGenerationDialog::onErrorMessage(QString const& message)
{
    console_->onErrorMessage(message);
}

//-----------------------------------------------------------------------------
// Function: HDLGenerationDialog::onSelectedFileChanged()
//-----------------------------------------------------------------------------
void HDLGenerationDialog::onSelectedFileChanged(QSharedPointer<GenerationOutput> newSelection)
{
    previewer_->setPlainText(newSelection->fileContent_);

    if (configuration_->isDesignGeneration())
    {
        return;
    }

    int implementationStart;
    int implementationEnd;
    QString error;

    if (!VerilogSyntax::findImplementation(
        previewer_->toPlainText(), implementationStart, implementationEnd, error))
    {
        return;
    }

    if (implementationStart == -1 || implementationEnd == -1 && implementationEnd
        > previewer_->toPlainText().length())
    {
        return;
    }

    QColor const& highlightColor = KactusColors::WARNING;

    QTextCursor cursor = previewer_->textCursor();
    cursor.setPosition(implementationStart);

    QTextCharFormat highlighFormat = cursor.charFormat();        
    highlighFormat.setBackground(QBrush(highlightColor));

    cursor.setPosition(implementationEnd, QTextCursor::KeepAnchor);        
    cursor.setCharFormat(highlighFormat);
}
