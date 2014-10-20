//-----------------------------------------------------------------------------
// File: ComponentDiffWidget.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 17.10.2014
//
// Description:
// Tree widget for displaying differences of two components.
//-----------------------------------------------------------------------------

#include "ComponentDiffWidget.h"

#include <IPXACTmodels/component.h>

#include <wizards/common/ComponentComparator/ComponentComparator.h>

//-----------------------------------------------------------------------------
// Function: ComponentDiffWidget::ComponentDiffWidget()
//-----------------------------------------------------------------------------
ComponentDiffWidget::ComponentDiffWidget(QWidget *parent)
    : QTreeWidget (parent)
{
    QStringList labels;
    labels << "Name" << "Changed element" << "Previous value" << "Updated value";
    setHeaderLabels(labels);

    setColumnCount(labels.count());    
}

//-----------------------------------------------------------------------------
// Function: ComponentDiffWidget::~ComponentDiffWidget()
//-----------------------------------------------------------------------------
ComponentDiffWidget::~ComponentDiffWidget()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentDiffWidget::setComponents()
//-----------------------------------------------------------------------------
void ComponentDiffWidget::setComponents(QSharedPointer<const Component> reference, 
    QSharedPointer<const Component> subject)
{
    clear();

    ComponentComparator comparator;
    QList<QSharedPointer<IPXactDiff> > diffs = comparator.diff(reference, subject);

    if (!diffs.isEmpty() && diffs.first()->changeType() != IPXactDiff::NO_CHANGE)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem();
        item->setText(ITEM_NAME, "Model parameters");
        addTopLevelItem(item);
    }

    foreach(QSharedPointer<IPXactDiff> diff, diffs)
    {
        if (diff->changeType() == IPXactDiff::ADD)
        {    
            addAddItem(diff->name(), topLevelItem(0));
        }
        else if (diff->changeType() == IPXactDiff::REMOVE)
        {    
            addRemoveItem(diff->name(), topLevelItem(0));
        }
        else if (diff->changeType() == IPXactDiff::MODIFICATION)
        {              
            addModificationItem(diff);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentDiffWidget::createAddItem()
//-----------------------------------------------------------------------------
void ComponentDiffWidget::addAddItem(QString const& name, QTreeWidgetItem* parent)
{
    QTreeWidgetItem* addItem = new QTreeWidgetItem(parent);
    addItem->setText(ITEM_NAME, name);
    addItem->setIcon(ITEM_NAME, QIcon(":/icons/common/graphics/add.png"));
}

//-----------------------------------------------------------------------------
// Function: ComponentDiffWidget::createRemoveItem()
//-----------------------------------------------------------------------------
void ComponentDiffWidget::addRemoveItem(QString const& name, QTreeWidgetItem* parent)
{
    QTreeWidgetItem* removeItem = new QTreeWidgetItem(parent);
    removeItem->setText(ITEM_NAME, name);
    removeItem->setIcon(ITEM_NAME, QIcon(":/icons/common/graphics/remove.png"));
}

//-----------------------------------------------------------------------------
// Function: ComponentDiffWidget::addModificationItem()
//-----------------------------------------------------------------------------
void ComponentDiffWidget::addModificationItem(QSharedPointer<IPXactDiff> diff)
{
    QList<IPXactDiff::Modification> changelist = diff->getChangeList();
    if (changelist.size() == 1)
    {                       
        addSingleLevelModificationItem(diff->name(), changelist.first());
    }
    else
    {
        addMultiLevelModificationItem(diff->name(), changelist);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentDiffWidget::createSingleLevelModificationItem()
//-----------------------------------------------------------------------------
void ComponentDiffWidget::addSingleLevelModificationItem(QString const& name, 
    IPXactDiff::Modification const& modification)
{
    QTreeWidgetItem* modificationItem = createModificationItem(modification, topLevelItem(0));
    modificationItem->setText(ITEM_NAME, name);  
    modificationItem->setIcon(ITEM_NAME, QIcon(":/icons/common/graphics/asterisk-yellow.png"));
}

//-----------------------------------------------------------------------------
// Function: ComponentDiffWidget::createModificationItem()
//-----------------------------------------------------------------------------
QTreeWidgetItem* ComponentDiffWidget::createModificationItem(IPXactDiff::Modification const& modification, 
    QTreeWidgetItem* parent) const
{
    QTreeWidgetItem* modificationItem = new QTreeWidgetItem(parent);
    modificationItem->setText(CHANGE_ELEMENT, modification.modifiedElement);
    modificationItem->setText(PREVIOUS_VALUE, modification.previousValue);
    modificationItem->setText(UPDATED_VALUE, modification.newValue);

    return modificationItem;
}

//-----------------------------------------------------------------------------
// Function: ComponentDiffWidget::createMultiLevelModificationItem()
//-----------------------------------------------------------------------------
void ComponentDiffWidget::addMultiLevelModificationItem(QString const& name, QList<IPXactDiff::Modification> changelist)
{
    QTreeWidgetItem* child = new QTreeWidgetItem(topLevelItem(0));
    child->setText(ITEM_NAME, name);  
    child->setIcon(ITEM_NAME, QIcon(":/icons/common/graphics/asterisk-yellow.png"));
    child->setText(CHANGE_ELEMENT, "[multiple changes]");

    foreach(IPXactDiff::Modification modification, changelist)
    {
        child->addChild(createModificationItem(modification, child));
    }
}
