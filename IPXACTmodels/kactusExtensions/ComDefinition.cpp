//-----------------------------------------------------------------------------
// File: ComDefinition.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 2.4.2012
//
// Description:
// Class which encapsulates the handling of a custom communication definition
// object.
//-----------------------------------------------------------------------------

#include "ComDefinition.h"

#include <IPXACTmodels/common/GenericVendorExtension.h>

#include <IPXACTmodels/kactusExtensions/ComProperty.h>

//-----------------------------------------------------------------------------
// Function: ComDefinition::ComDefinition()
//-----------------------------------------------------------------------------
ComDefinition::ComDefinition(VLNV const& vlnv) : Document(vlnv),
    transferTypes_(new QStringList),
    properties_(new QList<QSharedPointer<ComProperty> >)
{
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::ComDefinition()
//-----------------------------------------------------------------------------
ComDefinition::ComDefinition(ComDefinition const& rhs) : Document(rhs),
    transferTypes_(new QStringList(*rhs.transferTypes_)),
    properties_(new QList<QSharedPointer<ComProperty> >)
{
    // Make deep copies of the properties.
    foreach (QSharedPointer<ComProperty> property, *rhs.properties_)
    {
        properties_->append(QSharedPointer<ComProperty>(new ComProperty(*property.data())));
    }
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::~ComDefinition()
//-----------------------------------------------------------------------------
ComDefinition::~ComDefinition()
{
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::clone()
//-----------------------------------------------------------------------------
QSharedPointer<Document> ComDefinition::clone() const
{
    return QSharedPointer<Document>(new ComDefinition(*this));
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::getDependentFiles()
//-----------------------------------------------------------------------------
QStringList ComDefinition::getDependentFiles() const
{
    return QStringList();
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::getDependentVLNVs()
//-----------------------------------------------------------------------------
QList<VLNV> ComDefinition::getDependentVLNVs() const
{
    return QList<VLNV>();
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::addTransferType()
//-----------------------------------------------------------------------------
void ComDefinition::addTransferType(QString const& type)
{
    // Check for duplicates.
    if (!transferTypes_->contains(type))
    {
        transferTypes_->append(type);
    }
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::removeTransferType()
//-----------------------------------------------------------------------------
void ComDefinition::removeTransferType(QString const& type)
{
    transferTypes_->removeAll(type);
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::setTransferTypes()
//-----------------------------------------------------------------------------
void ComDefinition::setTransferTypes(QStringList const& types)
{
    transferTypes_ = QSharedPointer<QStringList>( new QStringList( types ) );
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::addProperty()
//-----------------------------------------------------------------------------
void ComDefinition::addProperty(QSharedPointer<ComProperty> prop)
{
    properties_->append(prop);
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::removeProperty()
//-----------------------------------------------------------------------------
void ComDefinition::removeProperty(QString const& name)
{
    foreach (QSharedPointer<ComProperty> property, *properties_)
    {
        if (property->name() == name)
        {
            properties_->removeOne(property);
            break;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::setProperties()
//-----------------------------------------------------------------------------
void ComDefinition::setProperties(QList< QSharedPointer<ComProperty> > const& properties)
{
    properties_ = QSharedPointer< QList< QSharedPointer<ComProperty> > >
		( new QList<QSharedPointer<ComProperty> >( properties ) );
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::getTransferTypes()
//-----------------------------------------------------------------------------
QSharedPointer< QStringList > ComDefinition::getTransferTypes() const
{
    return transferTypes_;
}

//-----------------------------------------------------------------------------
// Function: ComDefinition::getProperties()
//-----------------------------------------------------------------------------
QSharedPointer< QList< QSharedPointer<ComProperty> > > ComDefinition::getProperties() const
{
    return properties_;
}
