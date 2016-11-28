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

    QCheckBox* useInterfaces = new QCheckBox("Use interfaces");
    optionLayout->addWidget(useInterfaces);
    QCheckBox* generateMemory = new QCheckBox("Generate memory");
    optionLayout->addWidget(generateMemory);

	// Layout for things coming to the bottom part of the dialog.
	QHBoxLayout* bottomLayout = new QHBoxLayout();

	// Add Ok and cancel give the dialog results.
    QDialogButtonBox* dialogButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, 
        Qt::Horizontal);

	bottomLayout->addWidget(generalWarningLabel_);
	bottomLayout->addWidget(dialogButtons);

	// Add everything it their proper position in the final layout.
	QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addWidget(viewSelection_);
    topLayout->addWidget(optionGroup);
    topLayout->addWidget(fileOutput_);
    topLayout->addLayout(bottomLayout);

    // Finally, connect the relevant events to their handler functions.
    connect(viewSelection_, SIGNAL(viewChanged()), 
        this, SLOT(onViewChanged()), Qt::UniqueConnection);
    connect(useInterfaces, SIGNAL(stateChanged(int)), 
        this, SLOT(onInterfaceGenerationStateChanged(int)), Qt::UniqueConnection);
    connect(generateMemory, SIGNAL(stateChanged(int)), 
        this, SLOT(onMemoryGenerationStateChanged(int)), Qt::UniqueConnection);
    connect(dialogButtons, SIGNAL(accepted()), this, SLOT(accept()), Qt::UniqueConnection);
    connect(dialogButtons, SIGNAL(rejected()), this, SLOT(reject()), Qt::UniqueConnection);
    connect(configuration_.data(), SIGNAL(outputFilesChanged()), 
        fileOutput_, SLOT(onOutputFilesChanged()), Qt::UniqueConnection);

    configuration_->parseDocuments();
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
		generalWarningLabel_->setText(warning);
		return;
	}

    QDialog::accept();
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::onViewChanged()
//-----------------------------------------------------------------------------
void GeneratorConfigurationDialog::onViewChanged()
{
    configuration_->parseDocuments();
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