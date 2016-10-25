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

#define WARNING_FILE_EXISTS "<b>One or more of the output files already exists and will be overwritten.</b>"
#define COLUMN_VLNV 0
#define COLUMN_EXISTS 1
#define COLUMN_FILENAME 2

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::GeneratorConfigurationDialog()
//-----------------------------------------------------------------------------
GeneratorConfigurationDialog::GeneratorConfigurationDialog(QSharedPointer<GeneratorConfiguration> configuration,
	QWidget *parent) : 
	QDialog(parent,Qt::WindowTitleHint), 
    configuration_(configuration),
	pathEditor_(new QLineEdit(this)),
	viewSelection_(new ViewSelectionWidget(configuration->getViewSelection())),
    generalWarningLabel_(new QLabel),
    fileTable_(new QTableWidget)
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

	// Layout for path selection widgets.
    QHBoxLayout* pathSelectionLayout = new QHBoxLayout();
    pathSelectionLayout->addWidget(new QLabel(tr("Output directory:")));
    pathSelectionLayout->addWidget(pathEditor_);

	// Get the default output path from the generation configuration.
	pathEditor_->setText(configuration_->getOutputPath());

	// Add button for choosing the path via dialog.
    QPushButton* browseButton = new QPushButton(tr("Browse"), this);
    pathSelectionLayout->addWidget(browseButton);

    // Stuff that comes on top of the file table.
    QHBoxLayout* headerLayout = new QHBoxLayout();
    QLabel* tableHeader = new QLabel("Output files:");
    headerLayout->addWidget(tableHeader);

	// Layout for thing coming to the bottom part of the dialog.
	QHBoxLayout* bottomLayout = new QHBoxLayout();

	// Add Ok and cancel give the dialog results.
    QDialogButtonBox* dialogButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, 
        Qt::Horizontal, this);

	bottomLayout->addWidget(generalWarningLabel_);
	bottomLayout->addWidget(dialogButtons);

    // The headers for each of the column.
    QStringList headers;
    headers.append(tr("Top component VLNV"));
    headers.append(tr("Exists"));
    headers.append(tr("File name"));
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
    topLayout->addWidget(optionGroup);
	topLayout->addLayout(pathSelectionLayout);
    topLayout->addStretch(1);
    topLayout->addLayout(headerLayout);
    topLayout->addWidget(fileTable_);
	topLayout->addLayout(bottomLayout);

	// Use red to make it more warny.
	generalWarningLabel_->setStyleSheet("QLabel { color : red; }");
	onPathEdited("");

    // Finally, connect the relevant events to their handler functions.
    connect(viewSelection_, SIGNAL(viewChanged()), 
        this, SLOT(onViewChanged()), Qt::UniqueConnection);
    connect(configuration_.data(), SIGNAL(outputFilesChanged(QStringList)), 
        this, SLOT(onOutputFilesChanged(QStringList)), Qt::UniqueConnection);
    connect(useInterfaces, SIGNAL(stateChanged(int)), 
        this, SLOT(onInterfaceGenerationStateChanged(int)), Qt::UniqueConnection);
    connect(generateMemory, SIGNAL(stateChanged(int)), 
        this, SLOT(onMemoryGenerationStateChanged(int)), Qt::UniqueConnection);
    connect(pathEditor_, SIGNAL(textChanged(const QString &)), this,
		SLOT(onPathEdited(const QString &)), Qt::UniqueConnection);
    connect(browseButton, SIGNAL(clicked(bool)), this, SLOT(onBrowse()), Qt::UniqueConnection);
    connect(dialogButtons, SIGNAL(accepted()), this, SLOT(accept()), Qt::UniqueConnection);
    connect(dialogButtons, SIGNAL(rejected()), this, SLOT(reject()), Qt::UniqueConnection);

    configuration_->parseDocuments();

    connect(fileTable_, SIGNAL(itemChanged(QTableWidgetItem*)),
        this, SLOT(onItemChanged(QTableWidgetItem*)), Qt::UniqueConnection);
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
void GeneratorConfigurationDialog::onOutputFilesChanged(QStringList vlvns)
{
    // Remove the old stuff, set number of rows as number of files.
    fileTable_->clearContents();
    fileTable_->setRowCount(configuration_->getFileNames()->size());

    // Populate the table.
    int row = 0;

    foreach(QString* fileName, *configuration_->getFileNames())
    {
        // Insert VLNV string to the row.
        QTableWidgetItem* vlnvItem = new QTableWidgetItem(vlvns.at(row));
        fileTable_->setItem(row, COLUMN_VLNV, vlnvItem);

        // Create existence check box and insert to the row.
        QTableWidgetItem* defCheckItem = new QTableWidgetItem(Qt::Checked);
        fileTable_->setItem(row, COLUMN_EXISTS, defCheckItem);

        // Insert filename to the row.
        QTableWidgetItem* fileNameItem = new QTableWidgetItem(*fileName);
        fileTable_->setItem(row, COLUMN_FILENAME, fileNameItem);

        // Disable editing.
        vlnvItem->setFlags(Qt::ItemIsSelectable);
        defCheckItem->setFlags(Qt::ItemIsSelectable);

        // The next one goes to the next row.
        ++row;
    }

    // File paths are potentially changed -> update existince status.
    checkExistence();

    // Refit the columns.
    fileTable_->resizeColumnsToContents();
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

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::onPathEdited()
//-----------------------------------------------------------------------------
void GeneratorConfigurationDialog::onPathEdited(const QString &text)
{
	// Tell the path to the configuration.
    configuration_->setOutputPath(pathEditor_->text());

    // Path of of all files changed -> update existence status.
    checkExistence();
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

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::onItemChanged()
//-----------------------------------------------------------------------------
void GeneratorConfigurationDialog::onItemChanged(QTableWidgetItem *item)
{
    if (item->column() != COLUMN_FILENAME)
    {
        return;
    }

    configuration_->setOutputFileName(item->text(),item->row());

    // A name of a file changed -> update existence status.
    checkExistence();
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::checkExistence()
//-----------------------------------------------------------------------------
void GeneratorConfigurationDialog::checkExistence()
{
    bool existingFiles = false;

    for(int row = 0; row < fileTable_->rowCount(); ++row)
    {
        QTableWidgetItem* pathItem = fileTable_->item(row, COLUMN_FILENAME);
        QTableWidgetItem* defCheckItem = fileTable_->item(row, COLUMN_EXISTS);

        // Both must exist
        if (!defCheckItem || !pathItem)
        {
            continue;
        }

        // Select correct check state: Does it exist or not.
        QFile fileCandidate(configuration_->getOutputPath() + "/" + pathItem->text());

        if (fileCandidate.exists())
        {
            // Check and set a warning color.
            defCheckItem->setCheckState(Qt::Checked);
            pathItem->setTextColor(QColor(Qt::red));
            existingFiles = true;
        }
        else
        {
            // Uncheck and set regular color.
            defCheckItem->setCheckState(Qt::Unchecked);
            pathItem->setTextColor(QApplication::palette().brush(QPalette::Text).color());
        }
    }

    if (existingFiles)
    {
        // Warn user if it already exists
        generalWarningLabel_->setText(WARNING_FILE_EXISTS);
    }
    else
    {
        // Else clear any warning.
        generalWarningLabel_->setText("");
    }
}