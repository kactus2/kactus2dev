//-----------------------------------------------------------------------------
// File: CPUSelectionDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 21.04.2021
//
// Description:
// Dialog for selecting CPUs from a connectivity graph.
//-----------------------------------------------------------------------------

#include "CPUSelectionDialog.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Cpu.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <common/widgets/EnumCollectionEditor/EnumCollectionEditor.h>

#include <Plugins/SVDGenerator/CPUDialog/SVDCPUEditor.h>

#include <QDialogButtonBox>
#include <QPushButton>
#include <QFormLayout>
#include <QIcon>
#include <QFileDialog>

//-----------------------------------------------------------------------------
// Function: CPUSelectionDialog::CPUSelectionDialog()
//-----------------------------------------------------------------------------
CPUSelectionDialog::CPUSelectionDialog(QSharedPointer<Component> topComponent, LibraryInterface* library,
    QStringList const& viewNames, QStringList const& fileSetNames, QWidget *parent):
QDialog(parent),
viewSelection_(new QComboBox(this)),
fileSetSelection_(new QComboBox(this)),
fileSetBox_(new QGroupBox("Add files to file set")),
cpuSelection_(),
library_(library),
component_(topComponent),
graphFactory_(library),
cpuDetailEditor_(new SVDCPUEditor(this)),
folderLine_(new QLineEdit(this))
{
    setWindowTitle("File generation for SVD");

    QString componentPath = library->getDirectoryPath(topComponent->getVlnv());
    folderLine_->setText(componentPath);
    folderLine_->setToolTip(componentPath);

    viewSelection_->addItems(viewNames);
    viewSelection_->setCurrentIndex(0);

    fileSetSelection_->addItem("");
    fileSetSelection_->addItems(fileSetNames);
    fileSetSelection_->setEditable(true);

    fileSetBox_->setCheckable(true);

    setupLayout();

    connect(viewSelection_, SIGNAL(currentIndexChanged(int)), this, SLOT(onViewChanged()), Qt::UniqueConnection);

    setMinimumWidth(840);
}

//-----------------------------------------------------------------------------
// Function: CPUSelectionDialog::onViewChanged()
//-----------------------------------------------------------------------------
void CPUSelectionDialog::onViewChanged()
{
    setupCPUSelection();
}

//-----------------------------------------------------------------------------
// Function: CPUSelectionDialog::setupLayout()
//-----------------------------------------------------------------------------
void CPUSelectionDialog::setupLayout()
{
    QLabel* viewLabel(new QLabel("Select view:"));

    QHBoxLayout* viewLayout(new QHBoxLayout());
    viewLayout->addWidget(viewLabel);
    viewLayout->addWidget(viewSelection_, 1);

    QVBoxLayout* filesetLayout(new QVBoxLayout());
    filesetLayout->addWidget(fileSetSelection_);
    filesetLayout->addStretch(1);
    fileSetBox_->setLayout(filesetLayout);

    QPushButton* openFolderButton(
        new QPushButton(QIcon(":icons/common/graphics/folder-horizontal-open.png"), QString(), this));

    connect(openFolderButton, SIGNAL(clicked()), this, SLOT(onChangeTargetFolder()), Qt::UniqueConnection);

    QHBoxLayout* folderLayout(new QHBoxLayout());
    folderLayout->addWidget(folderLine_, 1);
    folderLayout->addWidget(openFolderButton);
    folderLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    QGroupBox* folderbox(new QGroupBox("Select destination folder"));
    folderbox->setLayout(folderLayout);

    QVBoxLayout* leftLayout(new QVBoxLayout());
    leftLayout->addLayout(viewLayout);
    leftLayout->addWidget(fileSetBox_);
    leftLayout->addWidget(folderbox);


    QHBoxLayout* topLayout(new QHBoxLayout());
    topLayout->addLayout(leftLayout);
    topLayout->addWidget(cpuDetailEditor_, 1);

    QDialogButtonBox* dialogButtons = new QDialogButtonBox(QDialogButtonBox::Cancel,
        Qt::Horizontal);
    QPushButton* writeButton = dialogButtons->addButton("Ok", QDialogButtonBox::AcceptRole);
    writeButton->setDefault(false);
    writeButton->setAutoDefault(false);

    connect(dialogButtons, SIGNAL(accepted()), this, SLOT(accept()), Qt::UniqueConnection);
    connect(dialogButtons, SIGNAL(rejected()), this, SLOT(reject()), Qt::UniqueConnection);

    QVBoxLayout* masterLayout(new QVBoxLayout(this));
    masterLayout->addLayout(topLayout);
    masterLayout->addWidget(dialogButtons);

    setupCPUSelection();
}

//-----------------------------------------------------------------------------
// Function: CPUSelectionDialog::setupCPUSelection()
//-----------------------------------------------------------------------------
void CPUSelectionDialog::setupCPUSelection()
{
    cpuSelection_.clear();

    QString activeView = viewSelection_->currentText();

    cpuSelection_ = ConnectivityGraphUtilities::getDefaultCPUs(library_, component_, activeView);
    cpuDetailEditor_->setupCPUDetails(cpuSelection_);
}

//-----------------------------------------------------------------------------
// Function: CPUSelectionDialog::getSelectedCPUs()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<ConnectivityGraphUtilities::cpuDetailRoutes> > CPUSelectionDialog::getSelectedCPUs()
{
    QVector<QSharedPointer<ConnectivityGraphUtilities::cpuDetailRoutes> > cpus;
    for (auto currentCPU : cpuSelection_)
    {
        if (currentCPU->createFile_)
        {
            cpus.append(currentCPU);
        }
    }

    return cpus;
}

//-----------------------------------------------------------------------------
// Function: CPUSelectionDialog::saveToFileSet()
//-----------------------------------------------------------------------------
bool CPUSelectionDialog::saveToFileSet() const
{
    return fileSetBox_->isChecked();
}

//-----------------------------------------------------------------------------
// Function: CPUSelectionDialog::getTargetFileSet()
//-----------------------------------------------------------------------------
QString CPUSelectionDialog::getTargetFileSet() const
{
    return fileSetSelection_->currentText();
}

//-----------------------------------------------------------------------------
// Function: CPUSelectionDialog::getTargetFolder()
//-----------------------------------------------------------------------------
QString CPUSelectionDialog::getTargetFolder() const
{
    return folderLine_->text();
}

//-----------------------------------------------------------------------------
// Function: CPUSelectionDialog::onChangeTargetFolder()
//-----------------------------------------------------------------------------
void CPUSelectionDialog::onChangeTargetFolder()
{
    QString newDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), folderLine_->text(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    folderLine_->setText(newDir);
    folderLine_->setToolTip(newDir);
}
