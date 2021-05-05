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

//-----------------------------------------------------------------------------
// Function: CPUSelectionDialog::CPUSelectionDialog()
//-----------------------------------------------------------------------------
CPUSelectionDialog::CPUSelectionDialog(QSharedPointer<Component> topComponent, LibraryInterface* library,
    QSharedPointer<QList<QSharedPointer<View>>> views, QWidget *parent):
QDialog(parent),
viewSelection_(new QComboBox(this)),
cpuSelection_(),
cpuLayout_(),
library_(library),
component_(topComponent),
graphFactory_(library),
blockPeripherals_(),
mapPeripherals_()
{
    if (!views->isEmpty())
    {
        QStringList viewNames;
        for (auto view : *views)
        {
            viewNames.append(view->name());
        }

        viewSelection_->addItems(viewNames);
        viewSelection_->setCurrentIndex(0);
    }

    setupLayout();

    connect(viewSelection_, SIGNAL(currentIndexChanged(int)), this, SLOT(onViewChanged()), Qt::UniqueConnection);
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
    QVBoxLayout* masterLayout(new QVBoxLayout(this));

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

    QLabel* peripheralLabel(new QLabel("Create peripherals from:"));
    blockPeripherals_ = new QCheckBox("Address blocks", this);
    mapPeripherals_ = new QCheckBox("Memory maps", this);
    blockPeripherals_->setChecked(true);
    mapPeripherals_->setChecked(false);

    QVBoxLayout* peripheralLayout(new QVBoxLayout());
    peripheralLayout->addWidget(peripheralLabel);
    peripheralLayout->addWidget(blockPeripherals_);
    peripheralLayout->addWidget(mapPeripherals_);
    peripheralLayout->addStretch(1);

    QHBoxLayout* middleLayout(new QHBoxLayout());
    middleLayout->addLayout(peripheralLayout);
    middleLayout->addWidget(cpuGroup);

    masterLayout->addLayout(viewLayout);
    masterLayout->addLayout(middleLayout);
    masterLayout->addWidget(dialogButtons);

    connect(dialogButtons, SIGNAL(accepted()), this, SLOT(accept()), Qt::UniqueConnection);
    connect(dialogButtons, SIGNAL(rejected()), this, SLOT(reject()), Qt::UniqueConnection);

    connect(blockPeripherals_, SIGNAL(clicked(bool)), this, SLOT(onBlockPeripherals(bool)), Qt::UniqueConnection);
    connect(mapPeripherals_, SIGNAL(clicked(bool)), this, SLOT(onMapPeripherals(bool)), Qt::UniqueConnection);

    void onMapPeripherals();

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
            QSharedPointer<CPUCheckInterface> checkInterface = getMatchingCheckInterface(master);
            checkInterface->cpuRoutes_.append(masterRoute);
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

                    QSharedPointer<CPUCheckInterface> checkInterface(new CPUCheckInterface());
                    checkInterface->cpuInterface_ = master;
                    checkInterface->cpuCheck_ = checkBox;
                    checkInterface->cpuRoutes_.append(masterRoute);

                    cpuSelection_.append(checkInterface);
                    cpuLayout_->addWidget(checkBox);
                }
            }
        }
    }
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
QSharedPointer<CPUSelectionDialog::CPUCheckInterface> CPUSelectionDialog::getMatchingCheckInterface(
    QSharedPointer<const ConnectivityInterface> master)
{
    for (auto checkInterface : cpuSelection_)
    {
        if (checkInterface->cpuInterface_ == master)
        {
            return checkInterface;
        }
    }

    return QSharedPointer<CPUCheckInterface>();
}

//-----------------------------------------------------------------------------
// Function: CPUSelectionDialog::getSelectedCPUs()
//-----------------------------------------------------------------------------
QVector<ConnectivityGraphUtilities::interfaceRoutes> CPUSelectionDialog::getSelectedCPUs()
{
    QVector<ConnectivityGraphUtilities::interfaceRoutes> interfaceRoutes;

    for (auto cpuCheck : cpuSelection_)
    {
        if (cpuCheck->cpuCheck_->isChecked())
        {
            ConnectivityGraphUtilities::interfaceRoutes newInterfaceContainer;
            newInterfaceContainer.masterInterface_ = cpuCheck->cpuInterface_;
            newInterfaceContainer.routes_ = cpuCheck->cpuRoutes_;

            interfaceRoutes.append(newInterfaceContainer);
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
