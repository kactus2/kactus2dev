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
#include <QCheckBox>
#include <QHeaderView>
#include <QApplication>

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::GeneratorConfigurationDialog()
//-----------------------------------------------------------------------------
GeneratorConfigurationDialog::GeneratorConfigurationDialog(QSharedPointer<GeneratorConfiguration> configuration,
	QWidget *parent) : 
	QDialog(parent), 
    configuration_(configuration),
	pathEditor_(new QLineEdit(this)),
	viewSelection_(new ViewSelectionWidget(configuration->getViewSelection())),
    generalWarningLabel_(new QLabel),
    fileTable_(new QTableWidget)
{    
    setWindowTitle(tr("Configure file generation for %1.").arg(configuration->getViewSelection()->getTargetLanguage()));

    QFormLayout* checkLayout = new QFormLayout();
    QCheckBox* useInterfaces = new QCheckBox();
    checkLayout->addRow(tr("Use interfaces:"), useInterfaces);

	// Layout for path selection widgets.
    QHBoxLayout* pathSelectionLayout = new QHBoxLayout();
    pathSelectionLayout->addWidget(new QLabel(tr("Output directory:")));
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

    // The headers for each of the column.
    QStringList headers;
    headers.append(tr("Exists"));
    headers.append(tr("File path"));
    // Set the number of columns correspond the number of headers.
    fileTable_->setColumnCount(headers.size());

    // Put the headers in to effect.
    fileTable_->setHorizontalHeaderLabels(headers);

    // Vertical headers are not used.
    fileTable_->verticalHeader()->hide();

    // Easier to see the different rows from one another.
    fileTable_->setAlternatingRowColors(true);

    // The last column is stretched take the available space in the widget.
    fileTable_->horizontalHeader()->setStretchLastSection(true);

	// Add everything it their proper position in the final layout.
	QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addWidget(viewSelection_);
    topLayout->addLayout(checkLayout);
	topLayout->addLayout(pathSelectionLayout);
    topLayout->addStretch(1);
    topLayout->addWidget(fileTable_);
	topLayout->addLayout(bottomLayout);

	// Use red to make it more warny.
	generalWarningLabel_->setStyleSheet("QLabel { color : red; }");
	onPathEdited("");

    // Finally, connect the relevant events to their handler functions.
    connect(viewSelection_, SIGNAL(viewChanged()), 
        this, SLOT(onViewChanged()), Qt::UniqueConnection);
    connect(configuration_.data(), SIGNAL(outputFilesChanged()), 
        this, SLOT(onOutputFilesChanged()), Qt::UniqueConnection);
    connect(useInterfaces, SIGNAL(stateChanged(int)), 
        this, SLOT(onInterfaceGenerationStateChanged(int)), Qt::UniqueConnection);
    connect(pathEditor_, SIGNAL(textChanged(const QString &)), this,
		SLOT(onPathEdited(const QString &)), Qt::UniqueConnection);
    connect(browseButton, SIGNAL(clicked(bool)), this, SLOT(onBrowse()), Qt::UniqueConnection);
    connect(dialogButtons, SIGNAL(accepted()), this, SLOT(accept()), Qt::UniqueConnection);
    connect(dialogButtons, SIGNAL(rejected()), this, SLOT(reject()), Qt::UniqueConnection);

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
	// Must have path for a file. 
    if (configuration_->getOutputPath().isEmpty())
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
// Function: GeneratorConfigurationDialog::onViewChanged()
//-----------------------------------------------------------------------------
void GeneratorConfigurationDialog::onViewChanged()
{
    configuration_->parseDocuments();
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::onOutputFilesChanged()
//-----------------------------------------------------------------------------
void GeneratorConfigurationDialog::onOutputFilesChanged()
{
    fileTable_->setRowCount(configuration_->getDocumentNames()->size());

    // Populate the table.
    int row = 0;
    bool existingFiles = false;

    foreach(QString name, *configuration_->getDocumentNames())
    {
        // The name goes to the last columns.
        QTableWidgetItem* nameItem = new QTableWidgetItem(name);
        fileTable_->setItem(row, 1, nameItem);

        QTableWidgetItem* defCheckItem = new QTableWidgetItem(Qt::Checked);

        // Disable editing.
        defCheckItem->setFlags(Qt::ItemIsSelectable);

        // Select correct check state: Does it exist or not.
        QFile fileCandidate(name);

        if (fileCandidate.exists())
        {
            defCheckItem->setCheckState(Qt::Checked);
            nameItem->setTextColor(QColor(Qt::red));
            existingFiles = true;
        }
        else
        {
            defCheckItem->setCheckState(Qt::Unchecked);
            nameItem->setTextColor(QApplication::palette().brush(QPalette::Text).color());
        }

        // Set the check box to the correct column at the current row.
        fileTable_->setItem(row, 0, defCheckItem);

        // The next one goes to the next row.
        ++row;
    }

    if (existingFiles)
    {
        // Warn user if it already exists
        generalWarningLabel_->setText("<b>One or more of the output files already exists and will be overwritten.</b>");
    }
    else
    {
        // Else clear any warning.
        generalWarningLabel_->setText("");
    }
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::onInterfaceGenerationStateChanged()
//-----------------------------------------------------------------------------
void GeneratorConfigurationDialog::onInterfaceGenerationStateChanged(int state)
{
    configuration_->setInterfaceGeneration(state == Qt::Checked);
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::onPathEdited()
//-----------------------------------------------------------------------------
void GeneratorConfigurationDialog::onPathEdited(const QString &text)
{
	// At any rate, tell the path to the configuration.
    configuration_->setOutputPath(pathEditor_->text());
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::onBrowse()
//-----------------------------------------------------------------------------
void GeneratorConfigurationDialog::onBrowse()
{
	// Acquire the path to the file through a dialog.
    QString selectedPath = QFileDialog::getExistingDirectory(this,
        tr("Select output path for generation"), pathEditor_->text(), 0);

	// If any path chosen, set it as the selected path.
    if (!selectedPath.isEmpty())
    {
        pathEditor_->setText(selectedPath);
    }
}
