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
#include <editors/common/ComponentItemAutoConnector/BusInterfaceItemMatcher.h>
#include <editors/common/ComponentItemAutoConnector/PortListFiller.h>
#include <editors/common/ComponentItemAutoConnector/PortTableAutoConnector.h>
#include <editors/common/ComponentItemAutoConnector/PortItemMatcher.h>

#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QLabel>

//-----------------------------------------------------------------------------
// Function: ComponentItemAutoConnector::ComponentItemAutoConnector()
//-----------------------------------------------------------------------------
ComponentItemAutoConnector::ComponentItemAutoConnector(ComponentItem* firstItem, ComponentItem* secondItem,
    LibraryInterface* library, QWidget* parent):
QDialog(parent),
firstItemName_(firstItem->name()),
secondItemName_(secondItem->name()),
connectButton_(new QPushButton(QIcon(":/icons/common/graphics/connect.png"), "Connect", this)),
autoConnectButton_(new QPushButton(QIcon(":/icons/common/graphics/configuration.png"), "Auto connect all", this)),
clearButton_(new QPushButton(QIcon(":/icons/common/graphics/cleanup.png"), tr("Clear"), this)),
busInterfaceConnector_(new AutoConnector(firstItem, secondItem, new BusInterfaceListFiller(),
    new BusInterfaceTableAutoConnector(library), tr("bus interfaces"), new BusInterfaceItemMatcher(library),
    this)),
portConnector_(new AutoConnector(firstItem, secondItem, new PortListFiller(), new PortTableAutoConnector(),
    tr("ports"), new PortItemMatcher(), this)),
tabs_(this)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    setMinimumWidth(1200);
    setMinimumHeight(720);

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

    autoConnectItems();
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
    QString introLabel = tr("Auto connector");
    QString introText = tr("Connect bus interfaces and ports from two component instances automatically.") +
        tr(" Automatic connections are created using the names of the items.\n") +
        tr("Bus interfaces create interconnections and ports create ad-hoc connections.");
    QWidget* introWidget = setupIntroWidget(introLabel, introText);

    QPushButton* okButton(new QPushButton(tr("Finish"), this));
    connect(okButton, SIGNAL(released()), this, SLOT(accept()), Qt::UniqueConnection);

    QPushButton* cancelButton (new QPushButton(tr("Cancel"), this));
    connect(cancelButton, SIGNAL(released()), this, SLOT(reject()), Qt::UniqueConnection);

    QDialogButtonBox* connectionButtons(new QDialogButtonBox(Qt::Horizontal));
    connectionButtons->addButton(connectButton_, QDialogButtonBox::ActionRole);
    connectionButtons->addButton(autoConnectButton_, QDialogButtonBox::ActionRole);
    connectionButtons->addButton(clearButton_, QDialogButtonBox::ActionRole);

    QDialogButtonBox* buttonBox (new QDialogButtonBox(Qt::Horizontal));
    buttonBox->addButton(okButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(cancelButton, QDialogButtonBox::ActionRole);

    setWindowTitle("Auto connect");

    connect(connectButton_, SIGNAL(released()), this, SLOT(connectSelectedItems()), Qt::UniqueConnection);
    connect(autoConnectButton_, SIGNAL(released()), this, SLOT(autoConnectItems()), Qt::UniqueConnection);
    connect(clearButton_, SIGNAL(released()), this, SLOT(clearConnections()), Qt::UniqueConnection);

    QVBoxLayout* mainLayout (new QVBoxLayout(this));
    mainLayout->addWidget(introWidget, 0, Qt::AlignTop);
    mainLayout->addWidget(&tabs_, 1);
    mainLayout->addWidget(connectionButtons);
    mainLayout->addWidget(buttonBox);
}

//-----------------------------------------------------------------------------
// Function: ComponentItemAutoConnector::connectSelectedItems()
//-----------------------------------------------------------------------------
void ComponentItemAutoConnector::connectSelectedItems()
{
    if (tabs_.currentWidget() == portConnector_)
    {
        portConnector_->connectSelectedItems();
    }
    else
    {
        busInterfaceConnector_->connectSelectedItems();
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentItemAutoConnector::autoConnectItems()
//-----------------------------------------------------------------------------
void ComponentItemAutoConnector::autoConnectItems()
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
// Function: ComponentItemAutoConnector::clearConnections()
//-----------------------------------------------------------------------------
void ComponentItemAutoConnector::clearConnections()
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

//-----------------------------------------------------------------------------
// Function: ComponentItemAutoConnector::setupIntroWidget()
//-----------------------------------------------------------------------------
QWidget* ComponentItemAutoConnector::setupIntroWidget(QString const& introName, QString const& introText) const
{
    QLabel* introLabel = new QLabel(introName);
    QLabel* introTextLabel = new QLabel(introText);
    QFont introFont = introLabel->font();
    introFont.setBold(true);
    introLabel->setFont(introFont);

    QLabel* iconLabel = new QLabel();
    iconLabel->setPixmap(QPixmap(QString(":/icons/common/graphics/autoConnect.png")));

    QVBoxLayout* introTextLayout = new QVBoxLayout();
    introTextLayout->addWidget(introLabel);
    introTextLayout->addWidget(introTextLabel);

    QHBoxLayout* mainIntroLayout = new QHBoxLayout();
    mainIntroLayout->addLayout(introTextLayout, 1);
    mainIntroLayout->addWidget(iconLabel);

    QWidget* introWidget = new QWidget();
    introWidget->setStyleSheet("background-color: white");
    introWidget->setLayout(mainIntroLayout);

    return introWidget;
}
