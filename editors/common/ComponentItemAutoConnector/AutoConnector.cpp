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
AutoConnector::AutoConnector(ComponentItemAutoConnector::AutoContainer const& firstComponentContainer, 
    ComponentItemAutoConnector::AutoContainer const& secondComponentContainer,
    ComponentItemAutoConnector::TableTools const& tableToolsContainer,
    QString const& itemName, QWidget* parent /*= 0*/) :
QWidget(parent),
firstComponentContainer_(firstComponentContainer),
secondComponentContainer_(secondComponentContainer),
connectorTableTools_(tableToolsContainer),
firstListFilter_(),
secondListFilter_(),
firstItemList_(),
secondItemList_(),
connectorTable_(),
tableInitializer_(connectorTableTools_.tableConnector_)
{
    setupLayout(firstComponentContainer_.visibleName_, secondComponentContainer_.visibleName_, itemName);
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
void AutoConnector::setupLayout(QString const& firstComponentName, QString const& secondComponentName, QString const& itemName)
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

    QGroupBox* firstComponentGroup(new QGroupBox(firstComponentName, this));
    QGroupBox* secondComponentGroup(new QGroupBox(secondComponentName, this));
    firstComponentGroup->setLayout(firstComponentLayout);
    secondComponentGroup->setLayout(secondComponentLayout);

    connectorTable_ = new AutoConnectorConnectionTable(firstComponentContainer_.component_, secondComponentContainer_.component_, firstItemList_,
        secondItemList_, firstComponentName, secondComponentName, connectorTableTools_.itemMatcher_, this);
    connectorTable_->setItemDelegate(new AutoConnectorConnectionDelegate(
        firstComponentContainer_.component_, secondComponentContainer_.component_, firstItemList_, secondItemList_, connectorTableTools_.itemMatcher_, this));

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

    connectorTableTools_.listFiller_->initializeList(firstListModel, firstComponentContainer_.component_);
    connectorTableTools_.listFiller_->initializeList(secondListModel, secondComponentContainer_.component_);

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
// Function: AutoConnector::connectSelectedItems()
//-----------------------------------------------------------------------------
void AutoConnector::connectSelectedItems()
{
    tableInitializer_->connectSelectedFromLists(firstComponentContainer_.component_, 
        secondComponentContainer_.component_, firstItemList_, secondItemList_, connectorTable_, 
        connectorTableTools_.itemMatcher_);
}

//-----------------------------------------------------------------------------
// Function: AutoConnector::connectAutomatically()
//-----------------------------------------------------------------------------
void AutoConnector::connectAutomatically()
{
    tableInitializer_->autoConnectItems(connectorTable_, firstComponentContainer_.component_, 
        secondComponentContainer_.component_);
}

//-----------------------------------------------------------------------------
// Function: AutoConnector::clearConnectedItems()
//-----------------------------------------------------------------------------
void AutoConnector::clearConnectedItems()
{
    tableInitializer_->clearTable(connectorTable_);
}

//-----------------------------------------------------------------------------
// Function: AutoConnector::connectAlreadyConnectedItems()
//-----------------------------------------------------------------------------
void AutoConnector::connectAlreadyConnectedItems(QSharedPointer<Design> design)
{
    tableInitializer_->populateTableWithConnectedItems(connectorTable_, firstComponentContainer_.name_, 
        secondComponentContainer_.name_, firstComponentContainer_.component_, 
        secondComponentContainer_.component_, design);
    connectorTable_->enableConnectionValidation();
}

//-----------------------------------------------------------------------------
// Function: AutoConnector::connectionTableHasInvalidConnections()
//-----------------------------------------------------------------------------
bool AutoConnector::connectionTableHasInvalidConnections() const
{
    return connectorTable_->hasInvalidConnections();
}
