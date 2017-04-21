//-----------------------------------------------------------------------------
// File: FileOutputWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 15.11.2016
//
// Description:
// Widget for listing generated files and some details about them.
//-----------------------------------------------------------------------------

#include "FileOutputWidget.h"

#include <common/KactusColors.h>

#include <QFileDialog>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QApplication>
#include <QPushButton>

#define WARNING_FILE_EXISTS "<b>One or more of the output files already exists and will be overwritten.</b>"
#define COLUMN_VLNV 0
#define COLUMN_EXISTS 1
#define COLUMN_FILENAME 2

//-----------------------------------------------------------------------------
// Function: FileOutputWidget::FileOutputWidget()
//-----------------------------------------------------------------------------
FileOutputWidget::FileOutputWidget(QSharedPointer<OutputControl> configuration) : 
    model_(configuration),
    pathEditor_(new QLineEdit(this)),
    generalWarningLabel_(new QLabel),
    fileTable_(new QTableWidget)
{
    // Layout for path selection widgets.
    QHBoxLayout* pathSelectionLayout = new QHBoxLayout();
    pathSelectionLayout->addWidget(new QLabel(tr("Output directory:")));
    pathSelectionLayout->addWidget(pathEditor_);

    QPalette p = pathEditor_->palette();
    p.setColor(QPalette::Base, KactusColors::MANDATORY_FIELD);
    pathEditor_->setPalette(p);

    // Get the default output path from the generation configuration.
    pathEditor_->setText(model_->getOutputPath());

    // Add button for choosing the path via dialog.
    QPushButton* browseButton = new QPushButton(tr("Browse"), this);
    pathSelectionLayout->addWidget(browseButton);

    // Stuff that comes on top of the file table.
    QHBoxLayout* headerLayout = new QHBoxLayout();
    QLabel* tableHeader = new QLabel("Output files:");
    headerLayout->addWidget(tableHeader);

    // Layout for thing coming to the bottom part of the dialog.
    QHBoxLayout* bottomLayout = new QHBoxLayout();

    bottomLayout->addWidget(generalWarningLabel_);

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

    // Refit the columns.
    fileTable_->resizeColumnsToContents();

    // Add everything it their proper position in the final layout.
    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addLayout(pathSelectionLayout);
    topLayout->addLayout(headerLayout);
    topLayout->addWidget(fileTable_);
    topLayout->addLayout(bottomLayout);

    // Use red to make it more warny.
    generalWarningLabel_->setStyleSheet("QLabel { color : " + KactusColors::WARNING.name() + "; }");

    connect(pathEditor_, SIGNAL(editingFinished()), this,
        SLOT(onPathEdited()), Qt::UniqueConnection);
    connect(browseButton, SIGNAL(clicked(bool)), this, SLOT(onBrowse()), Qt::UniqueConnection);
    connect(fileTable_, SIGNAL(itemChanged(QTableWidgetItem*)),
        this, SLOT(onItemChanged(QTableWidgetItem*)), Qt::UniqueConnection);
    connect(fileTable_, SIGNAL(itemSelectionChanged()),
        this, SLOT(onItemSelectionChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: FileOutputWidget::~FileOutputWidget()
//-----------------------------------------------------------------------------
FileOutputWidget::~FileOutputWidget()
{
}

//-----------------------------------------------------------------------------
// Function: FileOutputWidget::onOutputFilesChanged()
//-----------------------------------------------------------------------------
void FileOutputWidget::onOutputFilesChanged()
{
    // Remove the old stuff, set number of rows as number of files.
    fileTable_->clearContents();
    fileTable_->setRowCount(model_->getOutputs()->size());

    // Populate the table.
    int row = 0;

    foreach(QSharedPointer<GenerationOutput> output, *model_->getOutputs())
    {
        // Insert VLNV string to the row.
        QString vlnv = output->vlnv_;
        QTableWidgetItem* vlnvItem = new QTableWidgetItem(vlnv);
        vlnvItem->setToolTip(vlnv);
        fileTable_->setItem(row, COLUMN_VLNV, vlnvItem);

        // Create existence check box and insert to the row.
        QTableWidgetItem* fileExistsItem = new QTableWidgetItem(Qt::Checked);
        fileTable_->setItem(row, COLUMN_EXISTS, fileExistsItem);

        // Insert filename to the row.
        QTableWidgetItem* fileNameItem = new QTableWidgetItem(output->fileName_);
        fileNameItem->setToolTip(output->fileName_);
        fileTable_->setItem(row, COLUMN_FILENAME, fileNameItem);

        // Disable editing.
        vlnvItem->setFlags(Qt::ItemIsSelectable);
        fileExistsItem->setFlags(Qt::ItemIsSelectable);

        // The next one goes to the next row.
        ++row;
    }

    // File paths are potentially changed -> update existince status.
    checkExistence();

    if (fileTable_->rowCount() > 0 && fileTable_->columnCount() >= COLUMN_FILENAME)
    {
        fileTable_->setCurrentCell(0, COLUMN_FILENAME);
    }
}

//-----------------------------------------------------------------------------
// Function: FileOutputWidget::onPathEdited()
//-----------------------------------------------------------------------------
void FileOutputWidget::onPathEdited()
{
    // Tell the path to the configuration.
    model_->setOutputPath(pathEditor_->text());

    // Path of of all files changed -> update existence status.
    checkExistence();

    emit outputPathChanged();
}

//-----------------------------------------------------------------------------
// Function: FileOutputWidget::onBrowse()
//-----------------------------------------------------------------------------
void FileOutputWidget::onBrowse()
{
    // Acquire the path to the file through a dialog.
    QString selectedPath = QFileDialog::getExistingDirectory(this,
        tr("Select output path for generation"), pathEditor_->text(), QFileDialog::ShowDirsOnly);

    // If any path chosen, set it as the selected path.
    if (!selectedPath.isEmpty())
    {
        pathEditor_->setText(selectedPath);

        // Tell the path to the configuration.
        model_->setOutputPath(pathEditor_->text());

        // Path of of all files changed -> update existence status.
        checkExistence();

        emit outputPathChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: FileOutputWidget::onItemChanged()
//-----------------------------------------------------------------------------
void FileOutputWidget::onItemChanged(QTableWidgetItem *item)
{
    // Must be in the filename column.
    if (item->column() != COLUMN_FILENAME)
    {
        return;
    }

    // Inform the change to the model.
    QSharedPointer<GenerationOutput> affectedFile = model_->changeFileName(item->row(), item->text());

    if (affectedFile)
    {
        // A name of a file changed -> emit signal and update existence status.
        emit selectedFileChanged(affectedFile);
        checkExistence();
    }
}

//-----------------------------------------------------------------------------
// Function: FileOutputWidget::onItemSelectionChanged()
//-----------------------------------------------------------------------------
void FileOutputWidget::onItemSelectionChanged()
{
    if (fileTable_->selectedItems().size() < 1)
    {
        return;
    }

    QTableWidgetItem* item = fileTable_->selectedItems().last();

    if (item->row() < model_->getOutputs()->size())
    {
        QSharedPointer<GenerationOutput> selection = model_->getOutputs()->at(item->row());
        emit selectedFileChanged(selection);
    }
}

//-----------------------------------------------------------------------------
// Function: FileOutputWidget::checkExistence()
//-----------------------------------------------------------------------------
void FileOutputWidget::checkExistence()
{
    bool existingFiles = false;

    for(int row = 0; row < fileTable_->rowCount(); ++row)
    {
        QTableWidgetItem* pathItem = fileTable_->item(row, COLUMN_FILENAME);
        QTableWidgetItem* fileExistsItem = fileTable_->item(row, COLUMN_EXISTS);

        // Both must exist
        if (!fileExistsItem || !pathItem)
        {
            continue;
        }

        // Select correct check state: Does it exist or not.
        QFile fileCandidate(model_->getOutputPath() + "/" + pathItem->text());

        if (fileCandidate.exists())
        {
            // Check and set a warning color.
            fileExistsItem->setCheckState(Qt::Checked);
            pathItem->setTextColor(KactusColors::WARNING);
            existingFiles = true;
        }
        else
        {
            // Uncheck and set regular color.
            fileExistsItem->setCheckState(Qt::Unchecked);
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