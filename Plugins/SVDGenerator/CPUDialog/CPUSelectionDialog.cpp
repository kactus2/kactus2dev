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

#include <library/LibraryInterface.h>

#include <common/widgets/EnumCollectionEditor/EnumCollectionEditor.h>

#include <editors/MemoryDesigner/MasterSlavePathSearch.h>
#include <editors/MemoryDesigner/ConnectivityConnection.h>
#include <editors/MemoryDesigner/ConnectivityGraph.h>
#include <editors/MemoryDesigner/ConnectivityGraphFactory.h>
#include <editors/MemoryDesigner/ConnectivityInterface.h>
#include <editors/MemoryDesigner/ConnectivityComponent.h>
#include <editors/MemoryDesigner/MemoryItem.h>

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
blockPeripherals_(),
mapPeripherals_(),
cpuDetailEditor_(new SVDCPUEditor(this)),
// folderLine_(new QLineEdit(library->getDirectoryPath(topComponent->getVlnv()), this))
folderLine_(new QLineEdit(this))
{
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

    fileSetBox_->setLayout(filesetLayout);

    blockPeripherals_ = new QCheckBox("Address blocks", this);
    mapPeripherals_ = new QCheckBox("Memory maps", this);
    blockPeripherals_->setChecked(true);
    mapPeripherals_->setChecked(false);

    QVBoxLayout* peripheralLayout(new QVBoxLayout());
    peripheralLayout->addWidget(blockPeripherals_);
    peripheralLayout->addWidget(mapPeripherals_);

    QGroupBox* peripheralBox(new QGroupBox("Create peripherals from:"));
    peripheralBox->setLayout(peripheralLayout);

    QPushButton* openFolderButton(
        new QPushButton(QIcon(":icons/common/graphics/folder-horizontal-open.png"), QString(), this));

    connect(openFolderButton, SIGNAL(clicked()), this, SLOT(onChangeTargetFolder()), Qt::UniqueConnection);

    QHBoxLayout* folderLayout(new QHBoxLayout());
    folderLayout->addWidget(folderLine_, 1);
    folderLayout->addWidget(openFolderButton);

    QGroupBox* folderbox(new QGroupBox("Select destination folder"));
    folderbox->setLayout(folderLayout);

    QVBoxLayout* leftLayout(new QVBoxLayout());
    leftLayout->addLayout(viewLayout);
    leftLayout->addWidget(fileSetBox_);
    leftLayout->addWidget(folderbox);
    leftLayout->addWidget(peripheralBox);
    leftLayout->addStretch(1);

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

    connect(blockPeripherals_, SIGNAL(clicked(bool)), this, SLOT(onBlockPeripherals(bool)), Qt::UniqueConnection);
    connect(mapPeripherals_, SIGNAL(clicked(bool)), this, SLOT(onMapPeripherals(bool)), Qt::UniqueConnection);

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

    QSharedPointer<ConnectivityGraph> graph = graphFactory_.createConnectivityGraph(component_, activeView);
    MasterSlavePathSearch searchAlgorithm;
    QVector<QVector<QSharedPointer<const ConnectivityInterface>>> masterRoutes =
        searchAlgorithm.findMasterSlavePaths(graph);

    for (auto masterRoute : masterRoutes)
    {
        QSharedPointer<const ConnectivityInterface> master = masterRoute.first();
        if (interfacedCpuExists(master))
        {
            QSharedPointer<ConnectivityGraphUtilities::cpuDetailRoutes> checkInterface =
                getMatchingCpuContainer(master);
            checkInterface->routes_.append(masterRoute);
        }
        else
        {
            QSharedPointer<const ConnectivityComponent> routeComponent = master->getInstance();
            QSharedPointer<const Component> interfaceComponent =
                ConnectivityGraphUtilities::getInterfacedComponent(library_, routeComponent);
            if (interfaceComponent)
            {
                QSharedPointer<Cpu> interfaceCPU =
                    ConnectivityGraphUtilities::getInterfacedCPU(interfaceComponent, master);
                if (interfaceCPU)
                {
                    QString checkBoxText = interfaceCPU->name() + " [" + routeComponent->getName() + "]";
                    QSharedPointer<ConnectivityGraphUtilities::cpuDetailRoutes> cpuInterface(
                        new ConnectivityGraphUtilities::cpuDetailRoutes());
                    cpuInterface->cpuName_ = checkBoxText;
                    cpuInterface->cpuID_ = checkBoxText;
                    cpuInterface->cpuInterface_ = master;
                    cpuInterface->routes_.append(masterRoute);

                    cpuInterface->createFile_ = true;
                    cpuInterface->revision_ = "";
                    cpuInterface->endian_ = "";
                    cpuInterface->mpuPresent_ = false;
                    cpuInterface->fpuPresent_ = false;
                    cpuInterface->nvicPrioBits_ = "";
                    cpuInterface->vendorSystickConfig_ = false;

                    cpuSelection_.append(cpuInterface);
                }
            }
        }
    }

    cpuDetailEditor_->setupCPUDetails(cpuSelection_);
}

//-----------------------------------------------------------------------------
// Function: CPUSelectionDialog::interfacedCpuExists()
//-----------------------------------------------------------------------------
bool CPUSelectionDialog::interfacedCpuExists(QSharedPointer<const ConnectivityInterface> master)
{
    for (auto checkInterface : cpuSelection_)
    {
        if (checkInterface->cpuInterface_ == master)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: CPUSelectionDialog::getMatchingCheckInterface()
//-----------------------------------------------------------------------------
QSharedPointer<ConnectivityGraphUtilities::cpuDetailRoutes> CPUSelectionDialog::getMatchingCpuContainer(
    QSharedPointer<const ConnectivityInterface> master)
{
    for (auto checkInterface : cpuSelection_)
    {
        if (checkInterface->cpuInterface_ == master)
        {
            return checkInterface;
        }
    }

    return QSharedPointer<ConnectivityGraphUtilities::cpuDetailRoutes>();
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
// Function: CPUSelectionDialog::onBlockPeripherals()
//-----------------------------------------------------------------------------
void CPUSelectionDialog::onBlockPeripherals(bool blockPeripherals)
{
    mapPeripherals_->setChecked(!blockPeripherals);
}

//-----------------------------------------------------------------------------
// Function: CPUSelectionDialog::onMapPeripherals()
//-----------------------------------------------------------------------------
void CPUSelectionDialog::onMapPeripherals(bool mapPeripherals)
{
    blockPeripherals_->setChecked(!mapPeripherals);
}

//-----------------------------------------------------------------------------
// Function: CPUSelectionDialog::peripheralsAreBlocks()
//-----------------------------------------------------------------------------
bool CPUSelectionDialog::peripheralsAreBlocks() const
{
    return blockPeripherals_->isChecked();
}

//-----------------------------------------------------------------------------
// Function: CPUSelectionDialog::peripheralsAreMaps()
//-----------------------------------------------------------------------------
bool CPUSelectionDialog::peripheralsAreMaps() const
{
    return mapPeripherals_->isChecked();
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
