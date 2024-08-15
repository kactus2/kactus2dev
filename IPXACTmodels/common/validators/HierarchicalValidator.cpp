//-----------------------------------------------------------------------------
// File: HierarchicalValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: 
// Date: 06.08.2024
//
// Description:
// Parent validator for validators used in component editor to be able to mark child items as invalid.
//-----------------------------------------------------------------------------

#include "HierarchicalValidator.h"

#include <IPXACTmodels/common/NameGroup.h>

#include <QMultiHash>

//-----------------------------------------------------------------------------
// Function: HierarchicalValidator::checkChildren()
//-----------------------------------------------------------------------------
bool HierarchicalValidator::childrenHaveUniqueNames(QSharedPointer<QList<QSharedPointer<NameGroup> > > children)
{
    if (children->isEmpty() || !childValidator_)
    {
        return true;
    }

    bool childrenAreValid = true;

    QMultiHash<QString, QSharedPointer<NameGroup> > found;
    for (auto child : *children)
    {
        if (found.contains(child->name()))
        {
            found.insert(child->name(), child);

            for (auto duplicate : found.values(child->name()))
            {
                childValidator_->setChildItemValidity(duplicate, false);
            }
            
            childrenAreValid = false;
        }
        else
        {
            found.insert(child->name(), child);
            childValidator_->setChildItemValidity(child, true);
        }
    }

    return childrenAreValid;
}

//-----------------------------------------------------------------------------
// Function: HierarchicalValidator::setChildValidator()
//-----------------------------------------------------------------------------
void HierarchicalValidator::setChildValidator(QSharedPointer<HierarchicalValidator> childValidator)
{
    childValidator_ = childValidator;
}

//-----------------------------------------------------------------------------
// Function: HierarchicalValidator::validComparedToSiblings()
//-----------------------------------------------------------------------------
bool HierarchicalValidator::validComparedToSiblings(QSharedPointer<NameGroup> currentChildItem) const
{
    return childItemValidities_.contains(currentChildItem) ? childItemValidities_[currentChildItem] : true;
}

//-----------------------------------------------------------------------------
// Function: HierarchicalValidator::setChildItemValidity()
//-----------------------------------------------------------------------------
void HierarchicalValidator::setChildItemValidity(QSharedPointer<NameGroup> childItem, bool isValid)
{
    childItemValidities_.insert(childItem, isValid);
}

//-----------------------------------------------------------------------------
// Function: HierarchicalValidator::childItemChecked()
//-----------------------------------------------------------------------------
bool HierarchicalValidator::childItemIsValid(QSharedPointer<NameGroup> childItem) const
{
    return childItemValidities_.value(childItem, true);
}

//-----------------------------------------------------------------------------
// Function: HierarchicalValidator::clearChildItemValidities()
//-----------------------------------------------------------------------------
void HierarchicalValidator::clearChildItemValidities()
{
    childItemValidities_.clear();
}
