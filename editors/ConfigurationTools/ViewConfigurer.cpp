//-----------------------------------------------------------------------------
// File: ViewConfigurer.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 19.05.2015
//
// Description:
// Configures the views in the component design of the selected component.
//-----------------------------------------------------------------------------

#include "ViewConfigurer.h"
#include "ViewConfigurerColumns.h"
#include "ViewConfigurerDelegate.h"

#include <library/LibraryManager/libraryhandler.h>

#include <IPXACTmodels/vlnv.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/designconfiguration.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Placeholder.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Group.h>

#include <QSharedPointer>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QBrush>

//-----------------------------------------------------------------------------
// Function: ViewConfigurer::ViewConfigurer()
//-----------------------------------------------------------------------------
ViewConfigurer::ViewConfigurer(QSharedPointer<Component> selectedComponent, QSharedPointer<Design> selectedDesign,
    QSharedPointer<DesignConfiguration> selectedDesignConfiguration, LibraryHandler* libraryHandler,
    QString const& openViewName, QWidget* parent /* = 0 */):
QDialog(parent),
selectedDesignConfiguration_(selectedDesignConfiguration),
viewsTree_(new QTreeWidget(this)),
libraryHandler_(libraryHandler),
usedHierarchicalComponentVLNVS_(),
commitButton_ (new QPushButton(QIcon(":/icons/common/graphics/file-save.png"), tr("Save"), this)),
clearButton_(new QPushButton(QIcon(":/icons/common/graphics/cleanup.png"), tr("Clear"), this))
{
    clearButton_->setEnabled(false);

    setMinimumWidth(600);
    setMinimumHeight(400);

    QStringList treeHeaderLabels;
    treeHeaderLabels << "VLNV" << "Instance" << "View" << "ID";
    
    viewsTree_->setHeaderLabels(treeHeaderLabels);
    viewsTree_->setColumnCount(ViewConfigurerColumns::COLUMN_COUNT);
    viewsTree_->hideColumn(ViewConfigurerColumns::INSTANCE_ID);
    viewsTree_->setIndentation(15);

    viewsTree_->setAlternatingRowColors(true);

    viewsTree_->setItemDelegate(new ViewConfigurerDelegate(libraryHandler, this));

    QTreeWidgetItem* topComponentItem (new QTreeWidgetItem(viewsTree_));
    
    if (!selectedDesign && !selectedDesignConfiguration_)
    {
        topComponentItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);

        topComponentItem->setIcon(ViewConfigurerColumns::ITEM_VLNV,
            QIcon(":/icons/common/graphics/hw-component.png"));
    }
    else
    {
        usedHierarchicalComponentVLNVS_.append(selectedComponent->getVlnv()->toString(":"));

        topComponentItem->setIcon(ViewConfigurerColumns::ITEM_VLNV,
            QIcon(":/icons/common/graphics/hier-hw-component.png"));
    }

    topComponentItem->setText(ViewConfigurerColumns::ITEM_VLNV, selectedComponent->getVlnv()->toString(":"));
    topComponentItem->setText(ViewConfigurerColumns::INSTANCE_NAME, selectedComponent->getVlnv()->getName());

    if (openViewName.isEmpty())
    {
        topComponentItem->setText(ViewConfigurerColumns::INSTANCE_VIEW, ViewConfigurerColumns::NOVIEWTEXT);

        QBrush foreGroundBrush (Qt::red);
        topComponentItem->setForeground(ViewConfigurerColumns::INSTANCE_VIEW, foreGroundBrush);
    }
    else
    {
        topComponentItem->setText(ViewConfigurerColumns::INSTANCE_VIEW, openViewName);
    }

    viewsTree_->addTopLevelItem(topComponentItem);

    if (selectedDesign)
    {
        createChildTreeWidgetItems(selectedDesign, selectedDesignConfiguration_, topComponentItem);

        modifyTreeWithExistingViewGroup();
    }

    viewsTree_->expandAll();

    viewsTree_->resizeColumnToContents(ViewConfigurerColumns::ITEM_VLNV);
    viewsTree_->resizeColumnToContents(ViewConfigurerColumns::INSTANCE_NAME);
    viewsTree_->resizeColumnToContents(ViewConfigurerColumns::INSTANCE_VIEW);

    setupLayout();

    connect(viewsTree_, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
        this, SLOT(onInstanceViewChanged(QTreeWidgetItem*, int)), Qt::UniqueConnection);

    setCommmitButtonEnabledStatus();
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurer::~ViewConfigurer()
//-----------------------------------------------------------------------------
ViewConfigurer::~ViewConfigurer()
{

}

//-----------------------------------------------------------------------------
// Function: ViewConfigurer::setupLayout()
//-----------------------------------------------------------------------------
void ViewConfigurer::setupLayout()
{
    connect(clearButton_, SIGNAL(released()), this, SLOT(clearViewOverrides()), Qt::UniqueConnection);

    connect(commitButton_, SIGNAL(released()), this, SLOT(saveAndCloseConfigurer()), Qt::UniqueConnection);

    QPushButton* closeButton (new QPushButton(tr("Close"), this));
    connect(closeButton, SIGNAL(released()), this, SLOT(reject()), Qt::UniqueConnection);

    QDialogButtonBox* buttonBox (new QDialogButtonBox(Qt::Horizontal));
    buttonBox->addButton(clearButton_, QDialogButtonBox::ActionRole);
    buttonBox->addButton(commitButton_, QDialogButtonBox::ActionRole);
    buttonBox->addButton(closeButton, QDialogButtonBox::ActionRole);

    setWindowTitle("Configure view overrides");

    QVBoxLayout* mainLayout (new QVBoxLayout(this));
    mainLayout->addWidget(viewsTree_);
    mainLayout->addWidget(buttonBox);
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurer::createChildTreeWidgetItem()
//-----------------------------------------------------------------------------
void ViewConfigurer::createChildTreeWidgetItems(QSharedPointer<Design> currentDesign,
    QSharedPointer<DesignConfiguration> currentDesignConfiguration, QTreeWidgetItem* parentItem)
{
    foreach (ComponentInstance currentInstance, currentDesign->getComponentInstances())
    {
        VLNV componentReference = currentInstance.getComponentRef();
        QSharedPointer<Component> component = libraryHandler_->getModel(componentReference).dynamicCast<Component>();

        QString instanceViewName = currentDesignConfiguration->getActiveView(currentInstance.getInstanceName());

        QTreeWidgetItem* instanceItem (new QTreeWidgetItem(parentItem));
        instanceItem->setText(ViewConfigurerColumns::ITEM_VLNV, componentReference.toString(":"));
        instanceItem->setText(ViewConfigurerColumns::INSTANCE_NAME, currentInstance.getInstanceName());

        if (instanceViewName.isEmpty())
        {
            instanceViewName = ViewConfigurerColumns::NOVIEWTEXT;

            QBrush foreGroundBrush(Qt::red);
            instanceItem->setForeground(ViewConfigurerColumns::INSTANCE_VIEW, foreGroundBrush);
        }

        instanceItem->setText(ViewConfigurerColumns::INSTANCE_VIEW, instanceViewName);
        instanceItem->setText(ViewConfigurerColumns::INSTANCE_ID, currentInstance.getUuid());

        instanceItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);

        instanceItem->setIcon(ViewConfigurerColumns::ITEM_VLNV, QIcon(":/icons/common/graphics/hw-component.png"));

        if (usedHierarchicalComponentVLNVS_.contains(instanceItem->text(ViewConfigurerColumns::ITEM_VLNV)))
        {
            instanceItem->setText(ViewConfigurerColumns::INSTANCE_VIEW, ViewConfigurerColumns::CYCLICCOMPONENTTEXT);

            QBrush foreGroundBrush (Qt::red);
            instanceItem->setForeground(ViewConfigurerColumns::INSTANCE_VIEW, foreGroundBrush);
            instanceItem->setForeground(ViewConfigurerColumns::ITEM_VLNV, foreGroundBrush);
            instanceItem->setForeground(ViewConfigurerColumns::INSTANCE_NAME, foreGroundBrush);
            instanceItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        }
        else
        {
            usedHierarchicalComponentVLNVS_.append(instanceItem->text(ViewConfigurerColumns::ITEM_VLNV));

            checkInstanceDesign(component, instanceViewName, instanceItem);

            if (usedHierarchicalComponentVLNVS_.size() > 1)
            {
                usedHierarchicalComponentVLNVS_.removeAll(instanceItem->text(ViewConfigurerColumns::ITEM_VLNV));
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurer::checkInstanceDesign()
//-----------------------------------------------------------------------------
void ViewConfigurer::checkInstanceDesign(QSharedPointer<Component> component, QString const& viewName,
    QTreeWidgetItem* currentTreeItem)
{
    if (!viewName.isEmpty())
    {
        VLNV viewDesignVLNV = component->getHierRef(viewName);

        if (viewDesignVLNV.isValid())
        {
            QSharedPointer<DesignConfiguration> viewDesignConfiguration = libraryHandler_->getModel(
                viewDesignVLNV).dynamicCast<DesignConfiguration>();

            if (viewDesignConfiguration)
            {
                VLNV designReference = viewDesignConfiguration->getDesignRef();

                QSharedPointer<Design> viewDesign = libraryHandler_->getModel(designReference).
                    dynamicCast<Design>();

                if (viewDesign)
                {
                    currentTreeItem->setIcon(ViewConfigurerColumns::ITEM_VLNV,
                        QIcon(":/icons/common/graphics/hier-hw-component.png"));

                    createChildTreeWidgetItems(viewDesign, viewDesignConfiguration, currentTreeItem);

                    currentTreeItem->setExpanded(true);
                }
            }
        } 
    }
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurer::modifyTreeWithExistingViewGroup()
//-----------------------------------------------------------------------------
void ViewConfigurer::modifyTreeWithExistingViewGroup()
{
    if(selectedDesignConfiguration_)
    {
        QMap<QString, QString> viewOverrides = selectedDesignConfiguration_->getKactus2ViewOverrides();

        if (!viewOverrides.isEmpty())
        {
            clearButton_->setEnabled(true);

            for (int treeTopIndex = 0; treeTopIndex < viewsTree_->topLevelItemCount(); ++treeTopIndex)
            {
                parseExistingInstanceView(viewsTree_->topLevelItem(treeTopIndex), viewOverrides);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurer::parseExistingInstanceView()
//-----------------------------------------------------------------------------
void ViewConfigurer::parseExistingInstanceView(QTreeWidgetItem* currentTreeItem, QMap<QString, QString> viewOverrides)
{
    QString treeItemOverrideView = viewOverrides.value(currentTreeItem->text(ViewConfigurerColumns::INSTANCE_ID));

    if (!treeItemOverrideView.isEmpty())
    {
        currentTreeItem->setText(ViewConfigurerColumns::INSTANCE_VIEW, treeItemOverrideView);

        onInstanceViewChanged(currentTreeItem, ViewConfigurerColumns::INSTANCE_VIEW);
    }

    for (int treeItemChildIndex = 0; treeItemChildIndex < currentTreeItem->childCount(); ++treeItemChildIndex)
    {
        parseExistingInstanceView(currentTreeItem->child(treeItemChildIndex), viewOverrides);
    }
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurer::onInstanceViewChanged()
//-----------------------------------------------------------------------------
void ViewConfigurer::onInstanceViewChanged(QTreeWidgetItem* changedItem, int column)
{
    if (changedItem && column == ViewConfigurerColumns::INSTANCE_VIEW)
    {
        changedItem->takeChildren();

        changedItem->setIcon(ViewConfigurerColumns::ITEM_VLNV, QIcon(":/icons/common/graphics/hw-component.png"));

        QString itemVLNV = changedItem->text(ViewConfigurerColumns::ITEM_VLNV);
        VLNV componentVLNV (VLNV::COMPONENT, itemVLNV, ":");

        QSharedPointer<Component> component = libraryHandler_->getModel(componentVLNV).dynamicCast<Component>();

        QString viewName = changedItem->text(ViewConfigurerColumns::INSTANCE_VIEW);

        if (isChangedItemTopItem(changedItem))
        {
            clearButton_->setEnabled(false);
            changedTopItemChangesDesignConfiguration(component, viewName);
        }

        viewsTree_->blockSignals(true);

        checkInstanceDesign(component, viewName, changedItem);

        if (changedItem->text(ViewConfigurerColumns::INSTANCE_VIEW) == ViewConfigurerColumns::NOVIEWTEXT ||
            !component->getViewNames().contains(changedItem->text(ViewConfigurerColumns::INSTANCE_VIEW)))
        {
            QBrush foreGroundBrush (Qt::red);
            changedItem->setForeground(ViewConfigurerColumns::INSTANCE_VIEW, foreGroundBrush);

            if (isChangedItemTopItem(changedItem))
            {
                usedHierarchicalComponentVLNVS_.removeAll(itemVLNV);
            }
        }
        else
        {
            QBrush foreGroundBrush = changedItem->foreground(ViewConfigurerColumns::ITEM_VLNV);
            changedItem->setForeground(ViewConfigurerColumns::INSTANCE_VIEW, foreGroundBrush);
        }

        if (isChangedItemTopItem(changedItem))
        {
            modifyTreeWithExistingViewGroup();
        }

        viewsTree_->blockSignals(false);

        setCommmitButtonEnabledStatus();
    }
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurer::isChangedItemTopItem()
//-----------------------------------------------------------------------------
bool ViewConfigurer::isChangedItemTopItem(QTreeWidgetItem* changedTreeItem)
{
    for (int treeTopIndex = 0; treeTopIndex < viewsTree_->topLevelItemCount(); ++treeTopIndex)
    {
        if (changedTreeItem == viewsTree_->topLevelItem(treeTopIndex))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurer::changedTopItemChangesDesignConfiguration()
//-----------------------------------------------------------------------------
void ViewConfigurer::changedTopItemChangesDesignConfiguration(QSharedPointer<Component> component,
    QString const& viewName)
{
    selectedDesignConfiguration_.clear();

    VLNV viewDesignVLNV = component->getHierRef(viewName);
    if (viewDesignVLNV.isValid())
    {
        QSharedPointer<DesignConfiguration> viewDesignConfiguration =
            libraryHandler_->getModel(viewDesignVLNV).dynamicCast<DesignConfiguration>();

        if (viewDesignConfiguration)
        {
            selectedDesignConfiguration_ = viewDesignConfiguration;

            usedHierarchicalComponentVLNVS_.append(component->getVlnv()->toString(":"));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurer::setCommmitButtonEnabledStatus()
//-----------------------------------------------------------------------------
void ViewConfigurer::setCommmitButtonEnabledStatus()
{
    bool commitButtonStatus = true;

    for (int i = 0; i < viewsTree_->topLevelItemCount(); ++i)
    {
        if (treeItemContainsEmptyOrCyclicalViews(viewsTree_->topLevelItem(i)))
        {
            commitButtonStatus = false;
            break;
        }
    }

    commitButton_->setEnabled(commitButtonStatus);
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurer::treeContainsEmptyOrCyclicalViews()
//-----------------------------------------------------------------------------
bool ViewConfigurer::treeItemContainsEmptyOrCyclicalViews(QTreeWidgetItem* treeItem)
{
    if (treeItem->text(ViewConfigurerColumns::INSTANCE_VIEW) == ViewConfigurerColumns::NOVIEWTEXT ||
        treeItem->text(ViewConfigurerColumns::INSTANCE_VIEW) == ViewConfigurerColumns::CYCLICCOMPONENTTEXT)
    {
        return true;
    }

    for (int treeItemIndex = 0; treeItemIndex < treeItem->childCount(); ++treeItemIndex)
    {
        if (treeItemContainsEmptyOrCyclicalViews(treeItem->child(treeItemIndex)))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurer::saveAndCloseConfigurer()
//-----------------------------------------------------------------------------
void ViewConfigurer::saveAndCloseConfigurer()
{
    if(selectedDesignConfiguration_)
    {
        QMap<QString, QString> viewOverrides;
        for (int treetopIndex = 0; treetopIndex < viewsTree_->topLevelItemCount(); ++treetopIndex)
        {
            QTreeWidgetItem* topItem = viewsTree_->topLevelItem(treetopIndex);

            for (int topItemChildIndex = 0; topItemChildIndex < topItem->childCount(); ++topItemChildIndex)
            {
                parseChildTreeItem(topItem->child(topItemChildIndex), viewOverrides);
            }
        }

        selectedDesignConfiguration_->setKactus2ViewOverrides(viewOverrides);
        libraryHandler_->writeModelToFile(selectedDesignConfiguration_);
    }

    accept();
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurer::parseChildTreeItem()
//-----------------------------------------------------------------------------
void ViewConfigurer::parseChildTreeItem(QTreeWidgetItem* treeItem, QMap<QString, QString>& viewOverrides)
{
    QString treeItemID = treeItem->text(ViewConfigurerColumns::INSTANCE_ID);
    QString treeItemView = treeItem->text(ViewConfigurerColumns::INSTANCE_VIEW);

    if (treeItemView != ViewConfigurerColumns::NOVIEWTEXT &&
        treeItemView != ViewConfigurerColumns::CYCLICCOMPONENTTEXT)
    {
        viewOverrides.insert(treeItemID, treeItemView);

        for (int itemChildIndex = 0; itemChildIndex < treeItem->childCount(); ++itemChildIndex)
        {
            parseChildTreeItem(treeItem->child(itemChildIndex), viewOverrides);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurer::clearViewOverrides()
//-----------------------------------------------------------------------------
void ViewConfigurer::clearViewOverrides()
{
    if(selectedDesignConfiguration_)
    {
        selectedDesignConfiguration_->setKactus2ViewOverrides(QMap<QString, QString> ());
        libraryHandler_->writeModelToFile(selectedDesignConfiguration_);

        for (int treeTopIndex = 0; treeTopIndex < viewsTree_->topLevelItemCount(); ++treeTopIndex)
        {
            QTreeWidgetItem* treeTopItem = viewsTree_->topLevelItem(treeTopIndex);

            treeTopItem->takeChildren();

            QString topItemView = treeTopItem->text(ViewConfigurerColumns::INSTANCE_VIEW);
            QString topItemVLNV = treeTopItem->text(ViewConfigurerColumns::ITEM_VLNV);
            VLNV topComponentVLNV (VLNV::COMPONENT, topItemVLNV, ":");

            QSharedPointer<Component> topComponent = libraryHandler_->getModel(topComponentVLNV).
                dynamicCast<Component>();

            disconnect(viewsTree_, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
                this, SLOT(onInstanceViewChanged(QTreeWidgetItem*, int)));

            checkInstanceDesign(topComponent, topItemView, treeTopItem);

            connect(viewsTree_, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
                this, SLOT(onInstanceViewChanged(QTreeWidgetItem*, int)), Qt::UniqueConnection);
        }

        clearButton_->setEnabled(false);

        setCommmitButtonEnabledStatus();
    }
}
