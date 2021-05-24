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

#include <QDialogButtonBox>
#include <QPushButton>
#include <QFormLayout>

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
cpuLayout_(),
library_(library),
component_(topComponent),
graphFactory_(library),
blockPeripherals_(),
mapPeripherals_(),
cpuDetailSelector_(new QComboBox(this)),
revisionEditor_(new QLineEdit(this)),
endianEditor_(new QComboBox(this)),
nvicPrioBitsEditor_(new QLineEdit(this)),
mpuPresentCheckBox_(new QCheckBox("MPU presence", this)),
fpuPresentCheckBox_(new QCheckBox("FPU presence", this)),
vendorSystickConfigCheckBox_(new QCheckBox("Vendor-specific System Tick Timer", this))
{
    viewSelection_->addItems(viewNames);
    viewSelection_->setCurrentIndex(0);

    fileSetSelection_->addItem("");
    fileSetSelection_->addItems(fileSetNames);
    fileSetSelection_->setEditable(true);

    fileSetBox_->setCheckable(true);

    endianEditor_->addItem("little");
    endianEditor_->addItem("big");
    endianEditor_->addItem("selectable");
    endianEditor_->addItem("other");

    setupLayout();

    connect(viewSelection_, SIGNAL(currentIndexChanged(int)), this, SLOT(onViewChanged()), Qt::UniqueConnection);
    connect(cpuDetailSelector_, SIGNAL(currentTextChanged(const QString&)),
        this, SLOT(loadCPUDetails(QString const&)), Qt::UniqueConnection);
    connect(revisionEditor_, SIGNAL(editingFinished()), this, SLOT(onRevisionChanged()), Qt::UniqueConnection);
    connect(endianEditor_, SIGNAL(currentTextChanged(const QString)),
        this, SLOT(onEndianChanged(QString const&)), Qt::UniqueConnection);
    connect(mpuPresentCheckBox_, SIGNAL(clicked(bool)),
        this, SLOT(onMpuPresenceChanged(bool)), Qt::UniqueConnection);
    connect(fpuPresentCheckBox_, SIGNAL(clicked(bool)),
        this, SLOT(onFpuPresenceChanged(bool)), Qt::UniqueConnection);
    connect(nvicPrioBitsEditor_, SIGNAL(editingFinished()),
        this, SLOT(onNvicPrioBitsChanged()), Qt::UniqueConnection);
    connect(vendorSystickConfigCheckBox_, SIGNAL(clicked(bool)),
        this, SLOT(onVendorSystickConfigChanged(bool)), Qt::UniqueConnection);
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
    cpuLayout_ = new QVBoxLayout();

    QGroupBox* cpuGroup(new QGroupBox("Selected CPUs:"));
    cpuGroup->setLayout(cpuLayout_);

    QDialogButtonBox* dialogButtons = new QDialogButtonBox(QDialogButtonBox::Cancel,
        Qt::Horizontal);
    QPushButton* writeButton = dialogButtons->addButton("Ok", QDialogButtonBox::AcceptRole);
    writeButton->setDefault(false);
    writeButton->setAutoDefault(false);

    QLabel* viewLabel(new QLabel("Select configuration:"));

    QHBoxLayout* viewLayout(new QHBoxLayout());
    viewLayout->addWidget(viewLabel);
    viewLayout->addWidget(viewSelection_, 1);

    blockPeripherals_ = new QCheckBox("Address blocks", this);
    mapPeripherals_ = new QCheckBox("Memory maps", this);
    blockPeripherals_->setChecked(true);
    mapPeripherals_->setChecked(false);

    QVBoxLayout* peripheralLayout(new QVBoxLayout());
    peripheralLayout->addWidget(blockPeripherals_);
    peripheralLayout->addWidget(mapPeripherals_);

    QGroupBox* peripheralBox(new QGroupBox("Create peripherals from:"));
    peripheralBox->setLayout(peripheralLayout);

    QVBoxLayout* filesetLayout(new QVBoxLayout());
    filesetLayout->addWidget(fileSetSelection_);

    fileSetBox_->setLayout(filesetLayout);

    QVBoxLayout* leftLayout(new QVBoxLayout());
    leftLayout->addWidget(peripheralBox);
    leftLayout->addWidget(fileSetBox_);

    QHBoxLayout* middleLayout(new QHBoxLayout());
    middleLayout->addLayout(leftLayout);
    middleLayout->addWidget(cpuGroup);

    connect(dialogButtons, SIGNAL(accepted()), this, SLOT(accept()), Qt::UniqueConnection);
    connect(dialogButtons, SIGNAL(rejected()), this, SLOT(reject()), Qt::UniqueConnection);

    connect(blockPeripherals_, SIGNAL(clicked(bool)), this, SLOT(onBlockPeripherals(bool)), Qt::UniqueConnection);
    connect(mapPeripherals_, SIGNAL(clicked(bool)), this, SLOT(onMapPeripherals(bool)), Qt::UniqueConnection);

    QVBoxLayout* leftTopLayout(new QVBoxLayout());
    leftTopLayout->addLayout(viewLayout);
    leftTopLayout->addLayout(middleLayout);

    QFormLayout* cpuDetailsLayout(new QFormLayout());
    cpuDetailsLayout->addRow(cpuDetailSelector_);
    cpuDetailsLayout->addRow("Revision:", revisionEditor_);
    cpuDetailsLayout->addRow("NVIC bits:", nvicPrioBitsEditor_);
    cpuDetailsLayout->addRow("Endian:", endianEditor_);
    cpuDetailsLayout->addRow(mpuPresentCheckBox_);
    cpuDetailsLayout->addRow(fpuPresentCheckBox_);
    cpuDetailsLayout->addRow(vendorSystickConfigCheckBox_);

    QGroupBox* cpuDetailsBox(new QGroupBox("CPU details"));
    cpuDetailsBox->setLayout(cpuDetailsLayout);

    QHBoxLayout* topLayout(new QHBoxLayout());
    topLayout->addLayout(leftTopLayout, 1);
    topLayout->addWidget(cpuDetailsBox);

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
    for (auto cpuInterface : cpuSelection_)
    {
        cpuLayout_->removeWidget(cpuInterface->cpuCheck_);
    }

    cpuSelection_.clear();
    cpuDetailSelector_->clear();

    QString activeView = viewSelection_->currentText();

    QSharedPointer<ConnectivityGraph> graph = graphFactory_.createConnectivityGraph(component_, activeView);
    MasterSlavePathSearch searchAlgorithm;
    QVector<QVector<QSharedPointer<const ConnectivityInterface>>> masterRoutes =
        searchAlgorithm.findMasterSlavePaths(graph);

    for (auto masterRoute : masterRoutes)
    {
        QSharedPointer<const ConnectivityInterface> master = masterRoute.first();
        if (checkBoxExistsForInterface(master))
        {
            QSharedPointer<ConnectivityGraphUtilities::cpuCheckInterface> checkInterface =
                getMatchingCheckInterface(master);
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
                    QCheckBox* checkBox = new QCheckBox(checkBoxText, this);
                    checkBox->setChecked(true);

                    QSharedPointer<ConnectivityGraphUtilities::cpuCheckInterface> checkInterface(
                        new ConnectivityGraphUtilities::cpuCheckInterface());
                    checkInterface->cpuID_ = checkBoxText;
                    checkInterface->cpuInterface_ = master;
                    checkInterface->cpuCheck_ = checkBox;
                    checkInterface->routes_.append(masterRoute);

                    checkInterface->revision_ = "";
                    checkInterface->endian_ = "other";
                    checkInterface->mpuPresent_ = false;
                    checkInterface->fpuPresent_ = false;
                    checkInterface->nvicPrioBits_ = "";
                    checkInterface->vendorSystickConfig_ = false; 

                    cpuSelection_.append(checkInterface);
                    cpuLayout_->addWidget(checkBox);

                    cpuDetailSelector_->addItem(checkBoxText);
                }
            }
        }
    }

    if (cpuDetailSelector_->count() > 0)
    {
        cpuDetailSelector_->setCurrentIndex(0);
        loadCPUDetails(cpuDetailSelector_->currentText());
    }

    cpuLayout_->addStretch(1);
}

//-----------------------------------------------------------------------------
// Function: CPUSelectionDialog::checkBoxExistsForInterface()
//-----------------------------------------------------------------------------
bool CPUSelectionDialog::checkBoxExistsForInterface(QSharedPointer<const ConnectivityInterface> master)
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
QSharedPointer<ConnectivityGraphUtilities::cpuCheckInterface> CPUSelectionDialog::getMatchingCheckInterface(
    QSharedPointer<const ConnectivityInterface> master)
{
    for (auto checkInterface : cpuSelection_)
    {
        if (checkInterface->cpuInterface_ == master)
        {
            return checkInterface;
        }
    }

    return QSharedPointer<ConnectivityGraphUtilities::cpuCheckInterface>();
}

//-----------------------------------------------------------------------------
// Function: CPUSelectionDialog::getSelectedCPUs()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<ConnectivityGraphUtilities::cpuCheckInterface> > CPUSelectionDialog::getSelectedCPUs()
{
    QVector<QSharedPointer<ConnectivityGraphUtilities::cpuCheckInterface> > interfaceRoutes;

    for (auto cpuCheck : cpuSelection_)
    {
        if (cpuCheck->cpuCheck_->isChecked())
        {
            interfaceRoutes.append(cpuCheck);
        }
    }

    return interfaceRoutes;
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
// Function: CPUSelectionDialog::loadCPUDetails()
//-----------------------------------------------------------------------------
void CPUSelectionDialog::loadCPUDetails(QString const& selectedCPU)
{
    QSharedPointer<ConnectivityGraphUtilities::cpuCheckInterface> cpuCheck = getCheckCpuByID(selectedCPU);
    if (!cpuCheck)
    {
        return;
    }

    blockCpuDetailEditors(true);

    revisionEditor_->setText(cpuCheck->revision_);

    endianEditor_->setCurrentText(cpuCheck->endian_);
    mpuPresentCheckBox_->setChecked(cpuCheck->mpuPresent_);
    fpuPresentCheckBox_->setChecked(cpuCheck->fpuPresent_);
    nvicPrioBitsEditor_->setText(cpuCheck->nvicPrioBits_);
    vendorSystickConfigCheckBox_->setChecked(cpuCheck->vendorSystickConfig_);

    blockCpuDetailEditors(false);
}

//-----------------------------------------------------------------------------
// Function: CPUSelectionDialog::getCheckCpuByID()
//-----------------------------------------------------------------------------
QSharedPointer<ConnectivityGraphUtilities::cpuCheckInterface> CPUSelectionDialog::getCheckCpuByID(
    QString const& cpuID) const
{
    for (auto cpuCheck : cpuSelection_)
    {
        if (cpuCheck->cpuID_.compare(cpuID) == 0)
        {
            return cpuCheck;
        }
    }

    return QSharedPointer<ConnectivityGraphUtilities::cpuCheckInterface>();
}

//-----------------------------------------------------------------------------
// Function: CPUSelectionDialog::blockCpuDetailEditors()
//-----------------------------------------------------------------------------
void CPUSelectionDialog::blockCpuDetailEditors(bool blockStatus)
{
    revisionEditor_->blockSignals(blockStatus);
    endianEditor_->blockSignals(blockStatus);
    mpuPresentCheckBox_->blockSignals(blockStatus);
    fpuPresentCheckBox_->blockSignals(blockStatus);
    nvicPrioBitsEditor_->blockSignals(blockStatus);
    vendorSystickConfigCheckBox_->blockSignals(blockStatus);
}

//-----------------------------------------------------------------------------
// Function: CPUSelectionDialog::onRevisionChanged()
//-----------------------------------------------------------------------------
void CPUSelectionDialog::onRevisionChanged()
{
    QSharedPointer<ConnectivityGraphUtilities::cpuCheckInterface> cpuCheck =
        getCheckCpuByID(cpuDetailSelector_->currentText());

    if (cpuCheck)
    {
        cpuCheck->revision_ = revisionEditor_->text();
    }
}

//-----------------------------------------------------------------------------
// Function: CPUSelectionDialog::onEndianChanged()
//-----------------------------------------------------------------------------
void CPUSelectionDialog::onEndianChanged(QString const& newEndian)
{
    QSharedPointer<ConnectivityGraphUtilities::cpuCheckInterface> cpuCheck =
        getCheckCpuByID(cpuDetailSelector_->currentText());

    if (cpuCheck)
    {
        cpuCheck->endian_ = newEndian;
    }
}

//-----------------------------------------------------------------------------
// Function: CPUSelectionDialog::onMpuPresenceChanged()
//-----------------------------------------------------------------------------
void CPUSelectionDialog::onMpuPresenceChanged(bool newState)
{
    QSharedPointer<ConnectivityGraphUtilities::cpuCheckInterface> cpuCheck =
        getCheckCpuByID(cpuDetailSelector_->currentText());

    if (cpuCheck)
    {
        cpuCheck->mpuPresent_ = mpuPresentCheckBox_->isChecked();
    }
}

//-----------------------------------------------------------------------------
// Function: CPUSelectionDialog::onFpuPresenceChanged()
//-----------------------------------------------------------------------------
void CPUSelectionDialog::onFpuPresenceChanged(bool newState)
{
    QSharedPointer<ConnectivityGraphUtilities::cpuCheckInterface> cpuCheck =
        getCheckCpuByID(cpuDetailSelector_->currentText());

    if (cpuCheck)
    {
        cpuCheck->fpuPresent_ = fpuPresentCheckBox_->isChecked();
    }
}

//-----------------------------------------------------------------------------
// Function: CPUSelectionDialog::onNvicPrioBitsChanged()
//-----------------------------------------------------------------------------
void CPUSelectionDialog::onNvicPrioBitsChanged()
{
    QSharedPointer<ConnectivityGraphUtilities::cpuCheckInterface> cpuCheck =
        getCheckCpuByID(cpuDetailSelector_->currentText());

    if (cpuCheck)
    {
        cpuCheck->nvicPrioBits_ = nvicPrioBitsEditor_->text();
    }
}

//-----------------------------------------------------------------------------
// Function: CPUSelectionDialog::onVendorSystickConfigChanged()
//-----------------------------------------------------------------------------
void CPUSelectionDialog::onVendorSystickConfigChanged(bool newState)
{
    QSharedPointer<ConnectivityGraphUtilities::cpuCheckInterface> cpuCheck =
        getCheckCpuByID(cpuDetailSelector_->currentText());

    if (cpuCheck)
    {
        cpuCheck->vendorSystickConfig_ = vendorSystickConfigCheckBox_->isChecked();
    }
}
