//-----------------------------------------------------------------------------
// File: WireTypeDef.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 10.09.2015
//
// Description:
// Describes the ipxact:wireTypeDef element.
//-----------------------------------------------------------------------------

#include "WireTypeDef.h"

//-----------------------------------------------------------------------------
// Function: WireTypeDef::WireTypeDef()
//-----------------------------------------------------------------------------
WireTypeDef::WireTypeDef(const QString& typeName) :
typeName_(typeName)
{

}

//-----------------------------------------------------------------------------
// Function: WireTypeDef::WireTypeDef()
//-----------------------------------------------------------------------------
WireTypeDef::WireTypeDef(const WireTypeDef& other) :
typeName_(other.typeName_),
constrained_(other.constrained_)
{
    copyTypeDefinitions(*other.typeDefinitions_);
    copyViewNameReferences(*other.viewNameRefs_);
}

//-----------------------------------------------------------------------------
// Function: WireTypeDef::operator=()
//-----------------------------------------------------------------------------
WireTypeDef& WireTypeDef::operator=(const WireTypeDef& other)
{
    if (this != &other)
    {
        typeName_ = other.typeName_;
        constrained_ = other.constrained_;

        typeDefinitions_->clear();
        copyTypeDefinitions(*other.typeDefinitions_);

        viewNameRefs_->clear();
        copyViewNameReferences(*other.viewNameRefs_);
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: WireTypeDef::hasView()
//-----------------------------------------------------------------------------
bool WireTypeDef::hasView(QString const& viewName) const
{
    return (viewName.isEmpty() && viewNameRefs_->isEmpty()) || viewNameRefs_->contains(viewName);
}

//-----------------------------------------------------------------------------
// Function: WireTypeDef::getTypeName()
//-----------------------------------------------------------------------------
QString WireTypeDef::getTypeName() const
{
    return typeName_;
}

//-----------------------------------------------------------------------------
// Function: WireTypeDef::setTypeName()
//-----------------------------------------------------------------------------
void WireTypeDef::setTypeName(QString const& newTypeName)
{
    typeName_ = newTypeName;
}

//-----------------------------------------------------------------------------
// Function: WireTypeDef::isConstrained()
//-----------------------------------------------------------------------------
bool WireTypeDef::isConstrained() const
{
    return constrained_;
}

//-----------------------------------------------------------------------------
// Function: WireTypeDef::setConstrained()
//-----------------------------------------------------------------------------
void WireTypeDef::setConstrained(bool constrainedStatus)
{
    constrained_ = constrainedStatus;
}

//-----------------------------------------------------------------------------
// Function: WireTypeDef::getTypeDefinitions()
//-----------------------------------------------------------------------------
QSharedPointer<QStringList> WireTypeDef::getTypeDefinitions() const
{
    return typeDefinitions_;
}

//-----------------------------------------------------------------------------
// Function: WireTypeDef::setTypeDefinitions()
//-----------------------------------------------------------------------------
void WireTypeDef::setTypeDefinitions(QSharedPointer<QStringList> newTypeDefinitions)
{
    typeDefinitions_ = newTypeDefinitions;
}

//-----------------------------------------------------------------------------
// Function: WireTypeDef::getViewRefs()
//-----------------------------------------------------------------------------
QSharedPointer<QStringList> WireTypeDef::getViewRefs() const
{
    return viewNameRefs_;
}

//-----------------------------------------------------------------------------
// Function: WireTypeDef::setViewRefs()
//-----------------------------------------------------------------------------
void WireTypeDef::setViewRefs(QSharedPointer<QStringList> newViewRefs)
{
    viewNameRefs_ = newViewRefs;
}

//-----------------------------------------------------------------------------
// Function: WireTypeDef::isEmpty()
//-----------------------------------------------------------------------------
bool WireTypeDef::isEmpty() const
{
    return typeName_.isEmpty() && typeDefinitions_->isEmpty() && viewNameRefs_->isEmpty();
}

//-----------------------------------------------------------------------------
// Function: WireTypeDef::copyTypeDefinitions()
//-----------------------------------------------------------------------------
void WireTypeDef::copyTypeDefinitions(QStringList const& newTypeDefinitions)
{
    for (auto const& definition : newTypeDefinitions)
    {
        typeDefinitions_->append(definition);
    }
}

//-----------------------------------------------------------------------------
// Function: WireTypeDef::copyViewNameReferences()
//-----------------------------------------------------------------------------
void WireTypeDef::copyViewNameReferences(QStringList const& newViewReferences)
{
    for (auto const& viewReference : newViewReferences)
    {
        viewNameRefs_->append(viewReference);
    }
}
