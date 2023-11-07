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
void ModeReferenceInterface::setModeReferences(QSharedPointer<QList<QSharedPointer<ModeReference> > > modeReferences)
{
    modeReferences_ = modeReferences;
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceInterface::getModeReferenceCount()
//-----------------------------------------------------------------------------
int ModeReferenceInterface::getModeReferenceCount() const
{
    if (modeReferences_)
    {
        return modeReferences_->size();
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceInterface::getModeReferenceValue()
//-----------------------------------------------------------------------------
std::string ModeReferenceInterface::getModeReferenceValue(int modeReferenceIndex) const
{
    std::string modeRefValue;
    if (modeReferences_ && modeReferenceIndex >= 0 && modeReferenceIndex < modeReferences_->size())
    {
        modeRefValue = modeReferences_->at(modeReferenceIndex)->getReference().toStdString();
    }

    return modeRefValue;
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceInterface::getModeReferencePriority()
//-----------------------------------------------------------------------------
int ModeReferenceInterface::getModeReferencePriority(int modeReferenceIndex) const
{
    if (modeReferences_ && modeReferenceIndex >= 0 && modeReferenceIndex < modeReferences_->size())
    {
        return modeReferences_->at(modeReferenceIndex)->getPriority();
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceInterface::setModeReferenceValue()
//-----------------------------------------------------------------------------
bool ModeReferenceInterface::setModeReferenceValue(int modeReferenceIndex, std::string const& newValue)
{
    if (modeReferences_ && modeReferenceIndex >= 0 && modeReferenceIndex < modeReferences_->size())
    {
        modeReferences_->at(modeReferenceIndex)->setReference(QString::fromStdString(newValue));
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceInterface::setModeReferencePriority()
//-----------------------------------------------------------------------------
bool ModeReferenceInterface::setModeReferencePriority(int modeReferenceIndex, unsigned int newPriority)
{
    if (modeReferences_ && modeReferenceIndex >= 0 && modeReferenceIndex < modeReferences_->size())
    {
        modeReferences_->at(modeReferenceIndex)->setPriority(newPriority);
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceInterface::addModeReference()
//-----------------------------------------------------------------------------
bool ModeReferenceInterface::addModeReference()
{
    if (modeReferences_)
    {
        QSharedPointer<ModeReference> createdModeRef(new ModeReference());
        modeReferences_->append(createdModeRef);
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceInterface::removeModeReference()
//-----------------------------------------------------------------------------
bool ModeReferenceInterface::removeModeReference(int modeReferenceIndex)
{
    if (modeReferences_ && modeReferenceIndex >= 0 && modeReferenceIndex < modeReferences_->size())
    {
        modeReferences_->removeAt(modeReferenceIndex);
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceInterface::modeReferenceValueIsValid()
//-----------------------------------------------------------------------------
bool ModeReferenceInterface::modeReferenceValueIsValid(int modeReferenceIndex) const
{
    if (!(modeReferences_ && modeReferenceIndex >= 0 && modeReferenceIndex < modeReferences_->size()))
    {
        return false;
    }

    // Get the mode reference to check from the remap/access policy mode references. Check against mode references
    // in all remaps/access policies.
    auto modeRefToCheck = modeReferences_->at(modeReferenceIndex);

    auto allModeRefs = getModeReferencesInUse(modeReferenceIndex);

    QStringList availableModeNames;
    for (auto const& mode : *componentModes_)
    {
        availableModeNames << mode->name();
    }

    return CommonItemsValidator::modeReferenceValueIsValid(allModeRefs, modeRefToCheck, availableModeNames);
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceInterface::modeReferencePriorityIsValid()
//-----------------------------------------------------------------------------
bool ModeReferenceInterface::modeReferencePriorityIsValid(int modeReferenceIndex) const
{
    if (!(modeReferences_ && modeReferenceIndex >= 0 && modeReferenceIndex < modeReferences_->size()))
    {
        return false;
    }

    // Get the mode reference to check from the remap/access policy mode references. Check against mode references
    // in all remaps/access policies.
    auto modeRefToCheck = modeReferences_->at(modeReferenceIndex);

    auto allModeRefs = getModeReferencesInUse(modeReferenceIndex);

    return CommonItemsValidator::modeReferencePriorityIsValid(allModeRefs, modeRefToCheck, containingElementIsRemap);
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceInterface::getModeReferencesString()
//-----------------------------------------------------------------------------
std::vector<std::string> ModeReferenceInterface::getModeReferencesString() const
{
    std::vector<std::string> referencesAsStrings;
    std::transform(modeReferences_->cbegin(), modeReferences_->cend(), std::back_inserter(referencesAsStrings),
        [](QSharedPointer<ModeReference> modeRef)
        {
            return modeRef->getReference().toStdString();
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
void ModeReferenceInterface::setContainingElementModeReferences(QSharedPointer<QList<QSharedPointer<ModeReference> > > modeRefs)
{
    containingElementModeReferences_ = modeRefs;
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceInterface::getComponentModes()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Mode> > > ModeReferenceInterface::getComponentModes() const
{
    return componentModes_;
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
QSharedPointer<QList<QSharedPointer<ModeReference> > > ModeReferenceInterface::getModeReferences() const
{
    return modeReferences_;
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceInterface::getModeReferencesInUse()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<ModeReference> > > ModeReferenceInterface::getModeReferencesInUse(int modeReferenceIndex) const
{
    // List containing all other mode references in containing parent, than the one being validated (given by index).
    QSharedPointer<QList<QSharedPointer<ModeReference> > > allModeRefs(new QList<QSharedPointer<ModeReference> >());

    allModeRefs->append(*containingElementModeReferences_);

    int currentIndex = 0;
    for (auto modeRef : *modeReferences_)
    {
        if (currentIndex != modeReferenceIndex)
        {
            allModeRefs->append(modeRef);
        }
        currentIndex++;
    }

    return allModeRefs;
}
