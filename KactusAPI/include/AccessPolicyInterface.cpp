//-----------------------------------------------------------------------------
// File: AccessPolicyInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 20.9.2023
//
// Description:
// Interface for editing access policies.
//-----------------------------------------------------------------------------

#include "AccessPolicyInterface.h"

#include <IPXACTmodels/common/validators/CommonItemsValidator.h>

#include <IPXACTmodels/Component/AccessPolicy.h>

//-----------------------------------------------------------------------------
// Function: AccessPolicyInterface::setAccessPolicies()
//-----------------------------------------------------------------------------
void AccessPolicyInterface::setAccessPolicies(QSharedPointer<QList<QSharedPointer<AccessPolicy> > > accessPolicies)
{
    accessPolicies_ = accessPolicies;
}

//-----------------------------------------------------------------------------
// Function: AccessPolicyInterface::getAccessPolicyCount()
//-----------------------------------------------------------------------------
int AccessPolicyInterface::getAccessPolicyCount() const
{
    return accessPolicies_->size();
}

//-----------------------------------------------------------------------------
// Function: AccessPolicyInterface::addAccessPolicy()
//-----------------------------------------------------------------------------
void AccessPolicyInterface::addAccessPolicy()
{
    QSharedPointer<AccessPolicy> newAccessPolicy(new AccessPolicy());
    accessPolicies_->append(newAccessPolicy);
}

//-----------------------------------------------------------------------------
// Function: AccessPolicyInterface::removeAccessPolicy()
//-----------------------------------------------------------------------------
bool AccessPolicyInterface::removeAccessPolicy(int accessPolicyIndex)
{
    if (accessPolicyIndex <= accessPolicies_->size() - 1)
    {
        accessPolicies_->removeAt(accessPolicyIndex);
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AccessPolicyInterface::setModeRefs()
//-----------------------------------------------------------------------------
bool AccessPolicyInterface::setModeRefs(std::vector<std::pair<std::string, int> > const& newModeRefs, int accessPolicyIndex)
{
    if (accessPolicyIndex <= accessPolicies_->size() - 1)
    {
        // Clear old access policy mode refs.
        auto modeRefs = accessPolicies_->at(accessPolicyIndex)->getModeReferences();
        modeRefs->clear();

        // Create and add new mode refs.
        for (auto const& [reference, priority] : newModeRefs)
        {
            QSharedPointer<ModeReference> newModeRef(new ModeReference());
            newModeRef->setReference(QString::fromStdString(reference));
            newModeRef->setPriority(QString::number(priority));

            modeRefs->append(newModeRef);
        }

        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AccessPolicyInterface::getModeRefs()
//-----------------------------------------------------------------------------
std::vector<std::pair<std::string, int> > AccessPolicyInterface::getModeRefs(int accessPolicyIndex) const
{
    std::vector<std::pair<std::string, int> > modeRefsList;
    
    if (accessPolicyIndex <= accessPolicies_->size() - 1)
    {
        auto accessPolicyModeRefs = accessPolicies_->at(accessPolicyIndex)->getModeReferences();

        for (auto const& modeRef : *accessPolicyModeRefs)
        {
            modeRefsList.emplace_back(modeRef->getReference().toStdString(), modeRef->getPriority().toInt());
        }
    }
    
    return modeRefsList;
}

//-----------------------------------------------------------------------------
// Function: AccessPolicyInterface::hasValidAccessPolicies()
//-----------------------------------------------------------------------------
bool AccessPolicyInterface::hasValidAccessPolicies() const
{
    return CommonItemsValidator::hasValidAccessPolicies(accessPolicies_);
}

//-----------------------------------------------------------------------------
// Function: AccessPolicyInterface::setAccess()
//-----------------------------------------------------------------------------
bool AccessPolicyInterface::setAccess(std::string const& newAccess, int accessPolicyIndex)
{
    if (accessPolicyIndex <= accessPolicies_->size() - 1)
    {
        auto newAccessType = AccessTypes::str2Access(QString::fromStdString(newAccess), AccessTypes::ACCESS_COUNT);

        accessPolicies_->at(accessPolicyIndex)->setAccess(newAccessType);
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AccessPolicyInterface::getAccessString()
//-----------------------------------------------------------------------------
std::string AccessPolicyInterface::getAccessString(int accessPolicyIndex) const
{
    if (accessPolicyIndex <= accessPolicies_->size() - 1)
    {
        return AccessTypes::access2Str(accessPolicies_->at(accessPolicyIndex)->getAccess()).toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: AccessPolicyInterface::getAccess()
//-----------------------------------------------------------------------------
AccessTypes::Access AccessPolicyInterface::getAccess(int accessPolicyIndex) const
{
    if (accessPolicyIndex <= accessPolicies_->size() - 1)
    {
        return accessPolicies_->at(accessPolicyIndex)->getAccess();
    }

    return AccessTypes::ACCESS_COUNT;
}
