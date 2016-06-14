//-----------------------------------------------------------------------------
// File: GeneratorConfigurationDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 23.02.2015
//
// Description:
// Dialog for setting file generation options.
//-----------------------------------------------------------------------------

#include "GeneratorConfigurationDialog.h"
#include "GeneratorConfiguration.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::GeneratorConfigurationDialog()
//-----------------------------------------------------------------------------
GeneratorConfigurationDialog::GeneratorConfigurationDialog(QSharedPointer<GeneratorConfiguration> configuration,
	QSharedPointer<QList<QSharedPointer<View> > > views, 
	QSharedPointer<QList<QSharedPointer<ComponentInstantiation> > > instantiations, QWidget *parent) : 
	QDialog(parent), 
    configuration_(configuration),
	pathEditor_(new QLineEdit(this)),
	viewSelection_(new ViewSelectionWidget(configuration, views, instantiations)),
    generalWarningLabel_(new QLabel)
{    
    setWindowTitle(tr("Configure file generation"));

	// Layout for path selection widgets.
    QHBoxLayout* pathSelectionLayout = new QHBoxLayout();
    pathSelectionLayout->addWidget(new QLabel(tr("Select output file:")));
    pathSelectionLayout->addWidget(pathEditor_);

	// Get the default output path from the generation configuration.
	pathEditor_->setText(configuration_->getOutputPath());

	// Add button for choosing the path via dialog.
    QPushButton* browseButton = new QPushButton(tr("Browse"), this);
    pathSelectionLayout->addWidget(browseButton);

	// Layout for thing coming to the bottom part of the dialog.
	QHBoxLayout* bottomLayout = new QHBoxLayout();

	// Add Ok and cancel give the dialog results.
    QDialogButtonBox* dialogButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, 
        Qt::Horizontal, this);

	bottomLayout->addWidget(generalWarningLabel_);
	bottomLayout->addWidget(dialogButtons);

	// Add everything it their proper position in the final layout.
	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->addWidget(viewSelection_);
	topLayout->addLayout(pathSelectionLayout);
	topLayout->addStretch(1);
	topLayout->addLayout(bottomLayout);

	// Use red to make it more warny.
	generalWarningLabel_->setStyleSheet("QLabel { color : red; }");
	onPathEdited("");

	// Finally, connect the relevant events to their handler functions.
    connect(pathEditor_, SIGNAL(textChanged(const QString &)), this,
		SLOT(onPathEdited(const QString &)), Qt::UniqueConnection);
    connect(browseButton, SIGNAL(clicked(bool)), this, SLOT(onBrowse()), Qt::UniqueConnection);
    connect(dialogButtons, SIGNAL(accepted()), this, SLOT(accept()), Qt::UniqueConnection);
    connect(dialogButtons, SIGNAL(rejected()), this, SLOT(reject()), Qt::UniqueConnection);
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
	// Must have path for a file. 
    if (pathEditor_->text().isEmpty())
	{
		generalWarningLabel_->setText("File path must be defined!");
		return;
	}
	
	// If file is saved to file set, must have a file set and instantiation.
	if (!viewSelection_->validSelections())
	{
		return;
    }

    QDialog::accept();
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::onPathEdited()
//-----------------------------------------------------------------------------
void GeneratorConfigurationDialog::onPathEdited(const QString &text)
{
	// At any rate, tell the path to the configuration.
    configuration_->setOutputPath(pathEditor_->text());

	QFile fileCandidate(pathEditor_->text());

	if (fileCandidate.exists())
	{
		// Warn user if it already exists
		generalWarningLabel_->setText("<b>The selected file already exists and will be overwritten.</b>");
	}
	else
	{
		// Else clear any warning.
		generalWarningLabel_->setText("");
	}
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::onBrowse()
//-----------------------------------------------------------------------------
void GeneratorConfigurationDialog::onBrowse()
{
	// Acquire the path to the file through a dialog.
    QString selectedPath = QFileDialog::getSaveFileName(this,
        tr("Select output file for generation"), pathEditor_->text(),
        tr("Verilog files (*.v)"));

	// If any path chosen, set it as the selected path.
    if (!selectedPath.isEmpty())
    {
        pathEditor_->setText(selectedPath);
        configuration_->setOutputPath(selectedPath);
    }
}
