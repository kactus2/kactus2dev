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
#include "ModeReferenceInterface.h"

#include <IPXACTmodels/common/validators/CommonItemsValidator.h>

#include <IPXACTmodels/Component/AccessPolicy.h>
#include <IPXACTmodels/Component/ModeReference.h>
#include <IPXACTmodels/Component/Mode.h>

//-----------------------------------------------------------------------------
// Function: AccessPolicyInterface::AccessPolicyInterface()
//-----------------------------------------------------------------------------
AccessPolicyInterface::AccessPolicyInterface(ModeReferenceInterface* modeRefInterface):
    modeRefInterface_(modeRefInterface)
{

}

//-----------------------------------------------------------------------------
// Function: AccessPolicyInterface::setAccessPolicies()
//-----------------------------------------------------------------------------
void AccessPolicyInterface::setAccessPolicies(QSharedPointer<QList<QSharedPointer<AccessPolicy> > > accessPolicies)
{
    accessPolicies_ = accessPolicies;
}

//-----------------------------------------------------------------------------
// Function: AccessPolicyInterface::setAvailableModes()
//-----------------------------------------------------------------------------
void AccessPolicyInterface::setComponentModes(QSharedPointer<QList<QSharedPointer<Mode> > > componentModes)
{
    componentModes_ = componentModes;
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
// Function: AccessPolicyInterface::hasValidAccessPolicies()
//-----------------------------------------------------------------------------
bool AccessPolicyInterface::hasValidAccessPolicies() const
{
    return CommonItemsValidator::hasValidAccessPolicies(accessPolicies_, componentModes_);
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

//-----------------------------------------------------------------------------
// Function: AccessPolicyInterface::getModeReferenceInterface()
//-----------------------------------------------------------------------------
ModeReferenceInterface* AccessPolicyInterface::getModeReferenceInterface() const
{
    return modeRefInterface_;
}

//-----------------------------------------------------------------------------
// Function: AccessPolicyInterface::getAccesPolicyModeReferences()
//-----------------------------------------------------------------------------
std::vector<std::pair<unsigned int, std::string> > AccessPolicyInterface::getAccesPolicyModeReferences(int accessPolicyIndex) const
{
    std::vector<std::pair<unsigned int, std::string> > modeRefs;
    if (accessPolicyIndex >= 0 && accessPolicyIndex < accessPolicies_->size())
    {
        auto modeReferences = accessPolicies_->at(accessPolicyIndex)->getModeReferences();

        std::transform(modeReferences->cbegin(), modeReferences->cend(), std::back_inserter(modeRefs),
            [](auto modeReference)
            {
                return std::make_pair(modeReference->getPriority(), modeReference->getReference().toStdString());
            });
    }

    return modeRefs;
}

//-----------------------------------------------------------------------------
// Function: AccessPolicyInterface::setAccessPolicyModeReferences()
//-----------------------------------------------------------------------------
bool AccessPolicyInterface::setAccessPolicyModeReferences(int accessPolicyIndex, 
    std::vector<std::pair<unsigned int, std::string> > const& newModeRefs)
{
    if (accessPolicyIndex >= 0 && accessPolicyIndex < accessPolicies_->size())
    {
        QSharedPointer<QList<QSharedPointer<ModeReference> > > createdModeRefs(
            new QList<QSharedPointer<ModeReference> >());

        for (auto const& [priority, reference] : newModeRefs)
        {
            QSharedPointer<ModeReference> createdModeRef(new ModeReference());
            createdModeRef->setReference(QString::fromStdString(reference));
            createdModeRef->setPriority(priority);
            createdModeRefs->append(createdModeRef);
        }

        accessPolicies_->at(accessPolicyIndex)->setModeReferences(createdModeRefs);
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AccessPolicyInterface::getModeReferencesInUse()
//-----------------------------------------------------------------------------
std::vector<std::pair<unsigned int, std::string> > AccessPolicyInterface::getModeReferencesInUse(int accessPolicyIndex) const
{
    std::vector<std::pair<unsigned int, std::string> > modeRefsInUse;

    int currentIndex = 0;
    for (auto const& accessPolicy : *accessPolicies_)
    {
        // Get all mode references, excluding mode references in given access policy.
        if (currentIndex != accessPolicyIndex)
        {
            auto modeReferences = accessPolicy->getModeReferences();
            std::transform(modeReferences->cbegin(), modeReferences->cend(), std::back_inserter(modeRefsInUse),
                [](auto modeReference)
                {
                    return std::make_pair(modeReference->getPriority(), modeReference->getReference().toStdString());
                });
        }
        ++currentIndex;
    }

    return modeRefsInUse;
}
