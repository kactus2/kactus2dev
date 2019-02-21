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
#include <editors/common/ComponentItemAutoConnector/AutoConnectorListFilter.h>

#include <QVBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QStandardItemModel>

//-----------------------------------------------------------------------------
// Function: AutoConnector::AutoConnector()
//-----------------------------------------------------------------------------
AutoConnector::AutoConnector(ComponentItem* firstItem, ComponentItem* secondItem, ListFiller* listFiller,
    TableAutoConnector* tableInitializer, QString const& itemName, QWidget* parent) :
QWidget(parent),
firstComponent_(firstItem->componentModel()),
secondComponent_(secondItem->componentModel()),
firstListFilter_(),
secondListFilter_(),
firstItemList_(),
secondItemList_(),
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
    firstItemList_ = new QListView(this);
    secondItemList_ = new QListView(this);

    firstItemList_->setDragEnabled(true);
    firstItemList_->setDragDropMode(QAbstractItemView::DragOnly);
    secondItemList_->setDragEnabled(true);
    secondItemList_->setDragDropMode(QAbstractItemView::DragOnly);

    QCheckBox* firstHideBox(new QCheckBox(tr("Hide connected"), this));
    QCheckBox* secondHideBox(new QCheckBox(tr("Hide connected"), this));

    firstHideBox->setChecked(true);
    secondHideBox->setChecked(true);

    QVBoxLayout* firstComponentLayout(new QVBoxLayout());
    QVBoxLayout* secondComponentLayout(new QVBoxLayout());
    firstComponentLayout->addWidget(firstItemList_, 1);
    firstComponentLayout->addWidget(firstHideBox);
    secondComponentLayout->addWidget(secondItemList_, 1);
    secondComponentLayout->addWidget(secondHideBox);

    QString firstItemName = getComponentItemName(firstItem);
    QString secondItemName = getComponentItemName(secondItem);

    QGroupBox* firstComponentGroup(new QGroupBox(firstItemName, this));
    QGroupBox* secondComponentGroup(new QGroupBox(secondItemName, this));
    firstComponentGroup->setLayout(firstComponentLayout);
    secondComponentGroup->setLayout(secondComponentLayout);

    connectorTable_ =
        new AutoConnectorConnectionTable(firstItemList_, secondItemList_, firstItemName, secondItemName, this);
    connectorTable_->setItemDelegate(new AutoConnectorConnectionDelegate(firstItemList_, secondItemList_, this));

    connect(connectorTable_->model(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int> &)),
        this, SLOT(invalidateListFilters()), Qt::UniqueConnection);
    connect(connectorTable_->model(), SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
        this, SLOT(invalidateListFilters()), Qt::UniqueConnection);

    QVBoxLayout* connectedItemsLayout(new QVBoxLayout());
    connectedItemsLayout->addWidget(connectorTable_);

    QGroupBox* connectedItemsGroup(new QGroupBox(tr("Connected ") + itemName));
    connectedItemsGroup->setAlignment(Qt::AlignHCenter);
    connectedItemsGroup->setLayout(connectedItemsLayout);

    QHBoxLayout* mainLayout(new QHBoxLayout(this));
    mainLayout->addWidget(firstComponentGroup);
    mainLayout->addWidget(connectedItemsGroup, 2);
    mainLayout->addWidget(secondComponentGroup);

    QStandardItemModel* firstListModel(new QStandardItemModel(this));
    QStandardItemModel* secondListModel(new QStandardItemModel(this));

    listFiller->initializeList(firstListModel, firstComponent_);
    listFiller->initializeList(secondListModel, secondComponent_);

    firstListFilter_ = new AutoConnectorListFilter(connectorTable_, 0, firstHideBox);
    firstListFilter_->setSourceModel(firstListModel);
    firstItemList_->setModel(firstListFilter_);
    secondListFilter_ = new AutoConnectorListFilter(connectorTable_, 1, secondHideBox);
    secondListFilter_->setSourceModel(secondListModel);
    secondItemList_->setModel(secondListFilter_);
}

//-----------------------------------------------------------------------------
// Function: AutoConnector::invalidateListFilters()
//-----------------------------------------------------------------------------
void AutoConnector::invalidateListFilters()
{
    firstListFilter_->invalidate();
    secondListFilter_->invalidate();
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
// Function: AutoConnector::connectSelectedItems()
//-----------------------------------------------------------------------------
void AutoConnector::connectSelectedItems()
{
    tableInitializer_->connectSelectedFromLists(firstItemList_, secondItemList_, connectorTable_);
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
