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

#include <common/widgets/EnumCollectionEditor/EnumCollectionEditor.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <Plugins/common/CpuRoutesContainer.h>
#include <Plugins/common/CPUDialog/CPUEditor.h>

#include <QDialogButtonBox>
#include <QPushButton>
#include <QFormLayout>
#include <QIcon>
#include <QFileDialog>

//-----------------------------------------------------------------------------
// Function: CPUSelectionDialog::CPUSelectionDialog()
//-----------------------------------------------------------------------------
CPUSelectionDialog::CPUSelectionDialog(QSharedPointer<Component> topComponent, LibraryInterface* library,
    QStringList const& viewNames, QStringList const& fileSetNames, CPUEditor* cpuEditor, QString const& dialogType,
    QWidget* extraEditor, QWidget *parent, QString const& configurationFolder, bool saveToFileSet, QString const& configurationFileSet,
    QString const& configurationView):
QDialog(parent),
viewSelection_(new QComboBox(this)),
fileSetSelection_(new QComboBox(this)),
fileSetBox_(new QGroupBox("Add files to file set")),
library_(library),
component_(topComponent),
graphFactory_(library),
cpuDetailEditor_(cpuEditor),
folderLine_(new QLineEdit(this)),
extraEditor_(extraEditor)
{
    cpuDetailEditor_->setParent(this);

    setWindowTitle("File generation for " + dialogType);

    QString componentPath = library->getDirectoryPath(topComponent->getVlnv());
    folderLine_->setText(componentPath);
    folderLine_->setToolTip(componentPath);

    cpuDetailEditor_->setupFolderPath(componentPath);

    viewSelection_->addItems(viewNames);
    viewSelection_->setCurrentIndex(0);

    fileSetSelection_->addItem("");
    fileSetSelection_->addItems(fileSetNames);
    fileSetSelection_->setEditable(true);

    fileSetBox_->setCheckable(true);

    setupConfiguration(configurationFolder, saveToFileSet, configurationFileSet, configurationView);

    setupLayout();

    setupCPUSelection();

    connect(viewSelection_, SIGNAL(currentIndexChanged(int)), this, SLOT(onViewChanged()), Qt::UniqueConnection);

    connect(folderLine_, SIGNAL(textChanged(QString const&)), cpuDetailEditor_, SIGNAL(changeInSelectedPath(QString const&)), Qt::UniqueConnection);

    setMinimumWidth(840);
}

//-----------------------------------------------------------------------------
// Function: CPUSelectionDialog::setupConfiguration()
//-----------------------------------------------------------------------------
void CPUSelectionDialog::setupConfiguration(QString const& configurationFolder, bool saveToFileSet, QString const& configurationFileSet, QString const& configurationView)
{
    if (!configurationFolder.isEmpty())
    {
        folderLine_->setText(configurationFolder);
        emit changeInSelectedPath(configurationFolder);
    }

    fileSetBox_->setChecked(saveToFileSet);
    if (!configurationFileSet.isEmpty())
    {
        fileSetSelection_->setCurrentText(configurationFileSet);
    }

    if (!configurationView.isEmpty())
    {
        viewSelection_->setCurrentText(configurationView);
    }
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

    if (extraEditor_ != 0)
    {
        leftLayout->addWidget(extraEditor_);
    }

    leftLayout->addStretch(2);

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
}

//-----------------------------------------------------------------------------
// Function: CPUSelectionDialog::setupCPUSelection()
//-----------------------------------------------------------------------------
void CPUSelectionDialog::setupCPUSelection()
{
    QString activeView = viewSelection_->currentText();

    cpuDetailEditor_->setupCPUDetails(library_, component_, activeView);
}

//-----------------------------------------------------------------------------
// Function: CPUSelectionDialog::getSelectedCPUs()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<CpuRoutesContainer> > CPUSelectionDialog::getSelectedCPUs()
{
    return cpuDetailEditor_->getSelectedCPUs();
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
// Function: CPUSelectionDialog::getView()
//-----------------------------------------------------------------------------
QString CPUSelectionDialog::getView() const
{
    return viewSelection_->currentText();
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
