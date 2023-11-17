//-----------------------------------------------------------------------------
// File: Transactional.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 10.09.2015
//
// Description:
// Describes the ipxact:transactional element within component ports.
//-----------------------------------------------------------------------------

#include "Transactional.h"

#include <IPXACTmodels/common/TransactionalTypes.h>

#include <IPXACTmodels/utilities/Copy.h>

//-----------------------------------------------------------------------------
// Function: Transactional::Transactional()
//-----------------------------------------------------------------------------
Transactional::Transactional(const Transactional& other) :
    allLogicalInitiativesAllowed_(other.allLogicalInitiativesAllowed_),
    initiative_(other.initiative_),
    kind_(other.kind_),
    busWidth_(other.busWidth_),
    qualifier_(other.qualifier_->clone()),
    maxConnections_(other.maxConnections_),
    minConnections_(other.minConnections_)
{
    if (other.protocol_)
    {
        protocol_ = QSharedPointer<Protocol>(other.protocol_->clone());
    }

    Copy::copyList(other.transTypeDefs_, transTypeDefs_);
}

//-----------------------------------------------------------------------------
// Function: Transactional::clone()
//-----------------------------------------------------------------------------
Transactional* Transactional::clone() const
{
    return new Transactional(*this);
}

//-----------------------------------------------------------------------------
// Function: Transactional::operator=()
//-----------------------------------------------------------------------------
Transactional& Transactional::operator=( const Transactional& other)
{
    if (this != & other)
    {
        allLogicalInitiativesAllowed_ = other.allLogicalInitiativesAllowed_;
        initiative_ = other.initiative_;
        kind_ = other.kind_;
        busWidth_ = other.busWidth_;
        qualifier_ = QSharedPointer<Qualifier>(other.qualifier_->clone());
        maxConnections_ = other.maxConnections_;
        minConnections_ = other.minConnections_;

        protocol_.clear();
        if (other.protocol_)
        {
            protocol_ = QSharedPointer<Protocol>(other.protocol_->clone());
        }

        transTypeDefs_->clear();
        Copy::copyList(other.transTypeDefs_, transTypeDefs_);
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: Transactional::getAllLogicalInitiativesAllowed()
//-----------------------------------------------------------------------------
bool Transactional::getAllLogicalInitiativesAllowed() const
{
    return allLogicalInitiativesAllowed_;
}

//-----------------------------------------------------------------------------
// Function: Transactional::setAllLogicalInitiativesAllowed()
//-----------------------------------------------------------------------------
void Transactional::setAllLogicalInitiativesAllowed(bool allow)
{
    allLogicalInitiativesAllowed_ = allow;
}

//-----------------------------------------------------------------------------
// Function: Transactional::getInitiative()
//-----------------------------------------------------------------------------
QString Transactional::getInitiative() const
{
    return initiative_;
}

//-----------------------------------------------------------------------------
// Function: Transactional::setInitiative()
//-----------------------------------------------------------------------------
void Transactional::setInitiative(QString const& newInitiative)
{
    if (TransactionalTypes::isIpXactInitiativeType(newInitiative))
    {
        initiative_ = newInitiative;
    }
}

//-----------------------------------------------------------------------------
// Function: Transactional::getKind()
//-----------------------------------------------------------------------------
QString Transactional::getKind() const
{
    return kind_;
}

//-----------------------------------------------------------------------------
// Function: Transactional::setKind()
//-----------------------------------------------------------------------------
void Transactional::setKind(QString const& newKind)
{
    kind_ = newKind;
}

//-----------------------------------------------------------------------------
// Function: Transactional::getBusWidth()
//-----------------------------------------------------------------------------
QString Transactional::getBusWidth() const
{
    return busWidth_;
}

//-----------------------------------------------------------------------------
// Function: Transactional::setBusWidth()
//-----------------------------------------------------------------------------
void Transactional::setBusWidth(QString const& newBusWidth)
{
    busWidth_ = newBusWidth;
}

//-----------------------------------------------------------------------------
// Function: Transactional::getQualifier()
//-----------------------------------------------------------------------------
QSharedPointer<Qualifier> Transactional::getQualifier() const
{
    return qualifier_;
}

//-----------------------------------------------------------------------------
// Function: Transactional::getProtocol()
//-----------------------------------------------------------------------------
QSharedPointer<Protocol> Transactional::getProtocol() const
{
    return protocol_;
}

//-----------------------------------------------------------------------------
// Function: Transactional::setProtocol()
//-----------------------------------------------------------------------------
void Transactional::setProtocol(QSharedPointer<Protocol> newProtocol)
{
    protocol_ = newProtocol;
}

//-----------------------------------------------------------------------------
// Function: Transactional::getTransTypeDef()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<WireTypeDef> > > Transactional::getTransTypeDef() const
{
    return transTypeDefs_;
}

//-----------------------------------------------------------------------------
// Function: Transactional::setTransTypeDefs()
//-----------------------------------------------------------------------------
void Transactional::setTransTypeDefs(QSharedPointer<QList<QSharedPointer<WireTypeDef> > > newTransTypeDefs)
{
    transTypeDefs_->clear();
    transTypeDefs_ = newTransTypeDefs;
}

//-----------------------------------------------------------------------------
// Function: Transactional::getMaxConnections()
//-----------------------------------------------------------------------------
QString Transactional::getMaxConnections() const
{
    return maxConnections_;
}

//-----------------------------------------------------------------------------
// Function: Transactional::setMaxConnections()
//-----------------------------------------------------------------------------
void Transactional::setMaxConnections(QString const& newMaxConnectionsExpression)
{
	maxConnections_ = newMaxConnectionsExpression;
}

//-----------------------------------------------------------------------------
// Function: Transactional::getMinConnections()
//-----------------------------------------------------------------------------
QString Transactional::getMinConnections() const
{
    return minConnections_;
}

//-----------------------------------------------------------------------------
// Function: Transactional::setMinConnections()
//-----------------------------------------------------------------------------
void Transactional::setMinConnections(QString const& newMinConnectionsExpression)
{
    minConnections_ = newMinConnectionsExpression;
}

//-----------------------------------------------------------------------------
// Function: Transactional::getTypeName()
//-----------------------------------------------------------------------------
QString Transactional::getTypeName(QString const& viewName) const
{
    for (QSharedPointer<WireTypeDef> transactionalTypeDefinition : *transTypeDefs_)
    {
        if (transactionalTypeDefinition->hasView(viewName))
        {
            return transactionalTypeDefinition->getTypeName();
        }
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: Transactional::setTypeName()
//-----------------------------------------------------------------------------
void Transactional::setTypeName(QString const& typeName, QString const& viewName)
{
    for (QSharedPointer<WireTypeDef> transactionalTypeDefinition : *transTypeDefs_)
    {
        if (viewName.isEmpty() || transactionalTypeDefinition->hasView(viewName))
        {
            transactionalTypeDefinition->setTypeName(typeName);
            return;
        }
    }

    transTypeDefs_->clear();

    if (!typeName.isEmpty())
    {
        QSharedPointer<WireTypeDef> newTransactionalTypeDefinition(new WireTypeDef(typeName));
        newTransactionalTypeDefinition->getViewRefs()->append(viewName);
        transTypeDefs_->append(newTransactionalTypeDefinition);
    }
}

//-----------------------------------------------------------------------------
// Function: Transactional::hasType()
//-----------------------------------------------------------------------------
bool Transactional::hasType(QString const& viewName) const
{
    return std::any_of(transTypeDefs_->cbegin(), transTypeDefs_->cend(),
        [&viewName](auto const& transactionalTypeDefinition)
        {
            return ((viewName.isEmpty() && !transactionalTypeDefinition->getTypeName().isEmpty()) ||
                (transactionalTypeDefinition->hasView(viewName) &&
                    !transactionalTypeDefinition->getTypeName().isEmpty()));
        });
}

//-----------------------------------------------------------------------------
// Function: Transactional::getTypeDefinition()
//-----------------------------------------------------------------------------
QString Transactional::getTypeDefinition(QString const& typeName) const
{
    for (QSharedPointer<WireTypeDef> transactionalTypeDefinition : *transTypeDefs_)
    {
        if (transactionalTypeDefinition->getTypeName() == typeName)
        {
            return transactionalTypeDefinition->getTypeDefinitions()->value(0, QString());
        }
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: Transactional::getTypeDefinitions()
//-----------------------------------------------------------------------------
QStringList Transactional::getTypeDefinitions() const
{
    QStringList typeDefs;
    for (QSharedPointer<WireTypeDef> transactionalTypeDefinition : *transTypeDefs_)
    {
        typeDefs.append(*transactionalTypeDefinition->getTypeDefinitions());
    }
    return typeDefs;
}

//-----------------------------------------------------------------------------
// Function: Transactional::setTypeDefinition()
//-----------------------------------------------------------------------------
void Transactional::setTypeDefinition(const QString& typeName, const QString& typeDefinition)
{
    for (QSharedPointer<WireTypeDef> transactionalTypeDefinition : *transTypeDefs_)
    {
        if (transactionalTypeDefinition->getTypeName() == typeName)
        {
            QSharedPointer<QStringList> typeDefinitions(new QStringList());
            typeDefinitions->append(typeDefinition);
            transactionalTypeDefinition->setTypeDefinitions(typeDefinitions);
            return;
        }
    }

    transTypeDefs_->clear();

    QSharedPointer<WireTypeDef> transactionalTypeDefinition(new WireTypeDef(typeName));
    QSharedPointer<QStringList> typeDefinitions(new QStringList());
    typeDefinitions->append(typeDefinition);
    transactionalTypeDefinition->setTypeDefinitions(typeDefinitions);
    transTypeDefs_->append(transactionalTypeDefinition);
}

//-----------------------------------------------------------------------------
// Function: Transactional::hasTypeDefinitions()
//-----------------------------------------------------------------------------
bool Transactional::hasTypeDefinitions() const
{
    return std::any_of(transTypeDefs_->cbegin(), transTypeDefs_->cend(),
        [](auto const& transactionalTypeDefinition)
        {
            return (!transactionalTypeDefinition->getTypeName().isEmpty() ||
                !transactionalTypeDefinition->getTypeDefinitions()->isEmpty());
        });
}
