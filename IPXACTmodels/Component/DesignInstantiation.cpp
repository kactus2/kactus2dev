//-----------------------------------------------------------------------------
// File: DesignInstantiation.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 03.09.2015
//
// Description:
// Describes the ipxact:view element of a component.
//-----------------------------------------------------------------------------

#include "DesignInstantiation.h"

//-----------------------------------------------------------------------------
// Function: DesignInstantiation::DesignInstantiation()
//-----------------------------------------------------------------------------
DesignInstantiation::DesignInstantiation(QString const& name /* = QString() */,
    QString const& displayName /* = QString() */, QString const& description /* = QString() */) :
NameGroup(name, displayName, description),
Extendable(),
designRef_(new ConfigurableVLNVReference())
{

}

//-----------------------------------------------------------------------------
// Function: DesignInstantiation::DesignInstantiation()
//-----------------------------------------------------------------------------
DesignInstantiation::DesignInstantiation(const DesignInstantiation& other):
NameGroup(other),
Extendable(other),
designRef_(new ConfigurableVLNVReference(*other.designRef_.data()))
{

}

//-----------------------------------------------------------------------------
// Function: DesignInstantiation::operator=()
//-----------------------------------------------------------------------------
DesignInstantiation& DesignInstantiation::operator=(DesignInstantiation const& other)
{
    if (this != &other)
    {
        NameGroup::operator=(other);
        Extendable::operator=(other);

        designRef_.clear();
        designRef_ =
            QSharedPointer<ConfigurableVLNVReference> (new ConfigurableVLNVReference(*other.designRef_.data()));
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: DesignInstantiation::~DesignInstantiation()
//-----------------------------------------------------------------------------
DesignInstantiation::~DesignInstantiation()
{
    designRef_.clear();
}

//-----------------------------------------------------------------------------
// Function: DesignInstantiation::getDesignReference()
//-----------------------------------------------------------------------------
QSharedPointer<ConfigurableVLNVReference> DesignInstantiation::getDesignReference() const
{
    return designRef_;
}

//-----------------------------------------------------------------------------
// Function: DesignInstantiation::setDesignReference()
//-----------------------------------------------------------------------------
void DesignInstantiation::setDesignReference(QSharedPointer<ConfigurableVLNVReference> newDesignReference)
{
    designRef_ = newDesignReference;
}