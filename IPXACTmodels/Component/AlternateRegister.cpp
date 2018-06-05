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
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: AlternateRegister::~AlternateRegister()
//-----------------------------------------------------------------------------
AlternateRegister::~AlternateRegister()
{
	alternateGroups_.clear();
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
// Function: AlternateRegister::copyAlternateGroups()
//-----------------------------------------------------------------------------
void AlternateRegister::copyAlternateGroups(const AlternateRegister& other)
{
    foreach (QString group, *other.alternateGroups_)
    {
        alternateGroups_->append(group);
    }
}