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

//-----------------------------------------------------------------------------
// Function: HDLGenerationDialog::HDLGenerationDialog()
//-----------------------------------------------------------------------------
HDLGenerationDialog::HDLGenerationDialog(QSharedPointer<GenerationControl> configuration,
	QWidget *parent) : 
	QDialog(parent,Qt::WindowTitleHint), 
    configuration_(configuration),
    viewSelection_(new ViewSelectionWidget(configuration->getViewSelection())),
    fileOutput_(new FileOutputWidget(configuration->getFileOuput())),
    generalWarningLabel_(new QLabel)
{
    setWindowTitle(tr("Configure file generation for %1.").arg(configuration->getViewSelection()->getTargetLanguage()));

    // Create font for previewing.
    QFont font("Courier");
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setPointSize(9);

    // Create the previewer.
    previewer_ = new QPlainTextEdit;
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
    QDialogButtonBox* dialogButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, 
        Qt::Horizontal);

    QHBoxLayout* topLayout = new QHBoxLayout;
    QGroupBox* leftBox = new QGroupBox("Settings");
    leftBox->setLayout(leftLayout);
    QGroupBox* rightBox = new QGroupBox("Preview");
    QVBoxLayout* rightLayout = new QVBoxLayout();
    rightLayout->addWidget(previewer_);
    rightBox->setLayout(rightLayout);

    topLayout->addWidget(leftBox);
    topLayout->addWidget(rightBox);

    QGroupBox* botBox = new QGroupBox("Log");
    QVBoxLayout* botLayout = new QVBoxLayout();
    botBox->setLayout(botLayout);
    botLayout->addWidget(new QPlainTextEdit);

    QVBoxLayout* topmostLayout = new QVBoxLayout(this);
    topmostLayout->addLayout(topLayout);
    topmostLayout->addWidget(botBox);
    topmostLayout->addWidget(dialogButtons);


    // Finally, connect the relevant events to their handler functions.

    // Connect the view selection.
    connect(viewSelection_, SIGNAL(viewChanged()), 
        this, SLOT(onViewChanged()), Qt::UniqueConnection);

    // Connect file output.
    connect(fileOutput_, SIGNAL(selectedFileChanged(QSharedPointer<GenerationFile>)), 
        this, SLOT(onSelectedFileChanged(QSharedPointer<GenerationFile>)), Qt::UniqueConnection);

    // Connect the dialog buttons to their respective functions.
    connect(dialogButtons, SIGNAL(accepted()), this, SLOT(accept()), Qt::UniqueConnection);
    connect(dialogButtons, SIGNAL(rejected()), this, SLOT(reject()), Qt::UniqueConnection);

    configuration_->parseDocuments();
    fileOutput_->onOutputFilesChanged();
}

//-----------------------------------------------------------------------------
// Function: HDLGenerationDialog::~HDLGenerationDialog()
//-----------------------------------------------------------------------------
HDLGenerationDialog::~HDLGenerationDialog()
{
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

    configuration_->writeDocuments();

    QDialog::accept();
}

//-----------------------------------------------------------------------------
// Function: HDLGenerationDialog::onSelectedFileChanged()
//-----------------------------------------------------------------------------
void HDLGenerationDialog::onSelectedFileChanged(QSharedPointer<GenerationFile> newSelection)
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

    QColor const& highlightColor = QColor::fromRgb(183,225,252);

    QTextCursor cursor = previewer_->textCursor();
    cursor.setPosition(implementationStart);

    QTextCharFormat highlighFormat = cursor.charFormat();        
    highlighFormat.setBackground(QBrush(highlightColor));

    cursor.setPosition(implementationEnd, QTextCursor::KeepAnchor);        
    cursor.setCharFormat(highlighFormat);
}

//-----------------------------------------------------------------------------
// Function: HDLGenerationDialog::onViewChanged()
//-----------------------------------------------------------------------------
void HDLGenerationDialog::onViewChanged()
{
    configuration_->parseDocuments();
    fileOutput_->onOutputFilesChanged();
}