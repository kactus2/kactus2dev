//-----------------------------------------------------------------------------
// File: AutoConnector.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 12.02.2019
//
// Description:
// Automatically connects items of two component items.
//-----------------------------------------------------------------------------

#include "AutoConnector.h"

#include <common/graphicsItems/ComponentItem.h>

#include <editors/common/ComponentItemAutoConnector/ListFiller.h>
#include <editors/common/ComponentItemAutoConnector/TableAutoConnector.h>
#include <editors/common/ComponentItemAutoConnector/AutoConnectorConnectionTable.h>
#include <editors/common/ComponentItemAutoConnector/AutoConnectorConnectionDelegate.h>

#include <QVBoxLayout>
#include <QGroupBox>
#include <QListWidget>

//-----------------------------------------------------------------------------
// Function: AutoConnector::AutoConnector()
//-----------------------------------------------------------------------------
AutoConnector::AutoConnector(ComponentItem* firstItem, ComponentItem* secondItem, ListFiller* listFiller,
    TableAutoConnector* tableInitializer, QString const& itemName, QWidget* parent) :
QWidget(parent),
firstComponent_(firstItem->componentModel()),
secondComponent_(secondItem->componentModel()),
connectorTable_(),
tableInitializer_(tableInitializer)
{
    setupLayout(firstItem, secondItem, listFiller, itemName);
}

//-----------------------------------------------------------------------------
// Function: AutoConnector::~AutoConnector()
//-----------------------------------------------------------------------------
AutoConnector::~AutoConnector()
{

}

//-----------------------------------------------------------------------------
// Function: AutoConnector::getConnectedItems()
//-----------------------------------------------------------------------------
QVector<QPair<QString, QString> > AutoConnector::getConnectedItems() const
{
    return connectorTable_->getConnectedItems();
}

//-----------------------------------------------------------------------------
// Function: AutoConnector::setupLayout()
//-----------------------------------------------------------------------------
void AutoConnector::setupLayout(ComponentItem* firstItem, ComponentItem* secondItem, ListFiller* listFiller,
    QString const& itemName)
{
    QListWidget* secondItemList(new QListWidget(this));
    QListWidget* firstItemList(new QListWidget(this));
    listFiller->initializeList(firstItemList, firstComponent_);
    listFiller->initializeList(secondItemList, secondComponent_);
    firstItemList->setDragEnabled(true);
    firstItemList->setDragDropMode(QAbstractItemView::DragOnly);
    secondItemList->setDragEnabled(true);
    secondItemList->setDragDropMode(QAbstractItemView::DragOnly);

    QVBoxLayout* firstComponentLayout(new QVBoxLayout());
    QVBoxLayout* secondComponentLayout(new QVBoxLayout());
    firstComponentLayout->addWidget(firstItemList);
    secondComponentLayout->addWidget(secondItemList);

    QString firstItemName = getComponentItemName(firstItem);
    QString secondItemName = getComponentItemName(secondItem);

    QGroupBox* firstComponentGroup(new QGroupBox(firstItemName, this));
    QGroupBox* secondComponentGroup(new QGroupBox(secondItemName, this));
    firstComponentGroup->setLayout(firstComponentLayout);
    secondComponentGroup->setLayout(secondComponentLayout);

    connectorTable_ = new AutoConnectorConnectionTable(firstItemList, secondItemList, this);
    tableInitializer_->constructTableDetails(connectorTable_, firstItemName, secondItemName);
    connectAutomatically();
    connectorTable_->setItemDelegate(new AutoConnectorConnectionDelegate(firstItemList, secondItemList, this));

    QVBoxLayout* connectedPortsLayout(new QVBoxLayout());
    connectedPortsLayout->addWidget(connectorTable_);

    QGroupBox* connectedPortsGroup(new QGroupBox(tr("Connected ") + itemName));
    connectedPortsGroup->setAlignment(Qt::AlignHCenter);
    connectedPortsGroup->setLayout(connectedPortsLayout);

    QHBoxLayout* portsLayout(new QHBoxLayout(this));
    portsLayout->addWidget(firstComponentGroup);
    portsLayout->addWidget(connectedPortsGroup, 2);
    portsLayout->addWidget(secondComponentGroup);
}

//-----------------------------------------------------------------------------
// Function: AutoConnector::getComponentItemName()
//-----------------------------------------------------------------------------
QString AutoConnector::getComponentItemName(ComponentItem* componentItem) const
{
    if (!componentItem->displayName().isEmpty())
    {
        return componentItem->displayName();
    }
    else
    {
        return componentItem->name();
    }
}

//-----------------------------------------------------------------------------
// Function: AutoConnector::connectAutomatically()
//-----------------------------------------------------------------------------
void AutoConnector::connectAutomatically()
{
    tableInitializer_->initializeTable(connectorTable_, firstComponent_, secondComponent_);
}

//-----------------------------------------------------------------------------
// Function: AutoConnector::clearConnectedItems()
//-----------------------------------------------------------------------------
void AutoConnector::clearConnectedItems()
{
    tableInitializer_->clearTable(connectorTable_);
}
