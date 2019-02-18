//-----------------------------------------------------------------------------
// File: ComponentItemAutoConnector.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 23.01.2019
//
// Description:
// Automatically connects the ports and bus interfaces of two component items.
//-----------------------------------------------------------------------------

#include "ComponentItemAutoConnector.h"

#include <IPXACTmodels/Component/Component.h>

#include <common/graphicsItems/ComponentItem.h>

#include <editors/common/ComponentItemAutoConnector/AutoConnector.h>
#include <editors/common/ComponentItemAutoConnector/AutoConnectorItem.h>
#include <editors/common/ComponentItemAutoConnector/BusInterfaceListFiller.h>
#include <editors/common/ComponentItemAutoConnector/BusInterfaceTableAutoConnector.h>
#include <editors/common/ComponentItemAutoConnector/PortListFiller.h>
#include <editors/common/ComponentItemAutoConnector/PortTableAutoConnector.h>

#include <QDialogButtonBox>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: ComponentItemAutoConnector::ComponentItemAutoConnector()
//-----------------------------------------------------------------------------
ComponentItemAutoConnector::ComponentItemAutoConnector(ComponentItem* firstItem, ComponentItem* secondItem,
    LibraryInterface* library, QWidget* parent):
QDialog(parent),
firstItemName_(firstItem->name()),
secondItemName_(secondItem->name()),
autoConnectButton_(new QPushButton(QIcon(":/icons/common/graphics/connect.png"), "Auto connect all", this)),
clearButton_(new QPushButton(QIcon(":/icons/common/graphics/cleanup.png"), tr("Clear"), this)),
busInterfaceConnector_(new AutoConnector(firstItem, secondItem, new BusInterfaceListFiller(),
    new BusInterfaceTableAutoConnector(library), tr("bus interfaces"), this)),
portConnector_(new AutoConnector(
    firstItem, secondItem, new PortListFiller(), new PortTableAutoConnector(), tr("ports"), this)),
tabs_(this)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    setMinimumWidth(1000);
    setMinimumHeight(600);

    tabs_.addTab(busInterfaceConnector_, QString(tr("Bus interfaces")));
    tabs_.addTab(portConnector_, QString(tr("Ports")));

    setupLayout();

    QSharedPointer<Component> firstComponent = firstItem->componentModel();
    QSharedPointer<Component> secondComponent = secondItem->componentModel();

    if (firstComponent->getBusInterfaces()->isEmpty() && secondComponent->getBusInterfaces()->isEmpty() &&
        (!firstComponent->getPorts()->isEmpty() || !secondComponent->getPorts()->isEmpty()))
    {
        tabs_.setCurrentWidget(portConnector_);
    }

    connectCurrentItems();
}

//-----------------------------------------------------------------------------
// Function: ComponentItemAutoConnector::~ComponentItemAutoConnector()
//-----------------------------------------------------------------------------
ComponentItemAutoConnector::~ComponentItemAutoConnector()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentItemAutoConnector::getConnectedItems()
//-----------------------------------------------------------------------------
QVector<QPair<AutoConnectorItem*, AutoConnectorItem* > > ComponentItemAutoConnector::getConnectedItems() const
{
    QVector<QPair<AutoConnectorItem*, AutoConnectorItem*> > fullConnections;

    fullConnections.append(createItemPairs(busInterfaceConnector_, AutoConnectorItem::BUS_INTERFACE));
    fullConnections.append(createItemPairs(portConnector_, AutoConnectorItem::PORT));

    return fullConnections;
}

//-----------------------------------------------------------------------------
// Function: ComponentItemAutoConnector::createItemPairs()
//-----------------------------------------------------------------------------
QVector<QPair<AutoConnectorItem*, AutoConnectorItem*> > ComponentItemAutoConnector::createItemPairs(
    AutoConnector* connector, AutoConnectorItem::ItemType itemType) const
{
    QVector<QPair<AutoConnectorItem*, AutoConnectorItem*> > connectorItemPairs;

    QVector<QPair<QString, QString> > connectionPairs = connector->getConnectedItems();
    for (auto const& connection : connectionPairs)
    {
        AutoConnectorItem* firstItem(new AutoConnectorItem(connection.first, firstItemName_, itemType));
        AutoConnectorItem* secondItem(new AutoConnectorItem(connection.second, secondItemName_, itemType));

        QPair<AutoConnectorItem*, AutoConnectorItem*> newConnection;
        newConnection.first = firstItem;
        newConnection.second = secondItem;

        connectorItemPairs.append(newConnection);
    }

    return connectorItemPairs;
}

//-----------------------------------------------------------------------------
// Function: ComponentItemAutoConnector::setupLayout()
//-----------------------------------------------------------------------------
void ComponentItemAutoConnector::setupLayout()
{
    QPushButton* okButton(new QPushButton(tr("Ok"), this));
    connect(okButton, SIGNAL(released()), this, SLOT(accept()), Qt::UniqueConnection);

    QPushButton* cancelButton (new QPushButton(tr("Cancel"), this));
    connect(cancelButton, SIGNAL(released()), this, SLOT(reject()), Qt::UniqueConnection);

    QDialogButtonBox* buttonBox (new QDialogButtonBox(Qt::Horizontal));
    buttonBox->addButton(autoConnectButton_, QDialogButtonBox::ActionRole);
    buttonBox->addButton(clearButton_, QDialogButtonBox::ActionRole);
    buttonBox->addButton(okButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(cancelButton, QDialogButtonBox::ActionRole);

    setWindowTitle("Auto connect (experimental)");

    connect(autoConnectButton_, SIGNAL(released()), this, SLOT(connectCurrentItems()), Qt::UniqueConnection);
    connect(clearButton_, SIGNAL(released()), this, SLOT(clearCurrentConnections()), Qt::UniqueConnection);

    QVBoxLayout* mainLayout (new QVBoxLayout(this));
    mainLayout->addWidget(&tabs_, 1);
    mainLayout->addWidget(buttonBox);
}

//-----------------------------------------------------------------------------
// Function: ComponentItemAutoConnector::connectCurrentItems()
//-----------------------------------------------------------------------------
void ComponentItemAutoConnector::connectCurrentItems()
{
    if (tabs_.currentWidget() == portConnector_)
    {
        portConnector_->connectAutomatically();
    }
    else
    {
        busInterfaceConnector_->connectAutomatically();
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentItemAutoConnector::clearCurrentConnections()
//-----------------------------------------------------------------------------
void ComponentItemAutoConnector::clearCurrentConnections()
{
    if (tabs_.currentWidget() == portConnector_)
    {
        portConnector_->clearConnectedItems();
    }
    else
    {
        busInterfaceConnector_->clearConnectedItems();
    }
}
