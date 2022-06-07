//-----------------------------------------------------------------------------
// File: BusInterfacePortMapTab.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 23.05.2016
//
// Description:
// Tab for editing and viewing bus interface port maps.
//-----------------------------------------------------------------------------

#include "BusInterfacePortMapTab.h"

#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>
#include <editors/ComponentEditor/busInterfaces/interfaces/BusInterfaceInterface.h>
#include <editors/ComponentEditor/busInterfaces/interfaces/AbstractionTypeInterface.h>
#include <editors/ComponentEditor/busInterfaces/portmaps/PortMapDelegate.h>
#include <editors/ComponentEditor/busInterfaces/portmaps/PortMapTreeSortProxyModel.h>
#include <editors/ComponentEditor/busInterfaces/portmaps/PortMapHeaderView.h>
#include <editors/ComponentEditor/busInterfaces/portmaps/PortMapsColumns.h>
#include <editors/ComponentEditor/busInterfaces/portmaps/interfaces/PortMapInterface.h>

#include <library/LibraryInterface.h>

#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/PortMap.h>
#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>

#include <QSplitter>
#include <QSharedPointer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QFormLayout>

//-----------------------------------------------------------------------------
// Function: BusInterfacePortMapTab::BusInterfacePortMapTab()
//-----------------------------------------------------------------------------
BusInterfacePortMapTab::BusInterfacePortMapTab(LibraryInterface* libHandler, QSharedPointer<Component> component,
    BusInterfaceInterface* busInterface, std::string const& busName,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ParameterFinder> finder,
    PortMapInterface* portMapInterface, QWidget* parent):
QWidget(parent),
busInterface_(busInterface),
busName_(busName),
component_(component),
libHandler_(libHandler),
physicalPortView_(this),
physicalPortModel_(portMapInterface->getPhysicalPortInterface(), this),
physicalPortSorter_(0, portMapInterface, busInterface->getAbstractionTypeInterface(), this),
nameFilterEditor_(new QLineEdit(this)),
typeFilter_(this),
directionFilter_(this),
hideConnectedPortsBox_(this),
physicalPrefixEditor_(new QLineEdit(this)),
portMapModel_(finder, portMapInterface, this),
portMapSorter_(portMapInterface, this),
portMapView_(portMapInterface, this),
portMapDelegate_(0),
autoConnectButton_(QIcon(":/icons/common/graphics/connect.png"), "Auto connect all", this),
removeAllMappingsButton_(QIcon(":/icons/common/graphics/cross.png"), "Remove all", this),
autoConnector_(component, expressionParser, portMapInterface, libHandler, this),
abstractionSelector_(new QComboBox(this))
{
    hideConnectedPortsBox_.setCheckState(Qt::Checked);

    physicalPortSorter_.setSourceModel(&physicalPortModel_);
    physicalPortView_.setModel(&physicalPortSorter_);

    portMapSorter_.setSourceModel(&portMapModel_);
    portMapView_.setModel(&portMapSorter_);
    portMapView_.sortByColumn(PortMapsColumns::LOGICAL_PORT, Qt::AscendingOrder);

    ComponentParameterModel* componentParametersModel = new ComponentParameterModel(finder, this);
    componentParametersModel->setExpressionParser(expressionParser);

    ParameterCompleter* parameterCompleter = new ParameterCompleter(this);
    parameterCompleter->setModel(componentParametersModel);

    QStringList portNames = component_->getPortNames();

    portMapDelegate_ = new PortMapDelegate(parameterCompleter, finder, portMapInterface, this);

    connect(portMapDelegate_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(portMapDelegate_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    portMapView_.setItemDelegate(portMapDelegate_);
    portMapView_.resizeColumnsToContents();

	setupLayout();

    nameFilterEditor_->setToolTip(tr("Filter ports by name"));
    setTabOrder(nameFilterEditor_, &physicalPortView_);

    setupTypeFilter();
    addItemsToDirectionFilter();

    connectItems();
}

//-----------------------------------------------------------------------------
// Function: BusInterfacePortMapTab::setupTypeFilter()
//-----------------------------------------------------------------------------
void BusInterfacePortMapTab::setupTypeFilter()
{
    typeFilter_.addItem(PortTypes::WIRETYPE);
    typeFilter_.addItem(PortTypes::TRANSACTIONALTYPE);
}

//-----------------------------------------------------------------------------
// Function: BusInterfacePortMapTab::addItemsToDirectionFilter()
//-----------------------------------------------------------------------------
void BusInterfacePortMapTab::addItemsToDirectionFilter()
{
    QStringList directions;
    directions << "In" << "Out" << "InOut";

    directionFilter_.addItem("");
    directionFilter_.addItems(directions);

    for (int directionIndex = 0; directionIndex < directions.size(); directionIndex++)
    {
        QString portDirection = directions.at(directionIndex);
        QString iconPath = ":icons/common/graphics/cross.png";

        if (portDirection.compare("In", Qt::CaseInsensitive) == 0)
        {
            iconPath = ":icons/common/graphics/input.png";
        }
        else if (portDirection.compare("Out", Qt::CaseInsensitive) == 0)
        {
            iconPath = ":icons/common/graphics/output.png";
        }
        else if (portDirection.compare("Inout", Qt::CaseInsensitive) == 0)
        {
            iconPath = ":icons/common/graphics/inout.png";
        }

        QIcon directionIcon(iconPath);
        int itemIndex = directionFilter_.findText(portDirection);

        directionFilter_.setItemIcon(itemIndex, directionIcon);
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfacePortMapTab::refresh()
//-----------------------------------------------------------------------------
void BusInterfacePortMapTab::refresh()
{
    setupAbstraction();

    busInterface_->setupSubInterfaces(busName_);

    physicalPortModel_.refresh();
    portMapModel_.refresh();

    physicalPortSorter_.onConnectionsReset();
}

//-----------------------------------------------------------------------------
// Function: BusInterfacePortMapTab::setAbstractionDefinitions()
//-----------------------------------------------------------------------------
void BusInterfacePortMapTab::setAbstractionDefinitions()
{
    QString currentAbstraction = abstractionSelector_->currentText();

    abstractionSelector_->clear();

    busInterface_->setupSubInterfaces(busName_);

    AbstractionTypeInterface* abstractionInterface = busInterface_->getAbstractionTypeInterface();
    int abstractionCount = abstractionInterface->itemCount();
    if (abstractionCount > 0)
    {
        int currentIndex = 0;
        for (int i = 0; i < abstractionCount; ++i)
        {
            if (abstractionInterface->hasAbstractionReference(i))
            {
                QString abstractionReference =
                    QString::fromStdString(abstractionInterface->getIndexedAbstraction(i));

                abstractionSelector_->addItem(abstractionReference);

                if (!currentAbstraction.isEmpty() && abstractionReference == currentAbstraction)
                {
                    currentIndex = i;
                }
            }
        }

        if (abstractionCount > 0)
        {
            abstractionSelector_->setCurrentIndex(currentIndex);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfacePortMapTab::onAbstractionChanged()
//-----------------------------------------------------------------------------
void BusInterfacePortMapTab::onAbstractionChanged()
{
    setupAbstraction();

    refresh();
}

//-----------------------------------------------------------------------------
// Function: BusInterfacePortMapTab::setupAbstraction()
//-----------------------------------------------------------------------------
void BusInterfacePortMapTab::setupAbstraction()
{
    int currentAbstractionIndex = abstractionSelector_->currentIndex();
    setAbsType(currentAbstractionIndex);
}

//-----------------------------------------------------------------------------
// Function: BusInterfacePortMapTab::setAbsType()
//-----------------------------------------------------------------------------
void BusInterfacePortMapTab::setAbsType(int const& abstractionIndex)
{
    AbstractionTypeInterface* abstractionInterface = busInterface_->getAbstractionTypeInterface();
    General::InterfaceMode busMode = busInterface_->getMode(busName_);
    QString systemGroup = QString::fromStdString(busInterface_->getSystemGroup(busName_));

    if (busMode == General::MONITOR)
    {
        General::InterfaceMode monitorMode = busInterface_->getMonitorMode(busName_);
        if (monitorMode != General::INTERFACE_MODE_COUNT)
        {
            busMode = monitorMode;
            systemGroup = QString::fromStdString(busInterface_->getMonitorGroup(busName_));
        }
    }

    VLNV definitionVLNV;
    if (abstractionInterface->hasAbstractionReference(abstractionIndex))
    {
        definitionVLNV = *abstractionInterface->getAbstractionReference(abstractionIndex).data();
    }

    physicalPortSorter_.setNewAbstractionType(abstractionIndex);

    QSharedPointer<Document const> absDefDocument = libHandler_->getModelReadOnly(definitionVLNV);
    if (absDefDocument && libHandler_->getDocumentType(definitionVLNV) == VLNV::ABSTRACTIONDEFINITION)
    {
        QSharedPointer<AbstractionDefinition const> abstractionDefinition =
            absDefDocument.dynamicCast<AbstractionDefinition const>();

        PortMapInterface* portMapInterface = abstractionInterface->getPortMapInterface();
        portMapInterface->setupAbstractionDefinition(abstractionDefinition);
        portMapInterface->setupPortMaps(abstractionInterface->getAbstraction(abstractionIndex));
        portMapInterface->setupBusMode(busMode);
        portMapInterface->setupSystemGroup(systemGroup);
        portMapInterface->setupPhysicalPorts(component_);

        portMapDelegate_->setBusMode(busMode);
        portMapDelegate_->setSystemGroup(systemGroup);

        autoConnector_.setAbstractionDefinition(definitionVLNV, busMode, systemGroup);
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfacePortMapTab::setPhysicalPorts()
//-----------------------------------------------------------------------------
void BusInterfacePortMapTab::setPhysicalPorts(QStringList const& ports)
{
    QString filteredPorts = ports.join('|');

    nameFilterEditor_->setText(filteredPorts);

    refresh();
}

//-----------------------------------------------------------------------------
// Function: BusInterfacePortMapTab::setupLayout()
//-----------------------------------------------------------------------------
void BusInterfacePortMapTab::setupLayout()
{
    QFormLayout* filterLayout = new QFormLayout(this);
    filterLayout->addRow("Name:", nameFilterEditor_);
    filterLayout->addRow("Type:", &typeFilter_);
    filterLayout->addRow("Direction:", &directionFilter_);
    filterLayout->addRow("Hide connected:", &hideConnectedPortsBox_);

    QGroupBox* filterBox = new QGroupBox(tr("Physical port filters"), this);
    filterBox->setLayout(filterLayout);

    QGroupBox* autoconnectBox = new QGroupBox(tr("Auto connect options"), this);
    QGridLayout* functionLayout = new QGridLayout(autoconnectBox);
    functionLayout->addWidget(new QLabel(tr("Physical port prefix:"), this), 0, 0, 1, 1);
    functionLayout->addWidget(physicalPrefixEditor_, 0, 1, 1, 1);
    functionLayout->addWidget(&autoConnectButton_, 1, 1, 1, 1);

    QVBoxLayout* filterAndFunctionLayout = new QVBoxLayout();
    filterAndFunctionLayout->addWidget(filterBox);
    filterAndFunctionLayout->addWidget(autoconnectBox);

    QVBoxLayout* physicalPortLayout = new QVBoxLayout();
    physicalPortLayout->addWidget(&physicalPortView_);

    QGroupBox* physicalGroupBox = new QGroupBox(tr("Physical ports"), this);
    physicalGroupBox->setLayout(physicalPortLayout);

    QWidget* topWidget = new QWidget(this);
    QHBoxLayout* topLayout = new QHBoxLayout(topWidget);
    topLayout->addWidget(physicalGroupBox, 1);
    topLayout->addLayout(filterAndFunctionLayout);

    QLabel* abstractionLabel(new QLabel(tr("Abstraction definition: "), this));
    QHBoxLayout* abstractionLayout = new QHBoxLayout();
    abstractionLayout->addWidget(abstractionLabel, 0, Qt::AlignLeft);
    abstractionLayout->addWidget(abstractionSelector_, 0, Qt::AlignLeft);
    abstractionLayout->addStretch(5);

    QVBoxLayout* portMapsLayout = new QVBoxLayout();
    portMapsLayout->addLayout(abstractionLayout, 0);
    portMapsLayout->addWidget(&portMapView_);
    portMapsLayout->addWidget(&removeAllMappingsButton_, 0, Qt::AlignLeft);

    QGroupBox* portMapsGroupBox = new QGroupBox(tr("Port Maps"), this);
    portMapsGroupBox->setLayout(portMapsLayout);

    QVBoxLayout* masterLayout = new QVBoxLayout(this);

    QSplitter* splitter = new QSplitter(Qt::Vertical, this);    
    splitter->addWidget(topWidget);
    splitter->addWidget(portMapsGroupBox);
    splitter->setStretchFactor(1, 1);

    masterLayout->addWidget(splitter);
}

//-----------------------------------------------------------------------------
// Function: BusInterfacePortMapTab::connectItems()
//-----------------------------------------------------------------------------
void BusInterfacePortMapTab::connectItems()
{
    // Connect signals from physical port filters.
    connect(nameFilterEditor_, SIGNAL(textChanged(const QString&)), 
        &physicalPortSorter_, SLOT(setFilterPortName(const QString&)), Qt::UniqueConnection);
    connect(nameFilterEditor_, SIGNAL(textChanged(const QString&)), 
        &physicalPortView_, SLOT(onFilterNameChanged(const QString&)), Qt::UniqueConnection);

    connect(&typeFilter_, SIGNAL(currentIndexChanged(QString const&)),
        &physicalPortSorter_, SLOT(onChangeFilteredType(QString const&)), Qt::UniqueConnection);

    connect(&directionFilter_, SIGNAL(currentIndexChanged(QString const&)),
        this, SLOT(onDirectionFilterChanged(QString const&)), Qt::UniqueConnection);

    connect(&portMapModel_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&portMapModel_, SIGNAL(portConnected(QString const&)),
        this, SLOT(onPortConnected(QString const&)), Qt::UniqueConnection);
    connect(&portMapModel_, SIGNAL(portDisconnected(QString const&)),
        this, SLOT(onPortDisconnected(QString const&)), Qt::UniqueConnection);

    connect(&portMapView_, SIGNAL(addItem(const QModelIndex&)),
        &portMapModel_, SLOT(onAddPortMap(const QModelIndex&)), Qt::UniqueConnection);

    connect(&portMapView_, SIGNAL(removeItem(const QModelIndex&)),
        &portMapModel_, SLOT(onRemovePortMap(QModelIndex const&)), Qt::UniqueConnection);

    connect(&hideConnectedPortsBox_, SIGNAL(toggled(bool)),
        &physicalPortSorter_, SLOT(setFilterHideConnected(bool)), Qt::UniqueConnection);

    connect(&removeAllMappingsButton_, SIGNAL(clicked(bool)),
        &portMapModel_, SLOT(onRemoveAllPortMappings()), Qt::UniqueConnection);

    connect(physicalPrefixEditor_, SIGNAL(textChanged(QString const&)), 
        &autoConnector_, SLOT(setPrefix(QString const&)), Qt::UniqueConnection);
    connect(&autoConnectButton_, SIGNAL(clicked()), &autoConnector_, SLOT(onAutoConnect()), Qt::UniqueConnection);

    connect(&autoConnector_, SIGNAL(portMapsAutoConnected(QVector<QString>)),
        &portMapModel_, SLOT(onAddAutoConnectedPortMaps(QVector<QString>)), Qt::UniqueConnection);
    connect(&portMapView_, SIGNAL(autoConnecteLogicalSignals(QStringList const&)),
        &autoConnector_, SLOT(onAutoConnectLogicalSignals(QStringList const&)), Qt::UniqueConnection);

    connect(abstractionSelector_, SIGNAL(currentIndexChanged(int)),
        this, SLOT(onAbstractionChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: BusInterfacePortMapTab::onPortConnected()
//-----------------------------------------------------------------------------
void BusInterfacePortMapTab::onPortConnected(QString const& portName)
{
    physicalPortSorter_.onPortConnected(portName);
}

//-----------------------------------------------------------------------------
// Function: BusInterfacePortMapTab::onPortDisconnected()
//-----------------------------------------------------------------------------
void BusInterfacePortMapTab::onPortDisconnected(QString const& portName)
{
/*
    foreach (QSharedPointer<BusInterface> busInterface, *component_->getBusInterfaces())
    {
        foreach (QSharedPointer<PortMap> interfacePortMap, *busInterface->getAllPortMaps())
        {
            if (interfacePortMap->getPhysicalPort())
            {
                if (interfacePortMap->getPhysicalPort()->name_ == portName)
                {
                    return;
                }
            }
        }
    }
*/

    physicalPortSorter_.onPortDisconnected(portName);
}

//-----------------------------------------------------------------------------
// Function: BusInterfacePortMapTab::onDirectionFilterChanged()
//-----------------------------------------------------------------------------
void BusInterfacePortMapTab::onDirectionFilterChanged(QString const& newDirection)
{
    if (newDirection.compare("In", Qt::CaseInsensitive) == 0)
    {
        physicalPortSorter_.setFilterInDirection(true);
        physicalPortSorter_.setFilterOutDirection(false);
    }
    else if (newDirection.compare("Out", Qt::CaseInsensitive) == 0)
    {
        physicalPortSorter_.setFilterInDirection(false);
        physicalPortSorter_.setFilterOutDirection(true);
    }
    else if (newDirection.compare("InOut", Qt::CaseInsensitive) == 0)
    {
        physicalPortSorter_.setFilterInDirection(true);
        physicalPortSorter_.setFilterOutDirection(true);
    }
    else
    {
        physicalPortSorter_.setFilterInDirection(false);
        physicalPortSorter_.setFilterOutDirection(false);
    }
}

//-----------------------------------------------------------------------------
// Function: BusIfPortmapTab::focusInEvent()
//-----------------------------------------------------------------------------
void BusInterfacePortMapTab::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    emit helpUrlRequested("componenteditor/portmaps.html");
}

//-----------------------------------------------------------------------------
// Function: BusInterfacePortMapTab::changeBusName()
//-----------------------------------------------------------------------------
void BusInterfacePortMapTab::changeBusName(std::string const& newName)
{
    busName_ = newName;
}
