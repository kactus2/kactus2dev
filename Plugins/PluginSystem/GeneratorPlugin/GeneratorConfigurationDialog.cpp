//-----------------------------------------------------------------------------
// File: GeneratorConfigurationDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 23.02.2015
//
// Description:
// Dialog for setting file generation options.
//-----------------------------------------------------------------------------

#include "GeneratorConfigurationDialog.h"
#include "GeneratorConfiguration.h"

#include <QDialogButtonBox>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QGroupBox>

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::GeneratorConfigurationDialog()
//-----------------------------------------------------------------------------
GeneratorConfigurationDialog::GeneratorConfigurationDialog(QSharedPointer<GeneratorConfiguration> configuration,
	QWidget *parent) : 
	QDialog(parent,Qt::WindowTitleHint), 
    configuration_(configuration),
    viewSelection_(new ViewSelectionWidget(configuration->getViewSelection())),
    fileOutput_(new FileOutputWidget(configuration->getFileOuput())),
    generalWarningLabel_(new QLabel)
{
    setWindowTitle(tr("Configure file generation for %1.").arg(configuration->getViewSelection()->getTargetLanguage()));

    // Optional generation features.
    QGroupBox* optionGroup = new QGroupBox("Optional features:");
    QHBoxLayout* optionLayout = new QHBoxLayout();
    optionGroup->setLayout(optionLayout);

    QCheckBox* useInterfaces = new QCheckBox("Use interfaces (disabled)");
    useInterfaces->setEnabled(false);
    optionLayout->addWidget(useInterfaces);
    QCheckBox* generateMemory = new QCheckBox("Generate memory (experimental)");
    optionLayout->addWidget(generateMemory);

    if (configuration_->getInterfaceGeneration())
    {
        useInterfaces->setCheckState(Qt::Checked);
    }

    if (configuration_->getMemoryGeneration())
    {
        generateMemory->setCheckState(Qt::Checked);
    }

	// Layout for things coming to the bottom part of the dialog.
	QHBoxLayout* bottomLayout = new QHBoxLayout();

	// Add Ok and cancel give the dialog results.
    QDialogButtonBox* dialogButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, 
        Qt::Horizontal);

	bottomLayout->addWidget(generalWarningLabel_);
	bottomLayout->addWidget(dialogButtons);

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
    leftLayout->addWidget(optionGroup);
    leftLayout->addWidget(fileOutput_);
    leftLayout->addLayout(bottomLayout);

    QHBoxLayout* topLayout = new QHBoxLayout(this);
    QGroupBox* leftBox = new QGroupBox("Settings");
    leftBox->setLayout(leftLayout);
    QGroupBox* rightBox = new QGroupBox("Preview");
    QVBoxLayout* rightLayout = new QVBoxLayout();
    rightLayout->addWidget(previewer_);
    rightBox->setLayout(rightLayout);

    topLayout->addWidget(leftBox);
    topLayout->addWidget(rightBox);

    // Finally, connect the relevant events to their handler functions.

    // Connect the view selection.
    connect(viewSelection_, SIGNAL(viewChanged()), 
        this, SLOT(onViewChanged()), Qt::UniqueConnection);

    // Connect the optional features.
    connect(useInterfaces, SIGNAL(stateChanged(int)), 
        this, SLOT(onInterfaceGenerationStateChanged(int)), Qt::UniqueConnection);
    connect(generateMemory, SIGNAL(stateChanged(int)), 
        this, SLOT(onMemoryGenerationStateChanged(int)), Qt::UniqueConnection);

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
// Function: GeneratorConfigurationDialog::~GeneratorConfigurationDialog()
//-----------------------------------------------------------------------------
GeneratorConfigurationDialog::~GeneratorConfigurationDialog()
{
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::accept()
//-----------------------------------------------------------------------------
void GeneratorConfigurationDialog::accept()
{
	// Check it is sane.
    QString warning;
    if (!configuration_->validSelections(warning))
	{
        // If not, tell user why not.
		generalWarningLabel_->setText(warning);
		return;
	}

    QDialog::accept();
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::onSelectedFileChanged()
//-----------------------------------------------------------------------------
void GeneratorConfigurationDialog::onSelectedFileChanged(QSharedPointer<GenerationFile> newSelection)
{
    previewer_->setPlainText(newSelection->fileContent_);
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::onViewChanged()
//-----------------------------------------------------------------------------
void GeneratorConfigurationDialog::onViewChanged()
{
    configuration_->parseDocuments();
    fileOutput_->onOutputFilesChanged();
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::onInterfaceGenerationStateChanged()
//-----------------------------------------------------------------------------
void GeneratorConfigurationDialog::onInterfaceGenerationStateChanged(int state)
{
    configuration_->setInterfaceGeneration(state == Qt::Checked);
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::onMemoryGenerationStateChanged()
//-----------------------------------------------------------------------------
void GeneratorConfigurationDialog::onMemoryGenerationStateChanged(int state)
{
    configuration_->setMemoryGeneration(state == Qt::Checked);
}