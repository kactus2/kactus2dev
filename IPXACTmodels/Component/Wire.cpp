//-----------------------------------------------------------------------------
// File: Wire.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 10.09.2015
//
// Description:
// Describes the ipxact:wire element.
//-----------------------------------------------------------------------------

#include "Wire.h"

#include <IPXACTmodels/common/Vector.h>

#include <IPXACTmodels/utilities/Copy.h>

#include <QString>
#include <QList>
#include <QObject>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
// Function: Wire()
//-----------------------------------------------------------------------------
Wire::Wire(Wire const& other) : 
direction_(other.direction_),
allLogicalDirectionsAllowed_(other.allLogicalDirectionsAllowed_),
defaultDriverValue_(other.defaultDriverValue_),
defaultValueAttributes_(other.defaultValueAttributes_)
{
	if (other.vector_)
    {
        vector_ = QSharedPointer<Vector>(new Vector(*other.vector_));
	}

    Copy::copyList(other.wireTypeDefs_, wireTypeDefs_);
}

//-----------------------------------------------------------------------------
// Function: Wire::operator=()
//-----------------------------------------------------------------------------
Wire& Wire::operator=( const Wire &other )
{
	if (this != &other)
    {
		direction_ = other.direction_;
		allLogicalDirectionsAllowed_ = other.allLogicalDirectionsAllowed_;
		defaultDriverValue_ = other.defaultDriverValue_;
		defaultValueAttributes_ = other.defaultValueAttributes_;

		if (other.vector_)
        {
            vector_ = QSharedPointer<Vector>(new Vector(*other.vector_));
		}

        wireTypeDefs_.clear();
        Copy::copyList(other.wireTypeDefs_, wireTypeDefs_);
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: Wire::getDirection()
//-----------------------------------------------------------------------------
DirectionTypes::Direction Wire::getDirection() const
{
    return direction_;
}

//-----------------------------------------------------------------------------
// Function: Wire::getWireTypeDefs()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<WireTypeDef> > > Wire::getWireTypeDefs() const
{
    return wireTypeDefs_;
}

//-----------------------------------------------------------------------------
// Function: Wire::getVector()
//-----------------------------------------------------------------------------
QSharedPointer<Vector> Wire::getVector() const
{
    return vector_;
}

//-----------------------------------------------------------------------------
// Function: Wire::setVector()
//-----------------------------------------------------------------------------
void Wire::setVector(QSharedPointer<Vector> newVector)
{
    vector_ = newVector;
}

//-----------------------------------------------------------------------------
// Function: Wire::setDirection()
//-----------------------------------------------------------------------------
void Wire::setDirection(DirectionTypes::Direction direction)
{
	direction_ = direction;
}

//-----------------------------------------------------------------------------
// Function: Wire::getAllLogicalDirectionsAllowed()
//-----------------------------------------------------------------------------
bool Wire::getAllLogicalDirectionsAllowed() const
{
    return allLogicalDirectionsAllowed_;
}

//-----------------------------------------------------------------------------
// Function: Wire::setWireTypeDefs()
//-----------------------------------------------------------------------------
void Wire::setWireTypeDefs(QSharedPointer<QList<QSharedPointer<WireTypeDef> > > newWireTypeDefs)
{
	wireTypeDefs_ = newWireTypeDefs;
}

//-----------------------------------------------------------------------------
// Function: Wire::setAllLogicalDirectionsAllowed()
//-----------------------------------------------------------------------------
void Wire::setAllLogicalDirectionsAllowed(bool allow)
{
    allLogicalDirectionsAllowed_ = allow;
}

//-----------------------------------------------------------------------------
// Function: Wire::setDefaultDriverValue()
//-----------------------------------------------------------------------------
void Wire::setDefaultDriverValue(const QString& defaultDriverValue)
{
    defaultDriverValue_ = defaultDriverValue;
}

//-----------------------------------------------------------------------------
// Function: Wire::getDefaultDriverValue()
//-----------------------------------------------------------------------------
QString Wire::getDefaultDriverValue() const
{
    return defaultDriverValue_;
}

//-----------------------------------------------------------------------------
// Function: wire::setLeftBoundExpression()
//-----------------------------------------------------------------------------
void Wire::setVectorLeftBound(QString const& expression)
{
    if (!vector_)
    {
        vector_ = QSharedPointer<Vector>(new Vector(QStringLiteral("0"), QStringLiteral("0")));
    }

    vector_->setLeft(expression);
}

//-----------------------------------------------------------------------------
// Function: wire::setRightBoundExpression()
//-----------------------------------------------------------------------------
void Wire::setVectorRightBound(QString const& expression)
{
    if (!vector_)
    {
        vector_ = QSharedPointer<Vector>(new Vector(QStringLiteral("0"), QStringLiteral("0")));
    }

    vector_->setRight(expression);
}

//-----------------------------------------------------------------------------
// Function: wire::getLeftBoundExpression()
//-----------------------------------------------------------------------------
QString Wire::getVectorLeftBound() const
{
    if (!vector_)
    {
        return QString();
    }

    return vector_->getLeft();
}

//-----------------------------------------------------------------------------
// Function: wire::getRightBoundExpression()
//-----------------------------------------------------------------------------
QString Wire::getVectorRightBound() const
{
    if (!vector_)
    {
        return QString();
    }

    return vector_->getRight();
}

//-----------------------------------------------------------------------------
// Function: Wire::getTypeName()
//-----------------------------------------------------------------------------
QString Wire::getTypeName(QString const& viewName) const
{
    if (wireTypeDefs_)
    {
        for (QSharedPointer<WireTypeDef> wireTypeDefinition : *wireTypeDefs_)
        {
            if (wireTypeDefinition->hasView(viewName))
            {
                return wireTypeDefinition->getTypeName();
            }
        }
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: Wire::setTypeName()
//-----------------------------------------------------------------------------
void Wire::setTypeName(QString const& typeName, QString const& viewName)
{
    if (wireTypeDefs_)
    {
        for (QSharedPointer<WireTypeDef> wireTypeDefinition : *wireTypeDefs_)
        {
            if (wireTypeDefinition->getViewRefs()->contains(viewName) || viewName.isEmpty())
            {
                wireTypeDefinition->setTypeName(typeName);
                return;
            }
        }
    }
    else
    {
        wireTypeDefs_ = QSharedPointer<QList<QSharedPointer<WireTypeDef> > >(
            new QList<QSharedPointer<WireTypeDef> >());
    }

    wireTypeDefs_->clear();

    if (!typeName.isEmpty())
    {
        QSharedPointer<WireTypeDef> newWireTypeDefinition(new WireTypeDef(typeName));
        newWireTypeDefinition->getViewRefs()->append(viewName);
        wireTypeDefs_->append(newWireTypeDefinition);
    }
}

//-----------------------------------------------------------------------------
// Function: Wire::getTypeDefinition()
//-----------------------------------------------------------------------------
QString Wire::getTypeDefinition(QString const& typeName) const
{
    if (wireTypeDefs_)
    {  
        for (QSharedPointer<WireTypeDef> wireTypeDefinition : *wireTypeDefs_)
        {
            if (wireTypeDefinition->getTypeName() == typeName)
            {
                return wireTypeDefinition->getTypeDefinitions()->value(0, QString());
            }
        }
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: Wire::getTypeDefinitions()
//-----------------------------------------------------------------------------
QStringList Wire::getTypeDefinitions() const
{
    QStringList typeDefs;
    if (wireTypeDefs_)
    {
        for (QSharedPointer<WireTypeDef> wireTypeDefinition : *wireTypeDefs_)
        {
            for (QString singleDefinition : *wireTypeDefinition->getTypeDefinitions())
            {
                typeDefs.append(singleDefinition);
            }
        }
    }
	return typeDefs;
}

//-----------------------------------------------------------------------------
// Function: Wire::setTypeDefinition()
//-----------------------------------------------------------------------------
void Wire::setTypeDefinition(QString const& typeName, QString const& typeDefinition )
{
    if (wireTypeDefs_)
    {
        for (QSharedPointer<WireTypeDef> wireTypeDefinition : *wireTypeDefs_)
        {
            if (wireTypeDefinition->getTypeName() == typeName)
            {
                QSharedPointer<QStringList> typeDefinitions(new QStringList());
                typeDefinitions->append(typeDefinition);
                wireTypeDefinition->setTypeDefinitions(typeDefinitions);
                return;
            }
        }
    }
    else
    {
        wireTypeDefs_ = QSharedPointer<QList<QSharedPointer<WireTypeDef> > >(
            new QList<QSharedPointer<WireTypeDef> >());
    }

    wireTypeDefs_->clear();

    QSharedPointer<WireTypeDef> newWireTypeDefinition(new WireTypeDef(typeName));
    QSharedPointer<QStringList> typeDefinitions(new QStringList());
    typeDefinitions->append(typeDefinition);
    newWireTypeDefinition->setTypeDefinitions(typeDefinitions);
    wireTypeDefs_->append(newWireTypeDefinition);
}

//-----------------------------------------------------------------------------
// Function: Wire::hasType()
//-----------------------------------------------------------------------------
bool Wire::hasType(QString const& viewName) const
{
    if (!wireTypeDefs_)
    {
        return false;
    }

    for (QSharedPointer<WireTypeDef> wireTypeDefinition : *wireTypeDefs_)
    {
		if (!wireTypeDefinition->getTypeName().isEmpty() && 
            (viewName.isEmpty() ||  wireTypeDefinition->hasView(viewName) ))
        {
            return true;
		}
	}

    return false;
}

//-----------------------------------------------------------------------------
// Function: Wire::hasTypeDefinition()
//-----------------------------------------------------------------------------
bool Wire::hasTypeDefinition() const
{
    if (!wireTypeDefs_)
    {
        return false;
    }

    return std::any_of(wireTypeDefs_->cbegin(), wireTypeDefs_->cend(), [](auto const& typeDefinition)
        {
            return !typeDefinition->getTypeName().isEmpty() || !typeDefinition->getTypeDefinitions()->isEmpty();
        });
}
