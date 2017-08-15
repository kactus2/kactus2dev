//-----------------------------------------------------------------------------
// File: ObjectExportDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 04.07.2017
//
// Description:
// Dialog for selecting the exported items.
//-----------------------------------------------------------------------------

#include "ObjectExportDialog.h"

#include <QLabel>
#include <QFileDialog>

//-----------------------------------------------------------------------------
// Function: ObjectExportDialog::ObjectExportDialog()
//-----------------------------------------------------------------------------
ObjectExportDialog::ObjectExportDialog(QString const& initialPath, QWidget* parent, Qt::WindowFlags f):
ObjectSelectionDialog(tr("Export"), QStringLiteral(":icons/common/graphics/export.png"),
    tr("Select items to be exported"), parent, f),
folderPathEditor_(initialPath, this),
folderBrowseButton_(QIcon(":icons/common/graphics/folder-horizontal-open.png"), QString(), this),
currentPath_(initialPath)
{
    if (initialPath.isEmpty())
    {
        disableOkButton();
    }

    setupLayout();
    connectSignals();
}

//-----------------------------------------------------------------------------
// Function: ObjectExportDialog::~ObjectExportDialog()
//-----------------------------------------------------------------------------
ObjectExportDialog::~ObjectExportDialog()
{

}

//-----------------------------------------------------------------------------
// Function: ObjectExportDialog::setupLayout()
//-----------------------------------------------------------------------------
void ObjectExportDialog::setupLayout()
{
    QString introLabel = tr("Export");
    QString introText = tr("Export the selected items to the destination folder. ") +
        tr(" The original files will be retained in their containing folders.");
    QWidget* introWidget = setupIntroWidget(introLabel, introText);

    QString folderToolTip(tr("The destination folder for the export."));
    QLabel* folderLabel = new QLabel(tr("Destination folder:"), this);
    folderLabel->setToolTip(folderToolTip);
    folderPathEditor_.setToolTip(folderToolTip);
    folderPathEditor_.setProperty("mandatoryField", true);
    folderBrowseButton_.setToolTip("Browse.");

    QHBoxLayout* folderBrowseLayout = new QHBoxLayout();
    folderBrowseLayout->addWidget(folderLabel);
    folderBrowseLayout->addWidget(&folderPathEditor_);
    folderBrowseLayout->addWidget(&folderBrowseButton_);

    QHBoxLayout* buttonLayout = setupButtonLayout();

    QVBoxLayout* overallLayout = new QVBoxLayout(this);
    overallLayout->addWidget(introWidget);
    overallLayout->addLayout(folderBrowseLayout, 0);
    overallLayout->addWidget(getItemList(), 1);
    overallLayout->addLayout(buttonLayout, 0);
}

//-----------------------------------------------------------------------------
// Function: ObjectExportDialog::connectSignals()
//-----------------------------------------------------------------------------
void ObjectExportDialog::connectSignals()
{
    ObjectSelectionDialog::connectSignals();

    connect(&folderBrowseButton_, SIGNAL(clicked()), this, SLOT(onBrowseTarget()), Qt::UniqueConnection);
    connect(&folderPathEditor_, SIGNAL(textChanged(const QString&)),
        this, SLOT(onDestinationFolderChanged(const QString&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ObjectExportDialog::onBrowseTarget()
//-----------------------------------------------------------------------------
void ObjectExportDialog::onBrowseTarget()
{
    QFileDialog browseDialog(this, tr("Select destination folder for export"), currentPath_);
    browseDialog.setFileMode(QFileDialog::Directory);
    if (browseDialog.exec() == QDialog::Accepted)
    {
        QStringList selectedFiles = browseDialog.selectedFiles();

        if (!selectedFiles.isEmpty())
        {
            QString selectedFolder = selectedFiles.first();
            onDestinationFolderChanged(selectedFolder);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ObjectExportDialog::onDestinationFolderChanged()
//-----------------------------------------------------------------------------
void ObjectExportDialog::onDestinationFolderChanged(const QString& newDestination)
{
    folderPathEditor_.setText(newDestination);
    currentPath_ = newDestination;

    QPalette pathPalette = folderPathEditor_.palette();
    
    if (QFileInfo(newDestination).isDir())
    {
        pathPalette.setColor(QPalette::Text, Qt::black);
        enableOkButton();
    }
    else
    {
        pathPalette.setColor(QPalette::Text, Qt::red);
        disableOkButton();
    }

    folderPathEditor_.setPalette(pathPalette);
}

//-----------------------------------------------------------------------------
// Function: ObjectExportDialog::getTargetDirectory()
//-----------------------------------------------------------------------------
QString ObjectExportDialog::getTargetDirectory() const
{
    return currentPath_;
}
