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
WireTypeDef::WireTypeDef(const QString& typeName, const QString& viewNameRef) :
typeName_(typeName),
constrained_(false),
typeDefinitions_(new QStringList()),
viewNameRefs_(new QStringList())
{
    if (!viewNameRef.isEmpty())
    {
        viewNameRefs_->append(viewNameRef);
    }
}

//-----------------------------------------------------------------------------
// Function: WireTypeDef::WireTypeDef()
//-----------------------------------------------------------------------------
WireTypeDef::WireTypeDef(const WireTypeDef& other) :
typeName_(other.typeName_),
constrained_(other.constrained_),
typeDefinitions_(new QStringList()),
viewNameRefs_(new QStringList())
{
    copyTypeDefinitions(other.typeDefinitions_);
    copyViewNameReferences(other.viewNameRefs_);
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
        typeDefinitions_ = other.typeDefinitions_;

        typeDefinitions_->clear();
        viewNameRefs_->clear();

        copyTypeDefinitions(other.typeDefinitions_);
        copyViewNameReferences(other.viewNameRefs_);
    }
    return *this;
}

//-----------------------------------------------------------------------------
// Function: WireTypeDef::copyTypeDefinitions()
//-----------------------------------------------------------------------------
void WireTypeDef::copyTypeDefinitions(QSharedPointer<QStringList> newTypeDefinitions)
{
    foreach (QString definition, *newTypeDefinitions)
    {
        typeDefinitions_->append(definition);
    }
}

//-----------------------------------------------------------------------------
// Function: WireTypeDef::copyViewNameReferences()
//-----------------------------------------------------------------------------
void WireTypeDef::copyViewNameReferences(QSharedPointer<QStringList> newViewReferences)
{
    foreach (QString viewReference, *newViewReferences)
    {
        viewNameRefs_->append(viewReference);
    }
}

//-----------------------------------------------------------------------------
// Function: WireTypeDef::hasView()
//-----------------------------------------------------------------------------
bool WireTypeDef::hasView(QString const& viewName)
{
    // Is this truly necessary?
    if (viewName.isEmpty())
    {
        return true;
    }

    foreach (QString viewReference, *viewNameRefs_)
    {
        if (viewReference == viewName)
        {
            return true;
        }
    }
    return false;
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