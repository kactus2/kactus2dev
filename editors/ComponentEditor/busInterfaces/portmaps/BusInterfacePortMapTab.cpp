//-----------------------------------------------------------------------------
// File: BusInterfacePortMapTab.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.05.2016
//
// Description:
// Tab for editing and viewing bus interface port maps.
//-----------------------------------------------------------------------------

#include "BusInterfacePortMapTab.h"

#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>
#include <editors/ComponentEditor/busInterfaces/portmaps/PortMapTreeDelegate.h>
#include <editors/ComponentEditor/busInterfaces/portmaps/PortMapTreeSortProxyModel.h>
#include <editors/ComponentEditor/busInterfaces/portmaps/PortMapHeaderView.h>
#include <editors/ComponentEditor/busInterfaces/portmaps/PortMapsColumns.h>

#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/PortMap.h>

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
    QSharedPointer<BusInterface> busif, QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ExpressionFormatter> formatter, QSharedPointer<ParameterFinder> finder,
    QSharedPointer<PortMapValidator> portMapValidator, QWidget* parent):
QWidget(parent),
    busif_(busif),
    component_(component),
    libHandler_(libHandler),
    physicalPortView_(this),
    physicalPortModel_(component_, expressionParser, this),
    physicalPortSorter_(component_, this),
    nameFilterEditor_(new QLineEdit(this)),
    directionFilter_(this),
    hideConnectedPortsBox_(this),
    portMapsModel_(busif, component, libHandler, expressionParser, formatter, finder, portMapValidator, this),
    portMapsView_(this),
    portMapsDelegate_(0),
    autoConnectButton_(QIcon(":/icons/common/graphics/connect.png"), "Auto connect all", this),
    removeAllMappingsButton_(QIcon(":/icons/common/graphics/cleanup.png"), "Remove all", this),
    autoConnector_(component, busif, expressionParser, libHandler, this)
{
    hideConnectedPortsBox_.setCheckState(Qt::Checked);

    physicalPortSorter_.setSourceModel(&physicalPortModel_);
    physicalPortView_.setModel(&physicalPortSorter_);

    PortMapTreeSortProxyModel* portMapsSorter (new PortMapTreeSortProxyModel(busif_, this));
    portMapsSorter->setSourceModel(&portMapsModel_);
    portMapsView_.setModel(portMapsSorter);

    ComponentParameterModel* componentParametersModel = new ComponentParameterModel(finder, this);
    componentParametersModel->setExpressionParser(expressionParser);

    ParameterCompleter* parameterCompleter = new ParameterCompleter(this);
    parameterCompleter->setModel(componentParametersModel);

    QStringList portNames = component_->getPortNames();

    portMapsDelegate_ =
        new PortMapTreeDelegate(component_, parameterCompleter, finder, portNames, libHandler_, this);

    connect(portMapsDelegate_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(portMapsDelegate_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    PortMapHeaderView* portMapHorizontalHeader = new PortMapHeaderView(Qt::Horizontal, this);
    portMapHorizontalHeader->setStretchLastSection(true);
    portMapsView_.setHeader(portMapHorizontalHeader);
    portMapsView_.setItemDelegate(portMapsDelegate_);

    for (int i = 0; i < PortMapsColumns::COLUMN_COUNT; ++i)
    {
        portMapsView_.resizeColumnToContents(i);
    }

	setupLayout();

    nameFilterEditor_->setToolTip(tr("Filter ports by name"));
    setTabOrder(nameFilterEditor_, &physicalPortView_);

    addItemsToDirectionFilter();

    connectItems();
}

//-----------------------------------------------------------------------------
// Function: BusInterfacePortMapTab::~BusInterfacePortMapTab()
//-----------------------------------------------------------------------------
BusInterfacePortMapTab::~BusInterfacePortMapTab()
{
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
            iconPath = ":icons/common/graphics/control-180.png";
        }
        else if (portDirection.compare("Out", Qt::CaseInsensitive) == 0)
        {
            iconPath = ":icons/common/graphics/control.png";
        }
        else if (portDirection.compare("Inout", Qt::CaseInsensitive) == 0)
        {
            iconPath = ":icons/common/graphics/control-dual.png";
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
    physicalPortModel_.refresh();
    portMapsModel_.refresh();

    physicalPortSorter_.onConnectionsReset();
}

//-----------------------------------------------------------------------------
// Function: BusInterfacePortMapTab::setAbsType()
//-----------------------------------------------------------------------------
void BusInterfacePortMapTab::setAbsType(const VLNV& vlnv, General::InterfaceMode mode)
{
    portMapsModel_.setAbsType(vlnv, mode);
    portMapsDelegate_->updateLogicalPortNames(vlnv, mode);
    autoConnector_.setAbstractionDefinition(vlnv, mode);

    refresh();
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
    filterLayout->addRow("Port name:", nameFilterEditor_);
    filterLayout->addRow("Direction:", &directionFilter_);
    filterLayout->addRow("Hide connected ports:", &hideConnectedPortsBox_);

    QGroupBox* filterBox = new QGroupBox(tr("Filters"), this);
    filterBox->setLayout(filterLayout);

    QHBoxLayout* functionLayout = new QHBoxLayout();
    functionLayout->addWidget(&autoConnectButton_);
    functionLayout->addWidget(&removeAllMappingsButton_);

    QVBoxLayout* filterAndFunctionLayout = new QVBoxLayout();
    filterAndFunctionLayout->addWidget(filterBox);
    filterAndFunctionLayout->addStretch(1);
    filterAndFunctionLayout->addLayout(functionLayout);

    QVBoxLayout* physicalPortLayout = new QVBoxLayout();
    physicalPortLayout->addWidget(&physicalPortView_);

    QGroupBox* physicalGroupBox = new QGroupBox(tr("Physical ports"), this);
    physicalGroupBox->setLayout(physicalPortLayout);

    QWidget* topWidget = new QWidget(this);
    QHBoxLayout* topLayout = new QHBoxLayout(topWidget);
    topLayout->addWidget(physicalGroupBox, 1);
    topLayout->addLayout(filterAndFunctionLayout);

    QVBoxLayout* portMapsLayout = new QVBoxLayout();
    portMapsLayout->addWidget(&portMapsView_);

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

    connect(&directionFilter_, SIGNAL(currentIndexChanged(QString const&)),
        this, SLOT(onDirectionFilterChanged(QString const&)), Qt::UniqueConnection);

    connect(&portMapsModel_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&portMapsModel_, SIGNAL(portConnected(QString const&)),
        this, SLOT(onPortConnected(QString const&)), Qt::UniqueConnection);
    connect(&portMapsModel_, SIGNAL(portDisconnected(QString const&)),
        this, SLOT(onPortDisconnected(QString const&)), Qt::UniqueConnection);

    connect(&portMapsView_, SIGNAL(addItem(QModelIndex const&)),
        &portMapsModel_, SLOT(onAddPortMap(QModelIndex const&)), Qt::UniqueConnection);
    connect(&portMapsView_, SIGNAL(removeItem(QModelIndex const&)),
        &portMapsModel_, SLOT(onRemovePort(QModelIndex const&)), Qt::UniqueConnection);
    connect(&portMapsView_, SIGNAL(removeAllChildItemsFromIndex(QModelIndex const&)),
        &portMapsModel_, SLOT(onRemoveAllChildItemsFrom(QModelIndex const&)), Qt::UniqueConnection);

    connect(&hideConnectedPortsBox_, SIGNAL(toggled(bool)),
        &physicalPortSorter_, SLOT(setFilterHideConnected(bool)), Qt::UniqueConnection);

    connect(&removeAllMappingsButton_, SIGNAL(clicked(bool)),
        &portMapsModel_, SLOT(onRemoveAllPortMappings()), Qt::UniqueConnection);

    connect(&autoConnectButton_, SIGNAL(clicked()), &autoConnector_, SLOT(onAutoConnect()), Qt::UniqueConnection);

    connect(&autoConnector_, SIGNAL(portMapCreated(QSharedPointer<PortMap>)),
        &portMapsModel_, SLOT(onAddConnectedPortMap(QSharedPointer<PortMap>)), Qt::UniqueConnection);
    connect(&portMapsView_, SIGNAL(autoConnecteLogicalSignals(QStringList const&)),
        &autoConnector_, SLOT(onAutoConnectLogicalSignals(QStringList const&)), Qt::UniqueConnection);
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
    foreach (QSharedPointer<BusInterface> busInterface, *component_->getBusInterfaces())
    {
        foreach (QSharedPointer<PortMap> interfacePortMap, *busInterface->getPortMaps())
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
