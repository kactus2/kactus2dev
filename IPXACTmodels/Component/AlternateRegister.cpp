//-----------------------------------------------------------------------------
// File: AlternateRegister.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.09.2015
//
// Description:
// Describes the ipxact:alternateRegister element.
//-----------------------------------------------------------------------------

#include "AlternateRegister.h"

#include <IPXACTmodels/Component/ModeReference.h>

#include <IPXACTmodels/utilities/Copy.h>

//-----------------------------------------------------------------------------
// Function: AlternateRegister::AlternateRegister()
//-----------------------------------------------------------------------------
AlternateRegister::AlternateRegister(QString const& name, QString const& alternateGroup):
RegisterDefinition(name),
alternateGroups_(new QStringList())
{
    if (!alternateGroup.isEmpty())
    {
        alternateGroups_->append(alternateGroup);
    }
}

//-----------------------------------------------------------------------------
// Function: AlternateRegister::AlternateRegister()
//-----------------------------------------------------------------------------
AlternateRegister::AlternateRegister(const AlternateRegister& other):
RegisterDefinition(other),
alternateGroups_(new QStringList())
{
    copyAlternateGroups(other);
    Copy::copyList(other.modeReferences_, modeReferences_);
}

//-----------------------------------------------------------------------------
// Function: AlternateRegister::operator=()
//-----------------------------------------------------------------------------
AlternateRegister& AlternateRegister::operator=(const AlternateRegister& other)
{
    if (this != &other)
    {
        RegisterDefinition::operator=(other);

        alternateGroups_->clear();
        copyAlternateGroups(other);

        modeReferences_->clear();
        Copy::copyList(other.modeReferences_, modeReferences_);
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: AlternateRegister::~AlternateRegister()
//-----------------------------------------------------------------------------
AlternateRegister::~AlternateRegister()
{

}

//-----------------------------------------------------------------------------
// Function: AlternateRegister::clone()
//-----------------------------------------------------------------------------
QSharedPointer<RegisterBase> AlternateRegister::clone()
{
    return QSharedPointer<RegisterBase>(new AlternateRegister(*this));
}

//-----------------------------------------------------------------------------
// Function: AlternateRegister::getAlternateGroups()
//-----------------------------------------------------------------------------
QSharedPointer<QStringList> AlternateRegister::getAlternateGroups() const
{
    return alternateGroups_;
}

//-----------------------------------------------------------------------------
// Function: AlternateRegister::setAlternateGroups()
//-----------------------------------------------------------------------------
void AlternateRegister::setAlternateGroups(QSharedPointer<QStringList> newAlternateGroups)
{
    alternateGroups_->clear();
    alternateGroups_ = newAlternateGroups;
}

//-----------------------------------------------------------------------------
// Function: AlternateRegister::getModeReferences()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<ModeReference> > > AlternateRegister::getModeReferences() const
{
    return modeReferences_;
}

//-----------------------------------------------------------------------------
// Function: AlternateRegister::setModeReferences()
//-----------------------------------------------------------------------------
void AlternateRegister::setModeReferences(QSharedPointer<QList<QSharedPointer<ModeReference> > > newModeReferences)
{
    modeReferences_ = newModeReferences;
}

//-----------------------------------------------------------------------------
// Function: AlternateRegister::copyAlternateGroups()
//-----------------------------------------------------------------------------
void AlternateRegister::copyAlternateGroups(const AlternateRegister& other)
{
    for (auto const& group : *other.alternateGroups_)
    {
        alternateGroups_->append(group);
    }
}