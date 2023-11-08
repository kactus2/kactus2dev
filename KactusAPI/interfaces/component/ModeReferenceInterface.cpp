//-----------------------------------------------------------------------------
// File: ModeReferenceInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 26.10.2023
//
// Description:
// Interface for accessing mode references.
//-----------------------------------------------------------------------------

#include <KactusAPI/include/ModeReferenceInterface.h>

#include <IPXACTmodels/common/validators/CommonItemsValidator.h>
#include <IPXACTmodels/Component/ModeReference.h>
#include <IPXACTmodels/Component/Mode.h>

//-----------------------------------------------------------------------------
// Function: ModeReferenceInterface::setModeReferences()
//-----------------------------------------------------------------------------
void ModeReferenceInterface::setModeReferences(std::vector<std::pair<unsigned int, std::string> > modeReferences)
{
    modeReferences_ = modeReferences;
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceInterface::getModeReferenceCount()
//-----------------------------------------------------------------------------
int ModeReferenceInterface::getModeReferenceCount() const
{
    return modeReferences_.size();
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceInterface::getModeReferenceValue()
//-----------------------------------------------------------------------------
std::string ModeReferenceInterface::getModeReferenceValue(int modeReferenceIndex) const
{
    std::string modeRefValue;
    if (modeReferenceIndex >= 0 && modeReferenceIndex < modeReferences_.size())
    {
        modeRefValue = modeReferences_.at(modeReferenceIndex).second;
    }

    return modeRefValue;
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceInterface::getModeReferencePriority()
//-----------------------------------------------------------------------------
int ModeReferenceInterface::getModeReferencePriority(int modeReferenceIndex) const
{
    if (modeReferenceIndex >= 0 && modeReferenceIndex < modeReferences_.size())
    {
        return modeReferences_.at(modeReferenceIndex).first;
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceInterface::setModeReferenceValue()
//-----------------------------------------------------------------------------
bool ModeReferenceInterface::setModeReferenceValue(int modeReferenceIndex, std::string const& newValue)
{
    if (modeReferenceIndex >= 0 && modeReferenceIndex < modeReferences_.size())
    {
        modeReferences_.at(modeReferenceIndex).second = newValue;
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceInterface::setModeReferencePriority()
//-----------------------------------------------------------------------------
bool ModeReferenceInterface::setModeReferencePriority(int modeReferenceIndex, unsigned int newPriority)
{
    if (modeReferenceIndex >= 0 && modeReferenceIndex < modeReferences_.size())
    {
        modeReferences_.at(modeReferenceIndex).first = newPriority;
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceInterface::addModeReference()
//-----------------------------------------------------------------------------
bool ModeReferenceInterface::addModeReference()
{
    modeReferences_.emplace_back(0, std::string());
    return true;
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceInterface::removeModeReference()
//-----------------------------------------------------------------------------
bool ModeReferenceInterface::removeModeReference(int modeReferenceIndex)
{
    if (modeReferenceIndex >= 0 && modeReferenceIndex < modeReferences_.size())
    {
        modeReferences_.erase(modeReferences_.begin() + modeReferenceIndex);
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceInterface::modeReferenceValueIsValid()
//-----------------------------------------------------------------------------
bool ModeReferenceInterface::modeReferenceValueIsValid(int modeReferenceIndex) const
{
    if (!(modeReferenceIndex >= 0 && modeReferenceIndex < modeReferences_.size()))
    {
        return false;
    }

    // Get the mode reference to check from the remap/access policy mode references. Check against mode references
    // in all remaps/access policies.
    auto const& modeRefToCheck = modeReferences_.at(modeReferenceIndex);

    auto allModeRefs = getModeReferencesInUse(modeReferenceIndex);

    std::vector<std::string> modeRefValuesInUse;
    modeRefValuesInUse.reserve(allModeRefs.size());
    std::transform(allModeRefs.cbegin(), allModeRefs.cend(), std::back_inserter(modeRefValuesInUse),
        [](auto const& tuple)
        {
            return tuple.second;
        });

    QStringList availableModeNames;
    for (auto const& mode : *componentModes_)
    {
        availableModeNames << mode->name();
    }

    return CommonItemsValidator::modeReferenceValueIsValid(modeRefValuesInUse, modeRefToCheck.second, availableModeNames);
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceInterface::modeReferencePriorityIsValid()
//-----------------------------------------------------------------------------
bool ModeReferenceInterface::modeReferencePriorityIsValid(int modeReferenceIndex) const
{
    if (!(modeReferenceIndex >= 0 && modeReferenceIndex < modeReferences_.size()))
    {
        return false;
    }

    // Get the mode reference to check from the remap/access policy mode references. Check against mode references
    // in all remaps/access policies.
    auto modeRefToCheck = modeReferences_.at(modeReferenceIndex).first;

    auto allModeRefs = getModeReferencesInUse(modeReferenceIndex);

    QList<unsigned int> modeRefPrioritiesInUse;
    modeRefPrioritiesInUse.reserve(allModeRefs.size());

    std::transform(allModeRefs.cbegin(), allModeRefs.cend(), std::back_inserter(modeRefPrioritiesInUse),
        [](auto const& tuple)
        {
            return tuple.first;
        });

    return CommonItemsValidator::modeReferencePriorityIsValid(modeRefPrioritiesInUse, modeRefToCheck, containingElementIsRemap);
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceInterface::getModeReferencesString()
//-----------------------------------------------------------------------------
std::vector<std::string> ModeReferenceInterface::getModeReferencesString() const
{
    std::vector<std::string> referencesAsStrings;
    std::transform(modeReferences_.cbegin(), modeReferences_.cend(), std::back_inserter(referencesAsStrings),
        [](auto const& modeRef)
        {
            return modeRef.second;
        });

    return referencesAsStrings;
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceInterface::setContainingElementIsRemap()
//-----------------------------------------------------------------------------
void ModeReferenceInterface::setContainingElementIsRemap(bool isRemap)
{
    containingElementIsRemap = isRemap;
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceInterface::setContainingElementModeReferences()
//-----------------------------------------------------------------------------
void ModeReferenceInterface::setContainingElementModeReferences(std::vector<std::pair<unsigned int, std::string> > modeReferences)
{
    containingElementModeReferences_ = modeReferences;
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceInterface::getComponentModes()
//-----------------------------------------------------------------------------
std::vector<std::string> ModeReferenceInterface::getComponentModes() const
{
    std::vector<std::string> availableModes;
    std::transform(componentModes_->cbegin(), componentModes_->cend(), std::back_inserter(availableModes),
        [](auto mode)
        {
            return mode->name().toStdString();
        });

    return availableModes;
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceInterface::setComponentModes()
//-----------------------------------------------------------------------------
void ModeReferenceInterface::setComponentModes(QSharedPointer<QList<QSharedPointer<Mode> > > newModes)
{
    componentModes_ = newModes;
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceInterface::getModeReferences()
//-----------------------------------------------------------------------------
std::vector<std::pair<unsigned int, std::string> > ModeReferenceInterface::getModeReferences() const
{
    return modeReferences_;
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceInterface::getModeReferencesInUse()
//-----------------------------------------------------------------------------
std::vector<std::pair<unsigned int, std::string> > ModeReferenceInterface::getModeReferencesInUse(int modeReferenceIndex) const
{
    // List containing all other mode references in containing parent, than the one being validated (given by index).
    std::vector<std::pair<unsigned int, std::string> > allModeRefs;

    allModeRefs.insert(allModeRefs.end(), containingElementModeReferences_.begin(), 
        containingElementModeReferences_.end());

    int currentIndex = 0;
    for (auto const& modeRef : modeReferences_)
    {
        // Get all other mode references, excluding mode references given by index.
        if (currentIndex != modeReferenceIndex)
        {
            allModeRefs.push_back(modeRef);
        }
        currentIndex++;
    }

    return allModeRefs;
}
